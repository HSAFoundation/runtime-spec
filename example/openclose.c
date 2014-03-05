  /* The runtime is opened for the first time */
  status = hsa_open(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  /* now the context has been acquired, reference count associated with the
   * context incremented internally by the runtime*/
  hsa_context_acquire(&runtime_context);

  /* this close is just going to generate an asynchronous error and return the
   * success status */
  hsa_close(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  /* this queue create will still succeed provided component is valid and
   * runtime can allocate required resources*/
  status = hsa_queue_create(component, 1024, 0, NONE, runtime_context, &queue, NULL);
  if(status != HSA_STATUS_SUCCESS) {
    assert(HSA_STATUS_SUCCESS == hsa_close()); 
    exit(1);
  }

  /*
   * ....
   * .... 
   * ....
   */

  /* this API will succeed, the context is now as it was after the hsa_open */
  hsa_context_release(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  /* this close will attempt to release all runtime allocated releases,
   * components, topology table, queues, etc. are no longer valid */
  hsa_close(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  /* this queue create will fail*/
  status = hsa_queue_create(component, 1024, 0, NONE, runtime_context, &queue1, NULL);
  if(status != HSA_STATUS_SUCCESS) {
    exit(1);
  }


