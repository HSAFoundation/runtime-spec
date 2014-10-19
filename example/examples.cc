#include "assert.h"
#include "inttypes.h" // PRIus64
#include "stdio.h"
#include "string.h" // memset

#include <atomic>
#include <thread>
#include <vector>

#define HSA_LARGE_MODEL 1 // has to go before including hsa.h

#include "hsa.h"

std::atomic<int>* counter; // used in the multi-threaded dispatch
uint64_t KERNEL_OBJECT;

void initialize_packet(hsa_kernel_dispatch_packet_t* packet) {
    // Contents are zeroed:
    //    -Reserved fields must be 0
    //    -Type is set to HSA_PACKET_TYPE_ALWAYS_RESERVED, so the packet cannot be consumed by the packet processor
    memset(packet, 0, sizeof(hsa_kernel_dispatch_packet_t));

    packet->header |= HSA_FENCE_SCOPE_COMPONENT << HSA_PACKET_HEADER_ACQUIRE_FENCE_SCOPE;
    packet->header |= HSA_FENCE_SCOPE_COMPONENT << HSA_PACKET_HEADER_RELEASE_FENCE_SCOPE;

    packet->setup |= 1 << HSA_KERNEL_DISPATCH_PACKET_SETUP_DIMENSIONS;
    packet->workgroup_size_x = 256;
    packet->workgroup_size_y = 1;
    packet->workgroup_size_z = 1;
    packet->grid_size_x = 256;
    packet->grid_size_y = 1;
    packet->grid_size_z = 1;

    // Indicate which executable code to run.
    // The application is expected to have finalized a kernel (for example, using the finalization API).
    // We will assume that the kernel object containing the executable code is stored in KERNEL_OBJECT
    packet->kernel_object = KERNEL_OBJECT;

    // Assume our kernel receives no arguments
    packet->kernarg_address = NULL;
}

// Find HSA agent that can process Kernel Dispatch packets.
hsa_status_t get_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        // Store HSA component in the application-provided buffer and return
        hsa_agent_t* ret = (hsa_agent_t*) data;
        *ret = agent;
        return HSA_STATUS_INFO_BREAK;
    }
    // Keep iterating
    return HSA_STATUS_SUCCESS;
}

void packet_type_store_release(uint16_t* header, hsa_packet_type_t type) {
    __atomic_store_n((uint8_t*) header, (uint8_t) type, __ATOMIC_RELEASE);
}

void hello_world() {
	printf("Hello World!\n");
}

void simple_dispatch() {
    // Initialize the runtime
    hsa_init();

    // Retrieve the HSA component
    hsa_agent_t component;
    hsa_iterate_agents(get_component, &component);

    // Create a queue in the HSA component. The queue can hold 4 packets, and has no callback or service queue associated with it
    hsa_queue_t *queue;
    hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue);

    // Request a packet ID from the queue. Since no packets have been enqueued yet, the expected ID is zero
    uint64_t packet_id = hsa_queue_add_write_index_relaxed(queue, 1);

    // Calculate the virtual address where to place the packet
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*) queue->base_address + packet_id;

    // Populate fields in Kernel Dispatch packet, except for the completion signal and the header type
    initialize_packet(packet);

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_create(1, 0, NULL, &packet->completion_signal);

    // Notify the queue that the packet is ready to be processed
    packet_type_store_release(&packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue->doorbell_signal, packet_id);

    // Wait for the task to finish, which is the same as waiting for the value of the completion signal to be zero
    while (hsa_signal_wait_acquire(packet->completion_signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    // Done! The kernel has completed. Time to cleanup resources and leave
    hsa_signal_destroy(packet->completion_signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
}

void increment(void* kernarg) {
    std::atomic<int>* counter = (std::atomic<int>*) kernarg;
    counter->fetch_add(1, std::memory_order_release);
}

hsa_status_t get_multi_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        hsa_queue_type_t queue_type;
        hsa_agent_get_info(agent, HSA_AGENT_INFO_QUEUE_TYPE, &queue_type);
        if (queue_type == HSA_QUEUE_TYPE_MULTI) {
            hsa_agent_t* ret = (hsa_agent_t*)data;
            *ret = agent;
            return HSA_STATUS_INFO_BREAK;
        }
    }
    return HSA_STATUS_SUCCESS;
}

