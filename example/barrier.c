#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "hsa.h"


typedef struct comp_vector_s {
    int count;
    hsa_agent_t* comp;
} comp_vector_t;

// Find agent that can process Dispatch packets.
hsa_status_t get_components(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_DISPATCH) {
        comp_vector_t* ret = (comp_vector_t*)data;
        ret->comp[ret->count] = agent;
        ret->count++;
    }
    return HSA_STATUS_SUCCESS;
}

hsa_agent_t* get_two_components() {
    comp_vector_t comp_vector;
    comp_vector.count = 0;
    comp_vector.comp = (hsa_agent_t*) malloc(sizeof(hsa_agent_t) * 2);
    hsa_iterate_agents(get_components, &comp_vector);
    return comp_vector.comp;
}

hsa_dispatch_packet_t* initialize_packet(uint64_t base_address, uint64_t write_index) {
    const size_t packet_size = sizeof(hsa_dispatch_packet_t);
    uint64_t curr_address = base_address + write_index * packet_size;
    hsa_dispatch_packet_t* dispatch_packet = (hsa_dispatch_packet_t*)curr_address;

    memset(dispatch_packet, 0, packet_size); // reserved fields are zeroed

    dispatch_packet->header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->dimensions = 1;
    dispatch_packet->workgroup_size_x = 256;
    dispatch_packet->grid_size_x = 256;

    hsa_signal_t signal;
    hsa_signal_create(1, 0, NULL, &signal);
    dispatch_packet->completion_signal = signal;

    return dispatch_packet;
}

void fun_a() {
    printf("Component A\n");
}
void fun_b() {
    printf("Component B\n");
}

#define KERNEL_A (fun_a)
#define KERNEL_B (fun_b)











//DO NOT MOVE THE FOLLOWING FUNCTION, it should start at line 75
int main(){
    hsa_init();

    // Find available components. Let's assume there are two, A and B
    hsa_agent_t* component = get_two_components();

    // Create queue in component A and prepare a Dispatch packet
    hsa_queue_t *queue_a;
    hsa_queue_create(component[0], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue_a);
    uint64_t packet_id_a = hsa_queue_add_write_index_relaxed(queue_a, 1);

    hsa_dispatch_packet_t* packet_a = initialize_packet(queue_a->base_address, packet_id_a);
    // KERNEL_A is the memory location of the 1st kernel object
    packet_a->kernel_object_address = (uint64_t) KERNEL_A;

    // Create a signal with a value of 1 and attach it to the first Dispatch packet
    hsa_signal_t signal;
    hsa_signal_create(1, 0, NULL, &signal);
    packet_a->completion_signal = signal;

    // Tell packet processor of A to launch the first Dispatch packet
    packet_a->header.type = HSA_PACKET_TYPE_DISPATCH;
    hsa_signal_store_release(queue_a->doorbell_signal, packet_id_a);

    // Create queue in component B
    hsa_queue_t *queue_b;
    hsa_queue_create(component[1], 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue_b);
    uint64_t packet_id_b = hsa_queue_add_write_index_relaxed(queue_b, 2);

    // Create Barrier packet that is enqueued in a queue of B
    const size_t packet_size = sizeof(hsa_dispatch_packet_t);
    uint64_t curr_address = queue_b->base_address + packet_id_b * packet_size;
    hsa_barrier_packet_t* barrier_packet = (hsa_barrier_packet_t*)curr_address;
    memset(barrier_packet, 0, packet_size);
    barrier_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;

    // Add dependency on the first Dispatch Packet
    barrier_packet->dep_signal[0] = signal;
    barrier_packet->header.type = HSA_PACKET_TYPE_BARRIER;

    // Create and enqueue a second Dispatch packet after the Barrier in B. The second dispatch is launched after the first
    // has completed
    hsa_dispatch_packet_t* packet_b = initialize_packet(queue_b->base_address, packet_id_b + 1);
    // KERNEL_B is the memory location of the 2nd kernel object
    packet_b->kernel_object_address = (uint64_t) KERNEL_B;
    packet_b->header.type = HSA_PACKET_TYPE_DISPATCH;

    hsa_signal_store_release(queue_b->doorbell_signal, packet_id_b + 1);

    while (hsa_signal_wait_acquire(packet_b->completion_signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    hsa_signal_destroy(signal);
    hsa_queue_destroy(queue_b);
    hsa_queue_destroy(queue_a);
    hsa_shut_down();
}
