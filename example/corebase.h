#ifndef HSACOREBASE_H
#define HSACOREBASE_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** \addtogroup ENUMhsa_status -------------------------------------------
 *  @{
 */
typedef enum hsa_status_t{
    /**
     * indicates success. The API has been successfully executed per its definition.
     */
    HSA_STATUS_SUCCESS=0,

    /**
     * indicates an error occured, specifics were either not determinable or
     * not encoded in the error list.
     */
    HSA_STATUS_FAILURE,

    /**
     * indicates that intiatialzation attemt failed due to prior initialization
     */
    HSA_STATUS_ALREADY_INITIALZIED,

    /**
     * indicates that signal is timedout
     */
    HSA_STATUS_INFO_SIGNAL_TIMEOUT
} hsa_status_t;
/** @}**************************************************************/


/** \addtogroup APIhsa_status_query ----------------------------------
 *  @{
 */
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
 *      potentially describes the error status. The string terminates in a ISO
 *      646 defined NUL char.
 */
hsa_status_t hsa_status_query_description(hsa_status_t input_status,
    uint64_t *status_info,
    char * const * status_info_string);
/** @}**************************************************************/

typedef uint64_t hsa_runtime_context_t;

/** \addtogroup STRnotify_message -------------------------------------------
 *  @{
 */
typedef struct hsa_notification_info_s{
    /**
     * the info status enum value
     */
    hsa_status_t status;

    /**
     * a pointer to more information, this could be pointing to implementation
     * specific details that could be useful to some tools or to binary data
     */
    void         *ptr_info;

    /**
     * ISO/IEC 646 character encoding must be used. A string indicating some
     * error information. The string should be NUL terminated per ISO 646.
     */
    char         *string_info;

    /**
     * a pointer to user supplied data
     */
    void         *user_data;
} hsa_notification_info_t;
/** @}**************************************************************/


/** \addtogroup STRerror_message -------------------------------------------
 *  @{
 */
typedef struct hsa_async_error_info_s{
    /**
     * indicates the type of the error, based on this, the user knows if
     * and packet\_id is available in one of the reserved words.
     */
    hsa_status_t error_type;

    /**
     * the queue that processed the entity that caused the asynchronous error.
     */
    uint32_t     queue_id;

    /**
     * a pointer to more information, this could be pointing to implementation
     * specific details that could be useful to some tools or to binary data
     */
    void         *ptr_info;

    /**
     * ISO/IEC 646 character encoding must be used. A string indicating some
     * error information. The string should be NUL terminated per ISO 646.
     */
    char         *string_info;

    /**
     * a pointer to user supplied data
     */
    void         *user_data;

    /**
     * timestamp -- system timestamp to indicate when the error was
     * discovered, the implementation may chose to always return 0 and user
     * must take it into account when utilizing the timestamp.
     */
    uint64_t     timestamp;

    /**
     * additional info to be intepreted based on the error\_type
     */
    uint64_t     reserved1;

    /**
     * additional info to be intepreted based on the error\_type
     */
    uint64_t     reserved2;

    /**
     * additional info to be intepreted based on the error\_type
     */
    uint64_t     reserved3;

} hsa_async_error_info_t;
/** @}**************************************************************/


/** \addtogroup APIregister_notify -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param notification_callback
 *      input. the callback that the user is registering, the callback is
 *      called with info as a parameter. User can read the structure
 *      and access its elements.
 * @param user_data
 *      input. the user data to call the callback with. info $\rightarrow$ user\_data will be
 *      filled with value when the callback is called.
 * @param context
 *      the runtime context that this callback is being registered for.
 */
hsa_status_t hsa_notification_callback_register(void ( *notification_callback)(const hsa_notification_info_t *info),
    void *user_data,
    hsa_runtime_context_t *context);
/** @}**************************************************************/


/** \addtogroup APIregister_error -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param error_callback
 *      input. the callback that the user is registering, the callback is called with
 *      info structure. User can read the structure and access its elements.
 * @param user_data
 *      input. the user data to call the callback with. info->user\_data will be
 *      filled with value when the callback is called.
 * @param context
 *      the runtime context that this callback is being registered for.
 *
 */
hsa_status_t hsa_error_callback_register(void ( *error_callback)(const hsa_async_error_info_t *info),
    void *user_data,
    hsa_runtime_context_t *context);
/** @}**************************************************************/


/** \addtogroup APIhsa_initialize -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param context
 *      output, user allocated. A type for reference counting.
 *
 *
 */
hsa_status_t hsa_open(hsa_runtime_context_t **context);
/** @}**************************************************************/


/** \addtogroup APIhsa_close -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param context
 *      the context to close
 *
 */
hsa_status_t hsa_close(hsa_runtime_context_t *context);
/** @}**************************************************************/


/** \addtogroup APIacquire_context -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param input_context
 *      input, user allocated. the context that the user is expilicitly
 *      reference counting, increment reference count if not 0
 *
 */
hsa_status_t hsa_context_acquire(hsa_runtime_context_t *input_context);
/** @}**************************************************************/


/** \addtogroup APIrelease_context -----------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param context
 *      input, user allocated. the context that the user is expilicitly
 *      reference counting, decrement reference count if not 1
 *
 */
hsa_status_t hsa_context_release(hsa_runtime_context_t *input_context);
/** @}**************************************************************/


/** \addtogroup STRhsa_segment -------------------------------------------
 *  @{
 */
typedef struct hsa_segment_s{
    /**
     * if bit is set, the element/mask represents global segment
     */
    uint8_t global:1;

    /**
     * if bit is set, the element/mask represents private segment
     */
    uint8_t private:1;

    /**
     * if bit is set, the element/mask represents group segment
     */
    uint8_t group:1;

    /**
     * if bit is set, the element/mask represents kernarg segment
     */
    uint8_t kernarg:1;

    /**
     * if bit is set, the element/mask represents readonly segment
     */
    uint8_t readonly:1;

    /**
     * reserved
     */
    uint8_t reserved:1;
} hsa_segment_t;
/** @}**************************************************************/


/** \addtogroup STRhsa_memory_descriptor ---------------------------
 *  @{
 */
typedef struct hsa_memory_descriptor_s{

    /**
     * id of the node this memory belongs to.
     */
    uint32_t      hsa_node_id;

    /**
     * unique for this memory with in the system.
     */
    uint32_t      hsa_memory_id;

    /**
     * information on segments that can use this memory.
     */
    hsa_segment_t supported_segment_type_mask;

    /**
     * base of the virtual address for this memory, if applicable
     */
    uint64_t      virtual_address_base;

    /**
     * size
     */
    uint64_t      size_in_bytes;

    /**
     * theoretical peak bandwidth in mega-bits per second to access this memory
     * from the agent/component
     */
    uint64_t      peak_bandwidth_mbps;
} hsa_memory_descriptor_t;
/** @}**************************************************************/


/** \addtogroup STRhsa_cache_descriptor ---------------------------
 *  @{
 */
typedef struct hsa_cache_descriptor_s{
    /**
     * id of the node this memory belongs to.
     */
    uint32_t hsa_node_id;

    /**
     * unique identified for this cache with in the system.
     */
    uint32_t hsa_cache_id;

    /**
     * number of levels of cache (for a mult-level cache)
     */
    uint8_t  levels;

    /**
     * associativity of this cache, array with number of entries = number of
     * levels
     */
    uint8_t  *associativity;

    /**
     * size at each level, this array is of size = levels
     */
    uint64_t *cache_size;

    /**
     * cache line size at each level, this array is of size = levels
     */
    uint64_t *cache_line_size;

    /**
     * is the cache inclusive with the level above? The size of this array is level-1
     */
    bool     *is_inclusive;

} hsa_cache_descriptor_t;
/** @}**************************************************************/



/** \addtogroup STRhsa_tlb_descriptor ---------------------------
 *  @{
 */
typedef struct hsa_tlb_descriptor_s{

    /**
     * id of the node this memory belongs to.
     */
    uint32_t hsa_node_id;

    /**
     * unique identified for this tlb with in the system.
     */
    uint32_t hsa_tlb_id;

    /**
     * number of levels of TLB (for a mult-level TLB)
     */
    uint8_t  levels;

    /**
     * associativity of this TLB, size of this array is = levels
     */
    uint8_t  *associativity;

    /**
     * is the TLB inclusive with the level above? The size of this array is level-1
     */
    bool     *is_inclusive;

    /**
     * size of TLB at each level, this size of this array is = levels
     */
    uint64_t *tlb_size;
} hsa_tlb_descriptor_t;
/** @}**************************************************************/


/** \addtogroup ENUhsa_properties -------------------------------------------
 *  @{
 */
typedef enum hsa_property_t{
    /**
     * Theoretical peak floating point operations per second supported by the
     * component, if applicable.
     */
    HSA_MAX_FLOPS,

    /**
     * Theoretical peak integer operations supported by the agent, if applicable.
     */
    HSA_MAX_IOPS,

    /**
     * Theoretical peak integer operations supported by the agent, if applicable.
     */
    HSA_MAX_PROCESSING_ELEMENTS
} hsa_property_t;
/** @}**************************************************************/

/** \addtogroup ENUagent_type -------------------------------------------
 *  @{
 */
typedef enum hsa_agent_type_t{
    /**
     * indcates that the agent represents the host.
     */
    HOST=1,

    /**
     * indicates that agent represents an HSA component.
     */
    COMPONENT=2,

    /**
     * indicates that the agent is capable of agent dispatches, and can serve as a target for them.
     */
    AGENT_DISPATCH=4
}hsa_agent_type_t;

/** @}**************************************************************/

/** \addtogroup STRhsa_component -------------------------------------------
 *  @{
 */
typedef struct hsa_agent_s{
    /**
     * does it support position independent code?. Only applicable when the agent is a component.
     */
    bool                   is_pic_supported;

    /**
     * id of the node this agent/component belongs to.
     */
    uint32_t               hsa_node_id;

    /**
     * Unique identifier for an HSA agent.
     */
    uint32_t               agent_id;

    /**
     * an identifier for the type of this agent.
     */
    hsa_agent_type_t       agent_type;

    /**
     * The vendor of the agent/component. ISO/IEC 646 character encoding must
     * be used. If the name is less than 16 characters then remaining
     * characters must be set to 0.
     */
    char                   vendor[16];

    /**
     * The name of this agent/component. ISO/IEC 646 character encoding must
     * be used. If the name is less than 16 characters then remaining
     * characters must be set to 0.
     */
    char                   name[16];

    /**
     * table of properties of the agent, any property that is not available has a value of 0
     */
    uint64_t               *property_table;

    /**
     * number of the different types of memories available to this agent.
     */
    uint32_t               number_memory_descriptors;

    /**
     * Array of memory descriptor offsets.  Number of elements in array equals
     * number\_memory\_descriptors.
    */
    uint32_t               *memory_descriptors;

    /**
     * Number of caches available to this agent/component
     */
    uint32_t               number_cache_descriptors;

    /**
     * Array of cache descriptor offsets.  Number of elements in array equals
     * number\_cache\_descriptors.
    */
    uint32_t               *cache_descriptors;

    /**
     * Number of subagents.
     */
    uint32_t               number_of_subagents;

    /**
     * subagent list of offsets, points to the offsets in the topology table.
     */
    uint32_t               *subagent_offset_list;

    /**
     * Wave front size, i.e. number of work-items in a wavefront.
     */
    uint32_t               wavefront_size;

    /**
     * Maximum size of the user queue in bytes allocatable via the runtime.
    */
    uint32_t               queue_size;

    /**
     * Size (in bytes) of group memory available to a single work-group.
    */
    uint32_t               group_memory_size_bytes;

    /**
     * max number of fbarrier that can be used in any kernel and functions it
     * invokes.
     */
    uint32_t               fbarrier_max_count;
} hsa_agent_t;
/** @}**************************************************************/


/** \addtogroup STRhsa_platform -------------------------------------------
 *  @{
 */
typedef struct hsa_platform_s{
    /** constant observable timestamp value increase rate is in the range
     * 1-400MHz.
     */
    uint32_t                hsa_system_timestamp_frequency_mhz;

    /**
     * number of different nodes in this platform configuration.
     */
    uint8_t                 number_of_nodes;

    /**
     * ids of the nodes.
     */
    uint32_t                *node_id;

    /**
     * number of agent offsets specified in this structure.
     */
    uint32_t                number_of_agents;

    /**
     * agent list, refers to the offsets in platform table.
     */
    uint32_t                *agent_offset_list_bytes;

    /**
     * number of the different types of memories available to this agent.
     */
    uint32_t                number_memory_descriptors;

    /**
     * each element in the array carries an offset into the topology table to
     * where memory descriptors are located.  Number of elements in array
     * equals number\_memory\_descriptors.
    */
    uint32_t                *memory_descriptor_offset_list_bytes;

    /**
     * number of caches available to this agent/component
     */
    uint32_t                number_cache_descriptors;

    /**
     * array of offsets (into the topology table) to cache descriptors.  Number
     * of elements in array equals number\_cache\_descriptors.
    */
    uint32_t                *cache_descriptors_offset_list_bytes;

} hsa_platform_t;
/** @}**************************************************************/


/** \addtogroup STRtopology_header -------------------------------------------
 *  @{
 */
typedef struct hsa_topology_header_s{
    /**
     * size of the header
     */
    uint32_t       header_size_bytes;

    /**
     * the hierarchical platform structure that abstracts the table
     */
    hsa_platform_t platform;

    /**
     * size of the table
     */
    uint32_t       table_size;

    /**
     * table base address, points to the table which starts with
     * hsa\_platform\_t structure
     */
    void           *topology_table_base;

} hsa_topology_header_t;
/** @}**************************************************************/


/** \addtogroup APItopology_create ----------------------------------
 * @{
 */
/**
 * @brief
 *
 * @param header
 *      output, runtime allocated. The topology header, this includes the base
 *      pointers to the rest of the topology table.
 *
 */
hsa_status_t hsa_topology_table_create(hsa_topology_header_t **header);
/** @}**************************************************************/


/** \addtogroup APItopology_destroy ----------------------------------
 * @{
 */
/**
 * @brief
 *
 * @param header
 *      input, generated by \ttbf{hsa\_topology\_table\_create} API. The
 *      topology header, this includes the base pointers to the rest of the
 *      topology table.
 *
 */
hsa_status_t hsa_topology_table_destroy(hsa_topology_header_t *header);
/** @}**************************************************************/


/** \addtogroup APIclock_convert --------------------------------------------
 *  @{
 */
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
    uint64_t *host_time);

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
    uint64_t *component_time);
