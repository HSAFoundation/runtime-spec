#include "assert.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "hsa.h"


#define DEADBEEF (hello_world)

void hello_world(uint64_t args) {
  hsa_signal_t signal  = *((hsa_signal_t*) args);
  printf("%ld - Hello World!\n", hsa_signal_load_acquire(signal));
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






// DO NOT MOVE THE FOLLOWING FUNCTION - it should start at line 50
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







int main() {
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
    const size_t packet_size = sizeof(hsa_kernel_dispatch_packet_t);
    uint64_t curr_address = queue->base_address + write_index * packet_size;
    hsa_kernel_dispatch_packet_t* dispatch_packet = (hsa_kernel_dispatch_packet_t*) curr_address;

    // Setup the packet encoding the task to execute
    memset(dispatch_packet, 0, packet_size); // reserved fields are zeroed
    dispatch_packet->header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->dimensions = 1;
    dispatch_packet->workgroup_size_x = 256;
    dispatch_packet->grid_size_x = 256;

    // Indicate which ISA to run. The application is expected to have finalized a kernel (for example, using the finalization API).
    // We will assume the object is located at address 0xDEADBEEF
    dispatch_packet->kernel_object_address = (uint64_t) DEADBEEF;

    // Find region that serves as backing storage for the kernarg segment
    hsa_region_t region;
    hsa_agent_iterate_regions(component, get_kernarg, &region);

    // Allocate a buffer where to place the kernel arguments. The size of the buffer is normally determined by the allocation
    // requirements returned by the finalizer. We will assume that the size of the kernarg segment is 8 bytes.
    hsa_memory_allocate(region, 8, (void**) &dispatch_packet->kernarg_address);

    // Place the argument the argument buffer
    hsa_signal_t* buffer = (hsa_signal_t*) dispatch_packet->kernarg_address;
    assert(buffer != NULL);
    hsa_signal_t signal;
    hsa_signal_create(1, 1, &component, &signal);
    *buffer = signal;

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_t completion_signal;
    hsa_signal_create(1, 0, NULL, &completion_signal);
    dispatch_packet->completion_signal = completion_signal;

    // Notify the queue that the packet is ready to be processed
    dispatch_packet->header.type = HSA_PACKET_TYPE_KERNEL_DISPATCH;
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
