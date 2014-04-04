#ifndef HSACOREBASE_H
#define HSACOREBASE_H

#include <stdint.h> // uintXX_t
#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** \defgroup runtime_context TODO
 *  @{
 */
/**
 * @brief TODO
 */
typedef uint64_t hsa_runtime_context_t;
/** @} */

/** \defgroup status Errors and warnings.
 *  @{
 */
/**
 * TODO
 */
typedef enum {
    /**
     * Indicates success. The API has been successfully executed per its
     * definition.
     */
    HSA_STATUS_SUCCESS = 0,

    /**
     * Indicates an error occurred, specifics were either not determinable or
     * not encoded in the error list.
     */
    HSA_STATUS_FAILURE,

    /**
     * Indicates that initialization attempt failed due to prior initialization.
     */
    HSA_STATUS_ALREADY_INITIALIZED,

    /**
     * Indicates that signal is timed out.
     */
    HSA_STATUS_INFO_SIGNAL_TIMEOUT,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_INVALID_ARGUMENT,
    /**
     * The runtime was not able to allocate the necessary resources. This error
     * may also occur when the core runtime library needs to spawn threads or
     * create internal OS-specific events.
     */
    HSA_STATUS_ERROR_OUT_OF_RESOURCES,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_INVALID_CONTEXT,
    /**
     * TODO.
     */
    HSA_STATUS_INFO_UNRECOGNIZED_OPTIONS,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_DIRECTIVE_MISMATCH,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_RESOURCE_FREE,
    /**
     * TODO.
     */
    HSA_STATUS_CLOSE_CONTEXT_ACTIVE,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_NOT_INITIALIZED,
    /**
     * TODO.
     */
    HSA_STATUS_CONTEXT_LIMIT_REACHED,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_COMPONENT_INITIALIZATION,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_CONTEXT_NULL,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_SIGNAL_NOT_BOUND ,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR,
    /**
     * TODO.
     */
    HSA_STATUS_INFO_NOT_REGISTERED,
    /**
     * TODO.
     */
    HSA_STATUS_ERROR_EXTENSION_UNSUPPORTED,
    /**
     * TODO.
     */
    HSA_STATUS_UNSUPPORTED_IMAGE_FORMAT,
    /**
     * TODO.
     */
    HSA_STATUS_IMAGE_SIZE_NOT_SUPPORTED
} hsa_status_t;


/**
 * @brief Queries additional information on synchronous errors.
 *
 * @details Returns success if one or both of the @a status_info and @a
 * status_info_string have been successfully updated with information regarding
 * the input @a input_status.
 *
 * @param[in] input_status Any unsuccessful API return status that the user is
 *      seeking more information on.
 *
 * @param[in] status_info User allocated. In addition to the string. This value
 *      could be 0 and in itself (without @a status_info_string) may not be
 *      independently interpreted by the user.
 *
 * @param[out] status_info_string A ISO/IEC 646 encoded English language string
 *      that potentially describes the error status. The string terminates in a
 *      ISO 646 defined NUL char.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If a NULL value is passed for
 * either of the arguments.
 */
hsa_status_t hsa_status_query_description(hsa_status_t input_status,
      uint64_t *status_info,
      char * const * status_info_string);

/**
 * @brief Notification information.
 */
typedef struct _hsa_notification_info_s{
   /**
    * The info status enum value.
    */
    hsa_status_t status;
  /**
   * A pointer to more information, this could be pointing to implementation
   * specific details that could be useful to some tools or to binary data.
   */
    void *info;
  /**
   * ISO/IEC 646 character encoding must be used. A string indicating some error
   * information. The string should be NUL terminated per ISO 646.
   */
    char *string_info;
  /**
   * A pointer to user supplied data.
   */
    void *user_data;
} hsa_notification_info_t;

/**
 * @brief TODO
 */
typedef struct _hsa_async_error_info_s{
    /**
     * Indicates the type of the error, based on this, the user knows if and
     * packet_id is available in one of the reserved words.
     */
    hsa_status_t error_type;

    /**
     * The queue that processed the entity that caused the asynchronous error.
     */
    uint32_t queue_id;

    /**
     * A pointer to more information, this could be pointing to implementation
     * specific details that could be useful to some tools or to binary data.
     */
    void *info;

    /**
     * ISO/IEC 646 character encoding must be used. A string indicating some
     * error information. The string should be NUL terminated per ISO 646.
     */
    char *string_info;

    /**
     * A pointer to user supplied data
     */
    void *user_data;

    /**
     * System timestamp to indicate when the error was discovered, the
     * implementation may chose to always return 0 and user must take it into
     * account when utilizing the timestamp.
     */
    uint64_t timestamp;

    /**
     * Additional info to be interpreted based on @a error_type.
     */
    uint64_t reserved1;

    /**
     * Additional info to be interpreted based on @a error_type.
     */
    uint64_t reserved2;

    /**
     * Additional info to be interpreted based on @a error_type.
     */
    uint64_t reserved3;

} hsa_async_error_info_t;

/**
 * @brief Register a notification callback.
 *
 * @param[in] callback The callback that the user is registering, which will be
 *      invoked with info as a parameter.
 *
 * @param[in] user_data The user data to call the callback with. The @a
 *      user_data field of @a info will be filled with value when the callback
 *      is called.
 *
 * @param[in] context Identifies a particular runtime context that this callback
 *      is registered for. When a callback is registered for a particular
 *      context, it will only be invoked if the notification is for an action in
 *      that context.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime library in
 * the context of registering a callback. This error may also occur when the
 * core runtime library needs to spawn threads or create internal OS-specific
 * events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a info is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_CONTEXT If the context is invalid
 * (e.g. referenced counted to 0).
 */
hsa_status_t hsa_notification_callback_register(void ( *callback)(const hsa_notification_info_t *info),
    void *user_data,
    hsa_runtime_context_t *context);

/**
 * @brief Register an error callback.
 *
 * @details When a callback is registered for a particular context, it will only
 * be invoked if the notification is for an action in that context. For example,
 * if a queue was created for a runtime context @a c1 and a callback registered
 * for a context @a c2 but not for @a c1, any error on the queue, such as a
 * packet processing error, will not trigger the execution of asynchronous error
 * callback registered for context @a c1.
 *
 * @param[in] callback The callback that the user is registering, the callback
 *      is called with info structure. User can read the structure and access
 *      its elements.
 *
 * @param[in] user_data The user data to call the callback with. info.user_data
 *      will be filled with value when the callback is called.
 *
 * @param[in] context The runtime context that this callback is being registered
 *      for.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime library in
 * the context of registering a callback. This error may also occur when the
 * core runtime library needs to spawn threads or create internal OS-specific
 * events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a info is NULL.
 */
hsa_status_t hsa_error_callback_register(void ( *callback)(const hsa_async_error_info_t *info),
    void *user_data,
    hsa_runtime_context_t *context);
/** @} */


/** \defgroup openclose TODO
 *  @{
 */
/**
 * @brief Initialize the HSA runtime.
 *
 * @details Initializes the HSA runtime if it is not already initialized. It is
 * allowed for applications to invoke ::hsa_open multiple times. The HSA open
 * call returns a new context at every invocation.  Reference counting is a
 * mechanism that allows the runtime to keep a count of the number of different
 * usages of the runtime API within the same application process. This ensures
 * that the runtime stays active until ::hsa_close is called by the user when
 * the reference count represented by that runtime context is one.
 *
 * If the HSA runtime is already initialized, an asynchronous notification is
 * generated by the runtime and ::HSA_STATUS_SUCCESS is returned. If the user
 * chooses to capture this asynchronous notification, the user should define a
 * callback and associate it with the context returned by the ::hsa_open
 * call. Each open call increments the reference count before returning success.
 *
 * @param[out] context A type for reference counting. User allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime
 * library. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_COMPONENT_INITIALIZATION If there is a
 * non-specific failure in initializing one of the components.
 *
 * @retval ::HSA_STATUS_ERROR_CONTEXT_NULL If the context pointer passed by the
 * user is NULL. User is required to pass in a memory backed context pointer.
 */
hsa_status_t hsa_open(hsa_runtime_context_t **context);

/**
 * @brief Close the HSA runtime.
 *
 * @details Decreases the context reference count for every invocation. Once the
 * reference count is zero, it proceeds to relinquish any resources allocated
 * for the runtime and closes the runtime instance. It is possible in a
 * multi-threaded scenario that one thread is doing a close while the other is
 * trying to acquire the runtime context or do an open. The core runtime
 * specification defines that an acquire with an input context that represents a
 * closed runtime instance will fail. However, ::hsa_open can be called to
 * create a new instance of the runtime after it is closed.
 *
 * An invocation when the reference count is not one it is considered successful
 * in that ::HSA_STATUS_SUCCESS is returned with status
 * ::HSA_STATUS_CLOSE_CONTEXT_ACTIVE, is generated by the runtime on the context
 * that is still active before the API returns.

 * @param[in] context Context to close.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED If invoked before the runtime was
 * initialized, or after it has already been successfully closed.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If some of the resources consumed
 * during initialization by the runtime could not be freed.
 */
hsa_status_t hsa_close(hsa_runtime_context_t *context);

/**
 * @brief Increment reference count of a context, if it is not currently zero.
 *
 * @param[in] input_context The context that the user is explicitly reference
 *      counting. User allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED If invoked before the runtime was
 * initialized or after it has been closed.
 *
 * @retval ::HSA_STATUS_CONTEXT_LIMIT_REACHED If the reference count has reached
 * UINT64_MAX.
 */
hsa_status_t hsa_context_acquire(hsa_runtime_context_t *input_context);

/**
 * @brief Decrement reference count of a context.
 *
 * @param[in] input_context The context that the user is explicitly reference
 * counting. User allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED If invoked before the runtime was
 * initialized or when the reference count is already zero.
 */
hsa_status_t hsa_context_release(hsa_runtime_context_t *input_context);
/** @} */


/** \defgroup topology TODO
 *  @{
 */

/**
 * @brief Agent type.
 */
typedef enum {
    /**
     * Host agent (CPU).
     */
    HOST = 1,

    /**
     * HSA component.
     */
    COMPONENT = 2,

    /**
     * The agent is capable of agent dispatches, and can serve as a target for
     * them.
     */
    AGENT_DISPATCH = 4
}hsa_agent_type_t;


/**
 * @brief HSA agent.
 */
typedef struct hsa_agent_s{
    /**
     * ID of the node this agent/component belongs to.
     */
    uint32_t hsa_node_id;

    /**
     * Unique identifier for an HSA agent.
     */
    uint32_t agent_id;

    /**
     * Identifier for the type of this agent.
     */
    hsa_agent_type_t agent_type;

    /**
     * The vendor of the agent/component. ISO/IEC 646 character encoding must be
     * used. If the name is less than 16 characters then remaining characters
     * must be set to 0.
     */
    char vendor[16];

    /**
     * The name of this agent/component. ISO/IEC 646 character encoding must be
     * used. If the name is less than 16 characters then remaining characters
     * must be set to 0.
     */
    char  name[16];

    /**
     * Table of properties of the agent, any property that is not available has
     * a value of 0.
     */
    uint64_t *property_table;

    /**
     * Number of the different types of memories available to this agent. Zero
     * indicates that no information is available.
     */
    uint32_t number_memory_descriptors;

    /**
     * Array of memory descriptor offsets.  Number of elements in array equals
     * @a number_memory_descriptors.
    */
    uint32_t *memory_descriptors;

    /**
     * Number of caches available to this agent/component. Zero indicates that
     * no information is available.
     */
    uint32_t number_cache_descriptors;

    /**
     * Array of cache descriptor offsets.  Number of elements in array equals @a
     * number_cache_descriptors.
    */
    uint32_t  *cache_descriptors;

    /**
     * Number of subagents.
     */
    uint32_t number_of_subagents;

    /**
     * Subagent list of offsets, points to the offsets in the topology table.
     */
    uint32_t *subagent_offset_list;

    /**
     * Wave front size, i.e. number of work-items in a wavefront.
     */
    uint32_t wavefront_size;

    /**
     * Maximum size of the user queue in bytes allocatable via the runtime.
    */
    uint32_t queue_size;

    /**
     * Size (in bytes) of group memory available to a single work-group.
    */
    uint32_t group_memory_size_bytes;

    /**
     * Max number of fbarrier that can be used in any kernel and functions it
     * invokes.
     */
    uint32_t fbarrier_max_count;
    /**
     * Does it support position independent code?. Only applicable when the
     * agent is a component.
     */
    uint8_t is_pic_supported;

} hsa_agent_t;

/**
 * @brief Memory segment.
 */
typedef struct hsa_segment_s{
    /**
     * Global segment.
     */
    uint8_t global:1;

    /**
     * Private segment.
     */
    uint8_t privat:1;

    /**
     * Group segment.
     */
    uint8_t group:1;

    /**
     * Kernarg segment.
     */
    uint8_t kernarg:1;

    /**
     * Readonly segment.
     */
    uint8_t readonly:1;

    /**
     * Reserved.
     */
    uint8_t reserved:1;
} hsa_segment_t;

/**
 * @brief Memory descriptor.
 *
 * @details Representation of a physical memory block or region. Implementations
 * may choose not to provide memory bandwidth or latency information, which case
 * zero is returned.
 */
typedef struct hsa_memory_descriptor_s{
    /**
     * ID of the node this memory belongs to.
     */
    uint32_t hsa_node_id;

    /**
     * Unique for this memory with in the system.
     */
    uint32_t hsa_memory_id;

    /**
     * Information on segments that can use this memory.
     */
    hsa_segment_t supported_segment_type_mask;

    /**
     * Base of the virtual address for this memory, if applicable.
     */
    uint64_t virtual_address_base;

    /**
     * Size.
     */
    uint64_t size_in_bytes;

    /**
     * Theoretical peak bandwidth in mega-bits per second to access this memory
     * from the agent/component.
     */
    uint64_t peak_bandwidth_mbps;
} hsa_memory_descriptor_t;

/**
 * @brief Cache descriptor.
 */
typedef struct hsa_cache_descriptor_s{
    /**
     * ID of the node this memory belongs to.
     */
    uint32_t hsa_node_id;

    /**
     * Unique identified for this cache with in the system.
     */
    uint32_t hsa_cache_id;

    /**
     * Number of levels of cache (for a multi-level cache).
     */
    uint8_t levels;

    /**
     * Associativity of this cache. The array has size @a levels.
     */
    uint8_t *associativity;

    /**
     * Size at each level. The array has size @a levels.
     */
    uint64_t *cache_size;

    /**
     * Cache line size at each level. The array has size @a levels.
     */
    uint64_t *cache_line_size;

    /**
     * Is the cache inclusive with the level above? The array has size @a levels
     * - 1.
     */
    uint8_t *is_inclusive;

} hsa_cache_descriptor_t;

/**
 * @brief Topology header.
 */
typedef struct hsa_topology_table_s{
    /**
     * Size of the header.
     */
    uint32_t header_size_bytes;

    /**
     * Constant observable timestamp value increase rate is in the range
     * 1-400MHz.
     */
    uint32_t hsa_system_timestamp_frequency_mhz;

    /**
     * Number of different nodes in this platform configuration.
     */
    uint8_t number_of_nodes;

    /**
     * IDs of the nodes.
     */
    uint32_t *node_id;

    /**
     * Number of agent offsets specified in this structure. Zero indicates that
     * no information is available.
     */
    uint32_t number_agents;

    /**
     * Agent list, refers to the offsets in platform table.
     */
    uint32_t *agent_offset_list_bytes;

    /**
     * Number of the different types of memories available to this agent. Zero
     * indicates that no information is available.
     */
    uint32_t number_memory_descriptors;

    /**
     * Each element in the array carries an offset into the topology table to
     * where memory descriptors are located. Number of elements in array equals
     * @a number_memory_descriptors.
    */
    uint32_t *memory_descriptor_offset_list_bytes;

    /**
     * Number of caches available to this agent/component. Zero indicates that
     * no information is available.
     */
    uint32_t number_cache_descriptors;

    /**
     * Array of offsets (into the topology table) to cache descriptors. Number
     * of elements in array equals number_cache_descriptors.
    */
    uint32_t  *cache_descriptors_offset_list_bytes;

    /**
     * Size of the table.
     */
    uint32_t table_size;

    /**
     * Table base address, points to the table which starts with
     * ::hsa_platform_t structure.
     */
    void  *topology_table_base;

} hsa_topology_table_t;

/**
 * @brief Retrieve topology information.
 *
 * @param[out] table The topology header, this includes the base pointers to the
 * rest of the topology table. Runtime allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a header is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime or in the
 * creation of table header or the actual table.
 */
hsa_status_t hsa_topology_table_create(hsa_topology_table_t **table);

/**
 * @brief Release resources associated with the topology information.
 *
 * @param[in] table Topology table to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 */
hsa_status_t hsa_topology_table_destroy(hsa_topology_table_t *table);
/** @} */


/** \addtogroup hsa_tlb_descriptor
 *  @{
 */
/**
 * @brief TODO.
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
     * number of levels of TLB (for a mult-level TLB).
     */
    uint8_t  levels;

    /**
     * associativity of this TLB, size of this array is = levels.
     */
    uint8_t  *associativity;

    /**
     * is the TLB inclusive with the level above? The size of this array is
     * level-1.
     */
    uint8_t *is_inclusive;

    /**
     * size of TLB at each level, this size of this array is = levels.
     */
    uint64_t *tlb_size;
} hsa_tlb_descriptor_t;
/** @} */


