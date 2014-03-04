
void error_callabck(hsa_async_error_info_t *error)
{
  printf("An error has occured at: %" PRIu64 "\n", error->timestamp);
  printf("%s \n",error->string_info);
  if(error->error_type == HSA_STATUS_ERROR_QUEUE_WRITE_INDEX_UPDATE_ERROR || HSA_STATUS_QUEUE_FULL){
    printf("Queue update/full error\n");
  }
}


{
  /* The runtime is opened for the first time */
  status = hsa_open(&runtime_context);
  assert(status == HSA_STATUS_SUCCESS);

  /* error_callback is of type void error_callback(hsa_async_error_info_t
   * *info)
   */ 
  hsa_error_callback_register(&error_callback, NULL, &runtime_context);

  /* this queue create will succeed provided component is valid and
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
}


