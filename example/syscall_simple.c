
      assert (HSA_STATUS_SUCCESS ==
          hsa_signal_wait(queue_ptr->mailbox_signal, HSA_GREATER_EQUALS, 1));
      for(i = 0; i < queue->active_group_global_count; i++) {
        if(HSA_STATUS_SUCCESS ==
            hsa_signal_poll(queue->mailbox[i].full_flag,HSA_EQUALS, 1)) 
          for(j=0; j < queue->mailbox[i].workgroup_size; j++) { 
            process_syscall_args(queue->mailbox[i].args, queue->mailbox[i].workgroup_size);
          }
      }