/** @}**************************************************************/


/** \addtogroup STRsignal_value -------------------------------------------
 *  @{
 */
typedef union signal_value_s{
    /**
    * Pointer to the base of the HSAIL segment
    */
    int value32;

    /**
    * Pointer to the base of the HSAIL segment
    */
    int64_t value64;
} hsa_signal_value_t;
/** @}**************************************************************/

typedef uint64_t hsa_signal_handle_t;

/** \addtogroup APIsignal_create ---------------------------------------------
 *
 *  @{
 */
/**
 * @brief
 * @param initial_signal_value
 *      input. Initial value at the signal, the signal is initialized with this value.
 * @param signal_handle
 *      User allocated, output. The handle of the signal that this API creates.
 *      This handle is opaque.
 * @param context
 *      input. The context in which this signal is being created. Any
 *      errors/notifications will be reported via callbacks registered in the
 *      same context.
 */
hsa_status_t hsa_signal_create(hsa_signal_value_t intial_signal_value,
                          hsa_signal_handle_t *signal_handle,
                          hsa_runtime_context_t *context);
/** @}***********************************************************************/


/** \addtogroup APIsignal_bind --------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle that was obtained from \ttbf{hsa\_signal\_create} API.
 * @param context
 *      input. Additional context to which this signal should be bound to
 */
hsa_status_t hsa_signal_bind(hsa_signal_handle_t signal_handle,
    hsa_runtime_context_t *context);
/** @}***********************************************************************/

/** \addtogroup APIsignal_unbind --------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle that was obtained from \ttbf{hsa\_signal\_create} API.
 * @param context
 *      input. Unbind the signal from this context.
 */
hsa_status_t hsa_signal_unbind(hsa_signal_handle_t signal_handle,
    hsa_runtime_context_t *context);
/** @}***********************************************************************/

/** \addtogroup APIsignal_destroy --------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle that was obtained from \ttbf{hsa\_signal\_create} API.
 */
hsa_status_t hsa_signal_destroy(hsa_signal_handle_t signal_handle);
/** @}***********************************************************************/


/** \addtogroup APIsignal_timeout --------------------------------------------
 *  @{
 */
/**
 * @brief
 *
 */
uint64_t hsa_signal_get_timeout();
/** @}***********************************************************************/


/** \addtogroup APIsignal_all ------------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param signal_value
 *      input. Value of the signal, with relaxed semantics,
 *      signal\_value is assigned to the value@signal\_handle.
 */
hsa_status_t hsa_signal_send_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t signal_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param add_value
 *      input. Value to add to the value@signal\_handle (the addition is atomic).
 */
hsa_status_t hsa_signal_add_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t add_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param add_value
 *      input. Value to add to the value@signal\_handle (the addition is atomic).
 */
hsa_status_t hsa_signal_add_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t add_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param and_value
 *      input. Value to do an And with, value@signal\_handle \&= and\_value.
 */
hsa_status_t hsa_signal_and_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t and_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param and_value
 *      input. Value to do an And with, value@signal\_handle \&= and\_value.
 */
hsa_status_t hsa_signal_and_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t and_value);

/**
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param or_value
 *      input. value@signal\_handle OR= or\_value.
 */
hsa_status_t hsa_signal_or_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t or_value);

/**
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param or_value
 *      input. value@signal\_handle OR= or\_value.
 */
hsa_status_t hsa_signal_or_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t or_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal\ object that is to be signaled.
 * @param xor_value
 *      input. Value to do an XOR with, value@signal\_handle XOR= xor\_value.
 */
hsa_status_t hsa_signal_xor_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t xor_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal\ object that is to be signaled.
 * @param xor_value
 *      input. Value to do an XOR with, value@signal\_handle XOR= xor\_value.
 */
hsa_status_t hsa_signal_xor_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t xor_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param exchange_value
 *      user allocated, input/output. Exchange value, the value to be placed at
 *      signal, value@signal\_handle , after being stored in value\_at\_signal,
 *      is overwritten with exchange\_value.
 * @param value_at_signal
 *      user allocated, output. value\_at\_signal = value@signal\_handle;
 *      value@signal\_handle = exchange\_value.
 */
hsa_status_t hsa_signal_exchange_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t exchange_value,
                hsa_signal_value_t *value_at_signal);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param exchange_value
 *      user allocated, input/output. Exchange value, the value to be placed at
 *      signal, value@signal\_handle , after being stored in value\_at\_signal,
 *      is overwritten with exchange\_value.
 * @param value_at_signal
 *      user allocated, output. value\_at\_signal = value@signal\_handle;
 *      value@signal\_handle = exchange\_value.
 */
hsa_status_t hsa_signal_exchange_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t exchange_value,
                hsa_signal_value_t *value_at_signal);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param decrement_value
 *      input. Value the signal is to be decremented with, value@signal\_handle -=
 *      decrement\_value.
 */
hsa_status_t hsa_signal_decrement_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t decrement_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param decrement_value
 *      input. Value the signal is to be decremented with, value@signal\_handle -=
 *      decrement\_value.
 */
hsa_status_t hsa_signal_decrement_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t decrement_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param value_compare
 *      input. The value to compare value@signal\_handle against (operator is equal to).
 * @param value_replace
 *      input. the value to replace the value@signal\_handle with if signal.value was
 *      equal to value\_compare.
 * @param value_at_signal
 *      user allocated, output. The value at the signal, prior to the atomic replace, if the
 *      comparision was successful.
 */
hsa_status_t hsa_signal_cas_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value_compare,
                hsa_signal_value_t value_replace,
                hsa_signal_value_t *value_at_signal);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param compare_value
 *      input. Compared with value@signal\_handle to determine maximum.
 * @param max_value
 *      user allocated, output. MAX(compare\_value, value@signal\_handle). The
 *      signal value is updated with the max value.
 */
hsa_status_t hsa_signal_max(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t compare_value,
                hsa_signal_value_t *max_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled.
 * @param compare_value
 *      input. value@signal\_handle is compared with this and the minimum of the two
 *      returned.
 * @param min_value
 *      user allocated, output. min(compare\_value, value@signal\_handle). The
 *      signal value is updated with the min value.
*/
hsa_status_t hsa_signal_min(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t compare_value,
                hsa_signal_value_t *min_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled
 * @param signal_value
 *      input. Value of the signal
 */
hsa_status_t hsa_signal_send_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t signal_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled
 * @param subtract_value
 *      input. Value to be subtracted from the value@signal\_handle.
 */
hsa_status_t hsa_signal_subtract_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t subtract_value);


/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled
 * @param subtract_value
 *      input. Value to be subtracted from the value@signal\_handle.
 */
hsa_status_t hsa_signal_subtract_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t subtract_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled
 * @param increment_value
 *      input. Value to do the increment with
 */
hsa_status_t hsa_signal_increment_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t increment_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal object that is to be signaled
 * @param increment_value
 *      input. Value to do the increment with
 */
hsa_status_t hsa_signal_increment_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t increment_value);


/** @}***********************************************************************/


/** \addtogroup ENUwait_condition -----------------------------------------------
 *  @{
 */
