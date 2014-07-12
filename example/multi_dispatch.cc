#include "assert.h"
#include "stdio.h"
#include "string.h" // memset

#include <atomic>
#include <thread>
#include <vector>

#include "hsa.h"

std::atomic<int>* counter;
#define DEADBEEF (increment)

void increment(void* kernarg) {
    std::atomic<int>* counter = (std::atomic<int>*) kernarg;
    counter->fetch_add(1, std::memory_order_release);
}

void initialize_packet(hsa_dispatch_packet_t* dispatch_packet) {
    memset(dispatch_packet, 0, sizeof(hsa_dispatch_packet_t));
    dispatch_packet->header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->dimensions = 1;
    dispatch_packet->workgroup_size_x = 1;
    dispatch_packet->grid_size_x = 1;
    dispatch_packet->kernel_object_address = (uint64_t)DEADBEEF;
    dispatch_packet->kernarg_address = (uint64_t) counter;
}
// DO NOT MOVE THE FUNCTION BELOW - it should start at line 30
hsa_status_t get_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_DISPATCH) {
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





// DO NOT MOVE THE FUNCTION BELOW - it should start at line 50
void enqueue(hsa_queue_t* queue) {
    // Create a signal with an initial value of 1000 to monitor the overall task completion
    hsa_signal_t signal;
    hsa_signal_create(1000, 0, NULL, &signal);
    hsa_dispatch_packet_t* packets = (hsa_dispatch_packet_t*)queue->base_address;

    for (int i = 0; i < 1000; i++) {
        // Atomically request a new packet ID.
        uint64_t packet_id = hsa_queue_add_write_index_release(queue, 1);

        // Wait until the queue is not full before writing the packet
        while (packet_id - hsa_queue_load_read_index_acquire(queue) >= queue->size);

        // Compute packet offset, considering wrap-around
        hsa_dispatch_packet_t* dispatch_packet = packets + packet_id % queue->size;

        initialize_packet(dispatch_packet);
        dispatch_packet->completion_signal = signal;
        dispatch_packet->header.type = HSA_PACKET_TYPE_DISPATCH;
        hsa_signal_store_release(queue->doorbell_signal, packet_id);
    }

    // Wait until all the kernels are complete
    while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);
    hsa_signal_destroy(signal);
}


int main() {
    hsa_init();
    counter = new std::atomic<int>(0);
    hsa_agent_t component;
    hsa_iterate_agents(get_component, &component);
    hsa_queue_t *queue;
    // Create a queue in the selected component.
    hsa_queue_create(component, 4, HSA_QUEUE_TYPE_MULTI, NULL, NULL, &queue);

    std::vector<std::thread*> threads;
    const int kNumThreads = 4;
    for (int i = 0; i < kNumThreads; i++) {
        std::thread* thread = new std::thread(enqueue, queue);
        threads.push_back(thread);
    }

    for (int i = 0; i < threads.size(); i++) {
        threads.at(i)->join();
    }

    printf("Result: %d\n", counter->load());

    hsa_queue_destroy(queue);
    hsa_shut_down();
    return 0;
}