/** \addtogroup hsa_properties
 *  @{
 */
/**
 * @brief TODO
 */
typedef enum {
    /**
     * Theoretical peak floating point operations per second supported by the
     * component, if applicable.
     */
    HSA_MAX_FLOPS,

    /**
     * Theoretical peak integer operations supported by the agent, if
     * applicable.
     */
    HSA_MAX_IOPS,

    /**
     * Theoretical peak integer operations supported by the agent, if
     * applicable.
     */
    HSA_MAX_PROCESSING_ELEMENTS
} hsa_property_t;
/** @} */


/** \addtogroup clock_convert
 *  @{
 */
/**
 * @brief TODO.
 *
 * @param component_time input, the time in component clock frequency, this can
 *      be obtained by using clock_u64 instruction in HSAIL.
 *
 * @param host_time
 *      output, time per host frequency.
 *
 * @return TODO
 */
hsa_status_t hsa_clock_convert_time_component_to_host(uint64_t component_time,
    uint64_t *host_time);

/**
 * @brief TODO.
 *
 * @param component_time input, the time in component clock frequency, this can
 *      be obtained by using clock_u64 instruction in HSAIL.
 *
 * @param host_time time per host frequency.
 *
 * @return TODO.
 */
hsa_status_t hsa_clock_convert_convert_time_host_to_component(uint64_t host_time,
    uint64_t *component_time);
/** @} */


/** \defgroup signals TODO
 *  @{
 */

/**
 * @brief Signal handle.
 */
typedef uint64_t hsa_signal_handle_t;

/**
 * @brief Signal value.
 */
typedef union signal_value_s{
    /**
    * Signal value in the small machine model.
    */
    int32_t value32;

    /**
    * Signal value in the large machine model.
    */
    int64_t value64;
} hsa_signal_value_t;

/**
 * @brief Wait condition operator.
 */
typedef enum {
    /**
     * The two operands are equal.
     */
    HSA_EQ,
    /**
     * The two operands are not equal.
     */
    HSA_NE,
    /**
     * The first operand is less than the second operand.
     */
    HSA_LT,
    /**
     * The first operand is greater than or equal to the second operand.
     */
    HSA_GTE
} hsa_signal_condition_t;

/**
 * @brief Create a signal.
 *
 * @param[in] initial_signal_value Initial value of the signal.
 *
 * @param[out] signal_handle Signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is NULL.
 */
hsa_status_t hsa_signal_create(hsa_signal_value_t initial_signal_value,
                          hsa_signal_handle_t *signal_handle);
/**
 * @brief Destroy signal previous created by ::hsa_signal_create.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_destroy(hsa_signal_handle_t signal_handle);

/**
 * @brief Bind a signal to a context. A signal might be bound to several
 * contexts.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] context Additional context to which this signal should be bound
 * to.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is
 * invalid or if @a context is NULL or invalid.
 *
 */
hsa_status_t hsa_signal_bind(hsa_signal_handle_t signal_handle,
    hsa_runtime_context_t *context);

/**
 * @brief Unbind a signal from a context.
 *
 * @details Signals are unbound from a particular context if the user no longer
 * wants to receive notifications about this signal in the callback registered
 * for that context.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] context Unbind the signal from this context.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_SIGNAL_NOT_BOUND If the signal was not already
 * bound to that context.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid or
 * if @a context is NULL or invalid.
 */
hsa_status_t hsa_signal_unbind(hsa_signal_handle_t signal_handle,
    hsa_runtime_context_t *context);

/**
 * @brief Read the current signal value.
 *
 * @details If the signal is being updated by the component or other threads,
 * there is no guarantee that the value returned by the query API is the value
 * of the signal even at the instance it has been returned. This function is
 * non-blocking and does not take any condition as an input, unlike
 * ::hsa_signal_wait_acquire
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[out] value User-allocated pointer to where the current signal value
 * must be read into.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid or
 * @a value is NULL.
 *
*/
hsa_status_t hsa_signal_query_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_value_t *value);

/**
 * @copydoc hsa_signal_query_acquire
 */
hsa_status_t hsa_signal_query_relaxed(hsa_signal_handle_t signal_handle,
                          hsa_signal_value_t *value);
/**
 * @brief Set the value of a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to be assigned to the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_send_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_send_relaxed
 */
hsa_status_t hsa_signal_send_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Set the value of a signal and return its previous value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in,out] value Value to be placed at the signal
 *
 * @param[out] prev_value Pointer to the value of the signal prior to the
 * exchange. User allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_exchange_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *prev_value);

/**
 * @copydoc hsa_signal_exchange_release
 */
hsa_status_t hsa_signal_exchange_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *prev_value);

/**
 * @brief Perform a CAS on a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value_compare The value to compare the handle's value with.
 *
 * @param[in] value_replace The new value of the signal.
 *
 * @param[out] prev_value The value at the signal, prior to the atomic replace,
 * if the comparison was successful. User allocated.
 */
hsa_status_t hsa_signal_cas_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value_compare,
                hsa_signal_value_t value_replace,
                hsa_signal_value_t *prev_value);

/**
 * @brief Increment the value of a signal by a given amount. The
 * addition is atomic.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to add to the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_add_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_add_release
 */
hsa_status_t hsa_signal_add_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Decrement the value of a signal by a given amount.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to subtract from the value of the signal
 * handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_subtract_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_subtract_release
 */
hsa_status_t hsa_signal_subtract_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical AND of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to AND with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_and_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_and_release
 */
hsa_status_t hsa_signal_and_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical OR of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to OR with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_or_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_or_release
 */
hsa_status_t hsa_signal_or_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical XOR of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to XOR with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_xor_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_xor_release
 */
hsa_status_t hsa_signal_xor_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Set (increment) the signal value to a given input if it is
 * greater than the current value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value User defined value.
 *
 * @param[out] max_value Maximum of @a value and the signal's current value.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_max(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *max_value);

/**
 * @brief Set (decrement) the signal value to a given input if it is
 * smaller than the current value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value User defined value.
 *
 * @param[out] min_value Minimum of @a value and the signal's current value.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is
 * invalid.
 */
hsa_status_t hsa_signal_min(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *min_value);

/**
 * @brief Increment the value of a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value the signal is to be incremented with.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_increment_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_increment_release
 */
hsa_status_t hsa_signal_increment_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Decrement the value of a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value the signal is to be decremented with.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is invalid.
 */
hsa_status_t hsa_signal_decrement_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_decrement_release
 */
hsa_status_t hsa_signal_decrement_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Returns the maximum amount of time an implementation can spend in a
 * wait operation on the signal.
 *
 * @return Signal timeout. The returned value is in the units of the system-wide
 * clock whose frequency is available in the topology table.
 */
uint64_t hsa_signal_get_timeout();