typedef enum hsa_signal_condition_t{
    /**
     * the return from the wait API will be either when signal.value == wait\_value
     * or the max timeout has been reached.
     */
    HSA_EQUALS,

    /**
     * the return from the wait API will be either when signal.value != wait\_value
     * or the max timeout has been reached.
     */
    HSA_NOTEQUALS,

    /**
     * the return from the wait API will be either when signal.value > wait\_value
     * or the max timeout has been reached.
     */
    HSA_GREATER,

    /**
     * the return from the wait API will be either when signal.value >= wait\_value
     * or the max timeout has been reached.
     */
    HSA_GREATER_EQUALS,

    /**
     * the return from the wait API will be either when signal.value < wait\_value
     * or the max timeout has been reached.
     */
    HSA_LESSER,

    /**
     * the return from the wait API will be either when signal.value <= wait\_value
     * or the max timeout has been reached.
     */
    HSA_LESSER_EQUALS
}hsa_signal_condition_t;
/** @}***********************************************************************/

/** \addtogroup APIsignal_query -----------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal whose value is to be retrieved.
 * @param value
 *      user allocated, output. Pointer to where the current
 *      value@signal\_handle must be read into.
*/
hsa_status_t hsa_signal_query_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_value_t *value);

/** @}***********************************************************************/


/** \addtogroup APIsignal_wait -----------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal whose value is to be retrieved.
 * @param cond
 *      input. apply this condition to compare the wait\_value with
 *      value@signal\_handle and return the value@signal\_handle only when the
 *      condition is met.
 * @param compare_value
 *      input. value to compare with.
 * @param return_value
 *      user allocated, output. Pointer to where the current
 *      value@signal\_handle must be read into.
*/
hsa_status_t hsa_signal_wait_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t cond,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/**
 * @brief
 * @param signal_handle
 *      input. Opaque handle of the signal whose value is to be retrieved.
 * @param cond
 *      input. apply this condition to compare the compare\_value with
 *      value@signal\_handle and return the value@signal\_handle only when the
 *      condition is met.
 * @param compare_value
 *      input. value to compare with.
 * @param return_value
 *      user allocated, output. Pointer to where the current
 *      value@signal\_handle must be read into.
 */
hsa_status_t hsa_signal_wait_acquire_release(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t cond,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);
/** @}***********************************************************************/


/** \addtogroup ENUinterrupt_condition --------------------------------------------
 *  @{
 */
typedef enum hsa_interrupt_condition_t{
    /**
     * caused by debugtrap\_u32 instruction
     */
    HSA_DEBUGTRAP=1,

    /**
     * caused by syscall
     */
    HSA_SYSCALL=4,

    /**
     * caused by other interrupt
     */
    HSA_OTHER_INTERRUPT=8
} hsa_interrupt_condition_t;
/** @}***********************************************************************/


/** \addtogroup STRexecution_info --------------------------------------------
 *  @{
 */
typedef struct hsa_group_execution_info_s{

    /**
     * indicates the mailbox is full and needs to be consumed.
     */
    hsa_signal_handle_t full_flag;

    /**
     * the size of the workgroup, all pointers below are arrays of that size.
     */
    uint16_t workgroup_size;

    /**
     * what caused this execution to stop.
     */
    hsa_interrupt_condition_t *condition;

    /**
     * the flattend workitem IDs, array[workgroup\_size].
     */
    uint32_t *workitem_id;

    /**
     * the ID of the compute unit, array[workgroup\_size].
     */
    uint32_t *compute_unit_id;

    /**
     * pointer to the AQL packet, array[workgroup\_size].
     */
    uint64_t *aql_packet_ptr;

    /**
     * any pertinent virtual address, array[workgroup\_size].
     */
    uint64_t *virtual_address;

    /**
     * the current program counter, array[workgroup\_size].
     */
    uint64_t *current_program_counter;

    /**
     * location to where the arguments have been stored. The size and contents
     * are written by the component and need to be decoded by the agent when
     * reading this.
     */
    uint64_t args;

    /**
     * if the condition is syscall, location to where the outputs need to be
     * stored. This is array[workgroup\_size].
     */
    uint64_t **syscall_output;

} hsa_group_execution_info_t;
/** @}***********************************************************************/


/** \addtogroup STRqueue_struct ----------------------------------------------
 *  @{
 */
typedef struct hsa_queue_mailbox_t {
  /**
   * A pointer to the mailbox
   */
  const hsa_group_execution_info_t *mailbox_ptr;

  /**
   * the signal that user can wait on to get an indication that mailbox needs
   * processing
   */
  hsa_signal_handle_t mailbox_signal;
} hsa_queue_mailbox_t;
/** @}***********************************************************************/


/** \addtogroup STRqueue_struct ----------------------------------------------
 *  @{
 */
typedef struct hsa_queue_s{
  /**
   * used for dynamic queue protocol determination. Currently, 0, the default
   * queue type, is the only type supported.
   */
  uint32_t queue_type;


  /**
   * bitfield to indicate specific features supported by queue. On a queue
   * creation, if user observes that some unknown bits are set, then the user
   * should ignore them.
   */
  uint32_t queue_features;


  /**
   * A 64-bit pointer to the base of the virtual memory which holds the AQL
   * packets for the queue. At the time of queue creation, the address passed
   * in by the user as queue memory is copied here. This address must be
   * 64-byte aligned.
   */
  uint64_t base_address;


  /**
   * After writing a packet to the queue, user must signal this signal object
   * with the most recent write\_offset. The packet may already have been
   * processed by the packet processor by the time this doorbell is signaled,
   * however, it may not be processed at all if the doorbellSignal is not
   * signaled.
   */
  hsa_signal_handle_t doorbell_signal;


  /**
   * A 32-bit unsigned integer which specifies the maximum size of the queue in
   * the number of packets. The size of the queue is always aligned with a
   * power of two number of AQL packets.
   */
  uint32_t size;

  /**
   * A 32-bit ID for a queue which is unique-per-process.
   */
  uint32_t queue_id;

  /**
   * maximum number of concurrent workgroups that can run out of this queue
   */
  uint32_t queue_active_group_count_global;

  /**
   * A pointer to another User Mode Queue that can be used by the HSAIL kernel
   * to request system services. The serviceQueue property is provided by the
   * application to the runtime API call when the queue is created, and may be
   * NULL, the system provided serviceQueue or an application managed queue.
   */
  uint64_t service_queue;

} hsa_queue_t;
/** @}***********************************************************************/


/** \addtogroup STRaql_header ------------------------------------------------
 *  @{
 */
typedef enum hsa_aql_packet_type_t{
  INVALID=0,
  DISPATCH=1,
  BARRIER=2
}hsa_aql_packet_type_t;
/** @}***********************************************************************/


/** \addtogroup STRaql_header ------------------------------------------------
 *  @{
 */
typedef struct hsa_aql_packet_header_s {
  /**
   * 8 bits for describing the packet type, 0 for INVALID, 1 for COMPONENT
   * DISPATCH, 2 for BARRIER and 4 for AGENT DISPATCH. All other values are
   * reserverd.
   */
  uint16_t format:8;


  /**
   * If set then processing of packet will only begin when all preceding
   * packets are complete.
   */
  uint16_t barrier:1;


  /**
   * Determines the scope and type of the memory fence operation applied before
   * the packet enters the active phase. The different values are described in
   * Table~\ref{acquirefencescope}.
   */
  uint16_t acquire_fence_scope:2;


  /**
   * Determines the scope and type of the memory fence operation applied after
   * kernel completion but before the packet is completed. The different values
   * are discussed in Table~\ref{releasefencescope}.
   */
  uint16_t release_fence_scope:2;

  /**
   * must be 0
   */
  uint16_t reserved:3;

} hsa_aql_packet_header_t;
/** @}***********************************************************************/


/** \addtogroup STRdispatch_packet -------------------------------------------
 *  @{
 */
