#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<string.h>
#include "corebase.h"
/**
 * @brief
 * 
 * @param input_status
 *      is input argument. Any unsuccessful API return status that the user is seeking more
 *      information on.
 * @param status_info
 *      user allocated, output. In addition to the string. This value could be
 *      0 and in itself (without status\_info\_string) may not be independently
 *      interpretable by the user.
 * @param status_info_string
 *      output from the API, a ISO/IEC 646 encoded english language string that
 *      potentially describes the error status.
 */
hsa_status_t hsa_status_query_description(hsa_status_t input_status, uint64_t
    *status_info, char * const * status_info_string)
{
    return HSA_STATUS_SUCCESS;
}  



/**
 * @brief
 *
 * @param default_message_queue
 *      runtime allocated, output. The handle to the default error message queue
 *      that the runtime uses to report all asynchronous errors. The
 *      first initialization of the runtime creates the default error
 *      message queue, for subsequent initializations, when the return status
 *      is HSA\_INFO\_ALREADY\_INITIALIZED, handle from the first allocation is returned.
 *
 *
 */
hsa_status_t hsa_open(void (*notification_callback)(const hsa_notification_info_t *info), 
    void *notification_user_data, 
    void (*error_callback)(const hsa_async_error_info_t *info), 
    void *async_error_user_data, hsa_runtime_context_t **context)
{
    return HSA_STATUS_SUCCESS;
} 

hsa_status_t hsa_close()
{
    return HSA_STATUS_SUCCESS;
}
/**
 * @brief
 *
 */
hsa_status_t hsa_context_acquire(hsa_runtime_context_t *input_context,
    hsa_runtime_context_t **output_context)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 */
hsa_status_t hsa_context_release(hsa_runtime_context_t *input_context)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param count
 *      user allocated, output. The core runtime populates this value with the
 *      count of the number of components.
 * @param component_list
 *      runtime allocated, constant, output. List of HSA compoments in this HSA system.
 */
hsa_status_t hsa_component_get_list(unsigned int *count, 
    const hsa_agent_t **component_list)
{
hsa_agent_t agent;
    *count = 1;
    *component_list = &agent;
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 *
 * @param component_time 
 *      input, the time in component clock frequence, this can be obtained by
 *      using clock\_u64 instruction in HSAIL
 * @param host_time
 *      output, time per host frequency (which is available via
 *      hsa_clock_query_host_frequency API) 
 * @return TBD
 */
hsa_status_t hsa_clock_convert_time_component_to_host(uint64_t component_time, 
    uint64_t *host_time)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param component_time 
 *      input, the time in component clock frequence, this can be obtained by
 *      using clock\_u64 instruction in HSAIL
 * @param host_time
 *      time per host frequency (which is available via
 *      hsa_clock_query_host_frequency API)
 * @return TBD
 */
hsa_status_t hsa_clock_convert_convert_time_host_to_component(uint64_t host_time, 
    uint64_t *component_time)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param initial_signal_value  
 *      Initial value at the signal, the signal is initialized with this value.
 * @param signal_handle  
 *      The handle of the signal that this API creates. This handle is opaque.
 */
hsa_status_t hsa_signal_create(hsa_signal_value_t intial_signal_value,
                          hsa_signal_handle_t *signal_handle,
                          hsa_runtime_context_t *context)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle that was obtained from \ttbf{hsa\_signal\_create} API.
 */
hsa_status_t hsa_signal_destroy(hsa_signal_handle_t signal_handle)
{
    return HSA_STATUS_SUCCESS;
} 


/** 
 * @brief
 *
 */
uint64_t hsa_signal_get_timeout()
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param signal_value  
 *      Value of the signal, with acquire release semantics, signal\_value is
 *      assigned to the value@signal\_handle.
 */
hsa_status_t hsa_signal_send_acquire_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t signal_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief 
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param add_value  
 *      Value to add to the value@signal\_handle (the addition is atomic)
 */
hsa_status_t hsa_signal_add_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t add_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param and_value  
 *      Value to do an And with, value@signal\_handle \&= and\_value
 */
hsa_status_t hsa_signal_and_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t and_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal\ object that is to be signaled
 * @param xor_value  
 *      Value to do an XOR with, value@signal\_handle XOR= xor\_value
 */
hsa_status_t hsa_signal_xor_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t xor_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param value_exchange [input, output]] 
 *      the pointer to the value to exchange value@signal\_handle with. At the
 *      end of exchange, the value@signal\_handle is copied to the address
 *      pointed to by value\_exchange.
 */
