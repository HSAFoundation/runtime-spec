#include "assert.h"
#include "stdio.h"
#include "string.h"

#include "hsa.h"

hsa_status_t get_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        // Store component in the application-provided buffer and return
        hsa_agent_t* ret = (hsa_agent_t*) data;
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

int error_callback() {
  hsa_init();
  hsa_agent_t component;
  hsa_iterate_agents(get_component, &component);

  hsa_queue_t *queue;
  hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, callback, NULL, &queue);

  uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);
  const size_t packet_size = sizeof(hsa_kernel_dispatch_packet_t);
  uint64_t curr_address = queue->base_address + write_index * packet_size;
  hsa_kernel_dispatch_packet_t* dispatch_packet = (hsa_kernel_dispatch_packet_t*)curr_address;

  // Wrong setup: the grid dimensions have to be between 1 and 3.
  // causes HSA_STATUS_ERROR_INVALID_PACKET_FORMAT
  dispatch_packet->dimensions = 0;

  // Rest of dispatch packet is setup...

  hsa_signal_t signal;
  hsa_signal_create(1, 0, NULL, &signal);
  dispatch_packet->completion_signal = signal;
  dispatch_packet->header.type = HSA_PACKET_TYPE_KERNEL_DISPATCH;
  hsa_signal_store_release(queue->doorbell_signal, write_index);

  // Wait for the task to finish, which is the same as waiting for the value of the
  // completion signal to be zero.
  while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

  hsa_signal_destroy(signal);
  hsa_queue_destroy(queue);
  hsa_shut_down();
  return 0;
}

int main() { return error_callback(); }
