#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <thread>

#include "hsa.h"

const int kNum = 20;

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

// Find HSA agent that can process Kernel Dispatch packets.
hsa_status_t get_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        // Store HSA component in user-provided buffer and return
        hsa_agent_t* ret = (hsa_agent_t*)data;
        *ret = agent;
        return HSA_STATUS_INFO_BREAK;
    }
    // Keep iterating
    return HSA_STATUS_SUCCESS;
}
void callback(hsa_status_t status, hsa_queue_t* queue) {
  const char* message;
  hsa_status_string(status, &message);
  printf("Error at queue %d: %s", queue->id, message);
}

#define true ((read_index < kNum))

// DO NOT MOVE THE FOLLOWING FUNCTION - it should start at line 50
void process_agent_dispatch(hsa_queue_t* service_queue) {
    hsa_agent_dispatch_packet_t* packets = (hsa_agent_dispatch_packet_t*) service_queue->base_address;
    uint64_t read_index = hsa_queue_load_read_index_acquire(service_queue);
    assert(read_index == 0);
    hsa_signal_t doorbell = service_queue->doorbell_signal;

    while (true) {

        while (hsa_signal_wait_acquire(doorbell, HSA_GTE, read_index, UINT64_MAX, HSA_WAIT_EXPECTANCY_LONG) <
            (hsa_signal_value_t) read_index);

        hsa_agent_dispatch_packet_t* packet = packets + read_index % service_queue->size;
        // Agent Dispatch packet type must be an application defined function
        assert(packet->header.type == HSA_PACKET_TYPE_AGENT_DISPATCH);
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
        packet->header.type = HSA_PACKET_TYPE_INVALID;
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

    for (int i = 0; i < kNum; i++) {
        uint64_t write_index = hsa_queue_add_write_index_relaxed(service_queue, 1);
        const size_t packet_size = sizeof(hsa_agent_dispatch_packet_t);
        uint64_t curr_address = service_queue->base_address + (write_index * packet_size) % (service_queue->size * packet_size);
        hsa_agent_dispatch_packet_t* agent_dispatch_packet = (hsa_agent_dispatch_packet_t*)curr_address;
        memset(agent_dispatch_packet, 0, sizeof(hsa_packet_header_t));

        agent_dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
        agent_dispatch_packet->type = 0x8000;
        agent_dispatch_packet->arg[0] = i + 1;
        agent_dispatch_packet->return_address = (uint64_t)&ret;
        hsa_signal_store_release(signal, 1);
        agent_dispatch_packet->completion_signal = signal;

        agent_dispatch_packet->header.type = HSA_PACKET_TYPE_AGENT_DISPATCH;
        hsa_signal_store_release(service_queue->doorbell_signal, write_index);

        while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN));
        // print result
        printf("%p,", ret);
    }
}


int main(){
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
    const size_t packet_size = sizeof(hsa_kernel_dispatch_packet_t);
    uint64_t curr_address = queue->base_address + write_index * packet_size;
    hsa_kernel_dispatch_packet_t* dispatch_packet = (hsa_kernel_dispatch_packet_t*)curr_address;
    memset(dispatch_packet, 0, packet_size); // reserved fields are zeroed
    dispatch_packet->header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->dimensions = 1;
    dispatch_packet->workgroup_size_x = 256;
    dispatch_packet->grid_size_x = 256;

    dispatch_packet->kernel_object_address = (uint64_t)allocate;
    dispatch_packet->kernarg_address = (uint64_t) service_queue;

    hsa_signal_t signal;
    hsa_signal_create(1, 0, NULL, &signal);
    dispatch_packet->completion_signal = signal;

    // Notify the queue that the packet is ready to be processed
    dispatch_packet->header.type = HSA_PACKET_TYPE_KERNEL_DISPATCH;
    hsa_signal_store_release(queue->doorbell_signal, write_index);

    // Wait for the task to finish, which is the same as waiting for the value of the
    // completion signal to be zero.
    while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    agent_dispatch_thread->join();
    hsa_signal_destroy(signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
    return 1;
}