hsa_status_t hsa_signal_exchange_acquire_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t *value_exchange)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param exchange_value  
 *      Exchange value, the value to be placed at signal, value@signal\_handle ,
 *      after being stored in value\_at\_signal, is overwritten with
 *      exchange\_value
 * @param value_at_signal  
 *      value\_at\_signal = value@signal\_handle value@signal\_handle =
 *      exchange\_value
 */
hsa_status_t hsa_signal_exchange_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t exchange_value,
                hsa_signal_value_t *value_at_signal)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param decrement_value  
 *      Value the signal is to be decremented with, value@signal\_handle -=
 *      decrement\_value
 */
hsa_status_t hsa_signal_decrement_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t decrement_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param value_compare  
 *      the value to compare value@signal\_handle against (operator is equal to)
 * @param value_replace  
 *      the value to replace the value@signal\_handle with if signal.value was
 *      equal to value\_compare
 * @param value_at_signal  
 *      the value at the signal, prior to the atomic replace, if the
 *      comparision was successful
 */
hsa_status_t hsa_signal_cas_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t value_compare,
                hsa_signal_value_t value_replace,
                hsa_signal_value_t *value_at_signal)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param compare_value  
 *      Compared with value@signal\_handle to determine maximum
 * @param max_value  
 *      MAX(compare\_value, value@signal\_handle )
 */