/**
 * @brief Wait until the value of a signal satisfies a given condition, or a
 * user-provided timeout has elapsed.
 *
 * @details Waiting on a signal returns the current value at the signal. The
 * wait may return before the condition is satisfied, the specified timeout has
 * elapsed, or even before a valid value is obtained from the signal. It is the
 * users burden to check the return status of the wait API before consuming the
 * returned value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] timeout Maximum wait duration. A value of zero indicates no
 * maximum.
 *
 * @param[in] cond Condition used to compare the passed and signal values.
 *
 * @param[in] compare_value Value to compare with.
 *
 * @param[out] return_value Pointer to where the current value @a signal_handle
 * must be read into. User allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR If an error is signaled on the signal the user is
 * waiting on. The function still returns the current value at the signal. The
 * user may also inspect the value returned, when an error occurred.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the user is expecting an
 * output but the pointer to the output signal value is invalid, or the passed
 * signal is invalid.
 *
 * @retval ::HSA_STATUS_INFO_SIGNAL_TIMEOUT If the signal wait has timed out.
*/
hsa_status_t hsa_signal_wait_acquire(hsa_signal_handle_t signal_handle,
                          uint64_t timeout,
                          hsa_signal_condition_t cond,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/**
 * @copydoc hsa_signal_wait_acquire
 */
hsa_status_t hsa_signal_wait_relaxed(hsa_signal_handle_t signal_handle,
                          uint64_t timeout,
                          hsa_signal_condition_t cond,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/** @} */


/** \defgroup interrupt_condition TODO
 *  @{
 */
/**
 * @brief Interrupt condition.
 */
typedef enum {
    /**
     * Caused by debugtrap_u32 instruction.
     */
    HSA_DEBUGTRAP = 1,

    /**
     * Caused by syscall.
     */
    HSA_SYSCALL = 4,

    /**
     * Caused by other interrupt.
     */
    HSA_OTHER_INTERRUPT = 8
} hsa_interrupt_condition_t;
/** @} */


/** \defgroup execution_info TODO
 *  @{
 */
/**
 * @brief Group execution information.
 */
typedef struct hsa_group_execution_info_s{

    /**
     * Indicates the mailbox is full and needs to be consumed.
     */
    hsa_signal_handle_t full_flag;

    /**
     * Size of the workgroup, all pointers below are arrays of that size.
     */
    uint16_t workgroup_size;

    /**
     * What caused this execution to stop.
     */
    hsa_interrupt_condition_t *condition;

    /**
     * Flattend workitem IDs, array[workgroup_size].
     */
    uint32_t *workitem_id;

    /**
     * ID of the compute unit, array[workgroup_size].
     */
    uint32_t *compute_unit_id;

    /**
     * Pointer to the AQL packet, array[workgroup_size].
     */
    uint64_t *aql_packet_ptr;

    /**
     * Any pertinent virtual address, array[workgroup_size].
     */
    uint64_t *virtual_address;

    /**
     * Current program counter, array[workgroup_size].
     */
    uint64_t *current_program_counter;

    /**
     * Location to where the arguments have been stored. The size and contents
     * are written by the component and need to be decoded by the agent when
     * reading this.
     */
    uint64_t args;

    /**
     * If the condition is syscall, location to where the outputs need to be
     * stored. This is array[workgroup_size].
     */
    uint64_t **syscall_output;

} hsa_group_execution_info_t;
/** @} */


/** \addtogroup queue_mailbox_struct
 *  @{
 */
/**
 * @brief TODO
 */
typedef struct hsa_queue_mailbox_s {
  /**
   * A pointer to the mailbox.
   */
  const hsa_group_execution_info_t *mailbox_ptr;

  /**
   * The signal that user can wait on to get an indication that mailbox needs
   * processing.
   */
  hsa_signal_handle_t mailbox_signal;
} hsa_queue_mailbox_t;
/** @} */

/** \defgroup aql TODO
 *  @{
 */

/**
 * @brief Packet type.
 */
typedef enum {
  HSA_AQL_PACKET_TYPE_ALWAYS_RESERVED = 0,
  HSA_AQL_PACKET_TYPE_INVALID = 1,
  HSA_AQL_PACKET_TYPE_DISPATCH = 2,
  HSA_AQL_PACKET_TYPE_BARRIER = 3,
  HSA_AQL_PACKET_TYPE_AGENT_DISPATCH = 4,
} hsa_aql_packet_type_t;

/**
 * @brief AQL packet header.
 */
typedef struct hsa_aql_packet_header_s {
  /**
   * Packet type.
   */
  hsa_aql_packet_type_t format:8;

  /**
   * If set then processing of packets will only begin when all preceding
   * packets are complete.
   */
  uint16_t barrier:1;

  /**
   * Determines the scope and type of the memory fence operation applied before
   * the packet enters the active phase. The valid values are 1 (the fence is
   * applied with component scope for the global segment) and 2 (the fence is
   * applied across both component and sytem scope for the global segment).
   */
  uint16_t acquire_fence_scope:2;

  /**
   * Determines the scope and type of the memory fence operation applied after
   * kernel completion but before the packet is completed. The valid values are
   * 1 (the fence is applied with component scope for the global segment) and 2
   * (the fence is applied across both component and sytem scope for the global
   * segment).
   */
  uint16_t release_fence_scope:2;

  /**
   * Must be zero.
   */
  uint16_t reserved:3;

} hsa_aql_packet_header_t;

/**
 * @brief AQL dispatch packet
 */
typedef struct hsa_aql_dispatch_packet_s{
  /**
   *  Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * Number of dimensions specified in the grid size. Valid values are 1,2, or
   * 3.
   */
  uint16_t dimensions:2;

  /**
   * Reserved, must be zero.
   */
  uint16_t reserved:14;

  /**
   * X dimension of work-group (measured in work-items).
   */
  uint16_t  workgroup_size_x;

  /**
   * Y dimension of work-group (measured in work-items).
   */
  uint16_t workgroup_size_y;

  /**
   * Z dimension of work-group (measured in work-items).
   */
  uint16_t workgroup_size_z;

  /**
   * Reserved. Must be zero.
   */
  uint16_t reserved2;

  /**
   * X dimension of grid (measured in work-items).
   */
  uint32_t  grid_size_x;

  /**
   * Y dimension of grid (measured in work-items).
   */
  uint32_t  grid_size_y;

  /**
   * Z dimension of grid (measured in work-items).
   */
  uint32_t grid_size_z;

  /**
   * Size (in bytes) of private memory allocation request (per work-item).
   */
  uint32_t private_segment_size_bytes;

  /**
   *  Size (in bytes) of group memory allocation request (per work-group).
   */
  uint32_t group_segment_size_bytes;

  /**
   *  Address of an object in memory that includes an implementation-defined
   *  executable ISA image for the kernel.
   */
  uint64_t  kernel_object_address;

  /**
   * Address of memory containing kernel arguments.
   */
  uint64_t kernarg_address;

  /**
   * Reserved.
   */
  uint64_t reserved3;

  /**
   * HSA signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_dispatch_packet_t;

/**
 * @brief Agent dispatch packet.
 */
typedef struct hsa_aql_agent_dispatch_packet_s {
  /**
   *  Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * The function to be performed by the destination HSA Agent. The type value
   * is split into the following ranges: 0x0000:0x3FFF (vendor specific),
   * 0x4000:0x7FFF (HSA runtime) 0x8000:0xFFFF (user registered function).
   */
  uint16_t type;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved2;

  /**
   * Pointer to location to store the function return value(s) in.
   */
  uint64_t return_location;

  /**
   * 64-bit direct or indirect argument.
   */
  uint64_t arg0;

  /**
   * 64-bit direct or indirect argument.
   */
  uint64_t arg1;

  /**
   * 64-bit direct or indirect argument.
   */
  uint64_t arg2;

  /**
   * 64-bit direct or indirect argument.
   */
  uint64_t arg3;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved3;

  /**
   * HSA signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_agent_dispatch_packet_t;

/**
 * @brief Barrier packet.
 */
typedef struct hsa_aql_barrier_packet_s {
  /**
   * Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * Reserved. Must be zero.
   */
  uint16_t reserved2;

  /**
   * Reserved. Must be zero.
   */
  uint32_t reserved3;

  /**
   * Dependent signal object.
   */
  hsa_signal_handle_t dep_signal0;

  /**
   * Dependent signal object.
   */
  hsa_signal_handle_t dep_signal1;

  /**
   * Dependent signal object.
   */
  hsa_signal_handle_t dep_signal2;

  /**
   * Dependent signal object.
   */
  hsa_signal_handle_t dep_signal3;

  /**
   * Dependent signal object.
   */
  hsa_signal_handle_t dep_signal4;

  /**
   * Reserved. Must be zero.
   */
  uint64_t reserved4;

  /**
   * HSA signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_barrier_packet_t;

/** @} */


/** \defgroup queue TODO
 *  @{
 */

/**
 * @brief Queue type. Intended to be used for dynamic queue protocol
 * determination.
 */
typedef enum {
  /**
   * Multiple producers are supported.
   */
  HSA_QUEUE_TYPE_MULTI = 0,
  /**
   * Only a single producer is supported.
   */
  HSA_QUEUE_TYPE_SINGLE = 1,
} hsa_queue_type_t;

/**
 * @brief Queue features.
 */
typedef enum {
  /**
   * Queue supports dispatch packets.
   */
  HSA_QUEUE_FEATURE_DISPATCH = 1,

  /**
   * Queue supports agent dispatch packets.
   */
  HSA_QUEUE_FEATURE_AGENT_DISPATCH = 2,
} hsa_queue_feature_t;

/**
 * @brief User mode queue. Queues are read-only: HSA agents can only modify the
 * contents of the buffer pointed by @a base_address.
 */
typedef struct hsa_queue_s{
  /**
   * Queue type.
   */
  hsa_queue_type_t queue_type;

  /**
   * Queue features mask. See ::hsa_queue_feature_t. HSA applications should
   * ignore any unknown set bits.
   */
  uint32_t queue_features;

  /**
   * Pointer to the base of the virtual memory which holds the AQL packets for
   * the queue. Aligned to the AQL packet size.
   */
  uint64_t base_address;

  /**
   * After writing a packet to the queue, the user must signal this handle with
   * the most recent write index. Doorbell signals are allocated by the runtime
   * during queue creation.
   */
  hsa_signal_handle_t doorbell_signal;

  /**
   * Maximum number of packets the queue can hold. Must be a power of two.
   */
  uint32_t size;

  /**
   * Queue identifier which is unique-per-process.
   */
  uint32_t queue_id;

  /**
   * A pointer to another user mode queue that can be used by the HSAIL kernel
   * to request system services. Provided by the application to the runtime API
   * call when the queue is created. May be NULL, the system provided service
   * queue or an application managed queue.
   */
  uint64_t service_queue;

} hsa_queue_t;

/**
 * @brief Service queue type.
 */
typedef enum {
  /**
   * Do not return a service queue, the service queue pointer will be set to
   * NULL by the runtime.
   */
  NONE=0,

  /**
   * Use a common runtime provided service queue.
   */
  COMMON,

  /**
   * Create a new service queue.
   */
  NEW,

  /**
   * Create a queue that runtime will not manage, the application has to manage
   * the queue manually.
   */
  APPLICATION_MANAGED
} hsa_service_queue_type_t;

/**
 * @brief Create a user mode queue.
 *
 * @param[in] component The component on which this queue is to be created.
 *
 * @param[in] size Size of the queue memory in number of packets in is expected
 * to hold. Required to be aligned with a power of two number of AQL packets.
 *
 * @param[in] queue_type Type of the queue (only type 0, which is default
 * in-order issue queue, is supported at this time).
 *
 * @param[in] service_queue_type The user can choose between NONE (no service
 * queue), COMMON (runtime provided service queue that is shared), NEW (require
 * the runtime to create a new queue).
 *
 * @param[out] queue The queue structure, filled up and returned by the runtime.
 *
 * @param[out] mailbox Mailbox to gather execution information to be used for
 * debug trap. User may pass NULL here if the user doesn't want the mailbox
 * created.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the queue size is not a power
 * of two, when the error message queue handle is invalid, or the component is
 * not valid. This error code is also returned when the @a queue is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime library in
 * the context of the queue creation. This error may also occur when the core
 * runtime library needs to spawn threads or create internal OS-specific
 * events. This error is also returned when a service queue or a user mode queue
 * cannot be allocated.
 */
hsa_status_t hsa_queue_create(const hsa_agent_t *component,
                                         size_t size,
                                         uint32_t queue_type,
                                         hsa_service_queue_type_t service_queue_type,
                                         hsa_queue_t **queue,
                                         hsa_queue_mailbox_t *mailbox);

/**
 * @brief Destroy a user mode queue.
 *
 * @details After destruction it is considered undefined to access any field of
 * the queue structure.
 *
 * @param[in] queue Queue.
 *
 * @retval ::HSA_STATUS_SUCCESS
 */
hsa_status_t hsa_queue_destroy(hsa_queue_t *queue);

/**
 * @brief Inactivate a queue.
 *
 * @details Inactivating the queue aborts any pending executions and prevent any
 * new packets from being processed. Any more packets written to the queue once
 * it is inactivated will be ignored by the packet processor.
 *
 * @param[in] queue Queue.
 *
 * @retval ::HSA_STATUS_SUCCESS
 */
hsa_status_t hsa_queue_inactivate(hsa_queue_t *queue);

/**
 * @brief Retrieve read index of a queue.
 *
 * @param[in] queue Queue.
 *
 * @return Read index.
 */
uint64_t hsa_queue_load_read_index_relaxed(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_read_index_relaxed
 */
uint64_t hsa_queue_load_read_index_acquire(hsa_queue_t *queue);

/**
 * @brief Retrieve write index of a queue.
 *
 * @param[in] queue Queue.
 *
 * @return Write index.
 */
uint64_t hsa_queue_load_write_index_relaxed(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_write_index_relaxed
 */
uint64_t hsa_queue_load_write_index_acquire(hsa_queue_t *queue);

/**
 * @brief Set the read index of a queue.
 *
 * @param[in] queue Queue.
 *
 * @param[in] val The new value of the read index.
 *
 */
void hsa_queue_store_read_index_relaxed(hsa_queue_t *queue, uint64_t val);

/**
 * @copydoc hsa_queue_store_read_index_relaxed
 */
void hsa_queue_store_read_index_release(hsa_queue_t *queue, uint64_t val);

/**
 * @brief Set the write index of a queue.
 *
 * @param[in] queue Queue.
 *
 * @param[in] val The new value of the write index.
 *
 */
void hsa_queue_store_write_index_relaxed(hsa_queue_t *queue, uint64_t val);

/**
 * @copydoc hsa_queue_store_write_index_relaxed
 */
void hsa_queue_store_write_index_release(hsa_queue_t *queue, uint64_t val);

/**
 * @brief Atomically compare and set the write index of a queue.
 *
 * @param[in] queue Queue.
 *
 * @param[in] expected The expected index value.
 *
 * @param[in] val Value to copy to the write index if @a expected matches the
 * observed write index.
 *
 * @return Previous value of the write index.
 */
uint64_t hsa_queue_cas_write_index_relaxed(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t val);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_release(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t val);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_acquire(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t val);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_acquire_release(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t val);

/**
 * @brief Increment the write index of a queue by an offset.
 *
 * @param[in] queue Queue.
 *
 * @param[in] val The value to add to the write index.
 *
 * @return Previous value of the write index.
 */
uint64_t hsa_queue_add_write_index_relaxed(hsa_queue_t *queue, uint64_t val);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_acquire(hsa_queue_t *queue, uint64_t val);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_release(hsa_queue_t *queue, uint64_t val);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_acquire_release(hsa_queue_t *queue,
  uint64_t val);

/** @} */


/** \addtogroup queue_error
 *  @{
 */
/**
 * @brief TODO.
 *
 * @param error_code input. the error code to convert to string.
 *
 * @param error_string output. the string description of the error code.
 */
hsa_status_t hsa_queue_error_string_from_code(
                uint64_t error_code,
                char **error_string);
/** @} */


/** \addtogroup packet_validate
 *  @{
 */
/**
 * @brief TODO.
 *
 * @param aql_packet input. Pointer to the AQL packet structure of the packet
 *      that needs to be validated.
 */
hsa_status_t hsa_aql_dispatch_packet_validate(hsa_aql_dispatch_packet_t *aql_packet);
/** @} */


/**
 * @brief TODO.
 *
 * @param aql_packet input. Pointer to the AQL packet structure of the packet
 *      that needs to be validated.
 */
hsa_status_t hsa_aql_barrier_packet_validate(hsa_aql_barrier_packet_t *aql_packet);
/** @} */


/** \addtogroup hsa_symbol
 *  @{
 */
/**
 * @brief TODO
 */
typedef struct hsa_symbol_s{
    /**
     * Current binding of that symbol.
     */
    uint64_t current_binding;

    /**
     * Directive section offset in BRIG.
     */
    uint32_t directive_section_offset;
} hsa_symbol_t;
/** @} */


/** \addtogroup hsa_symbol_map
 *  @{
 */
/**
 * @brief TODO
 */
typedef struct hsa_symbol_map_s{
    /**
     * The count of the total number of symbols in this map.
     */
    uint32_t number_of_symbols;

    /**
     * The array of symbols.
     */
    hsa_symbol_t *symbol;

    /**
     * For implementations to store other information and allow for queries.
     */
    uint64_t reserved;
} hsa_symbol_map_t;
/** @} **/

/** \addtogroup hsail_profile
 *  @{
 */
/**
 * @brief TODO
 */
typedef enum {
    /**
     * The base profile, as defined in PRM/SAR.
     */
    HSA_PROFILE_BASE = 0,

    /**
     * The full profile, as defined in PRM/SAR.
     */
    HSA_PROFILE_FULL = 1
}hsa_profile_t;
/** @} **/

typedef uint16_t hsa_exception_kind16_t;

/** \defgroup exception_type TODO
 *  @{
 */
/**
 * @brief Exception values.
 */
typedef enum {
  /**
   * IEEE 754 INVALID operation exception.
   */
  HSA_EXCEPTION_INVALID_OPERATION = 1,

  /**
   * An operation on finite operands gives an exact infinite result.
   */
  HSA_EXCEPTION_DIVIDE_BY_ZERO = 2,

  /**
   * A result is too large to be represented correctly.
   */
  HSA_EXCEPTION_OVERFLOW = 4,

  /**
   * A result is very small (outside the normal range) and inexact.
   */
  HSA_EXCEPTION_UNDERFLOW = 8,

  /**
   * Returns correctly rounded result by default.
   */
  HSA_EXCEPTION_INEXACT = 16

}hsa_exception_kind_mask_t ;
/** @} **/


/** \addtogroup control_directive_present64_t
 *  @{
 */
/**
 * @brief TODO
 */
typedef uint64_t hsa_control_directive_present64_t;
/** @} */

typedef uint32_t hsa_kernel_code_version32_t;

/** \addtogroup directive_present
 *  @{
 */
/**
 * @brief Mask indicating which control directives have been specified.
 */
typedef enum {
  /**
   * Mask that indicates break on exceptions is required by the user, the kernel
   * pauses execution and the queue mailbox signal is signaled and mailbox
   * updated.
   */
  HSA_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,

  /**
   * says that exceptions are recorded.
   */
  HSA_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,

  /**
   * says that max for dynamic group size is specified.
   */
  HSA_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,

  /**
   * If enabled.
   */
  HSA_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256

} hsa_control_directive_present_mask_t;

/** @} **/


/** \defgroup dim3 TODO
 *  @{
 */
/**
 * @brief 3D Dimensions.
 */
typedef struct hsa_dim3_s {
     /**
      * X dimension.
      */
      uint32_t x;
     /**
      * Y dimension.
      */
      uint32_t y;
     /**
      * Z dimension.
      */
      uint32_t z;
} hsa_dim3_t;

// TODO(mmario): parse this guy instead of the simplified version
/* typedef struct hsa_dim3_s { */
/*   union { */
/*     struct { */
/*       // X-dimension. */
/*       uint32_t x; */
/*       // Y-dimension. */
/*       uint32_t y; */
/*       // Z-dimension. */
/*       uint32_t z; */
/*     }; */
/*     // Array of dimensions. */
/*     uint32_t dim[3]; */
/*   }; */
/** @} **/


/** \defgroup control_directive TODO
 *  @{
 */
/**
 * @brief Control directives.
 */
typedef struct hsa_control_directives_s {
  /**
   If the value is 0 then there are no control directives specified and the rest
   of the fields can be ignored. The bits are accessed using the
   hsa_control_directives_present_mask_t. Any control directive that is not
   enabled in this bit set must have the value of all 0s.
   */
  hsa_control_directive_present64_t enabled_control_directives;

  /**
   If enable break exceptions is not enabled in @a enabled_control_directives,
   then must be 0, otherwise must be non-0 and specifies the set of HSAIL
   exceptions that must have the BREAK policy enabled.  If the HSAIL kernel
   being finalized has any enablebreakexceptions control directives, then the
   values specified by this argument are unioned with the values in these
   control directives. If any of the functions the kernel calls have an
   enablebreakexceptions control directive, then they must be equal or a subset
   of, this union.
   */
  hsa_exception_kind16_t enable_break_exceptions;

  /**
   If enable detect exceptions is not enabled in @a enabled_control_directives,
   then must be 0, otherwise must be non-0 and specifies the set of HSAIL
   exceptions that must have the DETECT policy enabled.  If the kernel being
   finalized has any enabledetectexceptions control directives, then the values
   specified by this argument are unioned with the values in these control
   directives. If any of the functions the kernel calls have an
   enabledetectexceptions control directive, then they must be equal or a subset
   of, this union.
   */
  hsa_exception_kind16_t enable_detect_exceptions;

  /**
   If max dynamic group size is not enabled in @a enabled_control_directives
   then this must be 0, and any amount of dynamic group segment can be allocated
   for a dispatch, otherwise the value specifies the maximum number of bytes of
   dynamic group segment that can be allocated for a dispatch. If the kernel
   being finalized has any maxdynamicsize control directives, then the values
   must be the same, and must be the same as this argument if it is
   enabled. This value can be used by the finalizer to determine the maximum
   number of bytes of group memory used by each work-group by adding this value
   to the group memory required for all group segment variables used by the
   kernel and all functions it calls, and group memory used to implement other
   HSAIL features such as fbarriers and the detect exception operations. This
   can allow the finalizer to determine the expected number of work-groups that
   can be executed by a compute unit and allow more resources to be allocated to
   the work-items if it is known that fewer work-groups can be executed due to
   group memory limitations.
   */
  uint32_t max_dynamic_group_size;

  /**
   If this is not enabled in @a enabled_control_directives then must be 0,
   otherwise must be greater than 0.  See HSA Programmer's Reference Manual
   description of maxflatgridsize control directive.
   */
  uint32_t max_flat_grid_size;

  /**
   If this is not enabled in @a enabled_control_directives then must be 0,
   otherwise must be greater than 0.  See HSA Programmer's Reference Manual
   description of maxflatgridsize control directive.
   */
  uint32_t max_flat_workgroup_size;

  /**
   If this is not enabled in @a enabled_control_directives then must be 0 and
   the finalizer may generate ISA that could result in any number of work-groups
   executing on a single compute unit.  Otherwise, the finalizer will attempt to
   generate ISA that will allow the specified number of work-groups to execute
   on a single compute unit. This is only a hint and can be ignored by the
   finalizer. If the kernel being finalized, or any of the functions it calls,
   has the same control directive, then the values must be the same or the
   finalization can fail. This can be used to determine the number of resources
   that should be allocated to a single work-group and work-item.
   */
  uint32_t requested_workgroups_per_cu;

  /**
   If not enabled then all elements for Dim3 must be 0, otherwise every element
   must be greater than 0. See HSA Programmer's Reference Manual description of
   requiredgridsize control directive.
   */
  hsa_dim3_t required_grid_size;

  /**
   If not enabled then all elements for Dim3 must be 0, and the produced code
   can be dispatched with any legal work-group range consistent with the
   dispatch dimensions. Otherwise, the code produced must always be dispatched
   with the specified work-group range. No element of the specified range must
   be 0. It must be consistent with required_dim and max_flat_workgroup_size. If
   the kernel being finalized, or any of the functions it calls, has a
   requiredworkgroupsize control directive, then the values must be the
   same. Specifying a value can allow the finalizer to optimize work-group id
   operations, and if the number of work-items in the work-group is less than
   the WAVESIZE then barrier operations can be optimized to just a memory fence.
   */
  hsa_dim3_t required_workgroup_size;

  /**
   If disabled then must be 0 and the produced kernel code can be dispatched
   with 1, 2 or 3 dimensions. If enabled then the value is 1..3 and the code
   produced must only be dispatched with a dimension that matches. Other values
   are illegal. If the kernel being finalized, or any of the functions it calls,
   has a requireddimsize control directive, then the values must be the
   same. This can be used to optimize the code generated to compute the absolute
   and flat work-group and work-item id, and the dim HSAIL operations.
  */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];

} hsa_control_directives_t;
/** @} **/



/// The current version number of the HSA code object
/// format.
typedef uint32_t hsa_code_kind32_t;

/** \defgroup codeversion TODO
 *  @{
 */

/**
 * @brief TODO.
 */
typedef uint32_t hsa_code_version32_t;

/**
 * @brief TODO.
 */
typedef enum {
  /**
   * Code version
   */
  HSA_CODE_VERSION = 0
} hsa_code_version_t;
/** @} **/

/** \defgroup codeheader TODO
 *  @{
 */
/**
 * @brief TODO.
 */
typedef struct hsa_code_s {
  /**
   The code format version. The version of this definition is specified by
   HSA_CODE_VERSION. Must match the value in the hsa_compilationunit_code_t that
   contains it.
   */
  hsa_code_version32_t code_version;

  /**
   The byte size of the struct that contains this hsa_code_t. Must be set to
   sizeof(hsa_*_code_t). Used for backward compatibility.
   */
  uint32_t struct_byte_size;

  /**
   Offset from base of hsa_code_t to compilationunit_code_t that contains this
   hsa_code_t to the base of this hsa_code_t. Can be used to navigate back to
   the enclosing compilation unit. Since hsa_compilationunit_code_t is always at
   offset 0, this value must be negative.
   */
  int64_t compilationunit_byte_offset;

  /**
   * Type of code object.
   */
  hsa_code_kind32_t code_type;

} hsa_code_t;
/** @} **/

typedef uint8_t powertwo8_t;
/// Value expressed as a power of two.
typedef uint8_t hsa_powertwo8_t;

/**
 * @brief TODO
 */
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

/** \defgroup code_object TODO
 *  @{
 */
/**
 * @brief TODO.
 */
typedef struct hsa_kernel_code_s{

  /**
   * Common header that all code objects start with. code.type must be
   * HSA_CODE_KERNEL.
   */
  hsa_code_t code;

  /**
   * The amount of group segment memory required by a work-group in bytes. This
   * does not include any dynamically allocated group segment memory that may be
   * added when the kernel is dispatched.
   */
  uint32_t workgroup_group_segment_byte_size;

  /**
   * The size in bytes of the kernarg segment that holds the values of the
   * arguments to the kernel.
   */
  uint64_t kernarg_segment_byte_size;

  /**
   * The amount of memory required for the combined private, spill and arg
   * segments for a work-item in bytes.
  */
  uint32_t workitem_private_segment_byte_size;

  /**
   * Number of fbarrier's used in the kernel and all functions it calls. If the
   * implementation uses group memory to allocate the fbarriers then that amount
   * must already be included in the @a workgroup_group_segment_byte_size total.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   * The values are the actually values used by the finalizer in generating the
   * code. This may be the union of values specified as finalizer arguments and
   * explicit HSAIL control directives. If a finalizer implementation ignores a
   * control directive, and not generate constrained code, then the control
   * directive will not be marked as enabled even though it was present in the
   * HSAIL or finalizer argument. The values are intended to reflect the
   * constraints that the code actually requires to correctly execute, not the
   * values that were actually specified at finalize time.
   */
  hsa_control_directives_t control_directive;

  /**
   * Wavefront size expressed as a power of two. Must be a power of 2 in range
   * 1..64 inclusive. Used to support runtime query that obtains wavefront size,
   * which may be used by application to allocated dynamic group memory and set
   * the dispatch work-group size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   * The maximum byte alignment of variables used by the kernel in the specified
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16.
   */
  hsa_powertwo8_t kernarg_segment_alignment;
  /**
   * TODO.
   */
  hsa_powertwo8_t group_segment_alignment;
  /**
   * TODO.
   */
  hsa_powertwo8_t private_segment_alignment;

  /**
   * The optimization level specified when the kernel was finalized.
   */
  uint8_t optimization_level;

  /**
   * Reserved. Must be 0.
   * Component specific fields can follow this field.
   */
  uint8_t reserved1[3];

} hsa_kernel_code_t;
/** @} */


/** \defgroup brig TODO
 *  @{
 */
/**
 * @brief BRIG representation.
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
     * All of the executable operations. Most operations contain offsets to the
     * .operand section.
     */
    uint8_t *code_section;

    /**
     * The operands, such as immediate constants, registers, and address
     * expressions, that appear in the operations.
     */
    uint8_t *operand_section;
} hsa_brig_t;
/** @} */


/** \defgroup codeentry TODO
 *  @{
 */
/**
 * @brief TODO
 */
typedef struct hsa_code_entry_s {
  /**
   * ID of the entity that generated the code. For HSAIL will be the BRIG
   * directive offset of the kernel or function declaration. The array of
   * hsa_code_entry_t are required to be ordered in ascending code_id to allow
   * faster lookup.
   */
  uint64_t code_id;

  /**
   * Byte offset from start of hsa_compilationunit_code_t to corresponding
   * hsa_code_t. Every hsacode_t starts with a common hsa_code_t, and its
   * code_type field indicates what specific hsa_code_t it is.
   */
  int64_t code_byte_offset;
} hsa_code_entry_t;
/** @} */



/** \defgroup codekind TODO
 *  @{
 */
/**
 * @brief TODO.
 */
typedef enum {
  /**
   * Not a code object.
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
   * HSA runtime code objects. For example, partially linked code objects.
   */
  HSA_CODE_RUNTIME_FIRST = 0x40000000,
  /**
   * TODO.
   */
  HSA_CODE_RUNTIME_LAST = 0x7fffffff,

  /**
   * Vendor specific code objects.
   */
  HSA_CODE_VENDOR_FIRST = 0x80000000,
  /**
   * TODO.
   */
  HSA_CODE_VENDOR_LAST = 0xffffffff,
} hsa_code_kind_t;
/** @} */


typedef uint8_t hsa_machine_model8_t;
typedef uint8_t hsa_profile8_t;
typedef uint32_t hsa_code_properties32_t;
typedef uint16_t hsa_target_options16_t;


/** \defgroup codeproperties TODO
 *  @{
 */
/**
 * @brief TODO
 */
typedef enum {
  /**
   * The code is position independent (can be executed at any address that meets
   * the alignment requirement).
   */
  HSA_CODE_PROPERTY_PIC = 1
} hsa_code_properties_mask_t;
/** @} */

/** \defgroup compilationunit TODO
 *  @{
 */
/**
 * @brief TODO
 */
typedef struct hsa_compilationunit_code_s{
  /**
   * The code format version. The version of this definition is specified by
   * HSA_CODE_VERSION.
   */
  hsa_code_version32_t code_version;

  /**
   * The byte size of this struct. Must be set to
   * sizeof(hsa_compilationunit_code_t). Used for backward compatibility.
   */
  uint32_t struct_byte_size;

  /**
   * The vendor of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 624 character encoding must be used. If the name is less
   * than 16 characters then remaining characters must be set to 0.
   */
  char component_vendor[16];

  /**
   * The vendor's name of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 646 character encoding must be used. If the name is less
   * than 16 characters then remaining characters must be set to 0.
   */
  char component_name[16];

  /**
   * Byte offset from start of hsa_compilationunit_code_t to an array of
   * code_entry_count elements of type hsa_code_entry_t. Since
   * hsa_compilationunit_code_t is always at offset 0, this value must be
   * positive.
   */
  int64_t code_entry_byte_offset;

  /**
   * Number of code entries in this compilation unit.
   */
  uint32_t code_entry_count;

  /**
   * The required alignment of this hsa_compilationunit_code_t expressed as a
   * power of 2. The Finalizer must set this to the value required by the HSA
   * component it will execute on and the assumptions of the machine code it
   * contains.
   */
  hsa_powertwo8_t code_alignment;

  /**
   * Must be 0.
   */
  uint8_t reserved[3];

  /**
   The size of the single contiguous block of memory which includes this
   hsa_compilationunit_code_t header and all following hsa_*_code_t and
   associated machine code.
   */
  uint64_t code_size_bytes;

  /**
   The base address that this hsa_compilationunit_code_t must be allocated in
   order to execute the code it contains. The address must be a multiple of the
   alignment specified by the alignment field. If the code is position
   independent (can be executed at any address that meets the alignment
   requirement), then this field must be 0.
   */
  uint64_t code_base_address;

  /**
   A bit set of flags providing information about the code in this compilation
   unit. Unused flags must be 0.
   */
  hsa_code_properties32_t code_properties;

  /**
   The HSAIL major version. This information is from the HSAIL version
   directive. If this hsa_compilationunit_code_t is not generated from an HSAIL
   compilation unit then must be 0.
   */
  uint32_t hsail_version_major;

  /**
   The HSAIL minor version. This information is from the HSAIL version
   directive. If this hsa_compilationunit_code_t is not generated from an HSAIL
   compilation unit then must be 0.
   */
  uint32_t hsail_version_minor;

  /**
   The HSAIL profile defines which features are used. This information is from
   the HSAIL version directive. If this hsa_compilationunit_code_t is not
   generated from an HSAIL compilation unit then must still indicate what
   profile is being used.
   */
  hsa_profile8_t hsail_profile;

  /**
   The HSAIL machine model gives the address sizes used by the code. This
   information is from the HSAIL version directive. If not generated from an
   HSAIL compilation unit then must still indicate for what machine mode the
   code is generated.
   */
  hsa_machine_model8_t hsail_machine_model;

  /**
   The HSAIL target features. There are currently no target options so this
   field must be 0. If target options are added they will be specified by the
   HSAIL version directive. If this hsa_compilationunit_code_t is not generated
   from an HSAIL compilation unit then must be 0.
   */
  hsa_target_options16_t hsail_target_options;

} hsa_compilationunit_code_t;
/** @} */

typedef uint64_t hsa_debug_info_t;
typedef uint64_t hsa_finalize_compilationunit_caller_t;

/** \addtogroup brig_directive_offset
 *  @{
 */
/**
 * @brief TODO.
 */
typedef uint32_t hsa_brig_directive_offset_t;
/** @} */

/** \defgroup map_symbol_address TODO
 *  @{
 */
/**
 * @brief TODO.
 */
typedef hsa_status_t (*hsa_map_symbol_address_t)(hsa_finalize_compilationunit_caller_t caller, hsa_brig_directive_offset_t symbol_directive, uint64_t* address);
/** @} */

/** \defgroup compilation_debug TODO
 *  @{
 */
/**
 * @brief Debug unit.
 */
typedef struct hsa_compilationunit_debug_s {
   /**
    * HSA does not define the debug format, it is device specific. It is not
    * required to be implemented as a contiguous region of memory.
    */
  uint64_t reserved;
} hsa_compilationunit_debug_t;
/** @} */

/** \defgroup alloc_t TODO
 *  @{
 */
/**
 * @brief TODO.
 */
typedef void* hsa_finalize_brig_caller_t;
/** @} */

/**
 * @brief TODO.
 */
typedef hsa_status_t (*hsa_alloc_t)(
      hsa_finalize_brig_caller_t caller,
      size_t byte_size,
      size_t byte_alignment,
      void** address);

/** \defgroup finalize_brig TODO
 *  @{
 */

/**
 * @brief Finalize BRIG.
 *
 * @details The input pointer must reference an in memory location of the
 * BRIG. Also, the BRIG must contain at the minimum the 4 basic BRIG sections
 * (.strings, .directives, .code, .operands). All symbols in the symbol/offset
 * table must be resolved.
 *
 * @param caller Opaque pointer and will be passed to all call back functions
 *      made by this call of the finalizer.

 * @param component input. A valid pointer to the hsa_agent_t.
 *
 * @param brig input. A pointer to the in memory BRIG structure.
 *
 * @param code_count The number of kernels plus functions to produce
 *      hsa_kernel_code_t and hsa_function_code_t objects for in the generated
 *      hsa_compilationunit_code_t.
 *
 * @param code_directive A pointer to an array with code_count entries of
 *      hsa_brig_directive_offset_t. Each entry is the offset in the directive
 *      section of the passed brig of a kernel or function definition. These
 *      will be the kernels and functions that will have code objects generated
 *      in the produced hsa_compilationunit_code_t.
 *
 * @param control_directives The control directives that can be specified to
 *      influence how the finalizer generates code. If NULL then no control
 *      directives are used.
 *
 * @param map_symbol_address Used by the finalizer to obtain the segment address
 *      for global segment symbols. The value of caller will be passed to every
 *      call.

 * @param allocate_compilationunit_code The callback function that the finalizer
 *      will use to allocate the contiguous block of memory that will be used
 *      for the hsa_compilationunit_code_t that is returned. It is the
 *      responsibility of the call of the finalizer to deallocate this memory,
 *      even if the finalizer does not report success.

 * @param allocate_compilationunit_debug The callback function that the
 *      finalizer will use to allocate the memory that will be used for the
 *      hsa_compilationunit_debug_t that is returned. It is the responsibility
 *      of the call of the finalizer to destroy this memory, even if the
 *      finalizer does not report success.
 *
 * @param optimization_level an implementation defined value that control the
 *      level of optimization performed by the finalizer.
 *
 * @param options implementation defined options that can be specified to the
 *      finalizer.  compilationunit_code: if the return status is success then a
 *      pointer to the generated hsa_compilationunit_code_t for the HSA
 *      component must be written.
 *
 * @param compilationunit_code TODO.
 *
 * @param compilationunit_debug TODO.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a brig is NULL or invalid, or
 * if @a kernel_directive is invalid.
 *
 * @retval ::HSA_STATUS_INFO_UNRECOGNIZED_OPTIONS If the options are not
 * recognized, no error is returned, just an info status is used to indicate
 * invalid options.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the finalize API cannot
 * allocate memory for compilationunit_code and compilationunitdebug or the
 * deserialize cannot allocate memory for code_object.
*
 * @retval ::HSA_STATUS_ERROR_DIRECTIVE_MISMATCH If the directive in the control
 * directive structure and in the HSAIL kernel mismatch or if the same directive
 * is used with a different value in one of the functions used by this kernel.
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
/** @} */


/** \defgroup finalize_destroy TODO
 *  @{
 */

/**
 * @brief Destroys the compilation unit code object.
 *
 * @param[in] object A pointer to the compilation unit object that needs to be
 *        destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If some of the resources consumed
 * during initialization by the runtime could not be freed.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a object is NULL or does not
 * point to a valid code object.
 */
hsa_status_t hsa_compilationunit_code_destroy(hsa_compilationunit_code_t *object);


/**
 * @brief Destroys the compilation unit debug object.
 *
 * @param[in] object A pointer to the compilation unit debug object that needs
 *        to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If some of the resources consumed
 * during initialization by the runtime could not be freed.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a object is NULL or does not
 * point to a valid code object.
*/
hsa_status_t hsa_compilationunit_debug_destroy(hsa_compilationunit_debug_t *object);
/** @} */


/** \defgroup finalize_serial TODO
 *  @{
 */
/**
 * @brief Serialize a code object.
 *
 * @param[in] code_object The code object to serialize.
 *
 * @param allocate_compilationunit_code TODO.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a code_object or is NULL or
 * does not point to a valid code object. If @a serialized_object is either null
 * or is not valid or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated for
 * @a serialized_object.
*/
hsa_status_t hsa_compilationunit_serialize(hsa_compilationunit_code_t *code_object,
    hsa_alloc_t allocate_compilationunit_code,
    void *serialized_object);

/**
 * @brief Recreate a serialized code object.
 *
 * @param[in] serialized_object Pointer to the serialized object.
 *
 * @param[out] code_object The code object generated as a part of
 *      serialization. Runtime allocated.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a serialized_object is either
 * null or is not valid or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated for
 * @a code_object.
*/
hsa_status_t hsa_compilationunit_deserialize( void *serialized_object,
    hsa_compilationunit_code_t **code_object);
/** @} */


/** \addtogroup bind_kernel_code
 *  @{
 */
/**
 * @brief TODO
 *
 * @param kernel
 *      input. A pointer to the kernel code object.
 * @param symbol_map
 *      input. A pointer to the symbol information structure.
 * @param output_kernel
 *      runtime allocated, output. A pointer to the kernel code object.
*/
hsa_status_t hsa_symbol_bind_code_object( hsa_code_t *kernel,
    hsa_symbol_map_t *symbol_map,
    hsa_code_t *output_kernel);
/** @} */


/** \addtogroup query_symbol
 *  @{
 */
/**
 * @brief TODO.
 *
 * @param symbol_map input. a pointer to the kernel code object.
 *
 * @param brig input. the brig this symbol table came from.
 *
 * @param symbol_name input. the name of the symbol.
 *
 * @param symbol_details runtime allocated, output. Structure with symbol
 *      details.
*/
hsa_status_t hsa_symbol_map_query_by_name(hsa_symbol_map_t *symbol_map,
    hsa_brig_t brig,
    char *symbol_name,
    hsa_symbol_t *symbol_details);

/**
 * @brief TODO.
 *
 * @param symbol_map a pointer to the symbol map structure.
 *
 * @param brig the brig this symbol table came from.
 *
 * @param directive_section_offset TODO.
 *
 * @param symbol_details structure with symbol details.
*/
hsa_status_t hsa_symbol_map_query_by_offset(hsa_symbol_map_t *symbol_map,
    hsa_brig_t *brig,
    uint32_t directive_section_offset,
    hsa_symbol_t *symbol_details);

/**
 * @brief TODO
 *
 * @param symbol_map input. destroys the symbol map resources including any
 *      additional structures.
 */
hsa_status_t hsa_symbol_destroy(hsa_symbol_map_t *symbol_map);
/** @} */


/** \defgroup register TODO
 *  @{
 */
/**
 *
 * @brief Register memory.
 *
 * @details Registering a system memory region for use with all the available
 * devices This is an optional interface that is solely provided as a
 * performance optimization hint to the underlying implementation so it may
 * prepare for the future use of the memory by the devices. The interface is
 * only beneficial for system memory that will be directly accessed by a device.
 *
 * Overlapping registrations are allowed. This is neither detrimental nor
 * beneficial.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * registered. If a null pointer is passed, no operation is performed.
 *
 * @param[in] size Requested registration size in bytes. If a size of zero is
 * passed, no operation is performed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocating the necessary resources.
 *
 */
hsa_status_t hsa_memory_register(void *address,
    size_t size);

/**
 *
 * @brief Deregister memory.
 *
 * @details Used for deregistering a memory region previously registered.
 *
 * Deregistration must be performed using an address that was previously
 * registered.  In the event that deregistration is performed on an address that
 * has been used in multiple registrations, the smallest of the registrations is
 * deregistered.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * deregistered. If a NULL pointer is passed, no operation is performed.
 *
 *  @retval ::HSA_STATUS_SUCCESS
 *
 *  @retval ::HSA_STATUS_INFO_NOT_REGISTERED If the pointer has not been
 *  registered before.
 *
 */
hsa_status_t hsa_memory_deregister(void *address);
/** @} */

/** \defgroup memory_allocate TODO
 *  @{
 */
/**
 * @brief Allocate system memory.
 *
 * @details The returned buffer is already registered. Allocation of size 0 is
 * allowed and returns a NULL pointer.

 * @param[in] size_bytes Allocation size.
 *
 * @param[in] address Address pointer allocated by the user. Dereferenced and
 * assigned to the pointer to the memory allocated for this request.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed address is NULL.
 */
hsa_status_t hsa_memory_allocate(size_t size_bytes, void **address);

/** @} */

/** \defgroup memory_local TODO
 *  @{
 */
/**
 * @brief Allocate memory on HSA Device.
 *
 * @details Allocate global device memory associated with specified
 * device.
 *
 * @param[in] component A valid pointer to the HSA device for which the
 * specified amount of global memory is to be allocated.
 *
 * @param[in] size Requested allocation size in bytes. If size is 0, NULL is
 * returned.
 *
 * @param[out] address A valid pointer to the location of where to return the
 * pointer to the base of the allocated region of memory.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime
 * library. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed component is NULL
 * or invalid, or if the passed pointer is NULL.
 */
hsa_status_t hsa_memory_allocate_component_local( const hsa_agent_t *component,
    size_t size,
    void **address);

/**
 *
 * @brief Deallocate memory on HSA component.
 *
 * @details Deallocate component memory that was allocated with
 * ::hsa_memory_allocate_component_local.
 *
 * @param[in] address A pointer to the address to be deallocated. If the pointer
 * is NULL, no operation is performed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 */
hsa_status_t hsa_memory_free_component_local(void *address);

/**
 * @brief Copy between the system and local heaps.
 *
 * @param[out] dst A valid pointer to the destination array where the content is
 * to be copied.
 *
 * @param[in] src A valid pointer to the source of data to be copied.
 *
 * @param[in] size Number of bytes to copy.
 *
 * @param[in] signal The signal that will be incremented by the runtime when the
 * copy is complete.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime
 * library. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any argument is invalid.
 */
hsa_status_t hsa_memory_copy_component_local_to_system(void *dst,
    const void *src,
    size_t size,
    hsa_signal_handle_t signal);
/** @} */


/** \defgroup images Images API
 *  @{
 */

/**
 * @brief Image handle, populated by ::hsa_image_create_handle. Images handles
 * are only unique within an agent, not across agents.
 *
 */
typedef struct hsa_image_handle_s {
   /**
    * HSA component specific handle to the image.
    */
    uint64_t handle;

} hsa_image_handle_t;

/**
 * @brief Image format capability returned by ::hsa_image_get_format_capability
 */
typedef enum  {
   /**
    * Images of this format are not supported.
    */
    HSA_IMAGE_FORMAT_NOT_SUPPORTED = 0x0,

   /**
    * Images of this format can be accessed for read operations.
    */
    HSA_IMAGE_FORMAT_READ_ONLY = 0x1,

   /**
    * Images of this format can be accessed for write operations.
    */
    HSA_IMAGE_FORMAT_WRITE_ONLY = 0x2,

    /**
    * Images of this format can be accessed for read and write operations.
    */
    HSA_IMAGE_FORMAT_READ_WRITE = 0x4,

    /**
    * Images of this format can be accessed for read-modify-write operations.
    */
    HSA_IMAGE_FORMAT_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this format are guaranteed to have consistent data layout
    * regardless of the how it is accessed by the HSA agent.
    */
    HSA_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA = 0x10

} hsa_image_format_capability_t;

/**
 * @brief Agent-specific image size and alignment requirements. This structure
 * stores the agent-dependent image data sizes and alignment, and populated by
 * ::hsa_image_get_info.
 */
typedef struct hsa_image_info_s {
  /**
   * Component specific image data size in bytes.
   */
  size_t image_size;

  /**
   * Component specific image data alignment in bytes.
   */
  size_t image_alignment;

} hsa_image_info_t;

/**
 * @brief Defines how the HSA device expects to access the image. The access
 * pattern used by the HSA agent specified in ::hsa_image_create_handle.
 *
 */
typedef enum {
  /**
   * Image handle is to be used by the HSA agent as read-only using an HSAIL
   * roimg type.
   */
  HSA_AGENT_IMAGE_ACCESS_READ_ONLY,

  /**
   * Image handle is to be used by the HSA agent as write-only using an HSAIL
   * woimg type.
   */
  HSA_AGENT_IMAGE_ACCESS_WRITE_ONLY,

  /**
   * Image handle is to be used by the HSA agent as read and/or write using an
   * HSAIL rwimg type.
   */
  HSA_AGENT_IMAGE_ACCESS_READ_WRITE

} hsa_agent_image_access_t;

/**
 * @brief Geometry associated with the HSA image (image dimensions allowed in
 * HSA). The enumeration values match the HSAIL BRIG type BrigImageGeometry.
 */
typedef enum {
/**
   * One-dimensional image addressed by width coordinate.
   */
  HSA_IMAGE_GEOMETRY_1D = 0,

  /**
   * Two-dimensional image addressed by width and height coordinates.
   */
  HSA_IMAGE_GEOMETRY_2D = 1,

  /**
   * Three-dimensional image addressed by width, height, and depth coordinates.
   */
  HSA_IMAGE_GEOMETRY_3D = 2,

  /**
   * Array of one-dimensional images with the same size and format. 1D arrays
   * are addressed by index and width coordinate.
   */
  HSA_IMAGE_GEOMETRY_1DA = 3,

  /**
   * Array of two-dimensional images with the same size and format. 2D arrays
   * are addressed by index and width and height coordinates.
   */
  HSA_IMAGE_GEOMETRY_2DA = 4,

  /**
   * One-dimensional image interpreted as a buffer with specific restrictions.
   */
  HSA_IMAGE_GEOMETRY_1DB = 5,

  /**
   * Two-dimensional depth image addressed by width and height coordinates.
   */
  HSA_IMAGE_GEOMETRY_2DDEPTH = 6,

  /**
   * Array of two-dimensional depth images with the same size and format.  2D
   * arrays are addressed by index and width and height coordinates.
   */
  HSA_IMAGE_GEOMETRY_2DADEPTH = 7
} hsa_image_geometry_t;

/**
* @brief Component type associated with the image. See Image section in HSA
* Programming Reference Manual for definitions on each component type. The
* enumeration values match the HSAIL BRIG type BrigImageChannelType.
*/
typedef enum
{
    HSA_IMAGE_CHANNEL_TYPE_SNORM_INT8 = 0,
    HSA_IMAGE_CHANNEL_TYPE_SNORM_INT16 = 1,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_INT8 = 2,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_INT16 = 3,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_INT24 = 4,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_SHORT_555 = 5,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_SHORT_565 = 6,
    HSA_IMAGE_CHANNEL_TYPE_UNORM_SHORT_101010 = 7,
    HSA_IMAGE_CHANNEL_TYPE_SIGNED_INT8 = 8,
    HSA_IMAGE_CHANNEL_TYPE_SIGNED_INT16 = 9,
    HSA_IMAGE_CHANNEL_TYPE_SIGNED_INT32 = 10,
    HSA_IMAGE_CHANNEL_TYPE_UNSIGNED_INT8 = 11,
    HSA_IMAGE_CHANNEL_TYPE_UNSIGNED_INT16 = 12,
    HSA_IMAGE_CHANNEL_TYPE_UNSIGNED_INT32 = 13,
    HSA_IMAGE_CHANNEL_TYPE_HALF_FLOAT = 14,
    HSA_IMAGE_CHANNEL_TYPE_FLOAT = 15

} hsa_image_channel_type_t;

/**
 *
* @brief Image component order associated with the image. See Image section in
* HSA Programming Reference Manual for definitions on each component order. The
* enumeration values match the HSAIL BRIG type BrigImageChannelOrder.
*
*/
typedef enum {

    HSA_IMAGE_CHANNEL_ORDER_A = 0,
    HSA_IMAGE_CHANNEL_ORDER_R = 1,
    HSA_IMAGE_CHANNEL_ORDER_RX = 2,
    HSA_IMAGE_CHANNEL_ORDER_RG = 3,
    HSA_IMAGE_CHANNEL_ORDER_RGX = 4,
    HSA_IMAGE_CHANNEL_ORDER_RA = 5,
    HSA_IMAGE_CHANNEL_ORDER_RGB = 6,
    HSA_IMAGE_CHANNEL_ORDER_RGBX = 7,
    HSA_IMAGE_CHANNEL_ORDER_RGBA = 8,
    HSA_IMAGE_CHANNEL_ORDER_BGRA = 9,
    HSA_IMAGE_CHANNEL_ORDER_ARGB = 10,
    HSA_IMAGE_CHANNEL_ORDER_ABGR = 11,
    HSA_IMAGE_CHANNEL_ORDER_SRGB = 12,
    HSA_IMAGE_CHANNEL_ORDER_SRGBX = 13,
    HSA_IMAGE_CHANNEL_ORDER_SRGBA = 14,
    HSA_IMAGE_CHANNEL_ORDER_SBGRA = 15,
    HSA_IMAGE_CHANNEL_ORDER_INTENSITY = 16,
    HSA_IMAGE_CHANNEL_ORDER_LUMINANCE = 17,
    HSA_IMAGE_CHANNEL_ORDER_DEPTH = 18,
    HSA_IMAGE_CHANNEL_ORDER_DEPTH_STENCIL = 19

} hsa_image_channel_order_t;

/**
 * @brief Image format descriptor (attributes of the image format).
 */
typedef struct hsa_image_format_s {
/**
    * Channel type of the image.
    */
    hsa_image_channel_type_t channel_type;

   /**
    * Channel order of the image.
    */
    hsa_image_channel_order_t channel_order;
} hsa_image_format_t;

/**
 * @brief Implementation-independent HSA Image descriptor.
 */
typedef struct hsa_image_descriptor_s {
  /**
   * Geometry of the image.
   */
  hsa_image_geometry_t geometry;

  /**
   * Width of the image in components.
   */
  size_t width;

  /**
   * Height of the image in components, only used if geometry is 2D or higher.
   */
  size_t height;

  /**
   * Depth of the image in slices, only used if geometry is 3D depth = 0 is same
   * as depth = 1.
   */
  size_t depth;

  /**
   * Number of images in the image array, only used if geometry is 1DArray and
   * 2DArray.
   */
  size_t array_size;

  /**
   * Format of the image.
   */
  hsa_image_format_t format;


} hsa_image_descriptor_t;

/**
 * @brief 3D image coordinate offset for image manipulation.
 */
typedef struct hsa_image_offset_s {
   /**
    * X coordinate for the offset.
    */
    uint32_t x;

   /**
    * Y coordinate for the offset.
    */
    uint32_t y;

   /**
    * Z coordinate for the offset
    */
    uint32_t z;

} hsa_image_offset_t;

/**
 * @brief Three-dimensional image range description.
 */
typedef struct hsa_image_range_s {
   /**
    * The width for an image range (in coordinates).
    */
    uint32_t width;

   /**
    * The height for an image range (in coordinates).
    */
    uint32_t height;

   /**
    * The depth for an image range (in coordinates).
    */
    uint32_t depth;

} hsa_image_range_t;

/**
 * @brief Image region description. Used by image operations such as import,
 * export, copy, and clear.
 */
typedef struct hsa_image_region_s {
   /**
    * Offset in the image (in coordinates).
    */
    hsa_image_offset_t image_offset;

   /**
    * Dimensions of the image range (in coordinates).
    */
    hsa_image_range_t image_range;

} hsa_image_region_t;

/**
 * @brief Sampler handle. Samplers are populated by
 * ::hsa_sampler_create_handle. Sampler handles are only unique within an agent,
 * not across agents.
 */
typedef struct hsa_sampler_handle_s {
   /**
    * Component-specific HSA sampler.
    */
    uint64_t handle;

} hsa_sampler_handle_t;

/**
 * @brief Sampler address modes. The sampler address mode describes the
 * processing of out-of-range image coordinates.The values match the HSAIL BRIG
 * type BrigSamplerAddressing.
 */
typedef enum {

  /**
   * Out-of-range coordinates are not handled.
   */
  HSA_SAMPLER_ADDRESSING_UNDEFINED = 0,

  /**
   * Clamp out-of-range coordinates to the image edge.
   */
  HSA_SAMPLER_ADDRESSING_CLAMP_TO_EDGE = 1,

  /**
   * Clamp out-of-range coordinates to the image border.
   */
  HSA_SAMPLER_ADDRESSING_CLAMP_TO_BORDER = 2,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate range.
   */
  HSA_SAMPLER_ADDRESSING_REPEAT = 3,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate range.
   */
  HSA_SAMPLER_ADDRESSING_MIRRORED_REPEAT = 4

} hsa_sampler_addressing_mode_t;


/**
 * @brief Sampler coordinate modes. The enumeration values match the HSAIL BRIG
 * BRIG_SAMPLER_COORD bit in the type BrigSamplerModifier.
 */
typedef enum {
  /**
   * Coordinates are all in the range of 0.0 to 1.0.
   */
  HSA_SAMPLER_COORD_NORMALIZED = 0,

  /**
   * Coordinates are all in the range of 0 to (dimension-1).
   */
  HSA_SAMPLER_COORD_UNNORMALIZED = 1

} hsa_sampler_coordinate_mode_t;

/**
 * @brief Sampler filter modes. The enumeration values match the HSAIL BRIG type
 * BrigSamplerFilter.
 */
typedef enum {
  /**
   * Filter to the image element nearest (in Manhattan distance) to the
   * specified coordinate.
   */
  HSA_SAMPLER_FILTER_NEAREST = 0,

  /**
   * Filter to the image element calculated by combining the elements in a 2x2
   * square block or 2x2x2 cube block around the specified coordinate. The
   * elements are combined using linear interpolation.
   */
  HSA_SAMPLER_FILTER_LINEAR = 1

} hsa_sampler_filter_mode_t;

/**
 * @brief Implementation-independent sampler descriptor.
 */
typedef struct hsa_sampler_descriptor_s {
  /**
   * Sampler coordinate mode describes the normalization of image coordinates.
   */
  hsa_sampler_coordinate_mode_t coordinate_mode;

  /**
   * Sampler filter type describes the type of sampling performed.
   */
  hsa_sampler_filter_mode_t filter_mode;

  /**
   * Sampler address mode describes the processing of out-of-range image.
   * coordinates
   */
  hsa_sampler_addressing_mode_t address_mode;

} hsa_sampler_descriptor_t;

/**
 * @brief Retrieve image format capabilities for the specified image format on
 * the specified HSA component.
 *
 * @details If successful, the queried image format's capabilities bit-mask is
 * written to the location specified by @a capability_mask. See
 * ::hsa_image_format_capability_t to determine all possible capabilities that
 * can be reported in the bit mask.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_format Image format.
 *
 * @param[in] image_geometry Geometry of the image.
 *
 * @param[out] capability_mask Image format capability bit-mask.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a image_format, or
 *       @a capability_mask are NULL.
 */
hsa_status_t hsa_image_get_format_capability(const hsa_agent_t *agent,
                         const hsa_image_format_t *image_format,
                         hsa_image_geometry_t image_geometry,
                         uint32_t *capability_mask);
/**
 *
 * @brief Inquires the required HSA component-specific image data details from a
 * implementation independent image descriptor.
 *
 * @details If successful, the function the queried HSA agent-specific image
 * data info is written to the location specified by @a image_info. Based on the
 * implementation the optimal image data size and alignment requirements could
 * vary depending on the image attributes specified in @a image_descriptor.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[out] image_info Image info size and alignment requirements that the
 * HSA agent requires.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_STATUS_UNSUPPORTED_IMAGE_FORMAT If the HSA agent does not
 * support the image format specified by the descriptor.
 *
 * @retval ::HSA_STATUS_IMAGE_SIZE_NOT_SUPPORTED If the HSA agent does not
 * support the image dimensions specified by the format descriptor.
 */
hsa_status_t hsa_image_get_info(
                         const hsa_agent_t *agent,
                         const hsa_image_descriptor_t *image_descriptor,
                         hsa_image_info_t *image_info);

/**
 * @brief Creates a agent-defined image handle from an
 * implementation-independent image descriptor and a agent-specific image
 * data. The image access defines how the HSA agent expects to use the image and
 * must match the HSAIL image handle type used by the agent.
 *
 * @details If successful, the image handle is written to the location specified
 * by @a image_handle. The image data memory must be allocated using the
 * previously queried ::hsa_image_get_info memory requirements with the same HSA
 * agent and implementation-independent image descriptor.
 *
 * The image data is not initialized and any previous memory contents is
 * preserved. The memory management of image data is the application's
 * responsibility and can only be freed until the memory is no longer needed and
 * any image handles using it are destroyed.
 *
 * @a agent_access defines how the HSA agent expects to use the image
 * handle. The image format specified in @a image_descriptor must be capable by
 * the HSA agent for the intended access pattern.
 *
 * Image handles with different agent_access can be created using the same image
 * data with exactly the same image descriptor as long as
 * ::HSA_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA is reported by
 * ::hsa_image_get_format_capability for the image format specified in the image
 * descriptor. Images of non-linear SRGB channel order types can share the same
 * image data with its equivalent linear RGB channel order types, provided the
 * rest of image descriptor parameters are identical.
 *
 * If necessary, an application can use image operations (import, export, copy,
 * clear) to prepare the image for the intended use regardless of the access
 * pattern.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[in] image_data Address of the component-specific image data.
 *
 * @param[in] agent_access Access pattern of the image by the HSA agent.
 *
 * @param[out] image_handle Agent-specific image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_STATUS_UNSUPPORTED_IMAGE_FORMAT If the HSA agent does not have
 * the capability to support the image format using the specified @a
 * agent_access.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 *
 */
hsa_status_t hsa_image_create_handle(
                         const hsa_agent_t *agent,
                         const hsa_image_descriptor_t *image_descriptor,
                         const void *image_data,
                         hsa_agent_image_access_t agent_access,
                         hsa_image_handle_t *image_handle);

/**
 * @brief Imports a linearly organized image data from memory directly to an
 * image handle.
 *
 * @details This operation updates the image data referenced by the image handle
 * from the source memory. The size of the data imported from memory is
 * implicitly derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the @a
 * completion_signal is signaled when the operation completes.
 *
 * If @a src_row_pitch is smaller than the destination region width (in bytes),
 * then @a src_row_pitch = region width.
 *
 * If @a src_slice_pitch is smaller than the destination region width * region
 * height (in bytes), then @a src_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the source memory or image data memory in the previously created
 * ::hsa_image_create_handle image handle can overlap.  Overlapping of any of
 * the source and destination memory within the import operation produces
 * undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_memory Source memory.
 *
 * @param[in] src_row_pitch Number of bytes in one row of the source memory.
 *
 * @param[in] src_slice_pitch Number of bytes in one slice of the source memory.
 *
 * @param[in] dest_image_handle Destination Image handle.
 *
 * @param[in] image_region Image region to be updated.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a src_memory or @a
 * image_region are NULL.
 *
 */
hsa_status_t hsa_image_import (
                         const hsa_agent_t *agent,
                         const void *src_memory,
                         size_t src_row_pitch,
                         size_t src_slice_pitch,
                         hsa_image_handle_t dest_image_handle,
                         const hsa_image_region_t *image_region,
                         const hsa_signal_handle_t *completion_signal);

/**
 * @brief Export image data from the image handle directly to memory organized
 * linearly.
 *
 * @details The operation updates the destination memory with the image data in
 * the image handle. The size of the data exported to memory is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the @a
 * completion_signal is signaled when the operation completes.
 *
 * If @a dst_row_pitch is smaller than the source region width (in bytes), then
 * @a dst_row_pitch = region width.
 *
 * If @a dst_slice_pitch is smaller than the source region width * region height
 * (in bytes), then @a dst_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the destination memory or image data memory in the previously created
 * ::hsa_image_create_handle image handle can overlap – overlapping of any of
 * the source and destination memory within the export operation produces
 * undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_image_handle Source image handle.
 *
 * @param[in] dst_memory Destination memory.
 *
 * @param[in] dst_row_pitch Number of bytes in one row of the destination
 * memory.
 *
 * @param[in] dst_slice_pitch Number of bytes in one slice of the destination
 * memory.
 *
 * @param[in] image_region Image region to be exported.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a dst_memory or @a
 * image_region are NULL.
 */
hsa_status_t hsa_image_export(
                          const hsa_agent_t *agent,
                          hsa_image_handle_t src_image_handle,
                          void *dst_memory,
                          size_t dst_row_pitch,
                          size_t dst_slice_pitch,
                          const hsa_image_region_t *image_region,
                          const hsa_signal_handle_t *completion_signal);

/**
 * @brief Copies a region from one image to another.
 *
 * @details The operation copies the image data from the source image handle to
 * the destination image handle. The size of the image data copied is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the @a
 * completion_signal is signaled when the operation completes.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * The source and destination handles must have been previously created using
 * ::hsa_image_create_handle. The source and destination image data memory are
 * not allowed to be the same. Overlapping any of the source and destination
 * memory produces undefined results.
 *
 * The source and destination image formats don't have to match; appropriate
 * format conversion is performed automatically. The source and destination
 * images must be of the same geometry.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_image_handle Source image handle.
 *
 * @param[in] dst_image_handle Destination image handle.
 *
 * @param[in] image_region Image region to be copied.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_region
 * are NULL.
 */
hsa_status_t hsa_image_copy(
                       const hsa_agent_t *agent,
                       hsa_image_handle_t src_image_handle,
                       hsa_image_handle_t dst_image_handle,
                       const hsa_image_region_t *image_region,
                       const hsa_signal_handle_t *completion_signal);

/**
 * @brief Clears the image to a specified 4-component floating point data.
 *
 * @details The operation clears the elements of the image with the data
 * specified. The lowest bits of the data (number of bits depending on the image
 * component type) are stored in the cleared image are based on the image
 * component order. The size of the image data cleared is implicitly derived
 * from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the @a
 * completion_signal is signaled when the operation completes.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * Clearing an image automatically performs value conversion on the provided
 * floating point values as is appropriate for the image format used.
 *
 * For images of UNORM types, the floating point values must be in the [0..1]
 * range. For images of SNORM types, the floating point values must be in the
 * [-1..1] range. For images of UINT types, the floating point values are
 * rounded down to an integer value. For images of SRGB types, the clear data is
 * specified in a linear space, which is appropriately converted by the Runtime
 * to sRGB color space.
 *
 * Specifying clear value outside of the range representable by an image format
 * produces undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_handle Image to be cleared.
 *
 * @param[in] data 4-component clear value in floating point format.
 *
 * @param[in] image_region Image region to clear.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a component or @a
 * image_region are NULL.
 */
hsa_status_t hsa_image_clear(
                        const hsa_agent_t *agent,
                        hsa_image_handle_t image_handle,
                        const float data[4],
                        const hsa_image_region_t *image_region,
                        const hsa_signal_handle_t *completion_signal);

/**
 * @brief Destroys the specified image handle.
 *
 * @details If successful, the image handle previously created using
 *  ::hsa_image_create_handle is destroyed.
 *
 * Destroying the image handle does not free the associated image data.
 *
 * The image handle should not be destroyed while there are references to it
 * queued for execution or currently being used in a dispatch. Failure to
 * properly track image data lifetime causes undefined results due to premature
 * image handle deletion.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_handle Image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_handle is
 * NULL.
 */
hsa_status_t hsa_image_destroy_handle (
                        const hsa_agent_t *agent,
                        hsa_image_handle_t *image_handle);

/**
 * @brief Create an HSA component-defined sampler handle from a
 * component-independent sampler descriptor.
 *
 * @details If successful, the sampler handle is written to the location
 * specified by the sampler handle.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_descriptor Implementation-independent sampler descriptor.
 *
 * @param[out] sampler_handle Component-specific sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 */
hsa_status_t hsa_sampler_create_handle(
                     const hsa_agent_t *agent,
                     const hsa_sampler_descriptor_t *sampler_descriptor,
                     hsa_sampler_handle_t *sampler_handle);

/**
 * @brief Destroys the specified sampler handle.
 *
 * @details If successful, the sampler handle previously created using
 * ::hsa_sampler_create_handle is destroyed.
 *
 * The sampler handle should not be destroyed while there are references to it
 * queued for execution or currently being used in a dispatch.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_handle Sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 */
hsa_status_t hsa_sampler_destroy_handle(
                     const hsa_agent_t *agent,
                     hsa_sampler_handle_t *sampler_handle);
/** @} */

/** \defgroup kernargmem TODO
 *  @{
 */
/**
 * @brief Allocate kernarg memory.
 *
 * @param[in] component A valid pointer to the component for which the specified
 * amount of kernarg memory is to be allocated.
 *
 * @param[in] size Requested allocation size in bytes. If size is 0, NULL is
 * returned.
 *
 * @param[out] address A valid pointer to the location of where to return the
 * pointer to the base of the allocated region of memory.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed address is NULL.
 */
hsa_status_t hsa_memory_allocate_kernarg( const hsa_agent_t *component,
    size_t size,
    void **address);

/**
 * @brief Copy between the system and kernarg segments.
 *
 * @param[out] dst A valid pointer to the destination array where the content is
 *      to be copied.
 *
 * @param[in] src A valid pointer to the source of data to be copied.
 *
 * @param[in] size Number of bytes to copy.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the source or destination
 * pointers are invalid.
 */
hsa_status_t hsa_memory_copy_kernarg_to_system(void *dst,
    const void *src,
    size_t size);

/**
  * @copydoc hsa_memory_copy_kernarg_to_system
  */
hsa_status_t hsa_memory_copy_system_to_kernarg(void *dst,
    const void *src,
    size_t size);

/** @} */

/** \defgroup agent_dispatch TODO
 *  @{
 */
/**
 * @brief Agent dispatch runtime function registration.
 *
 * @param agent_dispatch_queue Agent dispatch queue.
 *
 * @param[in] agent_dispatch_callback Callback that the user is registering, the
 * callback is called with five 64 bit args as a parameter.
 *
 * @param context Context.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 */
hsa_status_t hsa_register_agent_dispatch_callback(hsa_queue_t *agent_dispatch_queue,
    void (*agent_dispatch_callback)(uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t retaddr),
    hsa_runtime_context_t *context);
/** @} */


/** \addtogroup hsa_ext
 *  @{
 */
/**
 * @brief TODO.
 */
typedef enum {
    /**
     * HSA extension to support images.
     */
    HSA_EXTENSION_IMAGES,

    /**
     * HSA extensions providing information necessary to debug tools.
     */
    HSA_EXTENSION_DEBUG,

    /**
     * HSA extension for Finalizer that finalizes the brig to compilation units
     * that represent kernel and function code objects.
     */
    HSA_EXTENSION_FINALIZER
}hsa_extension_t;
/** @} */


/** \defgroup vendor_ext TODO
 *  @{
 */
/**
 * @brief Vendor enumeration example.
 */
typedef enum {
  /**
   * Start of the single vendor extension range.
   */
  HSA_SVEXT_START= 0,
  /**
   * Company ACME, starts with FOO symbol.
   */
  HSA_SVEXT_ACME_FOO = 1,

  /**
   * Company ACME has another_ext symbol.
   */
  HSA_SVEXT_ACME_ANOTHER_EXT = 2,
  /**
   * Multi vendor extension starts at 1000000.
   */
  HSA_MVEXT_START = 1000000,
  /**
   * Multivendor extension has a symbol foo.
   */
  HSA_MVEXT_FOO   = 1000001
}hsa_vendor_extension_t;
/** @} */


/** \defgroup query_hsaextension TODO
 *  @{
 */
/**
 * @brief Query HSA extension.
 *
 * @param[in] extension HSA extension that is being queried.
 */
hsa_status_t hsa_extension_query(hsa_extension_t extension);
/** @} */


/** \defgroup query_vendorextension TODO
 *  @{
 */
/**
 * @brief Query vendor extensions.
 *
 * @details If successful, the extension information is written with
 * extension-specific information such as version information, function
 * pointers, and data values. If the extension is not supported, the extension
 * information is not modified and a error code is returned.
 *
 * @a vendor_extension.h defines a unique structure for each extension.
 *
 * @param[in] extension The vendor extension that is being queried.
 *
 * @param[out] extension_structure Extension structure.
 *
 * @retval ::HSA_STATUS_SUCCESS
 *
 * @retval ::HSA_STATUS_ERROR_EXTENSION_UNSUPPORTED If the
 * extension is not supported.
 */
hsa_status_t hsa_vendor_extension_query(hsa_vendor_extension_t extension, void *extension_structure);
/** @} */

#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif /*HSACOREBASE_H*/