void enqueue(hsa_queue_t* queue) {
    // Create a signal with an initial value of 1000 to monitor the overall task completion
    hsa_signal_t signal;
    hsa_signal_create(1000, 0, NULL, &signal);
    hsa_kernel_dispatch_packet_t* packets = (hsa_kernel_dispatch_packet_t*)queue->base_address;

    for (int i = 0; i < 1000; i++) {
        // Atomically request a new packet ID.
        uint64_t packet_id = hsa_queue_add_write_index_release(queue, 1);

        // Wait until the queue is not full before writing the packet
        while (packet_id - hsa_queue_load_read_index_acquire(queue) >= queue->size);

        // Compute packet offset, considering wrap-around
        hsa_kernel_dispatch_packet_t* packet = packets + packet_id % queue->size;

        initialize_packet(packet);
        packet->kernarg_address = counter;
        packet->completion_signal = signal;
        packet_type_store_release(&packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
        hsa_signal_store_release(queue->doorbell_signal, packet_id);
    }

    // Wait until all the kernels are complete
    while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);
    hsa_signal_destroy(signal);
}

void multithread_dispatch() {
    hsa_init();
    counter = new std::atomic<int>(0);
    hsa_agent_t component;
    hsa_iterate_agents(get_multi_component, &component);
    hsa_queue_t *queue;
    // Create a queue in the selected component.
    hsa_queue_create(component, 4, HSA_QUEUE_TYPE_MULTI, NULL, NULL, &queue);

    std::vector<std::thread*> threads;
    const int kNumThreads = 4;
    for (int i = 0; i < kNumThreads; i++) {
        std::thread* thread = new std::thread(enqueue, queue);
        threads.push_back(thread);
    }

    for (unsigned int i = 0; i < threads.size(); i++) {
        threads.at(i)->join();
    }

    printf("Result: %d\n", counter->load());

    hsa_queue_destroy(queue);
    hsa_shut_down();
}

void callback(hsa_status_t status, hsa_queue_t* queue) {
  const char* message;
  hsa_status_string(status, &message);
  printf("Error at queue %d: %s", queue->id, message);
}

void error_callback() {
  hsa_init();
  hsa_agent_t component;
  hsa_iterate_agents(get_component, &component);

  hsa_queue_t *queue;
  hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, callback, NULL, &queue);

  uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);
  hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*)queue->base_address + write_index;

  // Wrong setup: the grid dimensions have to be between 1 and 3.
  // causes HSA_STATUS_ERROR_INVALID_PACKET_FORMAT
  packet->setup = 0;

  // Rest of dispatch packet is setup...

  hsa_signal_t signal;
  hsa_signal_create(1, 0, NULL, &signal);
  packet->completion_signal = signal;
  packet_type_store_release(&packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
  hsa_signal_store_release(queue->doorbell_signal, write_index);

  // Wait for the task to finish, which is the same as waiting for the value of the
  // completion signal to be zero.
  while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

  hsa_signal_destroy(signal);
  hsa_queue_destroy(queue);
  hsa_shut_down();
}

void print_signal_value(uint64_t args) {
  hsa_signal_t signal  = *((hsa_signal_t*) args);
  printf("Signal value: %ld\n", hsa_signal_load_acquire(signal));
}

size_t round_up(size_t numToRound, size_t multiple) {
    if (multiple == 0) {
        return numToRound;
    }
    size_t remainder = numToRound % multiple;
    if (remainder == 0) {
        return numToRound;
    }
    return numToRound + multiple - remainder;
}

hsa_status_t get_kernarg(hsa_region_t region, void* data) {
    hsa_region_flag_t flags;
    hsa_region_get_info(region, HSA_REGION_INFO_FLAGS, &flags);
    if (flags & HSA_REGION_FLAG_KERNARG) {
        hsa_region_t* ret = (hsa_region_t*) data;
        *ret = region;
        return HSA_STATUS_INFO_BREAK;
    }
    return HSA_STATUS_SUCCESS;
}

