int main(int argc, char **argv)
{
  unsigned int count;
  hsa_brig_direcve_offset_t code_directive_offset = atoi(argv[2]);
  hsa_control_directives_t control = atoi(argv[3]);
  uint64_t kernel_input = (uint64_t)atoi(argv[4]);
  const hsa_agent_t *component = NULL;
  static hsa_aql_dispatch_packet_t dispatch_packet;
  hsa_runtime_context_t *runtime_context;
  hsa_status_t status;
  hsa_queue_t *queue;
  hsa_brig_t *brig = (hsa_brig_t *)argv[1];
  hsa_code_object_t *code_obj;
  hsa_symbol_map_t *symbol_map;
  hsa_debug_info_t *debug_info;
  hsa_signal_handle_t signal;
  hsa_signal_value_t sigval;
  hsa_topology_header_t *header;
  hsa_platform_t *platform;

  uint64_t index;

  /**** this part is the setup for a simple dispatch ****/
  status = hsa_open(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  hsa_context_acquire(&runtime_context);

  hsa_close(&runtime_context);

  status = hsa_topology_table_create(&header);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  platform = (hsa_platform_t *)(header->topology_table_base);

  component = (hsa_agent_t *)((char *)(header->topology_table_base) + platform->agent_offset_list[0]);

  status = hsa_queue_create(component, 1024, 0, NONE, runtime_context, &queue, NULL);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  /**** this is the compilation part where the brig is finalized ****/
  status = hsa_finalize_brig(component, brig, 1, &code_directive_offset, &control, NULL, &code_obj, &debug_info, &symbol_map);
  if(status != HSA_STATUS_SUCCESS || symbol_map != NULL) {
    assert(HSA_STATUS_SUCCESS == hsa_queue_destroy(queue));
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  /**** a signal is created for completion detection ****/
  sigval.value64 = 0;
  status = hsa_signal_create(sigval, &signal, runtime_context);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_queue_destroy(queue));
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  /**** the AQL packet is setup here for the simple kernel ****/
  dispatch_packet.header.format = 2;
  dispatch_packet.header.barrier = 1;
  dispatch_packet.header.acquire_fence_scope = 2;
  dispatch_packet.header.release_fence_scope = 2;
  dispatch_packet.header.dimensions = 1;
  dispatch_packet.workgroup_size_x = 256;
  dispatch_packet.grid_size_x = 256;
  dispatch_packet.kernel_object_address = (uint64_t)code_obj;
  dispatch_packet.kernarg_address = (uint64_t)(&kernel_input);
  dispatch_packet.completion_signal = signal;

  memcpy(queue->base_address, (void *)&dispatch_packet, sizeof(dispatch_packet));

  /**** packet processor is informed that a packet is on the queue ****/
  index = hsa_queue_set_write_index(queue, 1);
  if(index != 0){
    assert(HSA_STATUS_SUCCESS == hsa_queue_destroy(queue));
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }
  sigval.value64 = 1;
  status = hsa_signal_send_release(queue->doorbell_signal, sigval);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_queue_destroy(queue));
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  /**** await completion ****/
  do {
    status = hsa_signal_wait_acquire(signal,HSA_EQUALS, sigval, &sigval);
  } while(status == HSA_STATUS_INFO_SIGNAL_TIMEOUT);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_queue_destroy(queue));
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }

  printf("\nkernel successfully executed, value %llu\n", kernel_input);

  /**** close up and destroy queue, close the runtime ****/
  status = hsa_queue_destroy(queue);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_close());
    exit(1);
  }
  status = hsa_close();
  assert(status == HSA_STATUS_SUCCESS);
  return 0;
}