typedef struct hsa_aql_dispatch_packet_s{
  /**
     packet header packet header structure
  */
  hsa_aql_packet_header_t header;


  /**
     x dimension of work-group (measured in work-items).
  */
  uint16_t  workgroup_size_x;


  /**
     y dimension of work-group (measured in work-items).
  */
  uint16_t  workgroup_size_y;


  /**
     z dimension of work-group (measured in work-items).
  */
  uint16_t  workgroup_size_z;


  /**
     reserved
  */
  uint16_t  reserved2;


  /**
     x dimension of grid (measured in work-items).
  */
  uint32_t  grid_size_x;


  /**
     y dimension of grid (measured in work-items).
  */
  uint32_t  grid_size_y;


  /**
     z dimension of grid (measured in work-items).
  */
  uint32_t  grid_size_z;


  /**
     Total size in bytes of private memory allocation request
     (per work-item).
  */
  uint32_t  private_segment_size_bytes;


  /**
     Total size in bytes of group memory allocation request (per work-group).
  */
  uint32_t  group_segment_size_bytes;


  /**
     Address of an object in memory that includes an
     implementation-defined executable ISA image for the kernel.
  */
  uint64_t  kernel_object_address;


  /**
     Address of memory containing kernel arguments.
  */
  uint64_t  kernarg_address;


  /**
    reserved
  */
  uint64_t  reserved3;


  /**
      HSA signaling object used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_dispatch_packet_t;
/** @}***********************************************************************/


/** \addtogroup STRbarrier_packet -------------------------------------------
 *  @{
 */
typedef struct hsa_aql_barrier_packet_s{
  /**
     packet header packet header structure
   */
  hsa_aql_packet_header_t header;


  /**
     reserved
   */
  uint32_t reserved2;


  /**
     The first dependency signal, a negative value means dependency
     not met and the completion signal for this packet will be set to
   */
  uint64_t dep_signal0;


  /**
     The first dependency signal, a negative value means dependency
     not met and the completion signal for this packet will be set to
   */
  uint64_t dep_signal1;


  /**
     The first dependency signal, a negative value means dependency
     not met and the completion signal for this packet will be set to
   */
  uint64_t dep_signal2;


  /**
     The first dependency signal, a negative value means dependency
     not met and the completion signal for this packet will be set to
   */
  uint64_t dep_signal3;


  /**
     The first dependency signal, a negative value means dependency
     not met and the completion signal for this packet will be set to
   */
  uint64_t dep_signal4;


  /**
     reserved
  */
  uint64_t reserved3;


  /**
     HSA signaling object used to indicate completion of the dependency
     resolution, success of failure
   */
  uint64_t completion_signal;


} hsa_aql_barrier_packet_t;
/** @}***********************************************************************/


/** \addtogroup STRagent_packet -------------------------------------------
 *  @{
 */
typedef struct hsa_aql_agent_dispatch_packet_s{
  /**
     packet header packet header structure
   */
  hsa_aql_packet_header_t header;

  /**
   * The function to be performed by the destination HSA Agent. The type value
   * is split into the following ranges:
   *   0x0000:0x3FFF – Vendor specific
   *   0x4000:0x7FFF – HSA runtime
   *   0x8000:0xFFFF – User registered function
   */
  uint16_t  type;

  /**
   * Must be 0.
   */
  uint32_t  reserved2;

  /**
   * Pointer to location to store the function return value(s) in.
   */
  uint64_t  returnLocation;

  /**
   * 64-bit direct or indirect arguments.
   */
  uint64_t  arg0;

  /**
   * 64-bit direct or indirect arguments.
   */
  uint64_t  arg1;

  /**
   * 64-bit direct or indirect arguments.
   */
  uint64_t  arg2;

  /**
   * 64-bit direct or indirect arguments.
   */
  uint64_t  arg3;

  /**
   * Must be 0.
   */
  uint64_t  reserved3;

  /**
   * Address of HSA signaling object used to indicate completion of the job.
   */
  uint64_t  completionSignal;

} hsa_aql_agent_dispatch_packet_t;
/** @}***********************************************************************/


/** \addtogroup ENUservice_queue_type -------------------------------------------
 *  @{
 */
typedef enum hsa_service_queue_type_s {
  /**
   * donot return a service queue, the service queue pointer will be set to
   * NULL by the runtime
   */
  NONE=0,

  /**
   * use a common runtime provided service queue
   */
  COMMON,

  /**
   * create a new service queue
   */
  NEW,

  /**
   * create a queue that runtime will not manage, the application has to manage
   * the queue manually.
   */
  APPLICATION_MANAGED
} hsa_service_queue_type_t;
/** @}***************************************************************/


/** \addtogroup APIqueue_create ----------------------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param component
 *      input. The component on which this queue is to be created.
 * @param size
 *      input. Size of the queue memory in number of packets in is expected to hold.
 * @param queue_type
 *      input. type of the queue (only type 0, which is default in-order issue
 *      queue, is supported at this time).
 * @param service_queue_type
 *      input. The user can choose between NONE (no service queue), COMMON
 *      (runtime provided service queue that is shared), NEW (require the
 *      runtime to create a new queue).
 * @param context
 *      input. The context in which this queue is being created. Any
 *      errors/notifications will be reported via callbacks registered in the
 *      same context.
 * @param queue
 *      runtime allocated, output. The queue structure, filled up and returned
 *      by the runtime.
 * @param mailbox
 *      mailbox to gather execution information to be used for debug trap. User
 *      may pass NULL here if the user doesn't want the mailbox created.
 */
hsa_status_t hsa_queue_create(const hsa_agent_t *component,
                                         size_t size,
                                         uint32_t queue_type,
                                         hsa_service_queue_type_t service_queue_type,
                                         hsa_runtime_context_t *context,
                                         hsa_queue_t **queue,
                                         hsa_queue_mailbox_t *mailbox);
/** @}***********************************************************************/


/** \addtogroup APIqueue_inactivate ---------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param queue
 *      input. The queue that needs to be inactivated.
 */
hsa_status_t hsa_queue_inactivate(hsa_queue_t *queue);
/** @}***********************************************************************/


/** \addtogroup APIqueue_destroy ---------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param queue
 *      input. The queue structure that points to the queue that needs to be
 *      destroyed, after destruction base\_address or the rest of the queue
 *      structure, even if cached by the user, are no longer valid.
 *
 */
hsa_status_t hsa_queue_destroy(hsa_queue_t *queue);
/** @}***********************************************************************/


/** \addtogroup APIqueue_update  ----------------------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 */
uint64_t hsa_queue_get_read_index_relaxed(hsa_queue_t *queue);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 */
uint64_t hsa_queue_get_read_index_acquire(hsa_queue_t *queue);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 */
uint64_t hsa_queue_get_write_index_relaxed(hsa_queue_t *queue);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 */
uint64_t hsa_queue_get_write_index_acquire(hsa_queue_t *queue);


/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param val
 *      input. The new value of the write index.
 */
uint64_t hsa_queue_set_write_index_relaxed(hsa_queue_t *q,
    uint64_t val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param val
 *      input. The new value of the write index.
 */
uint64_t hsa_queue_set_write_index_release(hsa_queue_t *q,
    uint64_t val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param old_val
 *      input. The value to compare with
 * @param new_val
 *      input. If a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index(hsa_queue_t *q,
    uint64_t old_val,
    uint64_t new_val);


/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param old_val
 *      input. The value to compare with
 * @param new_val
 *      input. If a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index_release(hsa_queue_t *q,
    uint64_t old_val,
    uint64_t new_val);


/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param old_val
 *      input. The value to compare with
 * @param new_val
 *      input. If a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index_acquire(hsa_queue_t *q,
    uint64_t old_val,
    uint64_t new_val);


/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param old_val
 *      input. The value to compare with
 * @param new_val
 *      input. If a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index_relaxed(hsa_queue_t *q,
    uint64_t old_val,
    uint64_t new_val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure.
 * @param old_val
 *      input. The value to compare with
 * @param new_val
 *      input. If a match is determined, the write index is updated with new\_val
 */
uint64_t hsa_queue_cas_write_index_acquire_release(hsa_queue_t *q,
    uint64_t old_val,
    uint64_t new_val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure
 * @param val
 *      input. The value to add to the write index
 */
uint64_t hsa_queue_add_write_index_relaxed(hsa_queue_t *q,
    uint64_t val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure
 * @param val
 *      input. The value to add to the write index
 */
uint64_t hsa_queue_add_write_index_acquire(hsa_queue_t *q,
    uint64_t val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure
 * @param val
 *      input. The value to add to the write index
 */
uint64_t hsa_queue_add_write_index_release(hsa_queue_t *q,
    uint64_t val);

/**
 * @brief
 *
 * @param queue
 *      input. The HSA queue structure
 * @param val
 *      input. The value to add to the write index
 */
uint64_t hsa_queue_add_write_index_acquire_release(hsa_queue_t *q,
    uint64_t val);

/** @}***********************************************************************/


/** \addtogroup APIqueue_error  ----------------------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param error_code
 *      input. the error code to convert to string
 * @param error_string
 *      output. the string description of the error code
 */
hsa_status_t hsa_queue_error_string_from_code(
                uint64_t error_code,
                char **error_string);
/** @}***********************************************************************/


/** \addtogroup APIpacket_validate -------------------------------------------
 *  @{
 */
/**
 * @brief
 * @param aql_packet
 *      input. Pointer to the AQL packet structure of the packet that needs
 *      to be validated
 */
hsa_status_t hsa_aql_dispatch_packet_validate(hsa_aql_dispatch_packet_t *aql_packet);
/** @}***********************************************************************/


/**
 * @brief
 * @param aql_packet
 *      input. Pointer to the AQL packet structure of the packet that needs
 *      to be validated
 */
hsa_status_t hsa_aql_barrier_packet_validate(hsa_aql_barrier_packet_t *aql_packet);
/** @}***********************************************************************/


/** \addtogroup STRhsa_symbol ----------------------------------------
 *  @{
 */
typedef struct hsa_symbol_s{
    /**
     * current binding of that symbol
     */
    uint64_t current_binding;

    /**
     * directive section offset in BRIG
     */
    uint32_t directive_section_offset;
} hsa_symbol_t;
/** @}***********************************************************************/


/** \addtogroup STRhsa_symbol_map ----------------------------------------
 *  @{
 */
typedef struct hsa_symbol_map_s{
    /**
     * the count of the total number of symbols in this map
     */
    uint32_t number_of_symbols;

    /**
     * the array of symbols
     */
    hsa_symbol_t *symbol;

    /**
     * for implementations to store other information and allow for queries
     */
    uint64_t reserved;
} hsa_symbol_map_t;
/** @}***************************************************************/

/** \addtogroup ENUhsail_profile -------------------------------------------
 *  @{
 */
typedef enum hsa_profile_s {
    /**
     * the base profile, as defined in PRM/SAR
     */
    HSA_PROFILE_BASE = 0,

    /**
     * the full profile, as defined in PRM/SAR
     */
    HSA_PROFILE_FULL = 1
}hsa_profile_t;
/** @}***************************************************************/

typedef uint16_t hsa_exception_kind16_t;

/** \addtogroup ENUexception_type -------------------------------------------
 *  @{
 */
typedef enum hsa_exception_kind_mask_s {
  /**
   * IEEE 754 INVALID operation exception
   */
  HSA_EXCEPTION_INVALID_OPERATION = 1,

  /**
   * an operation on finite operands gives an exact infinite result
   */
  HSA_EXCEPTION_DIVIDE_BY_ZERO = 2,

  /**
   * a result is too large to be represented correctly
   */
  HSA_EXCEPTION_OVERFLOW = 4,

  /**
   * a result is very small (outside the normal range) and inexact
   */
  HSA_EXCEPTION_UNDERFLOW = 8,

  /**
   * returns correctly rounded result by default
   */
  HSA_EXCEPTION_INEXACT = 16

}hsa_exception_kind_mask_t ;
/** @}***************************************************************/

typedef uint64_t hsa_control_directive_present64_t;
typedef uint32_t hsa_kernel_code_version32_t;

/** \addtogroup ENUdirective_present -------------------------------------------
 *  @{
 */
typedef enum hsa_control_directive_present_mask_t {
  /**
   * mask that indicates break on exceptions is required by the user, the
   * kernel pauses execution and the queue mailbox signal is signaled and
   * mailbox updated.
   */
  HSA_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,

  /**
   * says that exeptions are recorded
   */
  HSA_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,

  /**
   * says that max for dynamic group size is specified
   */
  HSA_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,

  /**
   * if enabled
   */
  HSA_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256

} hsa_control_directive_present_mask_t;

/** @}***************************************************************/

//TODO
typedef struct hsa_dim3_t {
  union {
    struct {
      uint32_t x;
      uint32_t y;
      uint32_t z;
    };
    uint32_t dim[3]; // The dimensions may also be accessed as an array using the HsaDim.
  };
} hsa_dim3_t;


/** \addtogroup STRcontrol_directive -------------------------------------------
 *  @{
 */
typedef struct hsa_control_directives_s {
  /**
   If the value is 0 then there are no control directives specified and the
   rest of the fields can be ignored. The bits are accessed using the
   hsa\_control\_directives\_present\_mask\_t. Any control directive that is not
   enabled in this bit set must have the value of all 0s.
   */
  hsa_control_directive_present64_t enabled_control_directives;

  /**
   If enable break exceptions is not enabled in {\itshape
   enabled\_control\_directives}, then must be 0, otherwise must be non-0 and
   specifies the set of HSAIL exceptions that must have the BREAK policy
   enabled.  If the HSAIL kernel being finalized has any enablebreakexceptions
   control directives, then the values specified by this argument are unioned
   with the values in these control directives. If any of the functions the
   kernel calls have an enablebreakexceptions control directive, then they must
   be equal or a subset of, this union.
   */
  hsa_exception_kind16_t enable_break_exceptions;

  /**
   If enable detect exceptions is not enabled in {\itshape
   enabled\_control\_directives}, then must be 0, otherwise must be non-0 and
   specifies the set of HSAIL exceptions that must have the DETECT policy
   enabled.  If the kernel being finalized has any enabledetectexceptions
   control directives, then the values specified by this argument are unioned
   with the values in these control directives. If any of the functions the
   kernel calls have an enabledetectexceptions control directive, then they
   must be equal or a subset of, this union.
   */
  hsa_exception_kind16_t enable_detect_exceptions;

  /**
   If max dynamic group size is not enabled in {\itshape
   enabled\_control\_directives} then this must be 0, and any amount of dynamic
   group segment can be allocated for a dispatch, otherwise the value specifies
   the maximum number of bytes of dynamic group segment that can be allocated
   for a dispatch. If the kernel being finalized has any maxdynamicsize control
   directives, then the values must be the same, and must be the same as this
   argument if it is enabled. This value can be used by the finalizer to
   determine the maximum number of bytes of group memory used by each
   work-group by adding this value to the group memory required for all group
   segment variables used by the kernel and all functions it calls, and group
   memory used to implement other HSAIL features such as fbarriers and the
   detect exception operations. This can allow the finalizer to determine the
   expected number of work-groups that can be executed by a compute unit and
   allow more resources to be allocated to the work-items if it is known that
   fewer work-groups can be executed due to group memory limitations.
   */
  uint32_t max_dynamic_group_size;

  /**
   If this is is not enabled in {\itshape enabled\_control\_directives} then
   must be 0, otherwise must be greater than 0.
   See HSA Programmer's Reference Manual description of maxflatgridsize control
   directive.
   */
  uint32_t max_flat_grid_size;

  /**
   If this is is not enabled in {\itshape enabled\_control\_directives} then
   must be 0, otherwise must be greater than 0.
   See HSA Programmer's Reference Manual description of maxflatgridsize control
   directive.
   */
  uint32_t max_flat_workgroup_size;

  /**
   If this is is not enabled in {\itshape enabled\_control\_directives} then
   must be 0 and the finalizer may generate ISA that could result in any number
   of work-groups executing on a single compute unit.
   Otherwise, the finalizer will \emph{attempt} to generate ISA
   that will allow the specified number of work-groups to execute
   on a single compute unit. This is only a hint and can be ignored
   by the finalizer. If the kernel being finalized, or any of the
   functions it calls, has the same control directive, then the
   values must be the same or the finalization can fail. This can be used to
   determine the number of resources that should be allocated to a single
   work-group and work-item.
   */
  uint32_t requested_workgroups_per_cu;

  /**
   If not enabled then all elements for Dim3 must be 0, otherwise
   every element must be greater than 0. See HSA Programmer's
   Reference Manual description of requiredgridsize control
   directive.
   */
  hsa_dim3_t required_grid_size;

  /**
   If not enabled then all elements for Dim3 must be 0, and the produced code
   can be dispatched with any legal work-group range consistent with the
   dispatch dimensions. Otherwise, the code produced must always be dispatched
   with the specified work-group range. No element of the specified range must
   be 0. It must be consistent with required\_dimensions and
   max\_flat\_workgroup\_size. If the kernel being finalized, or any of the
   functions it calls, has a requiredworkgroupsize control directive, then the
   values must be the same. Specifying a value can allow the finalizer to
   optimize work-group id operations, and if the number of work-items in the
   work-group is less tha the WAVESIZE then barrier operations can be optimized
   to just a memory fence.
   */
  hsa_dim3_t required_workgroup_size;

  /**
   If required\_dim is not enabled then must be 0 and the produced
   kernel code can be dispatched with 1, 2 or 3 dimensions. If
   enabled then the value is 1..3 and the code produced must only
   be dispatched with a dimension that matches. Other values are
   illegal. If the kernel being finalized, or any of the functions
   it calls, has a requireddimsize control directive, then the
   values must be the same. This can be used to optimize the code
   generated to compute the absolute and flat work-group and
   work-item id, and the dim HSAIL operations.
  */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];

} hsa_control_directives_t;
/** @}***************************************************************/



/// The current version number of the HSA code object
/// format.
typedef uint32_t hsa_code_version32_t;
typedef uint32_t hsa_code_kind32_t;

/** \addtogroup ENUhsa_codeversion -------------------------------------------
 *  @{
 */
typedef enum hsa_code_version_t {
  /**
   * Code version
   */
  HSA_CODE_VERSION = 0
} hsa_code_version_t;
/** @}***************************************************************/

/** \addtogroup STRhsa_codeheader -------------------------------------------
 *  @{
 */
typedef struct hsa_code_s {
  /**
   The code format version. The version of this defintion is
   specified by HSA\_CODE\_VERSION. Must match the value in the
   hsa\_compilationunit\_code\_t that contains it.
   */
  hsa_code_version32_t code_version;

  /**
   The byte size of the struct that contains this hsa\_code\_t. Must
   be set to sizeof(hsa\_*\_code\_t). Used for backward compatibility.
   */
  uint32_t struct_byte_size;

  /**
   Offset from base of hsa\_code\_t to compilationunit\_code\_t that
   contains this hsa\_code\_t to the base of this hsa\_code\_t. Can be
   used to navigate back to the enclosing compilation unit. Since
   hsa\_compilationunit\_code\_t is always at offset 0, this value
   must be negative.
   */
  int64_t compilationunit_byte_offset;

  /**
   * Type of code object.
   */
  hsa_code_kind32_t code_type;

} hsa_code_t;
/** @}***************************************************************/

typedef uint8_t powertwo8_t;
/// Value expressed as a power of two.
typedef uint8_t hsa_powertwo8_t;
enum hsa_powertwo_t {
  HSA_POWERTWO_1 = 0,
  HSA_POWERTWO_2 = 1,
  HSA_POWERTWO_4 = 2,
  HSA_POWERTWO_8 = 3,
  HSA_POWERTWO_16 = 4,
  HSA_POWERTWO_32 = 5,
  HSA_POWERTWO_64 = 6,
  HSA_POWERTWO_128 = 7,
  HSA_POWERTWO_256 = 8
};

/** \addtogroup STRcode_object -------------------------------------------
 *  @{
 */
typedef struct hsa_kernel_code_s{

  /**
   Common header that all code objects start with. code.type must
   be HSA\_CODE\_KERNEL.
   */
  hsa_code_t code;

  /**
   The amount of group segment memory required by a work-group in
   bytes. This does not include any dynamically allocated group
   segment memory that may be added when the kernel is
   dispatched.
   */
  uint32_t workgroup_group_segment_byte_size;

  /**
   The size in bytes of the kernarg segment that holds the values
   of the arguments to the kernel.
   */
  uint64_t kernarg_segment_byte_size;

  /**
    The amount of memory required for the combined private, spill
    and arg segments for a work-item in bytes.
  */
  uint32_t workitem_private_segment_byte_size;

  /**
   Number of fbarrier's used in the kernel and all functions it
   calls. If the implemenation uses group memory to allocate the
   fbarriers then that amount must already be included in the
   {\itshape workgroup\_group\_segment\_byte\_size} total.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   The values are the actually values used by the finalizer
   in generating the code. This may be the union of values
   specified as finalizer arguments and explicit HSAIL control
   directives. If a finalizer implementation ignores a control
   directive, and not generate constrained code, then the control
   directive will not be marked as enabled even though it was
   present in the HSAIL or finalizer argument. The values are
   intended to reflect the constraints that the code actually
   requires to correctly execute, not the values that were
   actually specified at finalize time.
   */
  hsa_control_directives_t control_directive;

  /**
   Wavefront size expressed as a power of two. Must be a power of 2
   in range 1..64 inclusive. Used to support runtime query that
   obtains wavefront size, which may be used by application to
   allocated dynamic group memory and set the dispatch work-group
   size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   The maximum byte alignment of variables used by the kernel in
   the specified memory segment. Expressed as a power of two. Must
   be at lease HSA\_POWERTWO\_16.
   */
  hsa_powertwo8_t kernarg_segment_alignment;
  hsa_powertwo8_t group_segment_alignment;
  hsa_powertwo8_t private_segment_alignment;

  /**
   The optimization level specified when the kernel was
   finalized.
   */
  uint8_t optimization_level;

  /**
   * Reserved. Must be 0.
   * Component specific fields can follow this field.
   */
  uint8_t reserved1[3];

} hsa_kernel_code_t;
/** @}***************************************************************/


/** \addtogroup STRhsa_brig -------------------------------------------
 *  @{
 */
typedef struct hsa_brig_s{
    /**
     * From PRM: string section, containing all character strings and byte data
     * used in the compilation unit.
     */
    uint8_t *string_section;

    /**
     * The directives, which provide information for the finalizer. The
     * directives do not generate code.
     */
    uint8_t *directive_section;

    /**
     * All of the executable operations. Most operations contain offsets to
     * the .operand section.
     */
    uint8_t *code_section;

    /**
     * The operands, such as immediate constants, registers, and address
     * expressions, that appear in the operations.
     */
    uint8_t *operand_section;
} hsa_brig_t;
/** @}***********************************************************************/


/** \addtogroup STRhsa_codeentry -------------------------------------------
 *  @{
 */
typedef struct hsa_code_entry_s {
  /**
   ID of the entity that generated the code. For HSAIL will be the
   BRIG directive offset of the kernel or function declaration. The
   array of hsa\_code\_entry\_t are required to be ordered in
   ascending code\_id to allow faster lookup.
   */
  uint64_t code_id;

  /**
   Byte offset from start of hsa\_compilationunit\_code\_t to
   corresponding hsa\_*\_code\_t. Every hsa\_*\_code\_t starts with a
   common hsa\_code\_t, and hsa\_code\_t.code\_type indicates what
   specific hsa\_*\_code\_t it is.
   */
  int64_t code_byte_offset;
} hsa_code_entry_t;
/** @}***********************************************************************/



/** \addtogroup ENUhsa_codekind -------------------------------------------
 *  @{
 */
typedef enum hsa_code_kind_t {
  /**
   * Not a code object
   */
  HSA_CODE_NONE = 0,

  /**
   * HSAIL kernel that can be used with an AQL dispatch packet.
   */
  HSA_CODE_KERNEL = 1,

  /**
   * HSAIL function.
   */
  HSA_CODE_FUNCTION = 2,

  /**
   HSA runtime code objects. For example, partially linked code
   objects.
   */
  HSA_CODE_RUNTIME_FIRST = 0x40000000,
  HSA_CODE_RUNTIME_LAST = 0x7fffffff,

  /**
   * Vendor specific code objects.
   */
  HSA_CODE_VENDOR_FIRST = 0x80000000,
  HSA_CODE_VENDOR_LAST = 0xffffffff,
} hsa_code_kind_t;
/** @}***********************************************************************/


typedef uint8_t hsa_machine_model8_t;
typedef uint8_t hsa_profile8_t;
typedef uint32_t hsa_code_properties32_t;
typedef uint16_t hsa_target_options16_t;


/** \addtogroup ENUcodeproperties -------------------------------------------
 *  @{
 */
typedef enum hsa_code_properties_mask_t {
  /**
   * The code is position independent (can be executed at any address
   * that meets the alignment requirement).
   */
  HSA_CODE_PROPERTY_PIC = 1 << 0
} hsa_code_properties_mask_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_compilationunit -------------------------------------------
 *  @{
 */
typedef struct hsa_compilationunit_code_s{
  /**
   The code format version. The version of this defintion is
   specified by HSA\_CODE\_VERSION.
   */
  hsa_code_version32_t code_version;

  /**
   The byte size of this struct. Must be set to
   sizeof(hsa\_compilationunit\_code\_t). Used for backward
   compatibility.
   */
  uint32_t struct_byte_size;

  /**
   The vendor of the HSA Component on which this Kernel Code object can
   execute. ISO/IEC 624 character encoding must be used. If the
   name is less than 16 characters then remaining characters must
   be set to 0.
   */
  char component_vendor[16];

  /**
   The vendor's name of the HSA Component on which this Kernel Code object
   can execute. ISO/IEC 646 character encoding must be used. If the
   name is less than 16 characters then remaining characters must
   be set to 0.
   */
  char component_name[16];

  /**
   Byte offset from start of hsa\_compilationunit\_code\_t to an array
   of code\_entry\_count elements of type hsa\_code\_entry\_t. Since
   hsa\_compilationunit\_code\_t is always at offset 0, this value
   must be positive.
   */
  int64_t code_entry_byte_offset;

  /**
   Number of code entries in this compilation unit.
   */
  uint32_t code_entry_count;

  /**
   The required alignment of this hsa\_compilationunit\_code\_t
   expressed as a power of 2. The Finalizer must set this to the
   value required by the HSA component it will execute on and the
   assumptions of the machine code it contains.
   */
  hsa_powertwo8_t code_alignment;

  /**
   Must be 0.
   */
  uint8_t reserved[3];

  /**
   The size of the single contiguous block of memory which includes
   this hsa\_compilationunit\_code\_t header and all following
   hsa\_*\_code\_t and associated machine code.
   */
  uint64_t code_size_bytes;

  /**
   The base address that this hsa\_compilationunit\_code\_t must be
   allocated in order to execute the code it contains. The address
   must be a multiple of the alignment specified by the alignment
   field. If the code is position independent (can be executed at
   any address that meets the alignment requirement), then this
   field must be 0.
   */
  uint64_t code_base_address;

  /**
   A bit set of flags providing inforamtion about the code in this
   compilation unit. Unused flags must be 0.
   */
  hsa_code_properties32_t code_properties;

  /**
   The HSAIL major version. This information is from the HSAIL
   version directive. If this hsa\_compilationunit\_code\_t is not
   generated from an HSAIL compilation unit then must be 0.
   */
  uint32_t hsail_version_major;

  /**
   The HSAIL minor version. This information is from the HSAIL
   version directive. If this hsa\_compilationunit\_code\_t is not
   generated from an HSAIL compilation unit then must be 0.
   */
  uint32_t hsail_version_minor;

  /**
   The HSAIL profile defines which features are used. This
   information is from the HSAIL version directive. If this
   hsa\_compilationunit\_code\_t is not generated from an HSAIL
   compilation unit then must still indicate what profile is being
   used.
   */
  hsa_profile8_t hsail_profile;

  /**
   The HSAIL machine model gives the address sizes used by the
   code. This information is from the HSAIL version directive. If
   not generated from an HSAIL compilation unit then must still
   indicate for what machine mode the code is generated.
   */
  hsa_machine_model8_t hsail_machine_model;

  /**
   The HSAIL target features. There are currently no target options
   so this field must be 0. If target options are added they will
   be specified by the HSAIL version directive. If this
   hsa\_compilationunit\_code\_t is not generated from an HSAIL
   compilation unit then must be 0.
   */
  hsa_target_options16_t hsail_target_options;

} hsa_compilationunit_code_t;
/** @}***********************************************************************/

typedef uint64_t hsa_debug_info_t;
typedef uint64_t hsa_finalize_compilationunit_caller_t;
typedef uint32_t hsa_brig_directive_offset_t;
typedef hsa_status_t (*hsa_map_symbol_address_t)(
      hsa_finalize_compilationunit_caller_t caller,
        hsa_brig_directive_offset_t symbol_directive,
          uint64_t* address);

/** \addtogroup STRcompilation_debug -------------------------------------------
 *  @{
 */
typedef struct hsa_compilationunit_debug_s {
   /**
    * HSA does not define the debug format, it is device
    * specific. It is not required to be implemented as a contiguous
    * region of memory.
    */
  uint64_t reserved;
} hsa_compilationunit_debug_t;
/** @}***********************************************************************/

typedef void* hsa_finalize_brig_caller_t;

typedef hsa_status_t (*hsa_alloc_t)(
      hsa_finalize_brig_caller_t caller,
        size_t byte_size,
          size_t byte_alignment,
            void** address);


/** \addtogroup APIfinalize_brig -------------------------------------------
 *  @{
 */

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
 * @param caller
 *      Opaque pointer and will be passed to all call back functions made by
 *      this call of the finalizer.
 * @param component
 *      input. A valid pointer to the hsa\_agent\_t.
 * @param brig
 *      input. A pointer to the in memory BRIG structure.
 * @param code_count
 *      The number of kernels plus functions to produce hsa\_kernel\_code\_t and
 *      hsa\_function\_code\_t objects for in the generated
 *      hsa\_compilationunit\_code\_t.
 * @param code_directive
 *      A pointer to an array with code\_count entries of
 *      hsa\_brig\_directive\_offset\_t. Each entry is the offset in the directive
 *      section of the passed brig of a kernel or function definition. These
 *      will be the kernels and functions that will have code objects generated
 *      in the produced hsa\_compilationunit\_code\_t
 * @param control_directives
 *      The control directives that can be specified to influence how the
 *      finalizer generates code. If NULL then no control directives are used.
 * @param map_symbol_address
 *      Used by the finalizer to obtain the segment address for global
 *      segment symbols. The value of caller will be passed to every call.
 * @param allocate_compilationunit_code
 *      The callback function that the finalizer will use to allocate the
 *      contiguous block of memory that will be used for the
 *      hsa\_compilationunit\_code\_t that is returned. It is the
 *      responsibility of the call of the finalizer to deallocate this memory,
 *      even if the finalizer does not report success.
 * @param allocate_compilationunit_debug
 *      The callback function that the finalizer will use to allocate the
 *      memory that will be used for the hsa\_compilationunit\_debug\_t that is
 *      returned. It is the responsibility of the call of the finalizer to
 *      destroy this memory, even if the finalizer does not report success.
 * @param optimization_level
 *      an implementation defined value that control the level of optimization
 *      performed by the finalizer.
 * @param options
 *      implementation defined options that can be specified to the finalizer.
 *      compilationunit\_code: if the return status is success then a pointer to
 *      the generated hsa\_compilationunit\_code\_t for the HSA component must
 *      be written.
 *
*/
hsa_status_t hsa_finalize_brig( hsa_finalize_compilationunit_caller_t caller,
  hsa_agent_t* component,
  hsa_brig_t* brig,
  size_t code_count,
  hsa_brig_directive_offset_t* code_directive,
  hsa_control_directives_t* control_directives,
  hsa_map_symbol_address_t map_symbol_address,
  hsa_alloc_t allocate_compilationunit_code,
  hsa_alloc_t allocate_compilationunit_debug,
  uint8_t optimization_level,
  const char* options,
  hsa_compilationunit_code_t** compilationunit_code,
  hsa_compilationunit_debug_t** compilationunit_debug);
/** @}***********************************************************************/


/** \addtogroup APIfinalize_destroy ------------------------------------
 *  @{
 */

/**
 * @brief
 *
 * @param code_object
 *        input. a pointer to the compilation unit object that needs to be destroyed.
*/
hsa_status_t hsa_compilationunit_code_destroy(hsa_compilationunit_code_t *code_object);


/**
 * @brief
 *
 * @param debug_object
 *        input. a pointer to the compilation unit debug object that needs to
 *        be destroyed.
*/
hsa_status_t hsa_compilationunit_debug_destroy(hsa_compilationunit_debug_t *debug_object);
/** @}***********************************************************************/


/** \addtogroup APIfinalize_serial ------------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param code_object
 *      input. The code object to serialize.
 * @param size
 *      output. Size of the serialized object that is generated.
 * @param serialized_object
 *      Output. Pointer to the serialized object.
*/
hsa_status_t hsa_compilationunit_serialize(hsa_compilationunit_code_t *code_object,
    hsa_alloc_t allocate_compilationunit_code,
    void *serialized_object);

/**
 * @brief
 *
 * @param serialized_object
 *      input. Pointer to the serialized object.
 * @param size
 *      input. Size of the serialized object that is generated.
 * @param code_object
 *      runtime allocated, output. The code object generated as a part of serialization.
*/
hsa_status_t hsa_compilationunit_deserialize( void *serialized_object,
    hsa_compilationunit_code_t **code_object);
/** @}***********************************************************************/


/** \addtogroup APIbind_kernel_code -------------------------------------------
 *  @{
 */
/**
 * @brief
 *
 * @param kernel
 *      input. A pointer to the kernel code object.
 * @param symbol_map
 *      input. A pointer to the symbol information structure.
 * @param code_object
 *      runtime allocated, output. A pointer to the kernel code object.
*/
hsa_status_t hsa_symbol_bind_code_object( hsa_code_t *kernel,
    hsa_symbol_map_t *input_symbol_map,
    hsa_code_t *output_kernel);
/** @}***********************************************************************/


/** \addtogroup APIquery_symbol -------------------------------------------
 *  @{
 */
/**
 *******************************************************************************
 * @brief
 *
 * @param symbol_map
 *      input. a pointer to the kernel code object.
 * @param brig
 *      input. the the brig this symbol table came from.
 * @param symbol_name
 *      input. the name of the symbol as a char *.
 * @param symbol_details
 *      runtime allocated, output. Structure with symbol details.
 * @return -- tbd
*/
hsa_status_t hsa_symbol_map_query_by_name(hsa_symbol_map_t *symbol_map,
    hsa_brig_t brig,
    char *symbol_name,
    hsa_symbol_t *symbol_details);

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
    hsa_symbol_t *symbol_details);

/**
 * @brief
 *
 * @param symbol_map
 *      input. destroys the symbol map resouces including any additional structures
 * @return -- tbd
*/
hsa_status_t hsa_symbol_destroy(hsa_symbol_map_t *symbol_map);
/** @}***********************************************************************/


/** \addtogroup APIregister -------------------------------------------
 *  @{
 */
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
 *      input. A pointer to the base of the memory region to be registered. If a null
 *      pointer is passed, no operation is performed.
 * @param size
 *      input. Requested registration size in bytes. If a size of zero is passed, no
 *      operation is performed.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_register(void *address,
    size_t size);

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
 * @param address
 *      input. address A pointer to the base of the memory region to be deregistered.
 *      If a NULL pointer is passed, no operation is performed.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_deregister(void *address);
/** @}***********************************************************************/

/** \addtogroup APImemory_allocate -------------------------------------------
 *  @{
 */

/**
 * @brief Allocate memory that is registered
 *
 * @param size_bytes
 *      input, size of the segment that needs to be allocated in
 *      bytes.
 * @param address
 *      input, address pointer allocated by the user. Dereferenced
 *      and assigned to the pointer to the memory allocated for this
 *      request.
 */
hsa_status_t hsa_memory_allocate(size_t size_bytes, void **address);

/** @}***********************************************************************/

/** \addtogroup APImemory_local -------------------------------------------
 *  @{
 */
/**
 *******************************************************************************
 * @brief Allocate memory on HSA Device
 * @details The hsa_memory_allocate_component_local HSA core interface is used for
 * allocating global device memory associated with specified device.
 *
 * @param component
 *      input. A valid pointer to the HSA device for which the specified amount of
 *      global memory is to be allocated.
 * @param size
 *      input. Requested allocation size in bytes. If size is 0, NULL is returned.
 * @param address
 *      output. A valid pointer to the location of where to return the pointer to the
 *      base of the allocated region of memory.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_allocate_component_local( const hsa_agent_t *component,
    size_t size,
    void **address);

/**
 *******************************************************************************
 * @brief Deallocate memory on HSA Device
 * @details The hsa_memory_free_component_local HSA core interface is used for
 * deallocating global device memory that was previously allocated with
 * ::HsaAllocateDeviceMemory.
 *
 * @param address
 *      input. A pointer to the address to be deallocated. If the pointer is NULL, no
 *      operation is performed.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_free_component_local(void *address);

/**
 * @brief Copies the values of @a size bytes from the location pointed by @a src
 * directly to the memory block pointed by @a dst.

 * @details The ::HsaCopyMemory interface is used for copying memory in the
 * HSA address space. If the specified pointers do not belong to the HSA address
 * space, an error is returned. If the size of the destination or source arrays
 * is smaller than the indicated size, the behavior is undefined.
 *
 * @param dst
 *      user allocated, output. A valid pointer to the destination array where the content is to be
 *      copied.
 * @param src
 *      input. A valid pointer to the source of data to be copied.
 * @param size
 *      input. Number of bytes to copy.
 * @param signal
 *      input. The signal that will be incremented by the runtime
 *      when the copy is complete.
 */
hsa_status_t hsa_memory_copy_component_local_to_system(void *dst,
    const void *src,
    size_t size,
    hsa_signal_handle_t signal);
/** @}***********************************************************************/

/** \addtogroup APIhsa_get_image_format_capability -------------------------------------------
 *  @{
 */
/**
 * @brief todo
 *
 * @param component
 *       (in) HSA device to be associated with the image.
 * @param image_format
 *       (in) Image format.
 * @param image_geometry
 *       (in) Geometry of the image.
 * @param capability_mask
 *       (out) Image format capability bit-mask.
 */
hsa_status_t hsa_get_image_format_capability(
                         const hsa_agent_t *component,
                         const hsa_image_format_t *image_format,
                         hsa_image_geometry_t image_geometry,
                         uint32_t *capability_mask);
/** @}**************************************************************/

/** \addtogroup APIhsa_get_image_info -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param image_descriptor
 *        (in) Implementation-independent image descriptor describing the image.
 * @param image_info
 *        (out) Image info size and alignment requirements that the HSA agent requires.
 */
hsa_status_t hsa_get_image_info(
                         const hsa_agent_t *component,
                         const hsa_image_descriptor_t *image_descriptor,
                         hsa_image_info_t *image_info);
/** @}**************************************************************/

/** \addtogroup APIhsa_create_image_handle -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param image_descriptor
 *        (in) implementation-independent image descriptor describing the image
 * @param image_data
 *        (in) address of the component-specific image data
 * @param image_handle
 *        (out) HSA component-specific image handle
 */
hsa_status_t hsa_create_image_handle (
                         const hsa_agent_t *component,
                         const hsa_image_descriptor_t *image_descriptor,
                         const void *image_data,
                         hsa_image_handle_t *image_handle);

/** @}**************************************************************/

/** \addtogroup APIhsa_import_image -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param src_memory
 *        (in) source memory
 * @param src_row_pitch
 *        (in) number of bytes in one row of the source memory
 * @param src_slice_pitch
 *        (in) number of bytes in one slice of the source memory
 * @param dest_image_handle
 *        (in) destination HSA image handle
 * @param image_region
 *        (in) image region to be updated
 * @param completion_signal
 *        (in) signal to set when the operation is completed
 */
hsa_status_t hsa_import_image (
                         const hsa_agent_t *component,
                         const void *src_memory,
                         size_t src_row_pitch,
                         size_t src_slice_pitch,
                         hsa_image_handle_t dest_image_handle,
                         const hsa_image_region_t *image_region,
                         const hsa_signal_handle_t *completion_signal);
/** @}**************************************************************/

/** \addtogroup APIhsa_export_image -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param src_image_handle
 *        (in) source HSA image handle
 * @param dst_memory
 *        (in) destination memory
 * @param dst_row_pitch
 *        (in) number of bytes in one row of the destination memory
 * @param dst_slice_pitch
 *        (in) number of bytes in one slice of the destination  memory
 * @param image_region
 *        (in) image region to be exported
 * @param completion_signal
 *        (in) signal to set when the operation is completed
 */
hsa_status_t hsa_export_image(
                          const hsa_agent_t *component,
                          hsa_image_handle_t src_image_handle,
                          void *dst_memory,
                          size_t dst_row_pitch,
                          size_t dst_slice_pitch,
                          const hsa_image_region_t *image_region,
                          const hsa_signal_handle_t *completion_signal);
/** @}**************************************************************/

/** \addtogroup APIhsa_copy_image -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param src_image_handle
 *        (in) Source image handle
 * @param dst_image_handle
 *        (in) Destination image handle
 * @param image_region
 *        (in) Image region to be copied
 * @param completion_signal
 *        (in) Signal to set when the operation is completed.
 */
hsa_status_t hsa_copy_image(
                       const hsa_agent_t *component,
                       hsa_image_handle_t src_image_handle,
                       hsa_image_handle_t dst_image_handle,
                       const hsa_image_region_t *image_region,
                       const hsa_signal_handle_t *completion_signal);
/** @}**************************************************************/

/** \addtogroup APIhsa_clear_image -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param image_handle
 *        (in) HSA image to be cleared
 * @param  data
 *        (in) 4-component clear value in floating point format
 * @param image_region
 *        (in) image region to clear
 * @param completion_signal
 *        (in) signal to set when the operation is completed
 */
hsa_status_t hsa_clear_image(
                        const hsa_agent_t *component,
                        hsa_image_handle_t image_handle,
                        const float data[4],
                        const hsa_image_region_t *image_region,
                        const hsa_signal_handle_t *completion_signal);

/** @}**************************************************************/

/** \addtogroup APIhsa_destroy_image_handle -------------------------------------------
 *  @{
 */
/**
 * @param image_handle
 *        (in) HSA image handle
 */
hsa_status_t hsa_destroy_image_handle (
                                    hsa_image_handle_t *image_handle);
/** @}**************************************************************/

/** \addtogroup APIhsa_create_sampler_handle -------------------------------------------
 *  @{
 */
/**
 * @param component
 *        (in) HSA device to be associated with the image
 * @param sampler_descriptor
 *        (in) implementation-independent sampler descriptor
 * @param sampler_handle
 *        (out) HSA component-specific sampler handle
 */
hsa_status_t hsa_create_sampler_handle (
                     const hsa_agent_t *component,
                     const hsa_sampler_descriptor_t *sampler_descriptor,
                     hsa_sampler_handle_t *sampler_handle);
/** @}**************************************************************/

/** \addtogroup APIhsa_destroy_sampler_handle -------------------------------------------
 *  @{
 */
/**
 * @param sampler_handle
 *        (out) HSA component-specific sampler handle
 */
hsa_status_t hsa_destroy_sampler_handle (
                                   hsa_sampler_handle_t *sampler_handle);

/** @}**************************************************************/

/** \addtogroup STRhsa_image_handle ---------------------------
 *  @{
 */
typedef struct hsa_image_handle_s {
   /**
    * HSA component specific handle to the image
    */
    uint64_t handle;

} hsa_image_handle_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_image_format_capability ---------------------------
 *  @{
 */
typedef enum  {

   /**
    * Images of this format are not supported
    */
    HSA_IMAGE_FORMAT_NOT_SUPPORTED = 0x0,

   /**
    * Images of this format can be accessed for read operations
    */
    HSA_IMAGE_FORMAT_READ_ONLY = 0x1,

   /**
    * Images of this format can be accessed for write operations
    */
    HSA_IMAGE_FORMAT_WRITE_ONLY = 0x2,

    /**
    * Images of this format can be accessed for read and write operations
    */
    HSA_IMAGE_FORMAT_READ_WRITE = 0x4,

    /**
    * Images of this format can be accessed for read-modify-write operations
    */
    HSA_IMAGE_FORMAT_READ_MODIFY_WRITE = 0x8

} hsa_image_format_capability_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_info ---------------------------
 *  @{
 */
typedef struct hsa_image_info_s {
  /**
   * HSA component specific image data size in bytes
   */
  size_t image_size;

  /**
   * HSA component specific image data alignment in bytes
   */
  size_t image_alignment;

} hsa_image_info_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_image_geometry ---------------------------
 *  @{
 */
typedef enum {
  /**
   * One-dimensional image addressed by width coordinate
   */
  HSA_IMAGE_GEOMETRY_1D,

  /**
   * Two-dimensional image addressed by width and height coordinates
   */
  HSA_IMAGE_GEOMETRY_2D,

  /**
   * Three-dimensional image addressed by width, height, and depth
   * coordinates.
   */
  HSA_IMAGE_GEOMETRY_3D,

  /**
   * Array of one-dimensional images with the same size and format.
   * 1D arrays are addressed by index and width coordinate
   */
  HSA_IMAGE_GEOMETRY_1DArray,

  /**
   * Array of two-dimensional images with the same size and format.
   * 2D arrays are addressed by index and width and height coordinates.
   */
  HSA_IMAGE_GEOMETRY_2DArray,

  /**
   * One-dimensional image interpreted as a buffer with specific restrictions.
   */
  HSA_IMAGE_GEOMETRY_1DBuffer
} hsa_image_geometry_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_image_component_type ---------------------------
 *  @{
 */
typedef enum {
    HSA_IMAGE_COMPONENT_TYPE_SNORM_INT8,
    HSA_IMAGE_COMPONENT_TYPE_SNORM_INT16,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_INT8,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_INT16,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_INT24,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_SHORT_555,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_SHORT_565,
    HSA_IMAGE_COMPONENT_TYPE_UNORM_SHORT_101010,
    HSA_IMAGE_COMPONENT_TYPE_SIGNED_INT8,
    HSA_IMAGE_COMPONENT_TYPE_SIGNED_INT16,
    HSA_IMAGE_COMPONENT_TYPE_SIGNED_INT32,
    HSA_IMAGE_COMPONENT_TYPE_UNSIGNED_INT8,
    HSA_IMAGE_COMPONENT_TYPE_UNSIGNED_INT16,
    HSA_IMAGE_COMPONENT_TYPE_UNSIGNED_INT32,
    HSA_IMAGE_COMPONENT_TYPE_HALF_FLOAT,
    HSA_IMAGE_COMPONENT_TYPE_FLOAT
} hsa_image_component_type_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_image_component_order ---------------------------
 *  @{
 */
typedef enum {

    HSA_IMAGE_COMPONENT_ORDER_A,
    HSA_IMAGE_COMPONENT_ORDER_R,
    HSA_IMAGE_COMPONENT_ORDER_RX,
    HSA_IMAGE_COMPONENT_ORDER_RG,
    HSA_IMAGE_COMPONENT_ORDER_RGX,
    HSA_IMAGE_COMPONENT_ORDER_RA,
    HSA_IMAGE_COMPONENT_ORDER_RGB,
    HSA_IMAGE_COMPONENT_ORDER_RGBX,
    HSA_IMAGE_COMPONENT_ORDER_RGBA,
    HSA_IMAGE_COMPONENT_ORDER_BGRA,
    HSA_IMAGE_COMPONENT_ORDER_ARGB,
    HSA_IMAGE_COMPONENT_ORDER_ABGR,
    HSA_IMAGE_COMPONENT_ORDER_SRGB,
    HSA_IMAGE_COMPONENT_ORDER_SRGBX,
    HSA_IMAGE_COMPONENT_ORDER_SRGBA,
    HSA_IMAGE_COMPONENT_ORDER_SBGRA,
    HSA_IMAGE_COMPONENT_ORDER_INTENSITY,
    HSA_IMAGE_COMPONENT_ORDER_LUMINANCE,
    HSA_IMAGE_COMPONENT_ORDER_DEPTH,
    HSA_IMAGE_COMPONENT_ORDER_DEPTH_STENCIL

} hsa_image_component_order_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_format ---------------------------
 *  @{
 */
typedef struct hsa_image_format_s
{
   /**
    * Component type of the image
    */
    hsa_image_component_type_t component_type;

   /**
    * Component order of the image
    */
    hsa_image_component_order_t component_order;

} hsa_image_format_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_descriptor ---------------------------
 *  @{
 */
typedef struct hsa_image_descriptor_s {
  /**
   * Geometry of the image
   */
  hsa_image_geometry_t geometry;

  /**
   * Width of the image in components
   */
  size_t width;

  /**
   * Height of the image in components, only used if geometry is 2D or higher
   */
  size_t height;

  /**
   * Depth of the image in slices, only used if geometry is 3D
   * depth = 0 is same as depth = 1.
   */
  size_t depth;

  /**
   * Number of images in the image array, only used if geometry is 1DArray and
   * 2DArray.
   */
  size_t array_size;

  /**
   * Format of the image
   */
  hsa_image_format_t format;


} hsa_image_descriptor_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_offset ---------------------------
 *  @{
 */
typedef struct hsa_image_offset_s {
   /**
    * x coordinate for the offset
    */
    uint32_t x;

   /**
    * y coordinate for the offset
    */
    uint32_t y;

   /**
    * z coordinate for the offset
    */
    uint32_t z;

} hsa_image_offset_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_range ---------------------------
 *  @{
 */
typedef struct hsa_image_range_s {
   /**
    * The width for an image range (in coordinates)
    */
    uint32_t width;

   /**
    * The height for an image range (in coordinates)
    */
    uint32_t height;

   /**
    * The depth for an image range (in coordinates)
    */
    uint32_t depth;

} hsa_image_range_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_image_region ---------------------------
 *  @{
 */
typedef struct hsa_image_region_s {
   /**
    * offset in the image (in coordinates)
    */
    hsa_image_offset_t image_offset;

   /**
    * dimensions of the image range (in coordinates)
    */
    hsa_image_range_t image_range;

} hsa_image_region_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_sampler_handle ---------------------------
 *  @{
 */
typedef struct hsa_sampler_handle_s {
   /**
    * HSA component-specific HSA sampler
    */
    uint64_t handle;

} hsa_sampler_handle_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_sampler_address_mode ---------------------------
 *  @{
 */
typedef enum {

  /**
   * Out-of-range coordinates are not handled
   */
  HSA_SAMPLER_ADDRESS_UNDEFINED,

  /**
   * Clamp out-of-range coordinates to the image edge
   */
  HSA_SAMPLER_ADDRESS_CLAMP_TO_EDGE,

  /**
   * Clamp out-of-range coordinates to the image border
   */
  HSA_SAMPLER_ADDRESS_CLAMP_TO_BORDER,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate range
   */
  HSA_SAMPLER_ADDRESS_WRAP,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate range
   */
  HSA_SAMPLER_ADDRESS_MIRROR

} hsa_sampler_address_mode_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_sampler_coordinate_mode ---------------------------
 *  @{
 */
typedef enum {
  /**
   * Coordinates are all in the range of 0.0 to 1.0
   */
  HSA_SAMPLER_COORD_NORMALIZED,

  /**
   * Coordinates are all in the range of 0 to (dimension-1)
   */
  HSA_SAMPLER_COORD_UNNORMALIZED

} hsa_sampler_coordinate_mode_t;
/** @}***********************************************************************/

/** \addtogroup ENUhsa_sampler_filter_mode ---------------------------
 *  @{
 */
typedef enum {
  /**
   * Filter to the image element nearest (in Manhattan distance) to the
   * specified coordinate.
   */
  HSA_SAMPLER_FILTER_NEAREST,

  /**
   * Filter to the image element calculated by combining the elements in
   * a 2x2 square block or 2x2x2 cube block around the specified
   * coordinate. The elements are combined using linear interpolation.
   */
  HSA_SAMPLER_FILTER_LINEAR

} hsa_sampler_filter_mode_t;
/** @}***********************************************************************/

/** \addtogroup STRhsa_sampler_descriptor ---------------------------
 *  @{
 */
typedef struct hsa_sampler_descriptor_s {
  /**
   * Sampler coordinate mode describes the normalization of image coordinates
   */
  hsa_sampler_coordinate_mode_t coordinate_mode;

  /**
   * Sampler filter type describes the type of sampling performed
   */
  hsa_sampler_filter_type_t filter_mode;

  /**
   * Sampler address mode describes the processing of out-of-range image
   * coordinates
   */
  hsa_sampler_address_mode_t address_mode;

} hsa_sampler_descriptor_t;
/** @}***********************************************************************/

/** \addtogroup APIkernargmem -------------------------------------------
 *  @{
 */
  /**
 *******************************************************************************
 * @brief Allocate memory on HSA Device
 * @details The hsa_memory_allocate_component_local HSA core interface is used for
 * allocating global device memory associated with specified device.
 *
 * @param component
 *      input. A valid pointer to the HSA component for which the specified amount of
 *      kernarg memory is to be allocated.
 * @param size
 *      input. Requested allocation size in bytes. If size is 0, NULL is returned.
 * @param address
 *      output. A valid pointer to the location of where to return the pointer to the
 *      base of the allocated region of memory.
 *
 *******************************************************************************
 */
hsa_status_t hsa_memory_allocate_kernarg( const hsa_agent_t *component,
    size_t size,
    void **address);


/**
 * @brief Copies the values of @a size bytes from the location pointed by @a src
 * directly to the memory block pointed by @a dst.

 * @details The ::HsaCopyMemory interface is used for copying memory in the
 * HSA address space. If the specified pointers do not belong to the HSA address
 * space, an error is returned. If the size of the destination or source arrays
 * is smaller than the indicated size, the behavior is undefined.
 *
 * @param dst
 *      user allocated, output. A valid pointer to the destination array where the content is to be
 *      copied.
 * @param src
 *      input. A valid pointer to the source of data to be copied.
 * @param size
 *      input. Number of bytes to copy.
 */
hsa_status_t hsa_memory_copy_kernarg_to_system(void *dst,
    const void *src,
    size_t size);

/**
 * @brief Copies the values of @a size bytes from the location pointed by @a src
 * directly to the memory block pointed by @a dst.

 * @details The ::HsaCopyMemory interface is used for copying memory in the
 * HSA address space. If the specified pointers do not belong to the HSA address
 * space, an error is returned. If the size of the destination or source arrays
 * is smaller than the indicated size, the behavior is undefined.
 *
 * @param dst
 *      user allocated, output. A valid pointer to the destination array where the content is to be
 *      copied.
 * @param src
 *      input. A valid pointer to the source of data to be copied.
 * @param size
 *      input. Number of bytes to copy.
 * @param signal
 *      input. The signal that will be incremented by the runtime
 *      when the copy is complete.
 */
hsa_status_t hsa_memory_copy_system_to_kernarg(void *dst,
    const void *src,
    size_t size);
/** @}***********************************************************************/

/** \addtogroup APIagent_dispatch -------------------------------------------
 *  @{
 */
/**
 * @brief agent dispatch runtime function registration
 *
 * @param notification_callback
 *      input. the callback that the user is registering, the callback is
 *      called with 5 64 bit args (see Section \ref{agent_dispatch}) as a
 *      parameter.
 */
hsa_status_t hsa_register_agent_dispatch_callback(hsa_queue_t *agent_dispatch_queue,
    void (*agent_dispatch_callback)(uint64_t arg0, uint64_t arg1, uint64_t
      arg2, uint64_t arg3, uint64_t returnaddr),
    hsa_runtime_context_t *context);
/** @}***********************************************************************/


/** \addtogroup ENUhsa_ext -------------------------------------------
 *  @{
 */
typedef enum hsa_extension_t {
    /**
     * HSA extension to support images.
     */
    HSA_EXTENSION_IMAGES,

    /**
     * HSA extensions providing information necessary to debug
     * tools
     */
    HSA_EXTENSION_DEBUG,

    /**
     * HSA extension for Finalizer that finalizes the brig to
     * compilation units that represent kernel and function code objects
     */
    HSA_EXTENSION_FINALIZER
}hsa_extension_t;
/** @}***********************************************************************/


/** \addtogroup ENUvendor_ext -------------------------------------------
 *  @{
 */
typedef enum hsa_vendor_extension_s {
  /**
   * start of the single vendor extension range
   */
  HSA_SVEXT_START= 0,
  /**
   * Company ACME, starts with FOO symbol
   */
  HSA_SVEXT_ACME_FOO = 1,

  /**
   * Company ACME has another\_ext symbol
   */
  HSA_SVEXT_ACME_ANOTHER_EXT = 2,
  /**
   * multi vendor extension starts at 1000000
   */
  HSA_MVEXT_START = 1000000,

  /**
   * multivendor extension has a symbol foo
   */
  HSA_MVEXT_FOO   = 1000001

}hsa_vendor_extension_t;
/** @}***********************************************************************/


/** \addtogroup APIquery_hsaextension -------------------------------------------
 *  @{
 */
/**
 * @brief query hsa extension
 *
 * @param extension
 *      input. The HSA extention that is being queried
 */
hsa_status_t hsa_extension_query(hsa_extension_t extension);
/** @}***********************************************************************/


/** \addtogroup APIquery_vendorextension -------------------------------------------
 *  @{
 */
/**
 * @brief agent dispatch runtime function registration
 *
 * @param extension
 *      input. The vendor extention that is being queried
 */
hsa_status_t hsa_vendor_extension_query(hsa_vendor_extension_t extension, void *extension_structure);
/** @}***********************************************************************/

#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif /*HSACOREBASE_H*/
