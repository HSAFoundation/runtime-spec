
  assert(HSA_STATUS_SUCCESS == hsa_signal_wait(queue_ptr->mailbox_signal, HSA_GREATER_EQUALS, 1));
  for (i = 0; i < queue->active_group_global_count; i++) {
    assert(HSA_STATUS_SUCCESS == hsa_signal_query_acquire(queue->mailbox[i].full_flag,value);
    if (value.u64_value == 1){
      for (j=0; j < queue->mailbox[i].workgroup_size; j++) {
        printf("\n workitemid=%d computeunitid=%d\n",
               queue->mailbox[i].workitem_id[j],
               queue->mailbox[i].compute_unit_id[j]);
               //here we can check PC.
      }
      hsa_signal_send(queue_ptr->mailbox[i].full_flag, 0);
    }
  }