hsa_status_t hsa_signal_max(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t compare_value,
                hsa_signal_value_t *max_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param compare_value  
 *      value@signal\_handle is compared with this and the minimum of the two
 *      returned
 * @param min_value  
 *      min(compare\_value, value@signal\_handle) 
*/
hsa_status_t hsa_signal_min(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t compare_value,
                hsa_signal_value_t *min_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param or_value  
 *      value@signal\_handle OR= or\_value
{
    return HSA_STATUS_SUCCESS;
} 
 */
hsa_status_t hsa_signal_or_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t or_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param signal_value  
 *      Value of the signal
 */
hsa_status_t hsa_signal_send_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t signal_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param subtract_value  
 *      Value to be subtracted from the value@signal\_handle.
 */
hsa_status_t hsa_signal_subtract_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t subtract_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal object that is to be signaled
 * @param increment_value  
 *      Value to do the increment with
 */
hsa_status_t hsa_signal_increment_release(hsa_signal_handle_t signal_handle, 
                hsa_signal_value_t increment_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal whose value is to be retrieved.
 * @param cond 
 *      apply this condition to compare the signal\_value with
 *      value@signal\_handle and return the value@signal\_handle only when the
 *      condition is met
 * @param wait_value  
 *      Pointer to where the current value@signal\_handle must be read into.
*/
hsa_status_t hsa_signal_wait_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t cond, 
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param signal_handle   
 *      Opaque handle of the signal whose value is to be retrieved.
 * @param cond 
 *      apply this condition to compare the signal\_value with
 *      value@signal\_handle and return the value@signal\_handle only when the
 *      condition is met
 * @param wait_value  
 *      Pointer to where the current value@signal\_handle must be read into.
 */
hsa_status_t hsa_signal_wait_acquire_release(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t cond, 
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value)
{
    return HSA_STATUS_SUCCESS;
} 



/**
 * @brief
 *
 * @param input_queue  
 *      input. Queue structure that needs to be modified
 * @param queue_type 
 *      type of the queue (only type 0, which is default in-order issue queue,
 *      is supported at this time) 
 * @param size
 *      the size of the queue (in units of the number of packets)
 * @param altered_queue  
 *      runtime allocated. Modified queue structure 
*/
hsa_status_t hsa_queue_alter(hsa_queue_t *input_queue, 
                uint32_t queue_type, 
                uint64_t size,
                hsa_queue_t **altered_queue)
{
    return HSA_STATUS_SUCCESS;
} 



/**
 * @brief 
 *
 * @param component  
 *      The component on which this queue is to be created
 * @param size  
 *      Size of the queue memory in number of packets in is expected to hold
 * @param queue_type  
 *      type of the queue (only type 0, which is default in-order issue
 *                      queue, is supported at this time).
 * @param message_queue_handle
 *      the handle of the error message queue. Any asynchronous errors
 *      generated that are pertinent to this queue are reported via this
 *      message queue. The same message queue may be used for multiple queues.
 * @param queue  
 *      the queue structure, filled up and returned by the runtime.
 */
hsa_status_t hsa_queue_create(const hsa_agent_t *component,
                                         size_t size,
                                         uint32_t queue_type,
                                         hsa_runtime_context_t *context,
                                         hsa_queue_t **queue)
{
    hsa_queue_t que;

    *queue = &que;
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param queue  
 *      the queue that needs to be inactivated.
 */
hsa_status_t hsa_queue_force_inactivate(hsa_queue_t *queue)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param queue  
 *      The queue structure that points to the queue that needs to be
 *      destroyed, after destruction base\_address or the rest of the queue
 *      structure, even if cached by the user, are no longer valid. 
 *      
 */
hsa_status_t hsa_queue_destroy(hsa_queue_t *queue)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 *
 * @param queue
 *      the HSA queue structure
 */
uint64_t hsa_queue_get_read_index(hsa_queue_t *queue)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param queue
 *      the HSA queue structure
 */
uint64_t hsa_queue_get_write_index(hsa_queue_t *queue)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param queue
 *      the HSA queue structure
 * @param val
 *      the new value of the write index
 */
uint64_t hsa_queue_set_write_index(hsa_queue_t *q, 
    uint64_t val)
{
    return 0;
}  

/**
 * @brief
 *
 * @param queue
 *      the HSA queue structure
 * @param old_val
 *      the value to compare with 
 * @param new_val 
 *      if a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index(hsa_queue_t *q, 
    uint64_t old_val, 
    uint64_t new_val)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param queue
 *      the HSA queue structure
 * @param val
 *      the value to add to the write index
 */
uint64_t hsa_queue_add_write_index(hsa_queue_t *q, 
    uint64_t val)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 *
 * @param error_code  
 *      the error code to convert to string
 * @param error_string  
 *      the string description of the error code
 */
hsa_status_t hsa_queue_error_string_from_code(
                uint64_t error_code,
                char **error_string)
{
    return HSA_STATUS_SUCCESS;
} 



/**
 * @brief
 * @param aql_packet
 *      pointer to the AQL packet structure of the packet that needs
 *      to be validated
 */
hsa_status_t hsa_aql_dispatch_packet_validate(hsa_aql_dispatch_packet_t *aql_packet)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * @param aql_packet
 *      pointer to the AQL packet structure of the packet that needs
 *      to be validated
 */
hsa_status_t hsa_aql_barrier_packet_validate(hsa_aql_barrier_packet_t *aql_packet)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 *******************************************************************************
 * @brief Finalize BRIG
 * @details Finalize BRIG
 *  - Restrictions on BRIG
 *    - The input pointer must reference an in memory location of the BRIG. The
 *    BRIG must contain at the minimum the 4 basic BRIG sections (.strings, .
 *    directives, .code, .operands)
 *    - All symbols in the symbol/offset table must be resolved. Enforcing this
 *    restriction at the level of the core is advantageous for the following
 *    reasons
 * @param component 
 *      A valid pointer to the hsa\_component 
 * @param brig 
 *      A pointer to the in memory BRIG structure.
 * @param kernel_directive
 *      A valid kernel directive 
 * @param options 
 *      compilation options (a pointer), in ISO/IEC 646 encoded english
 *      language string.
 * @param input_symbols
 *      if it is know how some of the symbols can be resolved.
 * @param kernel 
 *      A pointer to the location to which the output AQL kernel
 *      object is to be copied.
 * @param symbol_map
 *      hsail symbol map infomation.
 * @param debug_info 
 *      A pointer to the debug information structure.
*/
hsa_status_t hsa_finalize_brig(const hsa_agent_t *component, 
    hsa_brig_t *brig,
    uint32_t kernel_directive,
    const char *options,
    hsa_symbol_map_t *input_symbols,
    hsa_code_object_t **kernel,
    hsa_debug_info_t **debug_info,
    hsa_symbol_map_t **symbol_map)
{
    *symbol_map = NULL;
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 *
 * @param code_object 
 *        a pointer to the kernel code object that needs to be destroyed
 * @return hsa\_status\_success, hsa\_status\_error      
*/
hsa_status_t hsa_code_object_destroy(hsa_code_object_t *code_object)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 * @brief
 * 
 * @param code_object 
 *      the code object to serialize
 * @param size
 *      size of the serialized object that is generated
 * @param serialized_object
 *      pointer to the serialized object
 * @return hsa\_memory\_allocation\_failure, hsa\_status\_success
 * hsa\_status\_failure, hsa\_status\_invalid\_argument
*/
hsa_status_t hsa_code_object_serialize(hsa_code_object_t *code_object,
    uint64_t *size,
    void *serialized_object)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 * 
 * @param serialized_object
 *      pointer to the serialized object
 * @param size
 *      size of the serialized object that is generated
 * @param code_object 
 *      the code object generated as a part of serialization
 * @return hsa\_memory\_allocation\_failure, hsa\_status\_success
 * hsa\_status\_failure, hsa\_status\_invalid\_argument
*/
hsa_status_t hsa_code_object_deserialize( void *serialized_object,
    uint64_t size,
    hsa_code_object_t **code_object)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param kernel
 *      a pointer to the kernel code object 
 * @param symbol_map
 *      a pointer to the symbol information structure
 * @param code_object
 *      a pointer to the kernel code object
 * @return hsa\_status\_success, hsa\_status\_invalid\_argument, hsa\_status\_error
*/
hsa_status_t hsa_symbol_bind_code_object( hsa_code_object_t *kernel, 
    hsa_symbol_map_t *input_symbol_map, 
    hsa_code_object_t *output_kernel)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 *******************************************************************************
 * @brief
 *
 * @param symbol_map
 *      a pointer to the kernel code object 
 * @param brig
 *      the the brig this symbol table came from 
 * @param symbol_name
 *      the name of the symbol as a char *
 * @param symbol_details
 *      structure with symbol details
 * @return -- tbd
*/
hsa_status_t hsa_symbol_map_query_by_name(hsa_symbol_map_t *symbol_map, 
    hsa_brig_t brig,
    char *symbol_name,
    hsa_symbol_t *symbol_details)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 *******************************************************************************
 * @brief
 *
 * @param symbol_map
 *      a pointer to the symbol map structure
 * @param brig
 *      the the brig this symbol table came from 
 * @param symbol_details
 *      structure with symbol details
 * @return -- tbd
*/
hsa_status_t hsa_symbol_map_query_by_offset(hsa_symbol_map_t *symbol_map, 
    hsa_brig_t *brig,
    uint32_t directive_section_offset,
    hsa_symbol_t *symbol_details)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief
 *
 * @param symbol_map
 *      destroys the symbol map resouces including any additional structures
 * @return -- tbd
*/
hsa_status_t hsa_symbol_destroy(hsa_symbol_map_t *symbol_map)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 *******************************************************************************
 * @brief register memory
 * @details The hsa_register_system_memory HSA core interface is used for
 * registering a system memory region for use with all the devices returned by
 * ::HsaGetDevices. This is an optional interface that is solely provided as a
 * performance optimization hint to the underlying implementation so it may
 * prepare for the future use of the memory by the devices. The interface is
 * only beneficial for system memory that will be directly accessed by a device.
 *
 * Overlapping registrations are allowed. This is neither detrimental nor
 * beneficial.
 *
 * @param address 
 *      A pointer to the base of the memory region to be registered. If a null
 *      pointer is passed, no operation is performed.
 * @param size 
 *      Requested registration size in bytes. If a size of zero is passed, no
 *      operation is performed.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_register(void *address, 
    size_t size)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 *******************************************************************************
 * @brief Deregister memory
 * @details The hsa_deregister_system_memory HSA core interface is used for
 * deregistering a memory region previously registered.
 *
 * Deregistration must be performed using an address that was previously
 * registered.  Attempting to deregister a memory region that
 * was not previously registered will result in undefined behavior.
 * In the event that deregistration is performed on an address that has been
 * used in multiple registrations, the smallest of the registrations is
 * deregistered.
 *
 * Other than the potential performance impact there should be no user
 * noticeable side-effects from deregistering a memory region.
 *
 * @param address A pointer to the base of the memory region to be deregistered.
 * If a NULL pointer is passed, no operation is performed.
 *
 * @retval ::kHsaStatusSuccess, ::kHsaStatusInvalidArgument
 *
 * @see ::HsaRegisterSystemMemory
 *******************************************************************************
 */
hsa_status_t hsa_memory_deregister(void *address)
{
    return HSA_STATUS_SUCCESS;
} 


/**
 *******************************************************************************
 * @brief Allocate memory on HSA Device
 * @details The hsa_memory_allocate_component_local HSA core interface is used for
 * allocating global device memory associated with specified device.
 *
 * @param size 
 *      Requested allocation size in bytes. If size is 0, NULL is returned.
 * @param component 
 *      A valid pointer to the HSA device for which the specified amount of
 *      global memory is to be allocated.
 * @param address 
 *      A valid pointer to the location of where to return the pointer to the
 *      base of the allocated region of memory.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_allocate_component_local(
    const hsa_agent_t *component,
    size_t size, 
    void **address)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 *******************************************************************************
 * @brief Deallocate memory on HSA Device
 * @details The hsa_memory_free_component_local HSA core interface is used for
 * deallocating global device memory that was previously allocated with
 * ::HsaAllocateDeviceMemory.
 *
 * @param address 
 *      A pointer to the address to be deallocated. If the pointer is NULL, no
 *      operation is performed.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_free_component_local(void *address)
{
    return HSA_STATUS_SUCCESS;
} 

/**
 * @brief Copies the values of @a size bytes from the location pointed by @a src
 * directly to the memory block pointed by @a dst.

 * @details The ::HsaCopyMemory interface is used for copying memory in the
 * HSA address space. If the specified pointers do not belong to the HSA address
 * space, an error is returned. If the size of the destination or source arrays
 * is smaller than the indicated size, the behavior is undefined.
 *
 * @param dst 
 *      A valid pointer to the destination array where the content is to be
 *      copied.
 * @param src 
 *      A valid pointer to the source of data to be copied.
 * @param size 
 *      Number of bytes to copy.
 */
hsa_status_t hsa_memory_copy_component_local_to_system(void *dst, 
    const void *src, 
    size_t size)
{
    return HSA_STATUS_SUCCESS;
} 

#include "main.c"