int kernarg_usage() {
    // Initialize the runtime
    hsa_init();

    // Retrieve the HSA component
    hsa_agent_t component;
    hsa_iterate_agents(get_component, &component);

    // Create a queue in the selected HSA component. The queue can hold up to 4 packets, and has no callback or service queue
    // associated with it.
    hsa_queue_t *queue;
    hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue);

    // Request a packet ID from the queue. Since no packets have been enqueued yet, the expected ID is zero.
    uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);

    // Calculate the virtual address where to place the packet.
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*) queue->base_address + write_index;

    // Setup the packet encoding the task to execute
    initialize_packet(packet);

    // Find region that serves as backing storage for the kernarg segment
    hsa_region_t region;
    hsa_agent_iterate_regions(component, get_kernarg, &region);

    // Allocate a buffer where to place the kernel arguments. The size of the buffer is normally determined by the allocation
    // requirements returned by the finalizer. We will assume that the size of the kernarg segment is 8 bytes.
    hsa_memory_allocate(region, 8, (void**) &packet->kernarg_address);

    // Place the argument the argument buffer
    hsa_signal_t* buffer = (hsa_signal_t*) packet->kernarg_address;
    assert(buffer != NULL);
    hsa_signal_t signal;
    hsa_signal_create(128, 1, &component, &signal);
    *buffer = signal;

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_t completion_signal;
    hsa_signal_create(1, 0, NULL, &completion_signal);
    packet->completion_signal = completion_signal;

    // Notify the queue that the packet is ready to be processed
    packet_type_store_release(&packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue->doorbell_signal, write_index);

    // Wait for the task to finish, which is the same as waiting for the value of the completion signal to be zero.
    while (hsa_signal_wait_acquire(completion_signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    // Done! The kernel has completed. Time to cleanup resources and leave.
    hsa_signal_destroy(completion_signal);
    hsa_signal_destroy(signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
    return 0;
}

void KERNEL_OBJECT_A() {
    printf("Component A\n");
}
void KERNEL_OBJECT_B() {
    printf("Component B\n");
}

// Find HSA agent that can process Kernel Dispatch packets.
hsa_status_t accumulate_components(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        std::vector<hsa_agent_t>* comp = (std::vector<hsa_agent_t>*)data;
        comp->push_back(agent);
    }
    return HSA_STATUS_SUCCESS;
}

hsa_agent_t* get_components() {
    std::vector<hsa_agent_t>* components = new std::vector<hsa_agent_t>();
    hsa_iterate_agents(accumulate_components, (void*) components);
    return &(components->at(0));
}

void barrier(){
    hsa_init();

    // Find available HSA components. Let's assume there are two, A and B
    hsa_agent_t* component = get_components();

    // Create queue in HSA component A and prepare a Kernel Dispatch packet
    hsa_queue_t *queue_a;
    hsa_queue_create(component[0], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue_a);
    uint64_t packet_id_a = hsa_queue_add_write_index_relaxed(queue_a, 1);

    hsa_kernel_dispatch_packet_t* packet_a =  (hsa_kernel_dispatch_packet_t*) queue_a->base_address + packet_id_a;
    initialize_packet(packet_a);
    // KERNEL_OBJECT_A is the 1st kernel object
    packet_a->kernel_object = (uint64_t) KERNEL_OBJECT_A;

    // Create a signal with a value of 1 and attach it to the first Kernel Dispatch packet
    hsa_signal_create(1, 0, NULL, &packet_a->completion_signal);

    // Tell packet processor of A to launch the first Kernel Dispatch packet
    packet_type_store_release(&packet_a->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue_a->doorbell_signal, packet_id_a);

    // Create queue in HSA component B
    hsa_queue_t *queue_b;
    hsa_queue_create(component[1], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue_b);
    uint64_t packet_id_b = hsa_queue_add_write_index_relaxed(queue_b, 2);

    // Create Barrier-AND packet that is enqueued in a queue of B
    const size_t packet_size = sizeof(hsa_kernel_dispatch_packet_t);
    hsa_barrier_and_packet_t* barrier_and_packet = (hsa_barrier_and_packet_t*)queue_b->base_address + packet_id_b;
    memset(barrier_and_packet, 0, packet_size);
    barrier_and_packet->header |= HSA_FENCE_SCOPE_COMPONENT << HSA_PACKET_HEADER_RELEASE_FENCE_SCOPE;

    // Add dependency on the first Kernel Dispatch Packet
    barrier_and_packet->dep_signal[0] = packet_a->completion_signal;
    packet_type_store_release(&barrier_and_packet->header, HSA_PACKET_TYPE_BARRIER_AND);

    // Create and enqueue a second Kernel Dispatch packet after the Barrier-AND in B. The second dispatch is launched after
    // the first has completed
    hsa_kernel_dispatch_packet_t* packet_b =  (hsa_kernel_dispatch_packet_t*) queue_b->base_address + packet_id_b;
    initialize_packet(packet_b);
    // KERNEL_OBJECT_B is the 2nd kernel object
    packet_b->kernel_object = (uint64_t) KERNEL_OBJECT_B;
    hsa_signal_create(1, 0, NULL, &packet_b->completion_signal);

    packet_type_store_release(&packet_b->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue_b->doorbell_signal, packet_id_b + 1);

    while (hsa_signal_wait_acquire(packet_b->completion_signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    hsa_signal_destroy(packet_b->completion_signal);
    hsa_queue_destroy(queue_b);
    hsa_signal_destroy(packet_a->completion_signal);
    hsa_queue_destroy(queue_a);
    hsa_shut_down();
}

// Find CPU HSA agent that can process Agent Dispatch packets.
hsa_status_t get_agent_dispatch_agent(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_device_type_t device;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_AGENT_DISPATCH) {
        hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device);
        if (device == HSA_DEVICE_TYPE_CPU) {
            hsa_agent_t* ret = (hsa_agent_t*)data;
            *ret = agent;
            return HSA_STATUS_INFO_BREAK;
        }
    }
    return HSA_STATUS_SUCCESS;
}

void process_agent_dispatch(hsa_queue_t* service_queue) {
    hsa_agent_dispatch_packet_t* packets = (hsa_agent_dispatch_packet_t*) service_queue->base_address;
    uint64_t read_index = hsa_queue_load_read_index_acquire(service_queue);
    assert(read_index == 0);
    hsa_signal_t doorbell = service_queue->doorbell_signal;

    while (read_index < 100) {

        while (hsa_signal_wait_acquire(doorbell, HSA_GTE, read_index, UINT64_MAX, HSA_WAIT_EXPECTANCY_LONG) <
            (hsa_signal_value_t) read_index);

        hsa_agent_dispatch_packet_t* packet = packets + read_index % service_queue->size;
        // Agent Dispatch packet type must be an application defined function
        assert(packet->type >= 0x8000);

        if (packet->type == 0x8000) {
            // HSA component requests memory
            void** ret = (void**) packet->return_address;
            size_t size = (size_t) packet->arg[0];
            *ret = malloc(size);
        } else {
            // Process other Agent Dispatch packet types...
        }
        if (packet->completion_signal != 0) {
            hsa_signal_subtract_release(packet->completion_signal, 1);
        }
        packet_type_store_release(&packet->header, HSA_PACKET_TYPE_INVALID);
        read_index++;
        hsa_queue_store_read_index_release(service_queue, read_index);
    }
}

// simulate an HSAIL kernel requesting N allocations from the host
void allocate(void* kernarg) {
    hsa_queue_t* service_queue = (hsa_queue_t*)kernarg;
    void* ret = NULL;

    hsa_signal_t signal; // use same signal in all packets
    hsa_signal_create(1, 0, NULL, &signal);

    for (int i = 0; i < 100; i++) {
        uint64_t write_index = hsa_queue_add_write_index_relaxed(service_queue, 1);
        hsa_agent_dispatch_packet_t* packet = (hsa_agent_dispatch_packet_t*)service_queue->base_address + (write_index % service_queue->size);
        memset(packet, 0, sizeof(*packet));

        packet->header |= HSA_FENCE_SCOPE_COMPONENT << HSA_PACKET_HEADER_ACQUIRE_FENCE_SCOPE;
        packet->header |= HSA_FENCE_SCOPE_COMPONENT << HSA_PACKET_HEADER_RELEASE_FENCE_SCOPE;

        packet->type = 0x8000;
        packet->arg[0] = i + 1;
        packet->return_address = &ret;
        hsa_signal_store_release(signal, 1);
        packet->completion_signal = signal;

        packet_type_store_release(&packet->header, HSA_PACKET_TYPE_AGENT_DISPATCH);
        hsa_signal_store_release(service_queue->doorbell_signal, write_index);

        while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN));
        // print result
        printf("%p,", ret);
    }
}

