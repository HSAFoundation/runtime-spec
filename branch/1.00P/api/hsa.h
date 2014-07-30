#ifndef HSA_H
#define HSA_H

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uintXX_t */


/* placeholder for calling convention - check macro naming convention */
#define HSA_API


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** \defgroup status Runtime Notifications
 *  @{
 */

/**
 * TODO
 */
typedef enum {
    /**
     * The function has been executed successfully.
     */
    HSA_STATUS_SUCCESS = 0,
    /**
     * A traversal over a list of elements has been interrupted by the
     * application before completing.
     */
    HSA_STATUS_INFO_BREAK = 0x1,
    /**
     * An initialization attempt failed due to prior initialization.
     */
    HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED = 0x4000,
    /**
     * The finalization options cannot be recognized.
     */
    HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS = 0x4001,
    /**
     * A generic error has occurred.
     */
    HSA_STATUS_ERROR = 0x10000,
    /**
     * One of the actual arguments does not meet a precondition stated in the
     * documentation of the corresponding formal argument.
     */
    HSA_STATUS_ERROR_INVALID_ARGUMENT = 0x10001,
    /**
     * The requested queue creation is not valid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE_CREATION = 0x10002,
    /**
     * The requested allocation is not valid.
     */
    HSA_STATUS_ERROR_INVALID_ALLOCATION = 0x10003,
    /**
     * The agent is invalid.
     */
    HSA_STATUS_ERROR_INVALID_AGENT = 0x10004,
    /**
     * The memory region is invalid.
     */
    HSA_STATUS_ERROR_INVALID_REGION = 0x10005,
    /**
     * The signal is invalid.
     */
    HSA_STATUS_ERROR_INVALID_SIGNAL = 0x10006,
    /**
     * The queue is invalid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE = 0x10007,
    /**
     * The runtime failed to allocate the necessary resources. This error
     * may also occur when the core runtime library needs to spawn threads or
     * create internal OS-specific events.
     */
    HSA_STATUS_ERROR_OUT_OF_RESOURCES = 0x10008,
    /**
     * The AQL packet is malformed.
     */
    HSA_STATUS_ERROR_INVALID_PACKET_FORMAT = 0x10009,
    /**
     * An error has been detected while releasing a resource.
     */
    HSA_STATUS_ERROR_RESOURCE_FREE = 0x1000A,
    /**
     * An API other than ::hsa_init has been invoked while the reference count
     * of the HSA runtime is zero.
     */
    HSA_STATUS_ERROR_NOT_INITIALIZED = 0x1000B,
    /**
     * The maximum reference count for the object has been reached.
     */
    HSA_STATUS_ERROR_REFCOUNT_OVERFLOW = 0x1000C,
    /**
     * Mismatch between a directive in the control directive structure and in
     * the HSAIL kernel.
     */
    HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH = 0x14000,
    /**
     * Image format is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED = 0x14001,
    /**
     * Image size is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED = 0x14002

} hsa_status_t;

/**
 * @brief Query additional information about a status code.
 *
 * @param[in] status Status code.
 *
 * @param[out] status_string A NUL-terminated string that describes the error
 * status.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p status is not a valid
 * status code or @p status_string is NULL.
 */
hsa_status_t HSA_API hsa_status_string(
    hsa_status_t status,
    const char ** status_string);

/** @} */

/** \defgroup common Common Definitions
 *  @{
 */

/**
 * @brief Value expressed as a power of 2.
 */
typedef uint8_t hsa_powertwo8_t;

/**
 * @brief Power of two between 1 and 256.
 */
typedef enum {
  HSA_POWERTWO_1 = 0,
  HSA_POWERTWO_2 = 1,
  HSA_POWERTWO_4 = 2,
  HSA_POWERTWO_8 = 3,
  HSA_POWERTWO_16 = 4,
  HSA_POWERTWO_32 = 5,
  HSA_POWERTWO_64 = 6,
  HSA_POWERTWO_128 = 7,
  HSA_POWERTWO_256 = 8
} hsa_powertwo_t;

/**
 * @brief Three-dimensional coordinate.
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

/**
 * @brief Dimensions in a 3D space.
 */
typedef enum {
  /**
   * X dimension.
   */
  HSA_DIM_X = 0,

  /**
   * Y dimension.
   */
  HSA_DIM_Y = 1,

  /**
   * Z dimension.
   */
  HSA_DIM_Z = 2
} hsa_dim_t;

/**
 * @brief Opaque pointer that is passed to all runtime functions that use
 * callbacks. The runtime passes this pointer as the first argument to all
 * callbacks made by the function.
 */
typedef struct hsa_runtime_caller_s {
  /**
   * Opaque pointer that is passed as the first argument to callback
   * functions invoked by a runtime function.
   */
  uint64_t caller;
} hsa_runtime_caller_t;

/**
 * @brief Call back function for allocating data.
 */
typedef hsa_status_t (*hsa_runtime_alloc_data_callback_t)(
  hsa_runtime_caller_t caller,
  size_t byte_size,
  void **address);

/** @} **/


/** \defgroup initshutdown Initialization and Shut Down
 *  @{
 */

/**
 * @brief Initialize the HSA runtime.
 *
 * @details Initializes the HSA runtime if it is not already initialized, and
 * increases the reference counter associated with the HSA runtime for the
 * current process. Invocation of any HSA function other than ::hsa_init results
 * in undefined behavior if the current HSA runtime reference counter is less
 * than one.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_REFCOUNT_OVERFLOW If the runtime reference
 * count reaches INT32_MAX.
 */
hsa_status_t HSA_API hsa_init();

/**
 * @brief Shut down the HSA runtime.
 *
 * @details Decreases the reference count of the runtime instance. When the
 * reference count reaches zero, the runtime is no longer considered valid but
 * the application might call ::hsa_init to initialize the HSA runtime again.
 *
 * Once the reference count of the runtime reaches zero, all the resources
 * associated with it (queues, signals, agent information, etc.) are
 * considered invalid and any attempt to reference them in subsequent API calls
 * results in undefined behavior. When the reference count reaches zero, the HSA
 * runtime may release resources associated with it.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_shut_down();

/** @} **/

/** \defgroup agentinfo System and Agent Information
 *  @{
 */

/**
 * @brief System attributes.
 */
typedef enum {
  /**
   * Major version of the HSA runtime specification supported by the
   * implementation. The type of this attribute is uint16_t.
   */
  HSA_SYSTEM_INFO_VERSION_MAJOR,
  /**
   * Minor version of the HSA runtime specification supported by the
   * implementation. The type of this attribute is uint16_t.
   */
  HSA_SYSTEM_INFO_VERSION_MINOR,
  /**
   * Current timestamp. The value of this attribute monotonically increases at a
   * constant rate. The type of this attribute is uint64_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP,
  /**
   * Timestamp value increase rate, in MHz. The timestamp (clock) frequency is
   * in the range 1-400MHz. The type of this attribute is uint16_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP_FREQUENCY,
  /**
   * Maximum duration of a signal wait operation. Expressed as a count based on
   * the timestamp frequency. The type of this attribute is uint64_t.
   */
  HSA_SYSTEM_INFO_SIGNAL_MAX_WAIT
} hsa_system_info_t;

/**
 * @brief Get the current value of a system attribute.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of @p attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p attribute is not a valid
 * system attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_system_get_info(
    hsa_system_info_t attribute,
    void* value);

/**
 * @brief Opaque handle representing an agent, a device that participates in the
 * HSA memory model. An agent can submit AQL packets for execution, and may
 * also accept AQL packets for execution (Agent Dispatch packets or Dispatch
 * packets launching HSAIL-derived binaries).
 */
typedef uint64_t hsa_agent_t;

/* /\** */
/*  * @brief Opaque handle representing a NUMA node, a set of agents around */
/*  * memory which they can directly access. */
/*  *\/ */
/* typedef uint64_t hsa_node_t; */

/**
 * @brief Agent features.
 */
typedef enum {
    /**
     * The agent supports AQL packets of Dispatch type. If this feature is
     * enabled, the agent is also a component.
     */
    HSA_AGENT_FEATURE_DISPATCH = 1,
    /**
     * The agent supports AQL packets of Agent Dispatch type.
     */
    HSA_AGENT_FEATURE_AGENT_DISPATCH = 2
} hsa_agent_feature_t;

/**
 * @brief Hardware device type.
 */
typedef enum {
    /**
     * CPU device.
     */
    HSA_DEVICE_TYPE_CPU = 0,
    /**
     * GPU device.
     */
    HSA_DEVICE_TYPE_GPU = 1,
    /**
     * DSP device.
     */
    HSA_DEVICE_TYPE_DSP = 2
} hsa_device_type_t;

/**
 * @brief Agent attributes.
 */
typedef enum {
  /**
   * Agent name. The type of this attribute is a NUL-terminated char[64].
   */
  HSA_AGENT_INFO_NAME,
  /**
   * Name of vendor. The type of this attribute is a NUL-terminated char[64].
   */
  HSA_AGENT_INFO_VENDOR_NAME,
  /**
   * Agent capability. The type of this attribute is ::hsa_agent_feature_t.
   */
  HSA_AGENT_INFO_FEATURE,
  /**
   * Number of work-items in a wavefront. Must be a power of 2 in the range
   * [1,256]. The value of this attribute is undefined if the agent is not a
   * component. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_WAVEFRONT_SIZE,
  /**
   * Maximum number of work-items of each dimension of a work-group.  Each
   * maximum must be greater than 0. No maximum can exceed the value of
   * ::HSA_AGENT_INFO_WORKGROUP_MAX_SIZE. The value of this attribute is
   * undefined if the agent is not a component. The type of this attribute is
   * uint16_t[3].
   */
  HSA_AGENT_INFO_WORKGROUP_MAX_DIM,
  /**
   * Maximum total number of work-items in a work-group. The value of this
   * attribute is undefined if the agent is not a component. The type of this
   * attribute is uint32_t.
   */
  HSA_AGENT_INFO_WORKGROUP_MAX_SIZE,
  /**
   * Maximum number of work-items of each dimension of a grid. Each maximum must
   * be greater than 0, and must not be smaller than the corresponding value in
   * ::HSA_AGENT_INFO_WORKGROUP_MAX_DIM. No maximum can exceed the value of
   * ::HSA_AGENT_INFO_GRID_MAX_SIZE. The value of this attribute is undefined if
   * the agent is not a component. The type of this attribute is ::hsa_dim3_t.
   */
  HSA_AGENT_INFO_GRID_MAX_DIM,
  /**
   * Maximum total number of work-items in a grid. The value of this attribute
   * is undefined if the agent is not a component. The type of this attribute is
   * uint32_t.
   */
  HSA_AGENT_INFO_GRID_MAX_SIZE,
  /**
   * Maximum number of fbarriers per work-group. Must be at least 32. The value
   * of this attribute is undefined if the agent is not a component. The type of
   * this attribute is uint32_t.
   */
  HSA_AGENT_INFO_FBARRIER_MAX_SIZE,
  /**
   * Maximum number of queues that can be active (created but not destroyed) at
   * one time in the agent. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_QUEUES_MAX,
  /**
   * Maximum number of packets that a queue created in the agent can hold. Must
   * be a power of 2 and greater than 0. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_QUEUE_MAX_SIZE,
  /**
   * Type of a queue created in the agent. The type of this attribute is
   * ::hsa_queue_type_t.
   */
  HSA_AGENT_INFO_QUEUE_TYPE,
  /**
   * Identifier of the NUMA node associated with the agent. The type of this
   * attribute is uint32_t.
   */
  HSA_AGENT_INFO_NODE,
  /**
   * Type of hardware device associated with the agent. The type of this
   * attribute is ::hsa_device_type_t.
   */
  HSA_AGENT_INFO_DEVICE,
  /**
   * Array of data cache sizes (L1..L4). Each size is expressed in bytes. A size
   * of 0 for a particular level indicates that there is no cache information
   * for that level. The type of this attribute is uint32_t[4].
   */
  HSA_AGENT_INFO_CACHE_SIZE,
  /**
   * Maximum dimensions (width, height, depth) of one-dimensional images, in
   * image elements. The Y and Z dimensions maximums must be 0. The X maximum
   * must be at most 16384.  The value of this attribute is undefined if the
   * agent is not a component, or the implementation does not support
   * images. The type of this attribute is ::hsa_dim3_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE1D_MAX_DIM,
  /**
   * Maximum dimensions (width, height, depth) of two-dimensional images, in
   * image elements. The Z dimension maximum must be 0. The X and Y maximums
   * must be at most 16384. The value of this attribute is undefined if the
   * agent is not a component, or the implementation does not support
   * images. The type of this attribute is ::hsa_dim3_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE2D_MAX_DIM,
  /**
   * Maximum dimensions (width, height, depth) of three-dimensional images, in
   * image elements. The maximum along any dimension cannot exceed 2048. The
   * value of this attribute is undefined if the agent is not a component, or
   * the implementation does not support images.  The type of this attribute is
   * ::hsa_dim3_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE3D_MAX_DIM,
  /**
   * Maximum number of image layers in a image array. Must not exceed 2048. The
   * value of this attribute is undefined if the agent is not a component, or
   * the implementation does not support images. The type of this attribute is
   * uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_ARRAY_MAX_SIZE,
  /**
   * Maximum number of read-only image handles that can be created at any one
   * time. Must be at least 128. The value of this attribute is undefined if the
   * agent is not a component, or the implementation does not support
   * images. The type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_RD_MAX,
  /**
   * Maximum number of write-only and read-write image handles that can be
   * created at any one time. Must be at least 64. The value of this attribute
   * is undefined if the agent is not a component, or the implementation does
   * not support images. The type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_RDWR_MAX,
  /**
   * Maximum number of sampler handlers that can be created at any one
   * time. Must be at least 16. The value of this attribute is undefined if the
   * agent is not a component, or the implementation does not support
   * images. The type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_SAMPLER_MAX
} hsa_agent_info_t;

/**
 * @brief Get the current value of an attribute for a given agent.
 *
 * @param[in] agent A valid agent.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of @p attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT If the agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p attribute is not a valid
 * agent attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_agent_get_info(
    hsa_agent_t agent,
    hsa_agent_info_t attribute,
    void* value);

/**
 * @brief Iterate over the available agents, and invoke an application-defined
 * callback on every iteration.
 *
 * @param[in] callback Callback to be invoked once per agent. The runtime passes
 * two arguments to the callback, the agent and the application data.  If @p
 * callback returns a status other than ::HSA_STATUS_SUCCESS for a particular
 * iteration, the traversal stops and ::hsa_iterate_agents returns that status
 * value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. Might be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p callback is NULL.
*/
hsa_status_t HSA_API hsa_iterate_agents(
    hsa_status_t (*callback)(hsa_agent_t agent, void* data),
    void* data);

/*

// If we do not know the size of an attribute, we need to query it first
// Note: this API will not be in the spec unless needed
hsa_status_t HSA_API hsa_agent_get_info_size(
    hsa_agent_t agent,
    hsa_agent_info_t attribute,
    size_t* size);

// Set the value of an agents attribute
// Note: this API will not be in the spec unless needed
hsa_status_t HSA_API hsa_agent_set_info(
    hsa_agent_t agent,
    hsa_agent_info_t attribute,
    void* value);

*/

/** @} */


/** \defgroup signals Signals
 *  @{
 */

/**
 * @brief Signal handle. The value 0 is reserved.
 */
typedef uint64_t hsa_signal_t;

/**
 * @brief Signal value. The value occupies 32 bits in small machine mode, and 64
 * bits in large machine mode.
 */
#ifdef HSA_SMALL_MODEL
  typedef int32_t hsa_signal_value_t;
#else
  typedef int64_t hsa_signal_value_t;
#endif

/**
 * @brief Create a signal.
 *
 * @param[in] initial_value Initial value of the signal.
 *
 * @param[in] num_consumers Size of @p consumers. A value of 0 indicates that
 * any HSA agent might wait on the signal.
 *
 * @param[in] consumers List of agents that might consume (wait on) the
 * signal. If @p num_consumers is 0, this argument is ignored; otherwise, the
 * runtime might use the list to optimize the handling of the signal object. If
 * an agent not listed in @p consumers waits on the returned signal, the
 * behavior is undefined. The memory associated with @p consumers can be reused
 * or freed after the function returns.
 *
 * @param[out] signal Pointer to a memory location where the runtime will store
 * the newly created signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p signal is NULL or @p
 * num_consumers is greater than 0 but @p consumers is NULL.
 */
hsa_status_t HSA_API hsa_signal_create(
    hsa_signal_value_t initial_value,
    uint32_t num_consumers,
    const hsa_agent_t *consumers,
    hsa_signal_t *signal);

/**
 * @brief Destroy a signal previous created by ::hsa_signal_create.
 *
 * @param[in] signal Signal.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @p signal is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p signal is 0.
 */
hsa_status_t HSA_API hsa_signal_destroy(
    hsa_signal_t signal);

/**
 * @brief Atomically read the current value of a signal.
 *
 * @param[in] signal Signal.
 *
 * @return Value of the signal.
*/
hsa_signal_value_t HSA_API hsa_signal_load_acquire(
    hsa_signal_t signal);

/**
 * @copydoc hsa_signal_load_acquire
 */
hsa_signal_value_t HSA_API hsa_signal_load_relaxed(
    hsa_signal_t signal);

/**
 * @brief Atomically set the value of a signal.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal.
 *
 * @param[in] value New signal value.
 */
void HSA_API hsa_signal_store_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_store_relaxed
 */
void HSA_API hsa_signal_store_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically set the value of a signal and return its previous value.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value New value.
 *
 * @return Value of the signal prior to the exchange.
 *
 */
hsa_signal_value_t HSA_API hsa_signal_exchange_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_exchange_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_exchange_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_exchange_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_exchange_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_exchange_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_exchange_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically set the value of a signal if the observed value is equal to
 * the expected value. The observed value is returned regardless of whether the
 * replacement was done.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue
 * doorbell signal, the behavior is undefined.
 *
 * @param[in] expected Value to compare with.
 *
 * @param[in] value New value.
 *
 * @return Observed value of the signal.
 *
 */
hsa_signal_value_t HSA_API hsa_signal_cas_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t expected,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_cas_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_cas_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t expected,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_cas_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_cas_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t expected,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_cas_acq_rel
 */
hsa_signal_value_t HSA_API hsa_signal_cas_release(
    hsa_signal_t signal,
    hsa_signal_value_t expected,
    hsa_signal_value_t value);

/**
 * @brief Atomically increment the value of a signal by a given amount.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value Value to add to the value of the signal.
 *
 */
void HSA_API hsa_signal_add_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_add_acq_rel
 */
void HSA_API hsa_signal_add_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_add_acq_rel
 */
void HSA_API hsa_signal_add_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_add_acq_rel
 */
void HSA_API hsa_signal_add_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically decrement the value of a signal by a given amount.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value Value to subtract from the value of the signal.
 *
 */
void HSA_API hsa_signal_subtract_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_subtract_acq_rel
 */
void HSA_API hsa_signal_subtract_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_subtract_acq_rel
 */
void HSA_API hsa_signal_subtract_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_subtract_acq_rel
 */
void HSA_API hsa_signal_subtract_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically perform a bitwise AND operation between the value of a signal and a given
 * value.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value Value to AND with the value of the signal.
 *
 */
void HSA_API hsa_signal_and_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_and_acq_rel
 */
void HSA_API hsa_signal_and_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_and_acq_rel
 */
void HSA_API hsa_signal_and_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_and_acq_rel
 */
void HSA_API hsa_signal_and_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically perform a bitwise OR operation between the value of a signal and a given
 * value.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value Value to OR with the value of the signal.
 */
void HSA_API hsa_signal_or_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_or_acq_rel
 */
void HSA_API hsa_signal_or_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_or_acq_rel
 */
void HSA_API hsa_signal_or_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_or_acq_rel
 */
void HSA_API hsa_signal_or_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @brief Atomically perform a bitwise XOR operation between the value of a signal and a given
 * value.
 *
 * @details If the value of the signal is modified, all the agents waiting on @p
 * signal for which @p value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @p signal is a queue doorbell signal, the
 * behavior is undefined.
 *
 * @param[in] value Value to XOR with the value of the signal.
 *
 */
void HSA_API hsa_signal_xor_acq_rel(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_xor_acq_rel
 */
void HSA_API hsa_signal_xor_acquire(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_xor_acq_rel
 */
void HSA_API hsa_signal_xor_relaxed(
    hsa_signal_t signal,
    hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_xor_acq_rel
 */
void HSA_API hsa_signal_xor_release(
    hsa_signal_t signal,
    hsa_signal_value_t value);

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
 * @brief Expected duration of a wait call.
 */
typedef enum {
    /**
     * The signal value is expected to meet the specified condition in a short
     * period of time.
     */
    HSA_WAIT_EXPECTANCY_SHORT,
    /**
     * The signal value is expected to meet the specified condition after a
     * longer period of time.
     */
    HSA_WAIT_EXPECTANCY_LONG,
    /**
     * The expected duration of the wait call is not known.
     */
    HSA_WAIT_EXPECTANCY_UNKNOWN
} hsa_wait_expectancy_t;

/**
 * @brief Wait until a signal value satisfies a specified condition, or a
 * certain amount of time has elapsed.
 *
 * @details A wait operation can spuriously resume at any time sooner than the
 * timeout (for example, due to system or other external factors) even when the
 * condition has not been met.
 *
 * The function is guaranteed to return if the signal value satisfies the
 * condition at some point in time during the wait, but the value returned to
 * the application might not satisfy the condition. When the wait operation
 * internally loads the value of the passed signal, it uses the memory order
 * indicated in the function name.
 *
 * The application might indicate a preference about the maximum wait duration.
 * The operation might block for a shorter or longer time even if the condition
 * is not met.
 *
 * @param[in] signal Signal.
 *
 * @param[in] condition Condition used to compare the signal value with @p
 * compare_value.
 *
 * @param[in] compare_value Value to compare with.
 *
 * @param[in] timeout_hint Maximum duration of the wait.  Specified in the same
 * unit as the system timestamp. A value of UINT64_MAX indicates no maximum.
 *
 * @param[in] wait_expectancy_hint Hint indicating whether the signal value is
 * expected to meet the given condition in a short period of time or not. The
 * HSA runtime may use this hint to optimize the wait implementation.
 *
 * @return Observed value of the signal, which might not satisfy the specified
 * condition.
 *
*/
hsa_signal_value_t HSA_API hsa_signal_wait_acquire(
    hsa_signal_t signal,
    hsa_signal_condition_t condition,
    hsa_signal_value_t compare_value,
    uint64_t timeout_hint,
    hsa_wait_expectancy_t wait_expectancy_hint);

/**
 * @copydoc hsa_signal_wait_acquire
 */
hsa_signal_value_t HSA_API hsa_signal_wait_relaxed(
    hsa_signal_t signal,
    hsa_signal_condition_t condition,
    hsa_signal_value_t compare_value,
    uint64_t timeout_hint,
    hsa_wait_expectancy_t wait_expectancy_hint);

/** @} */

/** \defgroup queue Queues
 *  @{
 */

/**
 * @brief Queue type. Intended to be used for dynamic queue protocol
 * determination.
 */
typedef enum {
  /**
   * Queue supports multiple producers.
   */
  HSA_QUEUE_TYPE_MULTI = 0,
  /**
   * Queue only supports a single producer.
   */
  HSA_QUEUE_TYPE_SINGLE = 1
} hsa_queue_type_t;

/**
 * @brief Queue features.
 */
typedef enum {
  /**
   * Queue supports Dispatch packets.
   */
  HSA_QUEUE_FEATURE_DISPATCH = 1,

  /**
   * Queue supports Agent Dispatch packets.
   */
  HSA_QUEUE_FEATURE_AGENT_DISPATCH = 2
} hsa_queue_feature_t;

/**
 * @brief User mode queue.
 *
 * @details Queues are read-only, but HSA agents can directly modify the
 * contents of the buffer pointed by @a base_address, or use runtime APIs to
 * access the doorbell signal.
 *
 */
typedef struct hsa_queue_s {
  /**
   * Queue type.
   */
  hsa_queue_type_t type;

  /**
   * Queue features mask. This is a bit-field of ::hsa_queue_feature_t
   * values. Applications should ignore any unknown set bits.
   */
  uint32_t features;

  /**
   * Starting address of the runtime-allocated buffer used to store the AQL
   * packets. Must be aligned to the size of an AQL packet.
   */
  uint64_t base_address;

  /**
   * Signal object used by the application to indicate the ID of a packet that
   * is ready to be processed. The HSA runtime manages the doorbell signal. If
   * the application tries to replace or destroy this signal, the behavior is
   * undefined.
   *
   * If @a type is ::HSA_QUEUE_TYPE_SINGLE the doorbell signal value must be
   * updated in a monotonically increasing fashion. If @a type is
   * ::HSA_QUEUE_TYPE_MULTI, the doorbell signal value can be updated with any
   * value.
   */
  hsa_signal_t doorbell_signal;

  /**
   * Maximum number of packets the queue can hold. Must be a power of 2.
   */
  uint32_t size;

  /**
   * Queue identifier which is unique per process.
   */
  uint32_t id;

  /**
   * A pointer to another user mode queue that can be used by an HSAIL kernel
   * to request runtime or application-defined services.
   */
  uint64_t service_queue;

} hsa_queue_t;

/**
 * @brief Create a user mode queue.
 *
 * @details When a queue is created, the runtime creates the packet buffer, the
 * completion signal, and the write and read indexes. The initial value of the
 * write and read indexes is 0. The type of every packet in the buffer is
 * initialized to ::HSA_PACKET_TYPE_ALWAYS_RESERVED.
 *
 * The application should only rely on the error code returned to determine if
 * the queue is valid.
 *
 * @param[in] agent Agent where to create the queue.
 *
 * @param[in] size Number of packets the queue is expected to hold. Must be a
 * power of 2 that does not exceed the value of ::HSA_AGENT_INFO_QUEUE_MAX_SIZE
 * in @p agent.
 *
 * @param[in] type Type of the queue. If the value of
 * ::HSA_AGENT_INFO_QUEUE_TYPE in @p agent is ::HSA_QUEUE_TYPE_SINGLE, then @p
 * type must also be ::HSA_QUEUE_TYPE_SINGLE.
 *
 * @param[in] callback Callback invoked by the runtime for every asynchronous
 * event related to the newly created queue. Might be NULL. The runtime passes
 * two arguments to the callback: a code identifying the event that triggered
 * the invocation, and a pointer to the queue where the event originated.
 *
 * @param[in] service_queue Pointer to a service queue to be associated with the
 * newly created queue. Might be NULL. If not NULL, the queue pointed by @p
 * service_queue must support the ::HSA_QUEUE_FEATURE_AGENT_DISPATCH feature.
 *
 * @param[out] queue Memory location where the runtime stores a pointer to the
 * newly created queue.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT If the agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE_CREATION If @p agent does not
 * support queues of the given type.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p size is not a power of two,
 * @p type is not a valid queue type, or @p queue is NULL.
 *
 */
hsa_status_t HSA_API hsa_queue_create(
    hsa_agent_t agent,
    uint32_t size,
    hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source),
    const hsa_queue_t* service_queue,
    hsa_queue_t **queue);

/**
 * @brief Destroy a user mode queue.
 *
 * @details When a queue is destroyed, the state of the AQL packets that have
 * not been yet fully processed (their completion phase has not finished)
 * becomes undefined. It is the responsibility of the application to ensure that
 * all pending queue operations are finished if their results are required.
 *
 * The resources allocated by the runtime during queue creation (queue
 * structure, ring buffer, doorbell signal) are released.  The queue should not
 * be accessed after being destroyed.
 *
 * @param[in] queue Pointer to a queue created using ::hsa_queue_create.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE If the queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_destroy(hsa_queue_t *queue);

/**
 * @brief Inactivate a queue.
 *
 * @details Inactivating the queue aborts any pending executions and prevent any
 * new packets from being processed. Any more packets written to the queue once
 * it is inactivated will be ignored by the packet processor.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE If the queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_inactivate(
    hsa_queue_t *queue);

/**
 * @brief Atomically load the read index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Read index of the queue pointed by @p queue.
 */
uint64_t HSA_API hsa_queue_load_read_index_acquire(
    hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_read_index_acquire
 */
uint64_t HSA_API hsa_queue_load_read_index_relaxed(
    hsa_queue_t *queue);

/**
 * @brief Atomically load the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Write index of the queue pointed by @p queue.
 */
uint64_t HSA_API hsa_queue_load_write_index_acquire(
    hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_write_index_acquire
 */
uint64_t HSA_API hsa_queue_load_write_index_relaxed(
    hsa_queue_t *queue);

/**
 * @brief Atomically set the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the write index.
 *
 */
void HSA_API hsa_queue_store_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_write_index_relaxed
 */
void HSA_API hsa_queue_store_write_index_release(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @brief Atomically set the write index of a queue if the observed value is
 * equal to the expected value. The application can inspect the returned value
 * to determine if the replacement was done.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] expected Expected value.
 *
 * @param[in] value Value to assign to the write index if @p expected matches
 * the observed write index. Must be greater than @p expected.
 *
 * @return Previous value of the write index.
 */
uint64_t HSA_API hsa_queue_cas_write_index_acq_rel(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_acquire(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_release(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @brief Atomically increment the write index of a queue by an offset.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to add to the write index.
 *
 * @return Previous value of the write index.
 */
uint64_t HSA_API hsa_queue_add_write_index_acq_rel(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_acquire(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_release(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @brief Atomically set the read index of a queue.
 *
 * @details Modifications of the read index are not allowed and result in
 * undefined behavior if the queue is associated with an HSA agent for which
 * only the corresponding packet processor is permitted to update the read
 * index.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the read index.
 *
 */
void HSA_API hsa_queue_store_read_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_read_index_relaxed
 */
void HSA_API hsa_queue_store_read_index_release(
   hsa_queue_t *queue,
   uint64_t value);
/** @} */


/** \defgroup aql Architected Queuing Language
 *  @{
 */

/**
 * @brief Packet type.
 */
typedef enum {
  /**
   * Initial type of any packet when the queue is created. A packet processor
   * must not process a packet of this type. All queues support this packet
   * type.
   */
  HSA_PACKET_TYPE_ALWAYS_RESERVED = 0,
  /**
   * The packet has been processed in the past, but has not been reassigned to
   * the packet processor. A packet processor must not process a packet of this
   * type. All queues support this packet type.
   */
  HSA_PACKET_TYPE_INVALID = 1,
  /**
   * Packet used by HSA agents for dispatching jobs to HSA components. Not all
   * queues support packets of this type (see ::hsa_queue_feature_t).
   */
  HSA_PACKET_TYPE_DISPATCH = 2,
  /**
   * Packet used by HSA agents to delay processing of subsequent packets, and to
   * express complex dependencies between multiple packets. All queues support
   * this packet type.
   */
  HSA_PACKET_TYPE_BARRIER = 3,
  /**
   * Packet used by HSA agents for dispatching jobs to HSA agents.  Not all
   * queues support packets of this type (see ::hsa_queue_feature_t).
   */
  HSA_PACKET_TYPE_AGENT_DISPATCH = 4
} hsa_packet_type_t;

/**
 * @brief Scope of the memory fence operation associated with a packet.
 */
typedef enum {
  /**
   * No scope. Must only be used as the acquire fence scope of a Barrier packet.
   */
  HSA_FENCE_SCOPE_NONE = 0,
  /**
   * The fence is applied with component scope for the global segment.
   */
  HSA_FENCE_SCOPE_COMPONENT = 1,
  /**
   * The fence is applied with system scope for the global segment.
   */
  HSA_FENCE_SCOPE_SYSTEM = 2
} hsa_fence_scope_t;

/**
 * @brief AQL packet header.
 */
typedef struct hsa_packet_header_s {
  /**
   * Packet type.
   */
  hsa_packet_type_t type:8;

  /**
   * If set, the processing of the current packet only launches when all
   * preceding packets (within the same queue) are complete.
   */
  uint16_t barrier:1;

  /**
   * Determines the scope and type of the memory fence operation applied before
   * the packet enters the active phase. Must be ::HSA_FENCE_SCOPE_NONE for
   * Barrier packets.
   */
  hsa_fence_scope_t acquire_fence_scope:2;

  /**
   * Determines the scope and type of the memory fence operation applied after
   * kernel completion but before the packet is completed.
   */
  hsa_fence_scope_t release_fence_scope:2;

  /**
   * Must be 0.
   */
  uint16_t reserved:3;

} hsa_packet_header_t;

/**
 * @brief AQL Dispatch packet
 */
typedef struct hsa_dispatch_packet_s {
  /**
   *  Packet header.
   */
  hsa_packet_header_t header;

  /**
   * Number of dimensions specified in the grid size. Valid values are 1, 2, or
   * 3.
   */
  uint16_t dimensions:2;

  /**
   * Reserved, must be 0.
   */
  uint16_t reserved:14;

  /**
   * X dimension of work-group, in work-items. Must be greater than 0.
   */
  uint16_t workgroup_size_x;

  /**
   * Y dimension of work-group, in work-items. Must be greater than
   * 0. If @a dimensions is 1, the only valid value is 1.
   */
  uint16_t workgroup_size_y;

  /**
   * Z dimension of work-group, in work-items. Must be greater than
   * 0. If @a dimensions is 1 or 2, the only valid value is 1.
   */
  uint16_t workgroup_size_z;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved2;

  /**
   * X dimension of grid, in work-items. Must be greater than 0. Must
   * not be smaller than @a workgroup_size_x.
   */
  uint32_t grid_size_x;

  /**
   * Y dimension of grid, in work-items. Must be greater than 0. If @a
   * dimensions is 1, the only valid value is 1. Must not be smaller than @a
   * workgroup_size_y.
   */
  uint32_t grid_size_y;

  /**
   * Z dimension of grid, in work-items. Must be greater than 0. If @a
   * dimensions is 1 or 2, the only valid value is 1. Must not be smaller than
   * @a workgroup_size_z.
   */
  uint32_t grid_size_z;

  /**
   * Size in bytes of private memory allocation request (per work-item).
   */
  uint32_t private_segment_size;

  /**
   * Size in bytes of group memory allocation request (per work-group). Must not
   * be less than the sum of the group memory used by the kernel (and the
   * functions it calls directly or indirectly) and the dynamically allocated
   * group segment variables.
   */
  uint32_t group_segment_size;

  /**
   * Address of an object in memory that includes an implementation-defined
   * executable ISA image for the kernel.
   */
  uint64_t kernel_object_address;

  /**
   * Pointer to a buffer containing the kernel arguments. Might be 0.
   *
   * The buffer must be allocated using ::hsa_memory_allocate, and must not be
   * modified once the Dispatch packet is enqueued until the dispatch has
   * completed execution.
   */
  uint64_t kernarg_address;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved3;

  /**
   * Signal used to indicate completion of the job. Might be 0 (no signal).
   */
  hsa_signal_t completion_signal;

} hsa_dispatch_packet_t;

/**
 * @brief Agent Dispatch packet.
 */
typedef struct hsa_agent_dispatch_packet_s {
  /**
   *  Packet header.
   */
  hsa_packet_header_t header;

  /**
   * The function to be performed by the destination HSA Agent. The type value
   * is split into the following ranges: 0x0000:0x3FFF (vendor specific),
   * 0x4000:0x7FFF (HSA runtime) 0x8000:0xFFFF (application defined).
   */
  uint16_t type;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved2;

  /**
   * Address where to store the function return values, if any.
   */
  uint64_t return_address;

  /**
   * Function arguments.
   */
  uint64_t arg[4];

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved3;

  /**
   * Signal used to indicate completion of the job. Might be 0 (no signal).
   */
  hsa_signal_t completion_signal;

} hsa_agent_dispatch_packet_t;

/**
 * @brief Barrier packet.
 */
typedef struct hsa_barrier_packet_s {
  /**
   * Packet header.
   */
  hsa_packet_header_t header;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved2;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved3;

  /**
   * Array of dependent signal objects. Entries with value 0 are valid and are
   * interpreted by the packet processor as satisfied dependencies.
   */
  hsa_signal_t dep_signal[5];

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved4;

  /**
   * Signal used to indicate completion of the job. Might be 0 (no signal).
   */
  hsa_signal_t completion_signal;

} hsa_barrier_packet_t;

/** @} */

/** \defgroup memory Memory
 *  @{
 */

/**
 * @brief A memory region represents a block of contiguous memory that is
 * directly accessible by an agent, and exposes properties about the block of
 * memory and how it is accessed from that particular agent.
 */
typedef uint64_t hsa_region_t;

/**
 * @brief Types of memory segments.
 */
typedef enum {
  /**
   * Global segment. Used to hold data that is shared by all agents.
   */
  HSA_SEGMENT_GLOBAL,
  /**
   * Private segment. Used to hold data that is local to a single work-item.
   */
  HSA_SEGMENT_PRIVATE,
  /**
   * Group segment. Used to hold data that is shared by the work-items of a
   * work-group.
  */
  HSA_SEGMENT_GROUP,
  /**
   * Kernarg segment. Used to pass arguments into a kernel. Memory in this
   * segment is visible to all work-items of the kernel dispatch with which it
   * is associated.
   */
  HSA_SEGMENT_KERNARG,
  /**
   * Read-only segment. Used to hold data that remains constant during the
   * execution of a kernel dispatch.
   */
  HSA_SEGMENT_READONLY,
  /**
   * Spill segment. Used to load or store register spills.
   */
  HSA_SEGMENT_SPILL,
  /**
   * Arg segment. Used to pass arguments into and out of functions.
   */
  HSA_SEGMENT_ARG
} hsa_segment_t;

/**
 * @brief Region flags.
 */
typedef enum {
  /**
   * The application can use memory in the region to store kernel arguments, and
   * provide the values for the kernarg segment of a kernel dispatch. If the
   * region is not in the global segment, this flag must not be set.
   */
  HSA_REGION_FLAG_KERNARG = 1,
  /**
   * Accesses to data in the region are cached in the L1 data cache of the
   * region's agent. If the agent does not have an L1 cache (as reported by the
   * attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L1 = 2,
  /**
   * Accesses to data in the region are cached in the L2 data cache of the
   * region's agent. If the agent does not have an L2 cache (as reported by the
   * attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L2 = 4,
  /**
   * Accesses to data in the region are cached in the L3 data cache of the
   * region's agent. If the agent does not have an L3 cache (as reported by the
   * attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L3 = 8,
  /**
   * Accesses to data in the region are cached in the L4 data cache of the
   * region's agent. If the agent does not have an L4 cache (as reported by the
   * attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L4 = 16
} hsa_region_flag_t;

/**
 * @brief Attributes of a memory region.
 */
typedef enum {
  /**
   * Base (starting) address. The type of this attribute is void*.
   */
  HSA_REGION_INFO_BASE,
  /**
   * Size, in bytes. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_SIZE,
  /**
   * Agent associated with this region. The type of this attribute is
   * hsa_agent_t.
   */
  HSA_REGION_INFO_AGENT,
  /**
   * Flag mask.  The type of this attribute is uint32_t, a bit-field of
   * ::hsa_region_flag_t values.
   */
  HSA_REGION_INFO_FLAGS,
  /**
   * Segment where memory in the region can be used. The type of this
   * attribute is ::hsa_segment_t.
   */
  HSA_REGION_INFO_SEGMENT,
  /**
   * Maximum allocation size in this region, in bytes. A value of 0 indicates
   * that the host cannot allocate memory in the region using
   * ::hsa_memory_allocate. If the value of ::HSA_REGION_INFO_SEGMENT is other
   * than ::HSA_SEGMENT_GLOBAL, the maximum allocation size must be 0. The type
   * of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_MAX_SIZE,
  /**
   * Allocation granularity of buffers allocated by ::hsa_memory_allocate in
   * this region. The size of a buffer allocated in this region is a multiple of
   * the value of this attribute. If ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the
   * allocation granularity must be 0. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_GRANULE,
  /**
   * Alignment of buffers allocated by ::hsa_memory_allocate in this region. If
   * ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the alignment must be 0. Otherwise,
   * it must be a power of 2. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_ALIGNMENT,
  /**
   * Peak bandwidth, in MB/s. The type of this attribute is uint32_t.
   */
  HSA_REGION_INFO_BANDWIDTH,
  /**
   * NUMA node associated with this region. The type of this attribute is
   * uint32_t.
   */
  HSA_REGION_INFO_NODE
} hsa_region_info_t;

/**
 * @brief Get the current value of an attribute of a region.
 *
 * @param[in] region A valid region.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to a application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of @p attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_REGION If the region is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p attribute is not a valid
 * region attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_region_get_info(
    hsa_region_t region,
    hsa_region_info_t attribute,
    void* value);

/**
 * @brief Iterate over the memory regions associated with a given agent, and
 * invoke an application-defined callback on every iteration.
 *
 * @param[in] agent A valid agent.
 *
 * @param[in] callback Callback to be invoked once per region that is directly
 * accessible from the agent.  The runtime passes two arguments to the callback,
 * the region and the application data.  If @p callback returns a status other
 * than ::HSA_STATUS_SUCCESS for a particular iteration, the traversal stops and
 * ::hsa_agent_iterate_regions returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. Might be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT If the agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p callback is NULL.
*/
hsa_status_t HSA_API hsa_agent_iterate_regions(
    hsa_agent_t agent,
    hsa_status_t (*callback)(hsa_region_t region, void* data),
    void* data);

/**
 * @brief Allocate a block of memory in a given region.
 *
 * @param[in] region Region where to allocate memory from.
 *
 * @param[in] size Allocation size, in bytes. This value is rounded up to 
 * the nearest multiple of ::HSA_REGION_INFO_ALLOC_GRANULE in @p
 * region. Allocations of size 0 are allowed and return a NULL pointer.
 *
 * @param[out] ptr Pointer to the location where to store the base address of
 * the allocated block. The returned base address is aligned to the value of
 * ::HSA_REGION_INFO_ALLOC_ALIGNMENT in @p region.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory is available.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_REGION If the region is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ALLOCATION If the host is not allowed to
 * allocate memory in @p region, or @p size is greater than the value of
 * HSA_REGION_INFO_ALLOC_MAX_SIZE in @p region.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p ptr is NULL.
 */
hsa_status_t HSA_API hsa_memory_allocate(hsa_region_t region,
    size_t size,
    void** ptr);

/**
 * @brief Deallocate a block of memory previously allocated using
 * ::hsa_memory_allocate.
 *
 * @param[in] ptr Pointer to a memory block. If @p ptr is NULL, no action is
 * performed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 */
hsa_status_t HSA_API hsa_memory_free(void* ptr);

/* /\** */
/*  * @brief Copy block of memory. */
/*  * */
/*  * @details Copying a number of bytes larger than the size of the memory regions */
/*  * pointed by @p dst or @p src results in undefined behavior. */
/*  * */
/*  * @param[out] dst A valid pointer to the destination array where the content is */
/*  * to be copied. */
/*  * */
/*  * @param[in] src A valid pointer to the source of data to be copied. */
/*  * */
/*  * @param[in] size Number of bytes to copy. If @p size is 0, no copy is */
/*  * performed and the function returns success. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been */
/*  * initialized. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the source or destination */
/*  * pointers are NULL. */
/*  *\/ */
/* hsa_status_t HSA_API hsa_memory_copy( */
/*     void *dst, */
/*     const void *src, */
/*     size_t size); */

/**
 *
 * @brief Register memory.
 *
 * @details Registering a buffer serves as an indication to the runtime that the
 * passed buffer might be accessed from a component other than the
 * host. Registration is a performance hint that allows the runtime
 * implementation to know which buffers will be accessed by some of the
 * components ahead of time.
 *
 * Registrations should not overlap.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * registered. If a NULL pointer is passed, no operation is performed.
 *
 * @param[in] size Requested registration size in bytes. A size of zero is
 * only allowed if @p address is NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocating the necessary resources.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p size is 0 but @p address
 * is not NULL.
 */
hsa_status_t HSA_API hsa_memory_register(
    void *address,
    size_t size);

/**
 *
 * @brief Deregister memory previously registered using ::hsa_memory_register.
 *
 * @details If the memory interval being deregistered does not match a previous
 * registration (start and end addresses), the behavior is undefined.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * deregistered. If a NULL pointer is passed, no operation is performed.
 *
 * @param[in] size Size of the region to be deregistered.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_memory_deregister(
    void *address,
    size_t size);

/** @} */


/** \defgroup extensions Extensions
 *  @{
 */
/**
 * @brief HSA extensions.
 */
typedef enum {
  /**
   * Start of the multi vendor extension range.
   */
  HSA_EXT_START = 0,
  /**
   * Finalizer extension. Finalizes the brig to compilation units that represent
   * kernel and function code objects.
   */
  HSA_EXT_FINALIZER = HSA_EXT_START,
  /**
   * Linker extension.
   */
  HSA_EXT_LINKER = 1,
  /**
   * Images extension.
   */
  HSA_EXT_IMAGES = 2,
  /**
   * Start of the single vendor extension range.
   */
  HSA_SVEXT_START = 10000
} hsa_extension_t;

/**
 * @brief Query vendor extensions.
 *
 * @details If successful, the extension information is written with
 * extension-specific information such as version information, function
 * pointers, and data values. If the extension is not supported, the extension
 * information is not modified.
 *
 * @param[in] extension The vendor extension that is being queried.
 *
 * @param[out] extension_structure Extension structure.
 *
 * @param[out] result Pointer to memory location where to store the query result.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p extension is not a valid
 * value for a single vendor extension or @p result is NULL.
 */
hsa_status_t HSA_API hsa_vendor_extension_query(
    hsa_extension_t extension,
    void *extension_structure,
    int* result);

/**
 * @brief Query HSA extensions.
 *
 * @param[in] extension The  extension that is being queried.
 *
 * @param[out] result Pointer to memory location where to store the query result.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @p extension is not a valid
 * value for an HSA extension or @p result is NULL.
 */
hsa_status_t HSA_API hsa_extension_query(
    hsa_extension_t extension,
    int* result);
/** @} */


#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif
