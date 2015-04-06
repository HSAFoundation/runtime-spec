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


// used in the specification as an example, never invoked as such

#define HSA_EXTENSION_HAL_FOO 23

typedef struct hsa_hal_foo_pfn_s {
hsa_status_t (*hsa_hal_foo_pfn_do_something)();
} hsa_hal_foo_pfn_t;

void extension_example(hsa_agent_t agent) {
bool system_support, agent_support;
hsa_system_extension_supported(HSA_EXTENSION_HAL_FOO, 1, 0, &system_support);
hsa_agent_extension_supported(HSA_EXTENSION_HAL_FOO, agent, 1, 0, &agent_support);
if (system_support && agent_support) {
    hsa_hal_foo_pfn_t pfns;
    hsa_system_get_extension_table(HSA_EXTENSION_HAL_FOO, 1, 0, &pfns);
    pfns.hsa_hal_foo_pfn_do_something();
}
}

void initialize_packet(hsa_kernel_dispatch_packet_t* packet) {
    // Reserved fields, private and group memory, and completion signal are all set to 0.
    memset(((uint8_t*) packet) + 4, 0, sizeof(hsa_kernel_dispatch_packet_t) - 4);

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

// Find HSA agent that can process kernel dispatch packets.
hsa_status_t get_kernel_agent(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        // Store kernel agent in the application-provided buffer and return
        hsa_agent_t* ret = (hsa_agent_t*) data;
        *ret = agent;
        return HSA_STATUS_INFO_BREAK;
    }
    // Keep iterating
    return HSA_STATUS_SUCCESS;
}

uint16_t header(hsa_packet_type_t type) {
    uint16_t header = type << HSA_PACKET_HEADER_TYPE;
    header |= HSA_FENCE_SCOPE_SYSTEM << HSA_PACKET_HEADER_SCACQUIRE_FENCE_SCOPE;
    header |= HSA_FENCE_SCOPE_SYSTEM << HSA_PACKET_HEADER_SCRELEASE_FENCE_SCOPE;
    return header;
}

uint16_t kernel_dispatch_setup() {
    return 1 << HSA_KERNEL_DISPATCH_PACKET_SETUP_DIMENSIONS;
}


void packet_store_release(uint32_t* packet, uint16_t header, uint16_t rest) {
    __atomic_store_n(packet, header | (rest << 16), __ATOMIC_RELEASE);
}

// used for document examples only
void set_header_and_setup(uint32_t* packet) {
uint16_t hdr = header(HSA_PACKET_TYPE_KERNEL_DISPATCH);
uint16_t setup = kernel_dispatch_setup();
__atomic_store_n(packet, hdr | (setup << 16), __ATOMIC_RELEASE);
}

void hello_world() {
	printf("Hello World!\n");
}

void simple_dispatch() {
    // Initialize the runtime
    hsa_init();

    // Retrieve the kernel agent
    hsa_agent_t kernel_agent;
    hsa_iterate_agents(get_kernel_agent, &kernel_agent);

    // Create a queue in the kernel agent. The queue can hold 4 packets, and has no callback associated with it
    hsa_queue_t *queue;
    hsa_queue_create(kernel_agent, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, 0, 0, &queue);

    // Since no packets have been enqueued yet, we use zero as the packet ID and bump the write index accordingly
    hsa_queue_store_write_index_relaxed(queue, 1);
    uint64_t packet_id = 0;

    // Calculate the virtual address where to place the packet
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*) queue->base_address + packet_id;

    // Populate fields in kernel dispatch packet, except for the header, the setup, and the completion signal fields
    initialize_packet(packet);

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_create(1, 0, NULL, &packet->completion_signal);

    // Notify the queue that the packet is ready to be processed
    packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
    hsa_signal_store_screlease(queue->doorbell_signal, packet_id);

    // Wait for the task to finish, which is the same as waiting for the value of the completion signal to be zero
    while (hsa_signal_wait_scacquire(packet->completion_signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);

    // Done! The kernel has completed. Time to cleanup resources and leave
    hsa_signal_destroy(packet->completion_signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
}

void increment(void* kernarg) {
    std::atomic<int>* counter = (std::atomic<int>*) kernarg;
    counter->fetch_add(1, std::memory_order_release);
}

hsa_status_t get_multi_kernel_agent(hsa_agent_t agent, void* data) {
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
        uint64_t packet_id = hsa_queue_add_write_index_screlease(queue, 1);

        // Wait until the queue is not full before writing the packet
        while (packet_id - hsa_queue_load_read_index_scacquire(queue) >= queue->size);

        // Compute packet offset, considering wrap-around
        hsa_kernel_dispatch_packet_t* packet = packets + packet_id % queue->size;

        initialize_packet(packet);
        packet->kernarg_address = counter;
        packet->completion_signal = signal;
        packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
        hsa_signal_store_screlease(queue->doorbell_signal, packet_id);
    }

    // Wait until all the kernels are complete
    while (hsa_signal_wait_scacquire(signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);
    hsa_signal_destroy(signal);
}

void multithread_dispatch() {
    hsa_init();
    counter = new std::atomic<int>(0);
    hsa_agent_t kernel_agent;
    hsa_iterate_agents(get_multi_kernel_agent, &kernel_agent);
    hsa_queue_t *queue;
    // Create a queue in the selected agent.
    hsa_queue_create(kernel_agent, 4, HSA_QUEUE_TYPE_MULTI, NULL, NULL, 0, 0, &queue);

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

void callback(hsa_status_t status, hsa_queue_t* queue, void* data) {
  const char* message;
  hsa_status_string(status, &message);
  printf("Error at queue %" PRIu64 ": %s", queue->id, message);
}

void error_callback() {
  hsa_init();
  hsa_agent_t kernel_agent;
  hsa_iterate_agents(get_kernel_agent, &kernel_agent);

  hsa_queue_t *queue;
  hsa_queue_create(kernel_agent, 4, HSA_QUEUE_TYPE_SINGLE, callback, NULL, 0, 0, &queue);

  uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);
  hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*)queue->base_address + write_index;

  // Rest of dispatch packet is setup...

  hsa_signal_t signal;
  hsa_signal_create(1, 0, NULL, &signal);
  packet->completion_signal = signal;
  // Wrong setup: the grid dimensions have to be between 1 and 3. Causes HSA_STATUS_ERROR_INVALID_PACKET_FORMAT
  packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), 0);
  hsa_signal_store_screlease(queue->doorbell_signal, write_index);

  // Wait for the task to finish, which is the same as waiting for the value of the
  // completion signal to be zero.
  while (hsa_signal_wait_scacquire(signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);

  hsa_signal_destroy(signal);
  hsa_queue_destroy(queue);
  hsa_shut_down();
}

void print_signal_value(uint64_t args) {
  hsa_signal_t signal  = *((hsa_signal_t*) args);
  printf("Signal value: %ld\n", hsa_signal_load_scacquire(signal));
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
    hsa_region_segment_t segment;
    hsa_region_get_info(region, HSA_REGION_INFO_SEGMENT, &segment);
    if (segment != HSA_REGION_SEGMENT_GLOBAL) {
        return HSA_STATUS_SUCCESS;
    }
    hsa_region_global_flag_t flags;
    hsa_region_get_info(region, HSA_REGION_INFO_GLOBAL_FLAGS, &flags);
    if (flags & HSA_REGION_GLOBAL_FLAG_KERNARG) {
        hsa_region_t* ret = (hsa_region_t*) data;
        *ret = region;
        return HSA_STATUS_INFO_BREAK;
    }
    return HSA_STATUS_SUCCESS;
}

void populate_kernarg(hsa_agent_t kernel_agent, hsa_kernel_dispatch_packet_t* packet) {
hsa_region_t region;
hsa_agent_iterate_regions(kernel_agent, get_kernarg, &region);

// Allocate a buffer where to place the kernel arguments.
hsa_memory_allocate(region, sizeof(hsa_signal_t), (void**) &packet->kernarg_address);

// Place the signal the argument buffer
hsa_signal_t* buffer = (hsa_signal_t*) packet->kernarg_address;
assert(buffer != NULL);
hsa_signal_t signal;
hsa_signal_create(128, 1, &kernel_agent, &signal);
*buffer = signal;
}

int kernarg_usage() {
    // Initialize the runtime
    hsa_init();

    // Retrieve the kernel agent
    hsa_agent_t kernel_agent;
    hsa_iterate_agents(get_kernel_agent, &kernel_agent);

    // Create a queue in the selected kernel agent. The queue can hold up to 4 packets associated with it.
    hsa_queue_t *queue;
    hsa_queue_create(kernel_agent, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, 0, 0, &queue);

    // Request a packet ID from the queue. Since no packets have been enqueued yet, the expected ID is zero.
    uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);

    // Calculate the virtual address where to place the packet.
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*) queue->base_address + write_index;

    // Setup the packet encoding the task to execute
    initialize_packet(packet);

    // Find region that serves as backing storage for the kernarg segment
    populate_kernarg(kernel_agent, packet);

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_t completion_signal;
    hsa_signal_create(1, 0, NULL, &completion_signal);
    packet->completion_signal = completion_signal;

    // Notify the queue that the packet is ready to be processed
    packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
    hsa_signal_store_screlease(queue->doorbell_signal, write_index);

    // Wait for the task to finish, which is the same as waiting for the value of the completion signal to be zero.
    while (hsa_signal_wait_scacquire(completion_signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);

    // Done! The kernel has completed. Time to cleanup resources and leave.
    hsa_signal_destroy(completion_signal);
    //hsa_signal_destroy(signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
    return 0;
}

void KERNEL_OBJECT_A() {
    printf("Kernel agent A\n");
}
void KERNEL_OBJECT_B() {
    printf("Kernel agent B\n");
}

// Find HSA agent that can process kernel dispatch packets.
hsa_status_t accumulate_kernel_agents(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        std::vector<hsa_agent_t>* comp = (std::vector<hsa_agent_t>*)data;
        comp->push_back(agent);
    }
    return HSA_STATUS_SUCCESS;
}

hsa_agent_t* get_kernel_agents() {
    std::vector<hsa_agent_t>* kernel_agents = new std::vector<hsa_agent_t>();
    hsa_iterate_agents(accumulate_kernel_agents, (void*) kernel_agents);
    return &(kernel_agents->at(0));
}

void barrier(){
    hsa_init();

    // Find available kernel agents. Let's assume there are two, A and B
    hsa_agent_t* kernel_agent = get_kernel_agents();

    // Create queue in kernel agent A and prepare a kernel dispatch packet
    hsa_queue_t *queue_a;
    hsa_queue_create(kernel_agent[0], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, 0, 0, &queue_a);
    uint64_t packet_id_a = hsa_queue_add_write_index_relaxed(queue_a, 1);

    hsa_kernel_dispatch_packet_t* packet_a =  (hsa_kernel_dispatch_packet_t*) queue_a->base_address + packet_id_a;
    initialize_packet(packet_a);
    // KERNEL_OBJECT_A is the 1st kernel object
    packet_a->kernel_object = (uint64_t) KERNEL_OBJECT_A;

    // Create a signal with a value of 1 and attach it to the first kernel dispatch packet
    hsa_signal_create(1, 0, NULL, &packet_a->completion_signal);

    // Tell packet processor of A to launch the first kernel dispatch packet
    packet_store_release((uint32_t*) packet_a, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
    hsa_signal_store_screlease(queue_a->doorbell_signal, packet_id_a);

    // Create queue in kernel agent B
    hsa_queue_t *queue_b;
    hsa_queue_create(kernel_agent[1], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, 0, 0, &queue_b);
    uint64_t packet_id_b = hsa_queue_add_write_index_relaxed(queue_b, 2);

    // Create barrier-AND packet that is enqueued in a queue of B
    hsa_barrier_and_packet_t* barrier_and_packet = (hsa_barrier_and_packet_t*)queue_b->base_address + packet_id_b;
    memset(((uint8_t*) barrier_and_packet) + 4, 0, sizeof(*barrier_and_packet) - 4);

    // Add dependency on the first kernel dispatch packet
    barrier_and_packet->dep_signal[0] = packet_a->completion_signal;
    packet_store_release((uint32_t*) barrier_and_packet, header(HSA_PACKET_TYPE_BARRIER_AND), 0);

    // Create and enqueue a second kernel dispatch packet after the barrier-AND in B. The second dispatch is launched after
    // the first has completed
    hsa_kernel_dispatch_packet_t* packet_b =  (hsa_kernel_dispatch_packet_t*) queue_b->base_address + packet_id_b;
    initialize_packet(packet_b);
    // KERNEL_OBJECT_B is the 2nd kernel object
    packet_b->kernel_object = (uint64_t) KERNEL_OBJECT_B;
    hsa_signal_create(1, 0, NULL, &packet_b->completion_signal);

    packet_store_release((uint32_t*) packet_b, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
    hsa_signal_store_screlease(queue_b->doorbell_signal, packet_id_b + 1);

    while (hsa_signal_wait_scacquire(packet_b->completion_signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);

    hsa_signal_destroy(packet_b->completion_signal);
    hsa_queue_destroy(queue_b);
    hsa_signal_destroy(packet_a->completion_signal);
    hsa_queue_destroy(queue_a);
    hsa_shut_down();
}

// Find CPU agent.
hsa_status_t get_cpu_agent(hsa_agent_t agent, void* data) {
    hsa_device_type_t device;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device);
    if (device == HSA_DEVICE_TYPE_CPU) {
      hsa_agent_t* ret = (hsa_agent_t*)data;
      *ret = agent;
      return HSA_STATUS_INFO_BREAK;
    }
    return HSA_STATUS_SUCCESS;
}

void process_agent_dispatch(hsa_queue_t* queue) {
    hsa_agent_dispatch_packet_t* packets = (hsa_agent_dispatch_packet_t*) queue->base_address;
    uint64_t read_index = hsa_queue_load_read_index_scacquire(queue);
    assert(read_index == 0);
    hsa_signal_t doorbell = queue->doorbell_signal;

    while (read_index < 100) {

        while (hsa_signal_wait_scacquire(doorbell, HSA_SIGNAL_CONDITION_GTE, read_index, UINT64_MAX, HSA_WAIT_STATE_BLOCKED) <
            (hsa_signal_value_t) read_index);

        hsa_agent_dispatch_packet_t* packet = packets + read_index % queue->size;

        if (packet->type == 0x8000) {
            // kernel agent requests memory
            void** ret = (void**) packet->return_address;
            size_t size = (size_t) packet->arg[0];
            *ret = malloc(size);
        } else {
            // Process other agent dispatch packet types...
        }
        if (packet->completion_signal.handle != 0) {
            hsa_signal_subtract_screlease(packet->completion_signal, 1);
        }
        packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_INVALID), packet->type);
        read_index++;
        hsa_queue_store_read_index_screlease(queue, read_index);
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
        memset((uint8_t*) packet + 4, 0, sizeof(*packet) - 4);

        packet->arg[0] = i + 1;
        packet->return_address = &ret;
        hsa_signal_store_screlease(signal, 1);
        packet->completion_signal = signal;

        packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_AGENT_DISPATCH), 0x8000);
        hsa_signal_store_screlease(service_queue->doorbell_signal, write_index);

        while (hsa_signal_wait_scacquire(signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE));
        // print result
        printf("%p,", ret);
    }
}

hsa_status_t get_fine_grained_region(hsa_region_t region, void* data) {
    hsa_region_segment_t segment;
    hsa_region_get_info(region, HSA_REGION_INFO_SEGMENT, &segment);
    if (segment != HSA_REGION_SEGMENT_GLOBAL) {
        return HSA_STATUS_SUCCESS;
    }
    hsa_region_global_flag_t flags;
    hsa_region_get_info(region, HSA_REGION_INFO_GLOBAL_FLAGS, &flags);
    if (flags & HSA_REGION_GLOBAL_FLAG_FINE_GRAINED) {
        hsa_region_t* ret = (hsa_region_t*) data;
        *ret = region;
        return HSA_STATUS_INFO_BREAK;
    }
    return HSA_STATUS_SUCCESS;
}

// THIS FUNCTION IS NOT USED IN THE CODE BUT FROM LATEX
void create_soft_queue() {
hsa_agent_t cpu_agent;
hsa_iterate_agents(get_cpu_agent, &cpu_agent);

hsa_region_t region;
hsa_agent_iterate_regions(cpu_agent, get_fine_grained_region, &region);

hsa_signal_t completion_signal;
hsa_signal_create(1, 0, NULL, &completion_signal);

hsa_queue_t *soft_queue;
hsa_soft_queue_create(region, 16, HSA_QUEUE_TYPE_MULTI, HSA_QUEUE_FEATURE_AGENT_DISPATCH, completion_signal, &soft_queue);
}

void agent_dispatch(){
    hsa_init();

    // Create service queue
    hsa_agent_t agent_dispatch_agent;
    hsa_iterate_agents(get_cpu_agent, &agent_dispatch_agent);
    hsa_queue_t *service_queue;
    hsa_queue_create(agent_dispatch_agent, 16, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, 0, 0, &service_queue);

    // Launch thread serving agent dispatch packets
    std::thread* agent_dispatch_thread = new std::thread(process_agent_dispatch, service_queue);

    // Retrieve the kernel agent
    hsa_agent_t kernel_agent;
    hsa_iterate_agents(get_kernel_agent, &kernel_agent);
    hsa_queue_t* queue;
    hsa_queue_create(kernel_agent, 16, HSA_QUEUE_TYPE_MULTI, callback, service_queue, 0, 0,  &queue);

    // Dispatch kernel on kernel agent that requests multiple allocation via agent dispatch packets
    uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);
    hsa_kernel_dispatch_packet_t* packet = (hsa_kernel_dispatch_packet_t*)queue->base_address + write_index;
    initialize_packet(packet);

    packet->kernel_object = (uint64_t)allocate;
    packet->kernarg_address = (void*) service_queue;

    hsa_signal_create(1, 0, NULL, &packet->completion_signal);

    // Notify the queue that the packet is ready to be processed
    packet_store_release((uint32_t*) packet, header(HSA_PACKET_TYPE_KERNEL_DISPATCH), kernel_dispatch_setup());
    hsa_signal_store_screlease(queue->doorbell_signal, write_index);

    // Wait for the task to finish, which is the same as waiting for the value of the
    // completion signal to be zero.
    while (hsa_signal_wait_scacquire(packet->completion_signal, HSA_SIGNAL_CONDITION_EQ, 0, UINT64_MAX, HSA_WAIT_STATE_ACTIVE) != 0);

    agent_dispatch_thread->join();
    hsa_signal_destroy(packet->completion_signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
}

int main (int argc, char *argv[]) {
  int test = 0;
  if (argc == 1) {
     printf("No test passed as argument, defaulting to 0\n");
  } else {
    test = atoi(argv[1]);
  }
   if (test == 0) {
     printf("Test: Simple Dispatch\n");
     KERNEL_OBJECT = (uint64_t) hello_world;
     simple_dispatch();
   } else if (test == 1) {
     printf("Test: Multithreaded Dispatch\n");
     KERNEL_OBJECT = (uint64_t) increment;
     multithread_dispatch();
   } else if (test == 2) {
     printf("Test: Queue error callback\n");
     error_callback();
   } else if (test == 3) {
     printf("Test: Kernarg usage\n");
     KERNEL_OBJECT = (uint64_t) print_signal_value;
     kernarg_usage();
   } else if (test == 4) {
     printf("Test: Barrier packet\n");
     barrier();
   } else if (test == 5) {
     printf("Test: Agent dispatch\n");
     agent_dispatch();
   }
   return 1;
}
