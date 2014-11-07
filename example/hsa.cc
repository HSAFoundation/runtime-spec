#define HSA_LARGE_MODEL 1
#include "hsa.h"


#include <inttypes.h>
#include <algorithm>
#include <cassert>
#include <cstring> // memset
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace hsa {

  typedef void(*queue_callback_t)(hsa_status_t status, hsa_queue_t *queue);
  typedef void(*dispatch_t)(void* args);

  typedef struct packet_s {
    uint16_t header;
    uint8_t padding[54]; // TODO(mmario): ensure compiler does not add padding
    hsa_signal_t completion_signal;
  } packet_t;

  class Signal {
  public:

    Signal() {
    }

    Signal(hsa_signal_value_t val) {
      val_ = val;
    }

    ~Signal() {
    }

    hsa_signal_value_t Load(std::memory_order order) {
      return val_.load(order);
    }

    void Store(hsa_signal_value_t val, std::memory_order order) {
      std::lock_guard<std::mutex> lock(mutex_);
      val_.store(val, order);
      condition_.notify_all();
    }

    hsa_signal_value_t Exchange(hsa_signal_value_t val, std::memory_order order) {
      std::lock_guard<std::mutex> lock(mutex_);
      hsa_signal_value_t ret = val_.exchange(val, order);
      condition_.notify_all();
      return ret;
    }

    hsa_signal_value_t Cas(hsa_signal_value_t expected, hsa_signal_value_t val, std::memory_order order) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (val_.compare_exchange_weak(expected, val, order)) {
        condition_.notify_all();
      }
      return expected;
    }

    void Add(hsa_signal_value_t value, std::memory_order order) {
      if (value == 0) {
        return;
      }
      std::lock_guard<std::mutex> lock(mutex_);
      val_.fetch_add(value, order);
      condition_.notify_all();
    }

    void Subtract(hsa_signal_value_t value, std::memory_order order) {
      if (value == 0) {
        return;
      }
      std::lock_guard<std::mutex> lock(mutex_);
      val_.fetch_sub(value, order);
      condition_.notify_all();
    }

    void Or(hsa_signal_value_t value, std::memory_order order) {
      if (value == 0) {
        return;
      }
      std::lock_guard<std::mutex> lock(mutex_);
      val_.fetch_or(value, order);
      condition_.notify_all();
    }

    void Xor(hsa_signal_value_t value, std::memory_order order) {
      std::lock_guard<std::mutex> lock(mutex_);
      val_.fetch_xor(value, order);
      condition_.notify_all();
    }

    // precondition: the caller holds a lock on mutex_
    bool Satisfies(std::memory_order order, hsa_signal_condition_t condition, hsa_signal_value_t comp) {
      if (condition == HSA_EQ) {
        return val_.load(order) == comp;
      }
      if (condition == HSA_NE) {
        return val_.load(order) != comp;
      }
      if (condition == HSA_LT) {
        return val_.load(order) < comp;
      }
      if (condition == HSA_GTE) {
        return val_.load(order) >= comp;
      }
      std::abort();
      return false;
    }

    hsa_signal_value_t Wait(std::memory_order order, hsa_signal_condition_t condition, hsa_signal_value_t comp) {
      std::unique_lock<std::mutex> lock(mutex_);
      if (!Satisfies(order, condition, comp)) {
        // no possible race condition between the check and the wait start since we are holding the lock
        condition_.wait(lock, [&]{ return Satisfies(order, condition, comp); });
      }
      return val_.load(order);
    }

  private:
    std::atomic<hsa_signal_value_t> val_;
    std::mutex mutex_;
    std::condition_variable condition_;
  };

  struct Queue {
    Queue(hsa_agent_t agent,
      uint32_t size,
      hsa_queue_type_t type,
      queue_callback_t callback,
      const hsa_queue_t* service_queue) {
      packets_ = new packet_t[size];
      memset(packets_, 0, size * sizeof(packet_t));
      read_index_ = 0;
      write_index_ = 0;
      agent_ = agent;
      callback_ = callback;

      q_.type = type;
      hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &(q_.features));
      q_.base_address = packets_;
      q_.doorbell_signal.handle = (uint64_t)&doorbell_;

      q_.size = size;
      q_.id = GetUniqueId();

      if (!AgentDispatchQueue()) {
        packet_processor_ = new std::thread(&Queue::Go, this);
        q_.service_queue = (hsa_queue_t*) service_queue;
      } else {
        assert(service_queue == NULL);
        // we need to set the initial value to some negative number
        // otherwise, the application code waiting for the signal to become zero will get confused between
        // unitialized doorbells and doorbells rang with ID zero
        doorbell_.Store(-1, std::memory_order_release);
      }
    }

    ~Queue() {
      if (!AgentDispatchQueue()) {
        packet_processor_->detach();
        delete packet_processor_;
      }
      delete[] packets_;
    }

    Queue(const Queue& q) = delete;
    Queue& operator=(Queue const&) = delete;

    static bool HasFence(uint16_t fence) {
      return fence == HSA_FENCE_SCOPE_COMPONENT || fence == HSA_FENCE_SCOPE_SYSTEM;
    }

    void DecrementCompletionSignal(packet_t& packet) {
      if (packet.completion_signal.handle != 0) {
        hsa::Signal* sig = (hsa::Signal*) packet.completion_signal.handle;
        sig->Subtract(1, std::memory_order_release);
      }
    }

    bool ProcessDispatch(hsa_kernel_dispatch_packet_t& packet) {
      if (packet.setup == 0) { // dimensions
        if (callback_) {
          callback_(HSA_STATUS_ERROR_INVALID_PACKET_FORMAT, &q_);
        }
        return false;
      }
      // other checks...

      // 'barrier' bit is ignored because we are completing dispatches in order

      std::atomic_thread_fence(std::memory_order_acquire);

      dispatch_t func = (dispatch_t)packet.kernel_object;
      func(packet.kernarg_address);
      std::atomic_thread_fence(std::memory_order_release);
      DecrementCompletionSignal((packet_t&)packet);
      return true;
    }

    bool ProcessBarrier(hsa_barrier_and_packet_t& packet) {
      for (int i = 0; i < 5; i++) {
        uint64_t dep = packet.dep_signal[i].handle;
        if (dep != 0) {
          hsa::Signal* sig = (hsa::Signal*) dep;
          sig->Wait(std::memory_order_acquire, HSA_EQ, 0);
        }
      }
      std::atomic_thread_fence(std::memory_order_release);
      DecrementCompletionSignal((packet_t&)packet);
      return true;
    }

    static void clear_field(uint16_t* header, unsigned int start, unsigned int width) {
      *header &= ~(((1 << width) - 1) << start);
    }

    static unsigned int get_field(uint16_t header, unsigned int start, unsigned int width) {
      return (header >> start) & ((1 << width) - 1);
    }

    static void set_field(uint16_t* header, unsigned int start, unsigned int width, unsigned int value) {
      clear_field(header, start, width);
      *header |= (value << start);
    }

    void Go() {
      bool ok = true;
      while (ok) {
        size_t curr = read_index_ % q_.size;
        packet_t* packet = packets_ + curr;

        while (get_field(packet->header, HSA_PACKET_HEADER_TYPE, HSA_PACKET_HEADER_WIDTH_TYPE) <= HSA_PACKET_TYPE_INVALID);
        hsa_packet_type_t type = (hsa_packet_type_t) get_field(packet->header, HSA_PACKET_HEADER_TYPE, HSA_PACKET_HEADER_WIDTH_TYPE);
        if (type == HSA_PACKET_TYPE_KERNEL_DISPATCH) {
          ok &= ProcessDispatch(*((hsa_kernel_dispatch_packet_t*)packet));
        } else if (type == HSA_PACKET_TYPE_BARRIER_AND) {
          ok &= ProcessBarrier(*((hsa_barrier_and_packet_t*)packet));
        } else {
          // packet format invalid (including agent dispatch)
          printf("Unknown type: %u\n", type);
          std::abort();
        }
        set_field(&packet->header, HSA_PACKET_HEADER_TYPE, HSA_PACKET_HEADER_WIDTH_TYPE, HSA_PACKET_TYPE_INVALID);
        read_index_++;
      }
    }

    uint64_t LoadReadIndex(std::memory_order order) const {
      return read_index_.load(order);
    }

    void StoreReadIndex(uint64_t value, std::memory_order order) {
      if (!AgentDispatchQueue()) {
        std::abort();
      }
      read_index_.store(value, order);
    }

    uint64_t LoadWriteIndex(std::memory_order order) const {
      return write_index_.load(order);
    }

    void StoreWriteIndex(uint64_t value, std::memory_order order) {
      write_index_.store(value, order);
    }

    uint64_t CasWriteIndex(uint64_t expected, uint64_t value, std::memory_order order) {
      write_index_.compare_exchange_strong(expected, value, order);
      return expected;
    }

    uint64_t AddWriteIndex(uint64_t value, std::memory_order order) {
      return write_index_.fetch_add(value, order);
    }

    bool AgentDispatchQueue() { return static_cast<bool>(q_.features & HSA_AGENT_FEATURE_AGENT_DISPATCH); }

    static uint32_t GetUniqueId() {
      static uint32_t curr = 0; // VS does not support magic static
      return curr++;
    }

    hsa_queue_t q_;

    packet_t *packets_;
    std::atomic<uint64_t> read_index_;
    std::atomic<uint64_t> write_index_;
    Signal doorbell_;
    queue_callback_t callback_;
    std::thread* packet_processor_;
    hsa_agent_t agent_;
  };


  class Region {
  public:
    virtual void* Alloc(size_t size) = 0;

    virtual void Free(void* ptr) = 0;

    virtual hsa_status_t Get(hsa_region_info_t attribute, void* value) const = 0;
  };

  class SystemMemory  {

  public:
    SystemMemory() {
    }

    ~SystemMemory() {
    }

    virtual void* Alloc(size_t size) {
      return malloc(size);
    }

    virtual void Free(void* ptr) {
      return free(ptr);
    }

    virtual hsa_status_t Get(hsa_region_info_t attribute, void* value) const {
      switch (attribute) {
      case HSA_REGION_INFO_BASE: {
        void** dst = (void**)value;
        *dst = (void*)0;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_SIZE: {
        size_t* dst = (size_t*)value;
        *dst = SIZE_MAX;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_AGENT: {
        hsa_agent_t* dst = (hsa_agent_t*)value;
        *dst = agent_;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_FLAGS: {
        uint32_t* dst = (uint32_t*)value;
        *dst = (uint32_t)HSA_REGION_FLAG_KERNARG;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_SEGMENT: {
        hsa_segment_t* dst = (hsa_segment_t*)value;
        *dst = HSA_SEGMENT_GLOBAL;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_ALLOC_MAX_SIZE: {
        size_t* dst = (size_t*)value;
        *dst = SIZE_MAX;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_ALLOC_GRANULE: {
        size_t* dst = (size_t*)value;
        *dst = 1;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_ALLOC_ALIGNMENT: {
        size_t* dst = (size_t*)value;
        *dst = 1;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_BANDWIDTH: {
        uint32_t* dst = (uint32_t*)value;
        *dst = bandwidth_;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_REGION_INFO_NODE: {
        uint32_t* dst = (uint32_t*)value;
        *dst = node_;
        return HSA_STATUS_SUCCESS;
      }
      default: return HSA_STATUS_ERROR_INVALID_ARGUMENT;
      }
      return HSA_STATUS_SUCCESS;
    }

    hsa_agent_t agent_;
    uint32_t bandwidth_;
    uint32_t node_;
  };


  class Agent {
  public:

    virtual hsa_status_t Get(hsa_agent_info_t attribute, void* value) const = 0;

    virtual hsa_status_t IterateRegions(hsa_status_t(*callback)(hsa_region_t region, void* data), void* data) = 0;
  };

  class HostAgent : Agent {

  public:

    HostAgent(bool agent_dispatch_enabled = false) {
      region_.agent_.handle = (uint64_t) this;
      region_.bandwidth_ = 0;
      region_.node_ = 0;

      agent_dispatch_enabled_ = agent_dispatch_enabled;
    }

    ~HostAgent(){
    }

    virtual hsa_status_t Get(hsa_agent_info_t attribute, void* value) const {
      switch (attribute) {
      case HSA_AGENT_INFO_DEVICE: {
        hsa_device_type_t* dst = (hsa_device_type_t*)value;
        *dst = HSA_DEVICE_TYPE_CPU;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_AGENT_INFO_FEATURE: {
        uint32_t* dst = (uint32_t*)value;
        *dst = agent_dispatch_enabled_ ? HSA_AGENT_FEATURE_AGENT_DISPATCH : HSA_AGENT_FEATURE_KERNEL_DISPATCH;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_AGENT_INFO_QUEUE_TYPE: {
        hsa_queue_type_t* dst = (hsa_queue_type_t*)value;
        *dst = HSA_QUEUE_TYPE_MULTI;
        return HSA_STATUS_SUCCESS;
      }
        // Fill as needed
      default: return HSA_STATUS_ERROR_INVALID_ARGUMENT;
      }
      return HSA_STATUS_SUCCESS;
    }

    virtual hsa_status_t IterateRegions(hsa_status_t(*callback)(hsa_region_t region, void* data), void* data) {
      hsa_region_t r;
      r.handle = (uint64_t)&region_;
      return callback(r, data);
    }

  private:
    SystemMemory region_;
    bool agent_dispatch_enabled_;
  };


  class Runtime {
  public:
    Runtime() {
      ref_count_ = 0;
    }

    ~Runtime() {
    }

    hsa_status_t Get(hsa_system_info_t attribute, void* value) const {
      switch (attribute) {
      case HSA_SYSTEM_INFO_VERSION_MAJOR: {
        uint16_t* dst = (uint16_t*)value;
        *dst = 1;
        return HSA_STATUS_SUCCESS;
      }
      case HSA_SYSTEM_INFO_VERSION_MINOR: {
        uint16_t* dst = (uint16_t*)value;
        *dst = 0;
        return HSA_STATUS_SUCCESS;
      }
        // Fill as needed
      default: return HSA_STATUS_ERROR_INVALID_ARGUMENT;
      }
      return HSA_STATUS_SUCCESS;
    }

    virtual hsa_status_t IterateAgents(hsa_status_t(*callback)(hsa_agent_t agent, void* data), void* data) {
      for (unsigned int i = 0; i < agents_.get()->size(); i++) {
        hsa_agent_t a;
        a.handle = (uint64_t)(agents_.get()->at(i));
        hsa_status_t stat = callback(a, data);
        if (stat != HSA_STATUS_SUCCESS) {
          return stat;
        }
      }
      return HSA_STATUS_SUCCESS;
    }

    hsa_status_t IncRefCount() {
      std::unique_lock<std::mutex> lk(mutex_);
      if (ref_count_ == INT32_MAX) {
        return HSA_STATUS_ERROR_REFCOUNT_OVERFLOW;
      }
      ref_count_++;
      if (ref_count_ == 1) {
        agents_.reset(new std::vector<HostAgent*>());
        agents_.get()->push_back(new HostAgent());
        agents_.get()->push_back(new HostAgent());
        agents_.get()->push_back(new HostAgent(true));
      }
      return HSA_STATUS_SUCCESS;
    }

    hsa_status_t DecRefCount() {
      std::unique_lock<std::mutex> lk(mutex_);
      if (!ref_count_) {
        return HSA_STATUS_ERROR_NOT_INITIALIZED;
      }
      ref_count_--;
      if (ref_count_ == 0) {
        agents_.reset(nullptr);
        // TODO: fix memory leak here
      }
      return HSA_STATUS_SUCCESS;
    }

  private:

    std::mutex mutex_;
    int32_t ref_count_;
    std::unique_ptr<std::vector<HostAgent*>> agents_;
  };

  static Runtime runtime_g;

} // hsa namespace

#ifdef __cplusplus
extern "C" {
#endif

  const char* invalid_packet_format = "Invalid packet format\n";


  hsa_status_t hsa_status_string(hsa_status_t status, const char ** status_string) {
    if (status == HSA_STATUS_ERROR_INVALID_PACKET_FORMAT) {
      *status_string = invalid_packet_format;
    } else {
      // fill as needed
      *status_string = "TODO";
    }
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_init() {
    return hsa::runtime_g.IncRefCount();
  }

  hsa_status_t hsa_shut_down() {
    return hsa::runtime_g.DecRefCount();
  }

  hsa_status_t hsa_system_get_info(hsa_system_info_t attribute, void* value) {
    return hsa::runtime_g.Get(attribute, value);
  }

  hsa_status_t hsa_agent_get_info(hsa_agent_t agent, hsa_agent_info_t attribute, void* value) {
    hsa::Agent* a = (hsa::Agent*) agent.handle;
    return a->Get(attribute, value);
  }

  hsa_status_t hsa_iterate_agents(
    hsa_status_t(*callback)(hsa_agent_t agent, void* data),
    void* data) {
    return hsa::runtime_g.IterateAgents(callback, data);
  }

  hsa_status_t hsa_signal_create(hsa_signal_value_t initial_value, uint32_t num_consumers, const hsa_agent_t *consumers, hsa_signal_t *signal) {
    // ignore consumers hint for now
    hsa::Signal* sig = new hsa::Signal(initial_value);
    signal->handle = (uint64_t) sig;
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_signal_destroy(hsa_signal_t signal) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    delete sig;
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_queue_inactivate(hsa_queue_t *queue) {
    std::abort();
    return HSA_STATUS_ERROR;
  }

  hsa_signal_value_t hsa_signal_load_acquire(hsa_signal_t signal) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Load(std::memory_order_acquire);
  }

  hsa_signal_value_t hsa_signal_load_relaxed(hsa_signal_t signal) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Load(std::memory_order_relaxed);
  }

  void hsa_signal_store_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Store(value, std::memory_order_relaxed);
  }

  void hsa_signal_store_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Store(value, std::memory_order_release);
  }

  hsa_signal_value_t hsa_signal_exchange_acq_rel(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Exchange(value, std::memory_order_acq_rel);
  }

  hsa_signal_value_t hsa_signal_exchange_acquire(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Exchange(value, std::memory_order_acquire);
  }

  hsa_signal_value_t hsa_signal_exchange_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Exchange(value, std::memory_order_relaxed);
  }

  hsa_signal_value_t hsa_signal_exchange_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Exchange(value, std::memory_order_release);
  }

  hsa_signal_value_t hsa_signal_cas_acq_rel(hsa_signal_t signal, hsa_signal_value_t expected,
    hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Cas(expected, value, std::memory_order_acq_rel);
  }

  hsa_signal_value_t HSA_API hsa_signal_cas_acquire(hsa_signal_t signal, hsa_signal_value_t expected,
    hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Cas(expected, value, std::memory_order_acquire);
  }

  hsa_signal_value_t HSA_API hsa_signal_cas_relaxed(hsa_signal_t signal, hsa_signal_value_t expected,
    hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Cas(expected, value, std::memory_order_relaxed);
  }

  hsa_signal_value_t HSA_API hsa_signal_cas_release(hsa_signal_t signal, hsa_signal_value_t expected, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Cas(expected, value, std::memory_order_release);
  }

  void hsa_signal_add_acq_rel(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Add(value, std::memory_order_acq_rel);
  }

  void hsa_signal_add_acquire(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Add(value, std::memory_order_acquire);
  }

  void hsa_signal_add_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Add(value, std::memory_order_relaxed);
  }

  void hsa_signal_add_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Add(value, std::memory_order_release);
  }

  void hsa_signal_subtract_acq_rel(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Subtract(value, std::memory_order_acq_rel);
  }

  void hsa_signal_subtract_acquire(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Subtract(value, std::memory_order_acquire);
  }

  void hsa_signal_subtract_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Subtract(value, std::memory_order_relaxed);
  }

  void hsa_signal_subtract_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Subtract(value, std::memory_order_release);
  }

  void hsa_signal_or_acq_rel(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Or(value, std::memory_order_acq_rel);
  }

  void hsa_signal_or_acquire(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Or(value, std::memory_order_acquire);
  }

  void hsa_signal_or_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Or(value, std::memory_order_relaxed);
  }

  void hsa_signal_or_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Or(value, std::memory_order_release);
  }

  void hsa_signal_xor_acq_rel(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Xor(value, std::memory_order_acq_rel);
  }

  void hsa_signal_xor_acquire(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Xor(value, std::memory_order_acquire);
  }

  void hsa_signal_xor_relaxed(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Xor(value, std::memory_order_relaxed);
  }

  void hsa_signal_xor_release(hsa_signal_t signal, hsa_signal_value_t value) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    sig->Xor(value, std::memory_order_release);
  }

  hsa_signal_value_t hsa_signal_wait_acquire(hsa_signal_t signal, hsa_signal_condition_t condition, hsa_signal_value_t compare_value, uint64_t timeout_hint, hsa_wait_expectancy_t wait_expectancy_hint) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Wait(std::memory_order_acquire, condition, compare_value);
  }

  hsa_signal_value_t hsa_signal_wait_relaxed(hsa_signal_t signal, hsa_signal_condition_t condition, hsa_signal_value_t compare_value, uint64_t timeout_hint, hsa_wait_expectancy_t wait_expectancy_hint) {
    hsa::Signal* sig = (hsa::Signal*) signal.handle;
    return sig->Wait(std::memory_order_relaxed, condition, compare_value);
  }

  hsa_status_t hsa_queue_create(
    hsa_agent_t agent,
    uint32_t size,
    hsa_queue_type_t type,
    void(*callback)(hsa_status_t status, hsa_queue_t *queue),
    const hsa_queue_t* service_queue,
    uint32_t private_segment_size,
    uint32_t group_segment_size,
    hsa_queue_t **queue) {
    hsa::Queue* q = new hsa::Queue(agent, size, type, callback, service_queue);
    *queue = (hsa_queue_t*)q;
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_queue_destroy(hsa_queue_t *queue) {
    hsa::Queue* q = (hsa::Queue*) queue;
    delete q;
    return HSA_STATUS_SUCCESS;
  }

  uint64_t hsa_queue_load_read_index_acquire(const hsa_queue_t *queue) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->LoadReadIndex(std::memory_order_acquire);
  }

  uint64_t hsa_queue_load_read_index_relaxed(const hsa_queue_t *queue) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->LoadReadIndex(std::memory_order_relaxed);
  }

  uint64_t hsa_queue_load_write_index_acquire(const hsa_queue_t *queue) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->LoadWriteIndex(std::memory_order_acquire);
  }

  uint64_t hsa_queue_load_write_index_relaxed(const hsa_queue_t *queue) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->LoadWriteIndex(std::memory_order_relaxed);
  }

  void hsa_queue_store_write_index_relaxed(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    q->StoreWriteIndex(value, std::memory_order_relaxed);
  }

  void hsa_queue_store_write_index_release(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    q->StoreWriteIndex(value, std::memory_order_release);
  }

  uint64_t hsa_queue_cas_write_index_acq_rel(const hsa_queue_t *queue, uint64_t expected, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->CasWriteIndex(expected, value, std::memory_order_acq_rel);
  }

  uint64_t hsa_queue_cas_write_index_acquire(const hsa_queue_t *queue, uint64_t expected, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->CasWriteIndex(expected, value, std::memory_order_acquire);
  }

  uint64_t hsa_queue_cas_write_index_relaxed(const hsa_queue_t *queue, uint64_t expected, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->CasWriteIndex(expected, value, std::memory_order_relaxed);
  }

  uint64_t hsa_queue_cas_write_index_release(const hsa_queue_t *queue, uint64_t expected, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->CasWriteIndex(expected, value, std::memory_order_release);
  }

  uint64_t hsa_queue_add_write_index_acq_rel(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->AddWriteIndex(value, std::memory_order_acq_rel);
  }

  uint64_t hsa_queue_add_write_index_acquire(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->AddWriteIndex(value, std::memory_order_acquire);
  }

  uint64_t hsa_queue_add_write_index_release(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->AddWriteIndex(value, std::memory_order_release);
  }

  uint64_t hsa_queue_add_write_index_relaxed(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    return q->AddWriteIndex(value, std::memory_order_relaxed);
  }

  void hsa_queue_store_read_index_relaxed(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    q->StoreReadIndex(value, std::memory_order_relaxed);
  }

  void hsa_queue_store_read_index_release(const hsa_queue_t *queue, uint64_t value) {
    hsa::Queue* q = (hsa::Queue*) queue;
    q->StoreReadIndex(value, std::memory_order_release);
  }

  hsa_status_t hsa_region_get_info(
    hsa_region_t region,
    hsa_region_info_t attribute,
    void* value) {
    hsa::Region* r = (hsa::Region*) region.handle;
    return r->Get(attribute, value);
  }

  hsa_status_t  hsa_agent_iterate_regions(
    hsa_agent_t agent,
    hsa_status_t(*callback)(hsa_region_t region, void* data),
    void* data) {
    hsa::Agent* a = (hsa::Agent*) agent.handle;
    return a->IterateRegions(callback, data);
  }

  hsa_status_t hsa_memory_allocate(hsa_region_t region, size_t size, void** ptr) {
    hsa::Region* r = (hsa::Region*) region.handle;
    *ptr = r->Alloc(size);
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_memory_free(void* ptr) {
    // TODO: given a ptr, find the corresponding region
    // empty for now
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_memory_register(void *address, size_t size) {
    return HSA_STATUS_SUCCESS;
  }

  hsa_status_t hsa_memory_deregister(void *address, size_t size) {
    return HSA_STATUS_SUCCESS;
  }

#ifdef __cplusplus
}
#endif