void agent_dispatch(){
    hsa_init();

    // Create service queue
    hsa_agent_t agent_dispatch_agent;
    hsa_iterate_agents(get_agent_dispatch_agent, &agent_dispatch_agent);
    hsa_queue_t *service_queue;
    hsa_queue_create(agent_dispatch_agent, 16, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &service_queue);

    // Launch thread serving Agent Dispatch packets
    std::thread* agent_dispatch_thread = new std::thread(process_agent_dispatch, service_queue);

    // Retrieve the HSA component
    hsa_agent_t component;
    hsa_iterate_agents(get_component, &component);
    hsa_queue_t* queue;
    hsa_queue_create(component, 16, HSA_QUEUE_TYPE_MULTI, callback, service_queue, &queue);

    // Dispatch kernel on HSA component that requests multiple allocation via Agent Dispatch packets
    uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*)queue->base_address + write_index;
    initialize_packet(packet);

    packet->kernel_object = (uint64_t)allocate;
    packet->kernarg_address = (void*) service_queue;

    hsa_signal_create(1, 0, NULL, &packet->completion_signal);

    // Notify the queue that the packet is ready to be processed
    packet_type_store_release(&packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue->doorbell_signal, write_index);

    // Wait for the task to finish, which is the same as waiting for the value of the
    // completion signal to be zero.
    while (hsa_signal_wait_acquire(packet->completion_signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    agent_dispatch_thread->join();
    hsa_signal_destroy(packet->completion_signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
}

int main() {
   int test = 5;
   if (test == 0) {
      KERNEL_OBJECT = (uint64_t) hello_world;
      simple_dispatch();
   } else if (test == 1) {
      KERNEL_OBJECT = (uint64_t) increment;
      multithread_dispatch();
   } else if (test == 2) {
      error_callback();
   } else if (test == 3) {
      KERNEL_OBJECT = (uint64_t) print_signal_value;
      kernarg_usage();
   } else if (test == 4) {
      barrier();
   } else if (test == 5) {
      agent_dispatch();
   }
   return 1;
}
