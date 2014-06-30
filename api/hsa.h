#ifndef HSACOREBASE_H
#define HSACOREBASE_H

#include <stdbool.h>  /* bool */
#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uintXX_t */


/* placeholder for calling convention - check macro naming convention */
#define HSA_API

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** \defgroup status Errors and warnings.
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
    HSA_STATUS_INFO_BREAK,
    /**
     * An initialization attempt failed due to prior initialization.
     */
    HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED,
    /**
     * The finalization options cannot be recognized.
     */
    HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS,
    /**
     * A generic error has occurred.
     */
    HSA_STATUS_ERROR,
    /**
     * One of the actual arguments does not meet a precondition stated in the
     * documentation of the corresponding formal argument.
     */
    HSA_STATUS_ERROR_INVALID_ARGUMENT,
    /**
     * The requested allocation is not valid.
     */
    HSA_STATUS_ERROR_INVALID_ALLOCATION,
    /**
     * The agent is invalid.
     */
    HSA_STATUS_ERROR_INVALID_AGENT,
    /**
     * The memory region is invalid.
     */
    HSA_STATUS_ERROR_INVALID_REGION,
    /**
     * The signal is invalid.
     */
    HSA_STATUS_ERROR_INVALID_SIGNAL,
    /**
     * The queue is invalid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE,
    /**
     * The runtime failed to allocate the necessary resources. This error
     * may also occur when the core runtime library needs to spawn threads or
     * create internal OS-specific events.
     */
    HSA_STATUS_ERROR_OUT_OF_RESOURCES,
    /**
     * The AQL packet is malformed.
     */
    HSA_STATUS_ERROR_INVALID_PACKET_FORMAT,
    /**
     * An error has been detected while releasing a resource.
     */
    HSA_STATUS_ERROR_RESOURCE_FREE,
    /**
     * An API other than ::hsa_init has been invoked while the reference count
     * of the HSA runtime is zero.
     */
    HSA_STATUS_ERROR_NOT_INITIALIZED,
    /**
     * The maximum reference count for the object has been reached.
     */
    HSA_STATUS_ERROR_REFCOUNT_OVERFLOW,
    /**
     * Mismatch between a directive in the control directive structure and in
     * the HSAIL kernel.
     */
    HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH,
    /**
     * Image format is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED,
    /**
     * Image size is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED

} hsa_status_t;

/**
 * @brief Query additional information about a status code.
 *
 * @param[in] status Status code.
 *
 * @param[out] status_string A ISO/IEC 646 encoded English language string that
 * potentially describes the error status. The string terminates in a NUL
 * character.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a status_string is NULL or
 * @a status is not a valid status code.
 */
hsa_status_t HSA_API hsa_status_string(
    hsa_status_t status,
    const char ** status_string);

/** @} */

/** \defgroup common Runtime Common
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


/** \defgroup initshutdown TODO
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
 * runtime might release resources associated with it.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_shut_down();

/** @} */

/** \defgroup agentinfo TODO
 *  @{
 */

/**
 * @brief System attributes.
 */
typedef enum {
  /**
   * Major version of the HSA runtime specification supported by the
   * implementation. The type of this attribute is \c uint16_t.
   */
  HSA_SYSTEM_INFO_VERSION_MAJOR,
  /**
   * Minor version of the HSA runtime specification supported by the
   * implementation. The type of this attribute is \c uint16_t.
   */
  HSA_SYSTEM_INFO_VERSION_MINOR,
  /**
   * Current timestamp. The value of this attribute monotonically increases at a
   * constant rate. The type of this attribute is \c uint64_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP,
  /**
   * Timestamp value increase rate, in MHz. The timestamp (clock) frequency is
   * in the range 1-400MHz. The type of this attribute is \c uint16_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP_FREQUENCY,
  /**
   * Maximum duration of a signal wait operation. Expressed as a count based on
   * the timestamp frequency. The type of this attribute is \c uint64_t.
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
 * large enough to hold the value of @a attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a attribute is not a valid
 * system attribute, or @a value is NULL.
 */
hsa_status_t HSA_API hsa_system_get_info(
    hsa_system_info_t attribute,
    void* value);

/**
 * @brief Opaque handle representing an agent, a device that participates in the
 * HSA memory model. An agent can submit AQL packets for execution, and might
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
   * Agent name. The type of this attribute is a NUL-terminated \c char[64].
   */
  HSA_AGENT_INFO_NAME,
  /**
   * Name of vendor. The type of this attribute is a NUL-terminated \c char[64].
   */
  HSA_AGENT_INFO_VENDOR_NAME,
  /**
   * Agent capability. The type of this attribute is ::hsa_agent_feature_t.
   */
  HSA_AGENT_INFO_FEATURE,
  /**
   * Number of work-items in a wavefront. Must be a power of 2 in the range
   * [1,256]. The value of this attribute is undefined if the agent is not a
   * component. The type of this attribute is \c uint32_t.
   */
  HSA_AGENT_INFO_WAVEFRONT_SIZE,
  /**
   * Maximum number of work-items of each dimension of a workgroup.  Each
   * maximum must be greater than 0. No maximum can exceed the value of
   * ::HSA_AGENT_INFO_WORKGROUP_MAX_SIZE. The value of this attribute is
   * undefined if the agent is not a component. The type of this attribute is
   * uint16_t[3].
   */
  HSA_AGENT_INFO_WORKGROUP_MAX_DIM,
  /**
   * Maximum total number of work-items in a workgroup. The value of this
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
   * this attribute is \c uint32_t.
   */
  HSA_AGENT_INFO_FBARRIER_MAX_SIZE,
  /**
   * Maximum number of queues that can be active (created but not destroyed) at
   * one time in the agent. The type of this attribute is \c uint32_t.
   */
  HSA_AGENT_INFO_QUEUES_MAX,
  /**
   * Maximum capacity (in terms of packets) that a queue can hold. Must be a
   * power of 2 and greater than 0. The type of this attribute is \c uint32_t.
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
   * for that level. The type of this attribute is \c uint32_t[4].
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
   * \c uint32_t.
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
 * large enough to hold the value of @a attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT If the agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a attribute is not a valid
 * agent attribute, or @a value is NULL.
 */
hsa_status_t HSA_API hsa_agent_get_info(
    hsa_agent_t agent,
    hsa_agent_info_t attribute,
    void* value);

/**
 * @brief Iterate over the available agents, and invoke an application-defined
 * callback on every iteration.
 *
 * @details If @a callback returns a status other than ::HSA_STATUS_SUCCESS for
 * a particular iteration, the traversal stops and the function returns that
 * status value.
 *
 *
 * @param[in] callback Callback to be invoked once per agent.
 *
 * @param[in] data Application data that is passed to @a callback on every
 * iteration. Might be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a callback is NULL.
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


/** \defgroup signals TODO
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
#ifdef HSA_LARGE_MODEL
  typedef int64_t hsa_signal_value_t;
#else
  typedef int32_t hsa_signal_value_t;
#endif

/**
 * @brief Create a signal.
 *
 * @param[in] initial_value Initial value of the signal.
 *
 * @param[in] num_consumers Size of @a consumers. A value of 0 indicates that
 * any HSA agent might wait on the signal.
 *
 * @param[in] consumers List of agents that might consume (wait on) the
 * signal. If @a num_consumers is 0, this argument is ignored; otherwise, the
 * runtime might use the list to optimize the handling of the signal object. If
 * an agent not listed in @a consumers waits on the returned signal, the
 * behavior is undefined.
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal is NULL or @a
 * num_consumers is greater than 0 but @a consumers is NULL.
 */
hsa_status_t HSA_API hsa_signal_create(
    hsa_signal_value_t initial_value,
    uint32_t num_consumers,
    hsa_agent_t *consumers,
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
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal is 0.
 */
hsa_status_t HSA_API hsa_signal_destroy(hsa_signal_t signal);

/**
 * @brief Atomically read the current signal value.
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
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which @a value satisfies their wait
 * condition are awakened.
 *
 *  @param[in] signal Signal.
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
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which @a value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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
 * @details If the value of the signal is modified, all the agents waiting on @a
 * signal for which @a value satisfies their wait condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue
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
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which the new value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which the new value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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
 * @brief Atomically perform a logical AND of the value of a signal and a given
 * value.
 *
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which the new value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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
 * @brief Atomically perform a logical OR of the value of a signal and a given
 * value.
 *
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which the new value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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
 * @brief Atomically perform a logical XOR of the value of a signal and a given
 * value.
 *
 * @details Modifying the value of the signal implies sending the signal. All
 * the agents waiting on @a signal for which the new value satisfies their wait
 * condition are awakened.
 *
 * @param[in] signal Signal. If @a signal is a queue doorbell signal, the
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

/* /\** */
/*  * @brief Wait until the value of a signal satisfies a given condition. */
/*  * */
/*  * @details The wait may return before the condition is satisfied due to */
/*  * multiple reasons. It is the application's responsibility to check the returned signal value. */
/*  * */
/*  * When the wait operation atomically loads the value of the passed signal, it */
/*  * uses the memory order indicated in the function name. */
/*  * */
/*  * @param[in] signal Signal. */
/*  * */
/*  * @param[in] condition Condition used to compare the signal value with @a */
/*  * compare_value. */
/*  * */
/*  * @param[in] compare_value Value to compare with. */
/*  * */
/*  * @return Observed value of the signal. The value might not satisfy the given */
/*  * condition. */
/*  * */
/* *\/ */
/* hsa_signal_value_t HSA_API hsa_signal_wait_acquire( */
/*     hsa_signal_t signal, */
/*     hsa_signal_condition_t condition, */
/*     hsa_signal_value_t compare_value); */

/* /\** */
/*  * @copydoc hsa_signal_wait_acquire */
/*  *\/ */
/* hsa_signal_value_t HSA_API hsa_signal_wait_relaxed( */
/*     hsa_signal_t signal, */
/*     hsa_signal_condition_t condition, */
/*     hsa_signal_value_t compare_value); */

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
 * @param[in] condition Condition used to compare the signal value with @a
 * compare_value.
 *
 * @param[in] compare_value Value to compare with.
 *
 * @param[in] timeout_hint Maximum duration hint.  Specified in the same unit as
 * the system timestamp. A value of UINT64_MAX indicates no maximum.
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
 * access the doorbell signal or the service queue.
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
   * Starting address of the runtime-allocated buffer which is used to store the
   * AQL packets. Aligned to the size of an AQL packet.
   */
  uint64_t base_address;

  /**
   * Signal object used by the application to indicate the ID of a packet that
   * is ready to be processed.
   *
   * The HSA runtime is responsible for the life cycle of the doorbell signal:
   * replacing it with another signal or destroying it is not allowed and
   * results in undefined behavior.
   *
   * If @a type is ::HSA_QUEUE_TYPE_SINGLE, it is the application's
   * responsibility to update the doorbell signal value with monotonically
   * increasing indexes.
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
   * A pointer to another user mode queue that can be used by the HSAIL kernel
   * to request system services.
   */
  uint64_t service_queue;

} hsa_queue_t;

/**
 * @brief Create a user mode queue.
 *
 * @details When a queue is created, the runtime also allocates the packet
 * buffer and the completion signal. The application should only rely on the
 * error code returned to determine if the queue is valid.
 *
 * @param[in] agent Agent where to create the queue.
 *
 * @param[in] size Number of packets the queue is expected to hold. Must be a
 * power of 2 that does not exceed the value of ::HSA_AGENT_INFO_QUEUE_MAX_SIZE
 * in @a agent.
 *
 * @param[in] type Type of the queue. If the value of
 * ::HSA_AGENT_INFO_QUEUE_TYPE in @a agent is ::HSA_QUEUE_TYPE_SINGLE, then @a
 * type must also be ::HSA_QUEUE_TYPE_SINGLE.
 *
 * @param[in] callback Callback invoked by the runtime for every asynchronous
 * event related to the newly created queue. Might be NULL.
 *
 * @param[in] service_queue Pointer to a service queue to be associated with the
 * newly created queue. Might be NULL. If not NULL, the queue pointed by @a
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a size is not a power of two,
 * @a type is not a valid queue type, or @a queue is NULL.
 *
 */
hsa_status_t HSA_API hsa_queue_create(
    hsa_agent_t agent,
    size_t size,
    hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *queue),
    hsa_queue_t* service_queue,
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a queue is NULL.
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_inactivate(hsa_queue_t *queue);

/**
 * @brief Atomically load the read index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Read index of the queue pointed by @a queue.
 */
uint64_t hsa_queue_load_read_index_acquire(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_read_index_acquire
 */
uint64_t hsa_queue_load_read_index_relaxed(hsa_queue_t *queue);

/**
 * @brief Atomically load the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Write index of the queue pointed by @a queue.
 */
uint64_t hsa_queue_load_write_index_acquire(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_write_index_acquire
 */
uint64_t hsa_queue_load_write_index_relaxed(hsa_queue_t *queue);

/**
 * @brief Atomically set the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the write index.
 *
 */
void hsa_queue_store_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_write_index_relaxed
 */
void hsa_queue_store_write_index_release(
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
 * @param[in] value Value to assign to the write index if @a expected matches
 * the observed write index.
 *
 * @return Previous value of the write index.
 */
uint64_t hsa_queue_cas_write_index_acq_rel(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t hsa_queue_cas_write_index_acquire(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t hsa_queue_cas_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t hsa_queue_cas_write_index_release(
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
uint64_t hsa_queue_add_write_index_acq_rel(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t hsa_queue_add_write_index_acquire(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t hsa_queue_add_write_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t hsa_queue_add_write_index_release(
    hsa_queue_t *queue,
    uint64_t value);


/**
 * @brief Atomically set the read index of a queue.
 *
 * @details Modifications of the read index are not allowed if the queue is
 * associated with an HSA agent for which only the corresponding packet
 * processor is permitted to update the read index. Invoking this operation in
 * any other queue results in undefined behavior.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the read index.
 *
 */
void hsa_queue_store_read_index_relaxed(
    hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_read_index_relaxed
 */
void hsa_queue_store_read_index_release(
   hsa_queue_t *queue,
   uint64_t value);
/** @} */


/** \defgroup aql TODO
 *  @{
 */

/**
 * @brief Packet type.
 */
typedef enum {
  /**
   * Initial format of a packets when the queue is created. Always reserved
   * packet have never been assigned to the packet processor. From a functional
   * view always reserved packets are equivalent to invalid packets. All queues
   * support this packet format.
   */
  HSA_PACKET_TYPE_ALWAYS_RESERVED = 0,
  /**
   * The packet slot has been processed in the past, and has not been reassigned
   * to the packet processor (is available). All queues support this packet
   * format.
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
   * this packet format.
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
   * No scope. Can be only used in the @a acquire_fence_scope field of a Barrier
   * packet.
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
   * 0x4000:0x7FFF (HSA runtime) 0x8000:0xFFFF (user registered function).
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

/** \defgroup memory TODO
 *  @{
 */

/**
 * @brief A memory region represents a block of memory that is directly
 * accessible by an specific agent, and contains properties about how memory is
 * accessed from that agent. The same memory interval might be accessible to
 * multiple agents, but the runtime associates a unique memory region to each
 * agent. The characteristics of a memory region include its size,
 * addressability, access speed, and corresponding memory segment.
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
   * Read-only segment. Used used to hold data that remains constant during the
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
   * Base (starting) address. The type of this attribute is \c void*.
   */
  HSA_REGION_INFO_BASE,
  /**
   * Size, in bytes. The type of this attribute is \c size_t.
   */
  HSA_REGION_INFO_SIZE,
  /**
   * Agent associated with this region. The type of this attribute is \c
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
   * ::hsa_memory_allocate. The type of this attribute is \c size_t.
   */
  HSA_REGION_INFO_ALLOC_MAX_SIZE,
  /**
   * Allocation granularity of buffers allocated by ::hsa_memory_allocate in
   * this region. The size of a buffer allocated in this region must be a
   * multiple of the value of this attribute. If
   * ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the allocation granularity is
   * also 0. The type of this attribute is \c size_t.
   */
  HSA_REGION_INFO_ALLOC_GRANULE,
  /**
   * Alignment of buffers allocated by ::hsa_memory_allocate in this region. If
   * ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the alignment is also 0. The
   * type of this attribute is \c size_t.
   */
  HSA_REGION_INFO_ALLOC_ALIGNMENT,
  /**
   * Peak bandwidth, in MB/s. The type of this attribute is \c uint32_t.
   */
  HSA_REGION_INFO_BANDWIDTH,
  /**
   * NUMA node associated with this region. The type of this attribute is \c
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
 * large enough to hold the value of @a attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_REGION If the region is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a attribute is not a valid
 * region attribute, or @a value is NULL.
 */
hsa_status_t HSA_API hsa_region_get_info(
    hsa_region_t region,
    hsa_region_info_t attribute,
    void* value);

/**
 * @brief Iterate over the memory regions associated with a given agent, and
 * invoke an application-defined callback on every iteration.
 *
 * @details If @a callback returns a status other than ::HSA_STATUS_SUCCESS for
 * a particular iteration, the traversal stops and the function returns that
 * status value.
 *
 * @param[in] agent A valid agent.
 *
 * @param[in] callback Callback to be invoked once per region that is visible
 * from the agent.
 *
 * @param[in] data Application data that is passed to @a callback on every
 * iteration. Might be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT If the agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a callback is NULL.
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
 * @param[in] size Allocation size, in bytes. Must be a multiple of the value of
 * ::HSA_REGION_INFO_ALLOC_GRANULE in @a region. Allocations of size 0 are
 * allowed and return a NULL pointer.
 *
 * @param[out] ptr Pointer to the location where to store the base address of
 * the allocated block. The returned base address is aligned to the value of
 * ::HSA_REGION_INFO_ALLOC_ALIGNMENT in @a region.
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
 * allocate memory in @a region, @a size is not a multiple of the value of
 * ::HSA_REGION_INFO_ALLOC_GRANULE in @a region, or @a size is greater than the
 * value of HSA_REGION_INFO_ALLOC_MAX_SIZE in @a region.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a ptr is NULL.
 */
hsa_status_t HSA_API hsa_memory_allocate(hsa_region_t region,
    size_t size,
    void** ptr);

/**
 * @brief Deallocate a block of memory previously allocated using
 * ::hsa_memory_allocate.
 *
 * @param[in] ptr Pointer to a memory block. If @a ptr is NULL, no action is
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
/*  * pointed by @a dst or @a src results in undefined behavior. */
/*  * */
/*  * @param[out] dst A valid pointer to the destination array where the content is */
/*  * to be copied. */
/*  * */
/*  * @param[in] src A valid pointer to the source of data to be copied. */
/*  * */
/*  * @param[in] size Number of bytes to copy. If @a size is 0, no copy is */
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
 * only allowed if @a address is NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocating the necessary resources.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a size is 0 but @a address
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

/** \defgroup finalizer Finalizer Core API
 *  @{
 */

/**
 * @brief Profile is used to specify the kind of profile. This controls what
 * features of HSAIL are supported. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef uint8_t hsa_ext_brig_profile8_t;

/**
 * @brief Profile kinds. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Base profile.
   */
  HSA_EXT_BRIG_PROFILE_BASE = 0,

  /**
   * Full profile.
   */
  HSA_EXT_BRIG_PROFILE_FULL = 1
} hsa_ext_brig_profile_t;

/**
 * @brief Machine model type. This controls the size of addresses used for
 * segment and flat addresses. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef uint8_t hsa_ext_brig_machine_model8_t;

/**
 * @brief Machine model kinds. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Use 32 bit addresses for global segment and flat addresses.
   */
  HSA_EXT_BRIG_MACHINE_SMALL = 0,

  /**
   * Use 64 bit addresses for global segment and flat addresses.
   */
  HSA_EXT_BRIG_MACHINE_LARGE = 1
} hsa_ext_brig_machine_model_t;

/**
 * @brief BRIG section id. The index into the array of sections in a BRIG
 * module.
 */
typedef uint32_t hsa_ext_brig_section_id32_t;

/**
 * @brief Predefined BRIG section kinds.
 */
typedef enum {
  /**
   * Textual character strings and byte data used in the module.
   * Also contains variable length arrays of offsets into other sections that
   * are used by entries in the hsa_code and hsa_operand sections. For more
   * information see HSA Programmer's Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_DATA = 0,

  /**
   * All of the directives and instructions of the module. Most
   * entries contain offsets to the hsa_operand or hsa_data sections. Directives
   * provide information to the finalizer, and instructions correspond to HSAIL
   * operations which the finalizer uses to generate executable ISA code.
   * For more information see HSA Programmer's Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_CODE = 1,

  /**
   * The operands of directives and instructions in the code
   * section. For example, immediate constants, registers and address
   * expressions. For more information see HSA Programmer's
   * Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_OPERAND = 2
} hsa_ext_brig_section_id_t;

/**
 * @brief BRIG section header. Every section starts with a
 * ::hsa_ext_brig_section_header_t which contains the section size, name and
 * offset to the first entry. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef struct hsa_ext_brig_section_header_s {
  /**
   * Size in bytes of the section, including the size of the
   * ::hsa_ext_brig_section_header_t. Must be a multiple of 4.
   */
  uint32_t byte_count;

  /**
   * Size of the header in bytes, which is also equal to the offset of the
   * first entry in the section. Must be a multiple of 4.
   */
  uint32_t header_byte_count;

  /**
   * Length of the section name in bytes.
   */
  uint32_t name_length;

  /**
   * Section name, @a name_length bytes long.
   */
  uint8_t name[1];
} hsa_ext_brig_section_header_t;

/**
 * @brief A module is the basic building block for HSAIL programs. When HSAIL
 * is generated it is represented as a module.
 */
typedef struct hsa_ext_brig_module_s {
  /**
   * Number of sections in the module. Must be at least 3.
   */
  uint32_t section_count;

  /**
   * A variable-sized array containing pointers to the BRIG sections. Must have
   * @a section_count elements. Indexed by ::hsa_ext_brig_section_id32_t. The
   * first three elements must be for the following predefined sections in
   * the following order: ::HSA_EXT_BRIG_SECTION_DATA,
   * ::HSA_EXT_BRIG_SECTION_CODE, ::HSA_EXT_BRIG_SECTION_OPERAND.
   */
  hsa_ext_brig_section_header_t *section[1];
} hsa_ext_brig_module_t;

/**
 * @brief An opaque handle to the ::hsa_ext_brig_module_t.
 */
typedef struct hsa_ext_brig_module_handle_s {
  /**
   * HSA component specific handle to the brig module.
   */
  uint64_t handle;
} hsa_ext_brig_module_handle_t;

/**
 * @brief An entry offset into the code section of the BRIG module. The value
 * is the byte offset relative to the start of the section to the beginning of
 * the referenced entry. The value 0 is reserved to indicate that the offset
 * does not reference any entry.
 */
typedef uint32_t hsa_ext_brig_code_section_offset32_t;

/**
 * @brief The set of exceptions supported by HSAIL. This is represented as a
 * bit set.
 */
typedef uint16_t hsa_ext_exception_kind16_t;

/**
 * @brief HSAIL exception kinds. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Operations are performed on values for which the results are not defined.
   * These are:
   *   - Operations on signaling NaN (sNaN) floating-point values.
   *   - Signalling comparisons: comparisons on quiet NaN (qNaN)
   *     floating-point values.
   *   - Multiplication: mul(0.0, infinity) or mul(infinity, 0.0).
   *   - Fused multiply add: fma(0.0, infinity, c) or fma(infinity, 0.0, c)
   *     unless c is a quiet NaN, in which case it is implementation-defined
   *     if an exception is generated.
   *   - Addition, subtraction, or fused multiply add: magnitude subtraction
   *     of infinities, such as: add(positive infinity, negative infinity),
   *     sub(positive infinity, positive infinity).
   *   - Division: div(0.0, 0.0) or div(infinity, infinity).
   *   - Square root: sqrt(negative).
   *   - Conversion: A cvt with a floating-point source type, an integer
   *     destination type, and a nonsaturating rounding mode, when the source
   *     value is a NaN, infinity, or the rounded value, after any flush to
   *     zero, cannot be represented precisely in the integer type of the
   *     destination.
   */
  HSA_EXT_EXCEPTION_INVALID_OPERATION = 1,

  /**
   * A finite non-zero floating-point value is divided by zero. It is
   * implementation defined if integer div or rem operations with a divisor of
   * zero will generate a divide by zero exception.
   */
  HSA_EXT_EXCEPTION_DIVIDE_BY_ZERO = 2,

  /**
   * The floating-point exponent of a value is too large to be represented.
   */
  HSA_EXT_EXCEPTION_OVERFLOW = 4,

  /**
   * A non-zero tiny floating-point value is computed and either the ftz
   * modifier is specified, or the ftz modifier was not specified and the value
   * cannot be represented exactly.
   */
  HSA_EXT_EXCEPTION_UNDERFLOW = 8,

  /**
   * A computed floating-point value is not represented exactly in the
   * destination. This can occur due to rounding. In addition, it is
   * implementation defined if operations with the ftz modifier that cause a
   * value to be flushed to zero generate the inexact exception.
   */
  HSA_EXT_EXCEPTION_INEXACT = 16
} hsa_ext_exception_kind_t;

/**
 * @brief Bit set of control directives supported in HSAIL. See HSA Programmer's
 * Reference Manual description of control directives with the same name for
 * more information. For control directives that have an associated value, the
 * value is given by the field in hsa_ext_control_directives_t. For control
 * directives that are only present of absent (such as
 * requirenopartialworkgroups) they have no corresponding field as the presence
 * of the bit in this mask is sufficient.
 */
typedef uint64_t hsa_ext_control_directive_present64_t;

/**
 * @brief HSAIL control directive kinds. For more information see HSA
 * Programmer's Reference Manual.
 */
typedef enum {
  /**
   * If not enabled then must be 0, otherwise must be non-0 and specifies the
   * set of HSAIL exceptions that must have the BREAK policy enabled.  If this
   * set is not empty then the generated code may have lower performance than if
   * the set is empty. If the kernel being finalized has any
   * enablebreakexceptions control directives, then the values specified by this
   * argument are unioned with the values in these control directives.  If any
   * of the functions the kernel calls have an enablebreakexceptions control
   * directive, then they must be equal or a subset of, this union.
   */
  HSA_EXT_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,

  /**
   * If not enabled then must be 0, otherwise must be non-0 and specifies the
   * set of HSAIL exceptions that must have the DETECT policy enabled.  If this
   * set is not empty then the generated code may have lower performance than if
   * the set is empty. However, an implementation should endeavour to make the
   * performance impact small. If the kernel being finalized has any
   * enabledetectexceptions control directives, then the values specified by
   * this argument are unioned with the values in these control directives. If
   * any of the functions the kernel calls have an enabledetectexceptions
   * control directive, then they must be equal or a subset of, this union.
   */
  HSA_EXT_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,

  /**
   * If not enabled then must be 0, and any amount of dynamic group segment can
   * be allocated for a dispatch, otherwise the value specifies the maximum
   * number of bytes of dynamic group segment that can be allocated for a
   * dispatch. If the kernel being finalized has any maxdynamicsize control
   * directives, then the values must be the same, and must be the same as this
   * argument if it is enabled. This value can be used by the finalizer to
   * determine the maximum number of bytes of group memory used by each
   * work-group by adding this value to the group memory required for all group
   * segment variables used by the kernel and all functions it calls, and group
   * memory used to implement other HSAIL features such as fbarriers and the
   * detect exception operations. This can allow the finalizer to determine the
   * expected number of work-groups that can be executed by a compute unit and
   * allow more resources to be allocated to the work-items if it is known that
   * fewer work-groups can be executed due to group memory limitations.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,

  /**
   * If not enabled then must be 0, otherwise must be greater than 0. Specifies
   * the maximum number of work-items that will be in the grid when the kernel
   * is dispatched. For more information see HSA Programmer's Reference Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  /**
   * If not enabled then must be 0, otherwise must be greater than 0. Specifies
   * the maximum number of work-items that will be in the work-group when the
   * kernel is dispatched. For more information see HSA Programmer's Reference
   * Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,

  /**
   * If not enabled then must be 0, and the finalizer is free to generate ISA
   * that may result in any number of work-groups executing on a single compute
   * unit. Otherwise, the finalizer should attempt to generate ISA that will
   * allow the specified number of work-groups to execute on a single compute
   * unit. This is only a hint and can be ignored by the finalizer. If the
   * kernel being finalized, or any of the functions it calls, has a requested
   * control directive, then the values must be the same. This can be used to
   * determine the number of resources that should be allocated to a single
   * work-group and work-item. For example, a low value may allow more resources
   * to be allocated, resulting in higher per work-item performance, as it is
   * known there will never be more than the specified number of work-groups
   * actually executing on the compute unit.  Conversely, a high value may
   * allocate fewer resources, resulting in lower per work-item performance,
   * which is offset by the fact it allows more work-groups to actually execute
   * on the compute unit.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,

  /**
   * If not enabled then all elements for Dim3 must be 0, otherwise every
   * element must be greater than 0. Specifies the grid size that will be used
   * when the kernel is dispatched. For more information see HSA Programmer's
   * Reference Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,

  /**
   * If not enabled then all elements for Dim3 must be 0, and the produced code
   * can be dispatched with any legal work-group range consistent with the
   * dispatch dimensions. Otherwise, the code produced must always be dispatched
   * with the specified work-group range. No element of the specified range must
   * be 0. It must be consistent with required_dimensions and
   * max_flat_workgroup_size. If the kernel being finalized, or any of the
   * functions it calls, has a requiredworkgroupsize control directive, then the
   * values must be the same. Specifying a value can allow the finalizer to
   * optimize work-group id operations, and if the number of work-items in the
   * work-group is less tha the WAVESIZE then barrier operations can be
   * optimized to just a memory fence.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,

  /**
   * If not enabled then must be 0 and the produced kernel code can be
   * dispatched with 1, 2 or 3 dimensions. If enabled then the value is 1..3 and
   * the code produced must only be dispatched with a dimension that
   * matches. Other values are illegal. If the kernel being finalized, or any of
   * the functions it calls, has a requireddimsize control directive, then the
   * values must be the same. This can be used to optimize the code generated to
   * compute the absolute and flat work-group and work-item id, and the dim
   * HSAIL operations.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,

  /**
   * Specifies that the kernel must be dispatched with no partial work-groups.
   * It can be placed in either a kernel or a function code block. This is only
   * a hint and can be ignored by the finalizer.
   *
   * It is undefined if the kernel is dispatched with any dimension of the grid
   * size not being an exact multiple of the corresponding dimension of the
   * work-group size.
   *
   * A finalizer might be able to generate better code for currentworkgroupsize
   * if it knows there are no partial work-groups, because the result becomes
   * the same as the workgroupsize operation. An HSA component might be able to
   * dispatch a kernel more efficiently if it knows there are no partial
   * work-groups.
   *
   * The control directive applies to the whole kernel and all functions it
   * calls. It can appear multiple times in a kernel or function. If it appears
   * in a function (including external functions), then it must also appear in
   * all kernels that call that function (or have been specified when the
   * finalizer was invoked), either directly or indirectly.
   *
   * If require no partial work-groups is specified when the finalizer is
   * invoked, the kernel behaves as if the requirenopartialworkgroups control
   * directive has been specified.
   *
   * \note require_no_partial_work_groups does not have a field since having the
   * bit set in enabledControlDirectives indicates that the cpntrol directive is
   * present.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256
} hsa_ext_control_directive_present_t;

/**
 * @brief The hsa_ext_control_directives_t specifies the values for the HSAIL
 * control directives. These control how the finalizer generates code. This
 * struct is used both as an argument to hsaFinalizeKernel to specify values for
 * the control directives, and is used in HsaKernelCode to record the values of
 * the control directives that the finalize used when generating the code which
 * either came from the finalizer argument or explicit HSAIL control
 * directives. See the definition of the control directives in HSA Programmer's
 * Reference Manual which also defines how the values specified as finalizer
 * arguments have to agree with the control directives in the HSAIL code.
 */
typedef struct hsa_ext_control_directives_s {
  /**
   * This is a bit set indicating which control directives have been
   * specified. If the value is 0 then there are no control directives specified
   * and the rest of the fields can be ignored. The bits are accessed using the
   * hsa_ext_control_directives_present_mask_t. Any control directive that is not
   * enabled in this bit set must have the value of all 0s.
   */
  hsa_ext_control_directive_present64_t enabled_control_directives;

  /**
   * If enableBreakExceptions is not enabled then must be 0, otherwise must be
   * non-0 and specifies the set of HSAIL exceptions that must have the BREAK
   * policy enabled. If this set is not empty then the generated code may have
   * lower performance than if the set is empty. If the kernel being finalized
   * has any enablebreakexceptions control directives, then the values specified
   * by this argument are unioned with the values in these control
   * directives. If any of the functions the kernel calls have an
   * enablebreakexceptions control directive, then they must be equal or a
   * subset of, this union.
   */
  hsa_ext_exception_kind16_t enable_break_exceptions;

  /**
   * If enableDetectExceptions is not enabled then must be 0, otherwise must be
   * non-0 and specifies the set of HSAIL exceptions that must have the DETECT
   * policy enabled. If this set is not empty then the generated code may have
   * lower performance than if the set is empty. However, an implementation
   * should endeavour to make the performance impact small. If the kernel being
   * finalized has any enabledetectexceptions control directives, then the
   * values specified by this argument are unioned with the values in these
   * control directives. If any of the functions the kernel calls have an
   * enabledetectexceptions control directive, then they must be equal or a
   * subset of, this union.
   */
  hsa_ext_exception_kind16_t enable_detect_exceptions;

  /**
   * If maxDynamicGroupSize is not enabled then must be 0, and any amount of
   * dynamic group segment can be allocated for a dispatch, otherwise the value
   * specifies the maximum number of bytes of dynamic group segment that can be
   * allocated for a dispatch. If the kernel being finalized has any
   * maxdynamicsize control directives, then the values must be the same, and
   * must be the same as this argument if it is enabled. This value can be used
   * by the finalizer to determine the maximum number of bytes of group memory
   * used by each work-group by adding this value to the group memory required
   * for all group segment variables used by the kernel and all functions it
   * calls, and group memory used to implement other HSAIL features such as
   * fbarriers and the detect exception operations. This can allow the finalizer
   * to determine the expected number of work-groups that can be executed by a
   * compute unit and allow more resources to be allocated to the work-items if
   * it is known that fewer work-groups can be executed due to group memory
   * limitations.
   */
  uint32_t max_dynamic_group_size;

  /**
   * If maxFlatGridSize is not enabled then must be 0, otherwise must be greater
   * than 0. See HSA Programmer's Reference Manual description of
   * maxflatgridsize control directive.
   */
  uint32_t max_flat_grid_size;

  /**
   * If maxFlatWorkgroupSize is not enabled then must be 0, otherwise must be
   * greater than 0. See HSA Programmer's Reference Manual description of
   * maxflatworkgroupsize control directive.
   */
  uint32_t max_flat_workgroup_size;

  /**
   * If requestedWorkgroupsPerCu is not enabled then must be 0, and the
   * finalizer is free to generate ISA that may result in any number of
   * work-groups executing on a single compute unit. Otherwise, the finalizer
   * should attempt to generate ISA that will allow the specified number of
   * work-groups to execute on a single compute unit. This is only a hint and
   * can be ignored by the finalizer. If the kernel being finalized, or any of
   * the functions it calls, has a requested control directive, then the values
   * must be the same. This can be used to determine the number of resources
   * that should be allocated to a single work-group and work-item. For example,
   * a low value may allow more resources to be allocated, resulting in higher
   * per work-item performance, as it is known there will never be more than the
   * specified number of work-groups actually executing on the compute
   * unit. Conversely, a high value may allocate fewer resources, resulting in
   * lower per work-item performance, which is offset by the fact it allows more
   * work-groups to actually execute on the compute unit.
   */
  uint32_t requested_workgroups_per_cu;

  /**
   * If not enabled then all elements for Dim3 must be 0, otherwise every
   * element must be greater than 0. See HSA Programmer's Reference Manual
   * description of requiredgridsize control directive.
   */
  hsa_dim3_t required_grid_size;

  /**
   * If requiredWorkgroupSize is not enabled then all elements for Dim3 must be
   * 0, and the produced code can be dispatched with any legal work-group range
   * consistent with the dispatch dimensions. Otherwise, the code produced must
   * always be dispatched with the specified work-group range. No element of the
   * specified range must be 0. It must be consistent with required_dimensions
   * and max_flat_workgroup_size. If the kernel being finalized, or any of the
   * functions it calls, has a requiredworkgroupsize control directive, then the
   * values must be the same. Specifying a value can allow the finalizer to
   * optimize work-group id operations, and if the number of work-items in the
   * work-group is less than the WAVESIZE then barrier operations can be
   * optimized to just a memory fence.
   */
  hsa_dim3_t required_workgroup_size;

  /**
   * If requiredDim is not enabled then must be 0 and the produced kernel code
   * can be dispatched with 1, 2 or 3 dimensions. If enabled then the value is
   * 1..3 and the code produced must only be dispatched with a dimension that
   * matches. Other values are illegal. If the kernel being finalized, or any of
   * the functions it calls, has a requireddimsize control directive, then the
   * values must be the same. This can be used to optimize the code generated to
   * compute the absolute and flat work-group and work-item id, and the dim
   * HSAIL operations.
   */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];
} hsa_ext_control_directives_t;

/**
 * @brief The kinds of code objects that can be contained in
 * ::hsa_ext_code_descriptor_t.
 */
typedef uint32_t hsa_ext_code_kind32_t;

/**
 * @brief Kinds of code object. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Not a code object.
   */
  HSA_EXT_CODE_NONE = 0,

  /**
   * HSAIL kernel that can be used with an AQL dispatch packet.
   */
  HSA_EXT_CODE_KERNEL = 1,

  /**
   * HSAIL indirect function.
   */
  HSA_EXT_CODE_INDIRECT_FUNCTION = 2,

  /**
   * HSA runtime code objects. For example, partially linked code objects.
   */
  HSA_EXT_CODE_RUNTIME_FIRST = 0x40000000,
  HSA_EXT_CODE_RUNTIME_LAST = 0x7fffffff,

  /**
   * Vendor specific code objects.
   */
  HSA_EXT_CODE_VENDOR_FIRST = 0x80000000,
  HSA_EXT_CODE_VENDOR_LAST = 0xffffffff
} hsa_ext_code_kind_t;

/**
 * @brief Each HSA component can support one or more call conventions. For
 * example, an HSA component may have different call conventions that each use
 * a different number of isa registers to allow different numbers of
 * wavefronts to execute on a compute unit.
 */
typedef uint32_t hsa_ext_program_call_convention_id32_t;

/**
 * @brief Kinds of program call convention IDs.
 */
typedef enum {
 /**
  * Finalizer determined call convention ID.
  */
  HSA_EXT_PROGRAM_CALL_CONVENTION_FINALIZER_DETERMINED = -1
} hsa_ext_program_call_convention_id_t;

/**
 * @brief The 64-bit opaque code handle to the finalized code that includes the
 * executable ISA for the HSA component. It can be used for the kernel
 * dispatch packet kernel object address field.
 */
typedef struct hsa_ext_code_handle_s {
  /**
   * HSA component specific handle to the code.
   */
  uint64_t handle;
} hsa_ext_code_handle_t;

/**
 * @brief An opaque handle to the debug information.
 */
typedef struct hsa_ext_debug_information_handle_s {
  /**
   * HSA component specific handle to the debug information.
   */
  uint64_t handle;
} hsa_ext_debug_information_handle_t;

/**
 * @brief Provides the information about a finalization of the kernel or
 * indirect function for a specific HSA component, and for indirect functions,
 * a specific call convention of that HSA component
 */
typedef struct hsa_ext_code_descriptor_s {
  /**
   * Type of code object this code descriptor associated with.
   */
  hsa_ext_code_kind32_t code_type;

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
   * must already be included in the workgroupGroupSegmentByteSize total.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   * The 64-bit opaque code handle to the finalized code that includes the
   * executable ISA for the HSA component. It can be used for the kernel
   * dispatch packet kernel object address field.
   */
  hsa_ext_code_handle_t code;

  /**
   * The maximum byte alignment of variables used by the kernel in the kernarg
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t kernarg_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the group
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t group_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the private
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t private_segment_alignment;

  /**
   * Wavefront size expressed as a power of two. Must be a power of 2 in range
   * 1..64 inclusive. Used to support runtime query that obtains wavefront size,
   * which may be used by application to allocated dynamic group memory and set
   * the dispatch work-group size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   * Program call convention id this code descriptor holds.
   */
  hsa_ext_program_call_convention_id32_t program_call_convention;

  /**
   * BRIG module handle this code descriptor associated with.
   */
  hsa_ext_brig_module_handle_t module;

  /**
   * BRIG directive offset this code descriptor associated with.
   */
  hsa_ext_brig_code_section_offset32_t symbol;

  /**
   * The HSAIL profile defines which features are used. This information is from
   * the HSAIL version directive. If this ::hsa_ext_code_descriptor_t is not
   * generated from an ::hsa_ext_finalize then must still indicate what
   * profile is being used.
   */
  hsa_ext_brig_profile8_t hsail_profile;

  /**
   * The HSAIL machine model gives the address sizes used by the code. This
   * information is from the HSAIL version directive. If this
   * ::hsa_ext_code_descriptor_t is not generated from an ::hsa_ext_finalize
   * then must still indicate for what machine mode the code is generated.
   */
  hsa_ext_brig_machine_model8_t hsail_machine_model;

  /**
   * Reserved for BRIG target options if any are defined in the future. Must
   * be 0.
   */
  uint16_t reserved1;

  /**
   * Opaque handle to debug information.
   */
  hsa_ext_debug_information_handle_t debug_information;

  /**
   * The vendor of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 624 character encoding must be used. If the
   * name is less than 24 characters then remaining characters must
   * be set to 0.
   */
  char agent_vendor[24];

  /**
   * The vendor's name of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 624 character encoding must be used. If the name is less
   * than 24 characters then remaining characters must be set to 0.
   */
  char agent_name[24];

  /**
   * The HSAIL major version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then must be 0.
   */
  uint32_t hsail_version_major;

  /**
   * The HSAIL minor version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then must be 0.
   */
  uint32_t hsail_version_minor;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * The values should be the actually values used by the finalizer in
   * generating the code. This may be the union of values specified as finalizer
   * arguments and explicit HSAIL control directives. If the finalizer chooses
   * to ignore a control directive, and not generate constrained code, then the
   * control directive should not be marked as enabled even though it was
   * present in the HSAIL or finalizer argument. The values are intended to
   * reflect the constraints that the code actually requires to correctly
   * execute, not the values that were actually specified at finalize time.
   */
  hsa_ext_control_directives_t control_directive;
} hsa_ext_code_descriptor_t;

/**
 * @brief Finalization request. Holds information about the module needed
 * to be finalized. If the module contains an indirect function, also holds
 * information about call convention id.
 */
typedef struct hsa_ext_finalization_request_s {
  /**
   * Handle to the ::hsa_ext_brig_module_t, which needs to be finalized.
   */
  hsa_ext_brig_module_handle_t module;
  /**
   * Entry offset into the code section.
   */
  hsa_ext_brig_code_section_offset32_t symbol;
  /**
   * If this finalization request is for indirect function, desired program
   * call convention.
   */
  hsa_ext_program_call_convention_id32_t program_call_convention;
} hsa_ext_finalization_request_t;

/**
 * @brief Finalization handle is the handle to the object produced by the
 * Finalizer that contains the isa code and related information needed to
 * execute that code for a specific agent for the set of  kernels/indirect
 * functions specified in the finalization request.
 */
typedef struct hsa_ext_finalization_handle_s {
  /**
   * HSA component specific handle to the finalization information.
   */
  uint64_t handle;
} hsa_ext_finalization_handle_t;

/**
 * @brief Call back function to get the definition of a module scope
 * variable/fbarrier or kernel/function.
 */
typedef hsa_status_t (*hsa_ext_symbol_definition_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_brig_module_handle_t *definition_module,
  hsa_ext_brig_module_t *definition_module_brig,
  hsa_ext_brig_code_section_offset32_t *definition_symbol);

/**
 * @brief Call back function to get the address of global segment variables,
 * kernel table variable, indirect function table variable.
 */
typedef hsa_status_t (*hsa_ext_symbol_address_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  uint64_t *symbol_address);

/**
 * @brief Call back function to get the string representation of the error
 * message.
 */
typedef hsa_status_t (*hsa_ext_error_message_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t statement,
  uint32_t indent_level,
  const char *message);

/**
 * @brief Finalizes provided list of kernel and/or indirect functions.
 *
 * @details Invokes the finalizer on the provided list of kernels and indirect
 * functions. A kernel can only be finalized once per program per agent.
 * An indirect function can only be finalized once per program per agent per
 * call convention. Only code for the HSA components specified when the
 * program was created can be requested. The program must contain a definition
 * for the requested kernels and indirect functions amongst the modules that
 * have been added to the program.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] agent HSA agents for which code must be produced.
 *
 * @param[in] program_agent_id Program agent id.
 *
 * @param[in] program_agent_count Number of program agents.
 *
 * @param[in] finalization_request_count The number of kernels and/or
 * indirect needed to be finalized.
 *
 * @param[in] finalization_request_list List of kernels and/or indirect
 * functions needed to be finalized.
 *
 * @param[in] control_directives The control directives that can be specified to
 * influence how the finalizer generates code. If NULL then no control
 * directives are used. If this call is successful and control_directives is not
 * NULL, then the resulting hsa_ext_code_descriptor_t object will have control
 * directives which were used by the finalizer.
 *
 * @param[in] symbol_definition_callback Call back function to get the
 * definition of a module scope variable/fbarrier or kernel/function.
 *
 * @param[in] symbol_address_callback Call back function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that control
 * the level of optimization performed by the finalizer.
 *
 * @param[in] options Implementation defined options that can be specified to
 * the finalizer.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] finalization Handle to the object produced that contains the
 * isa code and related information needed to execute that code for the
 * specific @a agent for the set of kernels/indirect functions specified in the
 * @a finalization_request.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH If the directive in the
 * control directive structure and in the HSAIL kernel mismatch or if the same
 * directive is used with a different value in one of the functions used by
 * this kernel.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization_request_list
 * is NULL or invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the finalize API cannot
 * allocate memory for @a finalization.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS If the options are not
 * recognized, no error is returned, just an info status is used to indicate
 * invalid options.
 */
hsa_status_t HSA_API hsa_ext_finalize(
  hsa_runtime_caller_t caller,
  hsa_agent_t agent,
  uint32_t program_agent_id,
  uint32_t program_agent_count,
  size_t finalization_request_count,
  hsa_ext_finalization_request_t *finalization_request_list,
  hsa_ext_control_directives_t *control_directives,
  hsa_ext_symbol_definition_callback_t symbol_definition_callback,
  hsa_ext_symbol_address_callback_t symbol_address_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  uint8_t optimization_level,
  const char *options,
  int debug_information,
  hsa_ext_finalization_handle_t *finalization);

/**
 * @brief Queries the total number of kernel and indirect functions that
 * have been finalized as part of the finalization object.
 *
 * @param[in] agent Agent for which the finalization object contains code.
 *
 * @param[in] finalization Finalization handle that references the finalization
 * object for @a agent.
 *
 * @param[out] code_descriptor_count Number of kernel and indirect functions that
 * have been finalized as part of the finalization object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and number of code descriptors is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a agent is
 * NULL or not valid. If @a finalization points to invalid finalization. If
 * @a code_descriptor_count is NULL.
 */
hsa_status_t hsa_ext_query_finalization_code_descriptor_count(
  hsa_agent_t agent,
  hsa_ext_finalization_handle_t finalization,
  uint32_t *code_descriptor_count);

/**
 * @brief Queries information about one of the kernel or indirect functions that
 * have been finalized as part of a finalization object.
 *
 * @param[in] agent Agent for which the finalization object contains code.
 *
 * @param[in] finalization Finalization handle that references the finalization
 * object for @a agent.
 *
 * @param[in] index Specifies which kernel or indirect function information is
 * being requested. Must be in the range 0 to
 * ::hsa_ext_query_finalization_code_descriptor_count – 1.
 *
 * @param[out] code_descriptor The information about the requested kernel or
 * indirect function.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and code descriptor is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a agent is
 * NULL or not valid. If @a finalization points to invalid finalization. If
 * @a code_descriptor_count is NULL.
 */
hsa_status_t hsa_ext_query_finalization_code_descriptor(
  hsa_agent_t agent,
  hsa_ext_finalization_handle_t finalization,
  uint32_t index,
  hsa_ext_code_descriptor_t *code_descriptor);

/**
 * @brief Destroys a finalization. This may reclaim the memory occupied by the
 * finalization object, and remove corresponding isa code from the associated
 * agent. Once destroyed, all code that is part of the finalization object is
 * invalidated. It is undefined if any dispatch is executing, or will
 * subsequently be executed, when the finalization containing its code is
 * destroyed.
 *
 * @param[in] agent Agent for which the finalization object contains code.
 *
 * @param[in] finalization Handle to the finalization to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization
 * is NULL or does not point to a valid finalization structure.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If some of the resources consumed
 * during initialization by the runtime could not be freed.
 */
hsa_status_t HSA_API hsa_ext_destroy_finalization(
  hsa_agent_t agent,
  hsa_ext_finalization_handle_t finalization);

/**
 * @brief Serializes the finalization.
 *
 * @details Serializes finalization descriptor for specified @a agent. The
 * caller can set @a debug_information to 1 in order to include debug
 * information of this finalization descriptor in the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] agent The HSA agent for which @a finalization must be
 * serialized.
 *
 * @param[in] finalization Handle to the finalization to be serialized.
 *
 * @param[in] alloc_serialize_data_callback Call back function for allocation.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization
 * is either NULL or does not point to a valid finalization descriptor object.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a serialized_object.
 */
hsa_status_t HSA_API hsa_ext_serialize_finalization(
  hsa_runtime_caller_t caller,
  hsa_agent_t agent,
  hsa_ext_finalization_handle_t finalization,
  hsa_runtime_alloc_data_callback_t alloc_serialize_data_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  void *serialized_object);

/**
 * @brief Deserializes the finalization.
 *
 * @details Deserializes finalization descriptor for specified @a agent. The
 * caller can set @a debug_information to 1 in order to include debug
 * information of this finalization descriptor from the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] serialized_object Serialized object to be deserialized.
 *
 * @param[in] agent The HSA agent for which @a finalization must be
 * serialized.
 *
 * @param[in] program_agent_id TODO.
 *
 * @param[in] program_agent_count TODO.
 *
 * @param[in] symbol_address_callback Call back function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] finalization Handle to the deserialized finalization.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a serialized_object is
 * either NULL, or is not valid, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a finalization.
 */
hsa_status_t HSA_API hsa_ext_deserialize_finalization(
  hsa_runtime_caller_t caller,
  void *serialized_object,
  hsa_agent_t agent,
  uint32_t program_agent_id,
  uint32_t program_agent_count,
  hsa_ext_symbol_address_callback_t symbol_address_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  hsa_ext_finalization_handle_t *finalization);
/** @} */

/** \defgroup linker HSAIL Linker Service Layer
 *  @{
 */

/**
 * @brief An opaque handle to the HSAIL program.
 *
 * @details An application can use the HSA runtime to create zero or more
 * HSAIL programs, to which it can add zero or more HSAIL modules. HSAIL
 * program manages linking of symbol declaration to symbol definitions
 * between modules. In addition, the application can provide symbol definitions
 * to an HSAIL program, and can obtain the address of symbols defined by the
 * HSAIL program using the HSA runtime. An HSAIL program can be created with
 * ::hsa_ext_program_create, which returns a handle to the created program
 * ::hsa_ext_program_handle_t. A program handle has to be further used to
 * add particular modules to the program using ::hsa_ext_add_module, perform
 * various define, query and validation operations, finalize the program using
 * ::hsa_ext_finalize_program. A program has to be destroyed once not needed
 * any more using ::hsa_ext_program_destroy.
 */
typedef struct hsa_ext_program_handle_s {
  /**
   * HSA component specific handle to the program.
   */
  uint64_t handle;
} hsa_ext_program_handle_t;

/**
 * ID of an agent within a program that it is a member. It is used to index
 * a kernel descriptor to access the code descriptor for the agent. An agent
 * can be a member of multiple programs and can have different
 * ::hsa_ext_program_agent_id_t in each program.
 */
typedef uint32_t hsa_ext_program_agent_id_t;

/**
 * @brief Creates an HSAIL program.
 *
 * @details Creates an HSAIL program. When an HSAIL program is created, one
 * or more HSA components that are part of the HSA platform must be specified
 * (::hsa_agent_t), together with the machine model
 * (::hsa_ext_brig_machine_model8_t) and profile (::hsa_ext_brig_profile8_t).
 * The set of agents associated with the HSAIL being created cannot be changed
 * after the program is created. The machine model address size for the global
 * segment must match the size used by the application. All modules added to
 * the program must have the same machine model and profile as the program.
 * Once the program is created, the program handle (::hsa_ext_program_handle_t)
 * is returned. See ::hsa_ext_program_handle_t for more details.
 *
 * @param[in] agents One or more HSA components that are part of the HSA
 * platform to create a program for.
 *
 * @param[in] agent_count Number of HSA components to create an HSAIL program
 * for.
 *
 * @param[in] machine_model The kind of machine model this HSAIL program is
 * created for. The machine model address size for the global segment must
 * match the size used by the applications. All module added to the program
 * must have the same machine model.
 *
 * @param[in] profile The kind of profile this HSAIL program is created for.
 * All modules added to the program must have the same profile as the program.
 *
 * @param[out] program A valid pointer to a program handle for the HSAIL
 * program created.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and an HSAIL program is created.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent is NULL, or not
 * valid. If @a agent_count is 0. If @a machine_model is not valid. If
 * @a profile is not valid. In this case @a program will be NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure to
 * allocate resources required for program creation.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED If @a program is already a
 * valid program. No error is returned, just an info status is used to indicate
 * invalid options.
 */
hsa_status_t HSA_API hsa_ext_program_create(
  hsa_agent_t *agents,
  uint32_t agent_count,
  hsa_ext_brig_machine_model8_t machine_model,
  hsa_ext_brig_profile8_t profile,
  hsa_ext_program_handle_t *program);

/**
 * @brief Destroys an HSAIL program.
 *
 * @details Destroys an HSAIL program pointed to by program handle @a program.
 * When the program is destroyed, member code objects are destroyed as well.
 *
 * @param[in] program Program handle for the HSAIL program to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and an HSAIL program is destroyed.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program is not a valid
 * ::hsa_ext_program_handle_t object.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If @a program is already destroyed
 * or has never been created.
 */
hsa_status_t HSA_API hsa_ext_program_destroy(
  hsa_ext_program_handle_t program);

/**
 * @brief Adds an existing HSAIL module to an existing HSAIL program.
 *
 * @details Adds an existing HSAIL module to an existing HSAIL program. An
 * HSAIL module is the unit of HSAIL generation, and can contain multiple
 * symbol declarations and definitions. An HSAIL module can be added to zero
 * or more HSAIL programs. Distinct instances of the symbols it defines are
 * created within each program, and symbol declarations are only linked to
 * the definitions provided by other modules in the same program. The same
 * HSAIL module can be added to multiple HSAIL programs, which allows multiple
 * instances of the same kernel and indirect functions that reference distinct
 * allocations of global segment variables. The same HSAIL module cannot be
 * added to the same HSAIL program more than once. The machine model and
 * profile of the HSAIL module that is being added has to match the machine
 * model and profile of the HSAIL program it is added to. HSAIL modules and
 * their handles can be queried from the program using several query
 * operations.
 *
 * @param[in] program HSAIL program to add HSAIL module to.
 *
 * @param[in] brig_module HSAIL module to add to the HSAIL program.
 *
 * @param[out] module The handle for the @a brig_module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the module is added successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the @a program is not a valid
 * HSAIL program. If the @a brig_module is not a valid HSAIL module. If the
 * machine model and/or profile of the @a brig_module do not match the
 * machine model and/or profile @a program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure to
 * allocate resources required for module addition.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED If the @a brig_module is
 * already added to the HSAIL program. No error is returned, just an info
 * status is used to indicate invalid options.
 */
hsa_status_t HSA_API hsa_ext_add_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_t *brig_module,
  hsa_ext_brig_module_handle_t *module);

/**
 * @brief Finalizes provided HSAIL program.
 *
 * @details Finalizes provided HSAIL program. The HSA runtime finalizer
 * can be used to generate code for kernels and indirect functions from a
 * specific program for a specific HSA component. A kernel can only be
 * finalized once per program per agent. An indirect function can only
 * finalized once per program per agent per call convention. Only code for
 * HSA components specified when the program was created can be requested. The
 * program must contain a definition for the requested kernels and indirect
 * functions amongst the modules that have been added to the program. The
 * modules of the program must collectively define all variables, fbarriers,
 * kernels and functions referenced by operations in the code block. In
 * addition, the caller of this function can specify control directives as
 * an input argument, which will be passed to the finalizer. These control
 * directives can be used for low-level performance tuning, for more
 * information on control directives see HSA Programmer's Reference Manual.
 *
 * @param[in] program HSAIL program to be finalized.
 *
 * @param[in] agent Specific HSA component(s) to finalize program for.
 *
 * @param[in] finalization_request_count The number of kernels and indirect
 * functions that are requested to be finalized.
 *
 * @param[in] finalization_request_list List of kernels and indirect functions
 * that are requested to be finalized.
 *
 * @param[in] control_directives The control directives that can be specified to
 * influence how the finalizer generates code. If NULL then no control
 * directives are used. If this call is successful and control_directives is not
 * NULL, then the resulting hsa_ext_code_descriptor_t object will have control
 * directives which were used by the finalizer.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that control
 * the level of optimization performed by the finalizer. For more information
 * see HSA Programmer's Reference Manual.
 *
 * @param[in] options Implementation defined options that can be passed to
 * the finalizer. For more information HSA Programmer's Reference Manual.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for. 0 - exclude debug information, 1 - include debug
 * information.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the requested list of kernels/functions is finalized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH If the directive in the
 * control directive structure and in the HSAIL kernel mismatch or if the same
 * directive is used with a different value in one of the functions used by
 * this kernel. The @a error_message_callback can be used to get the string
 * representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent is NULL or invalid
 * (if one of the specified HSA components is not the part of the HSAIL
 * program). If the @a program is not a valid HSAIL program. If
 * @a finalization_request_list is NULL or invalid.
 * If @a finalization_request_count is 0. The @a error_message_callback can
 * be used to get the string representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure to
 * allocate resources required for finalization. The @a error_message_callback
 * can be used to get the string representation of the error.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS If the @a options or
 * @a optimization_level are not recognized. No error is returned, just an
 * info status is used to indicate invalid options.
 */
hsa_status_t HSA_API hsa_ext_finalize_program(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  size_t finalization_request_count,
  hsa_ext_finalization_request_t *finalization_request_list,
  hsa_ext_control_directives_t *control_directives,
  hsa_ext_error_message_callback_t error_message_callback,
  uint8_t optimization_level,
  const char *options,
  int debug_information);

/**
 * @brief Queries HSA component's id for specified HSA component contained in
 * specified HSAIL program.
 *
 * @param[in] program HSAIL program to query HSA component's id from.
 *
 * @param[in] agent HSA component for which the id is queried.
 *
 * @param[out] program_agent_id HSA component's id contained in specified
 * HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and HSA component's id is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program or
 * @a agent is not valid. If @a program_agent_id is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_agent_id(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_program_agent_id_t *program_agent_id);

/**
 * @brief Queries the number of HSA components contained in specified
 * HSAIL program.
 *
 * @param[in] program HSAIL program to query number of HSA components from.
 *
 * @param[out] program_agent_count Number of HSA components contained in
 * specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and number of HSA components in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a program_agent_count is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_agent_count(
  hsa_ext_program_handle_t program,
  uint32_t *program_agent_count);

/**
 * @brief Queries specified number of HSA components contained in specified
 * HSAIL program.
 *
 * @param[in] program HSAIL program to query HSA agents from.
 *
 * @param[in] program_agent_count Number of HSA agents to query.
 *
 * @param[out] agents HSA agents contained in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and HSA agents contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_agents(
  hsa_ext_program_handle_t program,
  uint32_t program_agent_count,
  hsa_agent_t *agents);

/**
 * @brief Queries the number of HSAIL modules contained in the HSAIL program.
 *
 * @param[in] program HSAIL program to query number of HSAIL modules from.
 *
 * @param[out] program_module_count Number of HSAIL modules in specified HSAIL
 * program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the number of HSAIL modules contained in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid. If @a program_module_count is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_module_count(
  hsa_ext_program_handle_t program,
  uint32_t *program_module_count);

/**
 * @brief Queries specified number of HSAIL module handles contained in
 * specified HSAIL program.
 *
 * @param[in] program HSAIL program to query HSAIL module handles from.
 *
 * @param[in] program_module_count Number of HSAIL module handles to query.
 *
 * @param[out] modules HSAIL module handles in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and HSAIL module handles contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid. If @a modules is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_modules(
  hsa_ext_program_handle_t program,
  uint32_t program_module_count,
  hsa_ext_brig_module_handle_t *modules);

/**
 * @brief Queries HSAIL module with specified handle that contained in
 * specified HSAIL program.
 *
 * @param[in] program HSAIL program to query HSAIL modules from.
 *
 * @param[in] module HSAIL module handle for which to query the HSAIL module.
 *
 * @param[out] brig_module HSAIL module contained in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and HSAIL module contained in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_brig_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_module_t *brig_module);

/**
 * @brief Queries call convention IDs used for a specified HSA agent of a
 * specified HSAIL program.
 *
 * @param[in] program HSAIL program to query call convention IDs from.
 *
 * @param[in] agent HSA agent to query call convention IDs for.
 *
 * @param[out] first_call_convention_id Set of call convention IDs for
 * specified HSA agent of a specified HSAIL program.
 *
 * @param[out] call_convention_count Number of call convention IDs
 * available for specified HSA agent of a specified program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and call convention IDs contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a agent is not valid or NULL. If @a first_call_convention_id
 * is NULL. If @a call_convention_count is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_call_convention(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_program_call_convention_id32_t *first_call_convention_id,
  uint32_t *call_convention_count);

/**
 * @brief Queries the definition of a module scope variable/fbarrier or
 * kernel/function for a specified HSAIL program.
 *
 * @param[in] program HSAIL program to query symbol definition from.
 *
 * @param[in] module HSAIL module to query symbol definition from.
 *
 * @param[in] symbol Offset to query symbol definition from.
 *
 * @param[out] definition_module Queried HSAIL module handle.
 *
 * @param[out] definition_module_brig Queried HSAIL module.
 *
 * @param[out] definition_symbol Queried symbol.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and symbol definition contained in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid.
 */
hsa_status_t HSA_API hsa_ext_query_symbol_definition(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_brig_module_handle_t *definition_module,
  hsa_ext_brig_module_t *definition_module_brig,
  hsa_ext_brig_code_section_offset32_t *definition_symbol);

/**
 * @brief Defines global variable address in specified HSAIL program. Allows
 * direct access to host variables from HSAIL.
 *
 * @param[in] program HSAIL program to define global variable address for.
 *
 * @param[in] module HSAIL module to define global variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified global variable address is defined in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a module is not valid. If @a address is NULL.
 */
hsa_status_t HSA_API hsa_ext_define_program_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void *address);

/**
 * @brief Queries global variable address from specified HSAIL program. Allows
 * host program to directly access variables.
 *
 * @param[in] program HSAIL program to query global variable address from.
 *
 * @param[in] module HSAIL module to query global variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the global variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Defines global variable address for specified HSA agent in specified
 * HSAIL program. Allows direct access to host variables from HSAIL.
 *
 * @param[in] program HSAIL program to define global variable address for.
 *
 * @param[in] agent HSA agent to define global variable address for.
 *
 * @param[in] module HSAIL module to define global variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified global variable address is defined for specified HSA agent in
 * specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a agent is NULL or not valid If @a module is not valid.
 * If @a address is NULL.
 */
hsa_status_t HSA_API hsa_ext_define_agent_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void *address);

/**
 * @brief Queries global variable address for specified HSA agent from
 * specified HSAIL program. Allows host program to directly access variables.
 *
 * @param[in] program HSAIL program to query global variable address from.
 *
 * @param[in] agent HSA agent to query global variable address from.
 *
 * @param[in] module HSAIL module to query global variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the global variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a agent is NULL or not valid If @a module is not valid.
 * If @a address is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_agent_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Defines readonly variable address for specified HSA agent in
 * specified HSAIL program. Allows direct access to host variables from HSAIL.
 *
 * @param[in] program HSAIL program to define readonly variable address for.
 *
 * @param[in] agent HSA agent to define readonly variable address for.
 *
 * @param[in] module HSAIL module to define readonly variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified readonly variable address is defined for specified HSA agent
 * in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a agent is NULL or not valid If @a module is not valid.
 * If @a address is NULL.
 */
hsa_status_t HSA_API hsa_ext_define_readonly_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void* address);

/**
 * @brief Queries readonly variable address for specified HSA agent from
 * specified HSAIL program. Allows host program to directly access variables.
 *
 * @param[in] program HSAIL program to query readonly variable address from.
 *
 * @param[in] agent HSA agent to query readonly variable address from.
 *
 * @param[in] module HSAIL module to query readonly variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the readonly variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * not valid. If @a agent is NULL or not valid If @a module is not valid.
 * If @a address is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_readonly_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Queries kernel descriptor address from specified HSAIL program.
 * Needed to create a dispatch packet.
 *
 * @param[in] program HSAIL program to query kernel descriptor address from.
 *
 * @param[in] module HSAIL module to query kernel descriptor address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] kernel_descriptor The address of the kernel descriptor for the
 * requested kernel, which is an array of ::hsa_ext_code_descriptor_t
 * indexed by ::hsa_ext_program_agent_id_t.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the kernel descriptor address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_kernel_descriptor_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_code_descriptor_t** kernel_descriptor);

/**
 * @brief Queries indirect function descriptor address from specified HSAIL
 * program. Allows host program to perform indirect function table variable
 * initialization.
 *
 * @param[in] program HSAIL program to query indirect function descriptor
 * address from.
 *
 * @param[in] module HSAIL module to query indirect function descriptor address
 * from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] indirect_function_descriptor The address of the indirect
 * function descriptor for the requested indirect function, which is an array
 * of ::hsa_ext_code_descriptor_t indexed by
 * ::hsa_ext_program_call_convention_id32_t.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and the indirect function descriptor address is queried from specified HSAIL
 * program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_indirect_function_descriptor_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_code_descriptor_t** indirect_function_descriptor);

/**
 * @brief Validates HSAIL program with specified HSAIL program handle. Checks
 * if all declarations and definitions match, if there is at most one
 * definition. The caller decides when to call validation routines. For
 * example, it can be done in the debug mode.
 *
 * @param[in] program HSAIL program to validate.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified HSAIL program is a valid program.
 *
 * @retval ::HSA_STATUS_ERROR If specified HSAIL program is not valid, refer to
 * the error call back function to get string representation of the failure.
 */
hsa_status_t HSA_API hsa_ext_validate_program(
  hsa_ext_program_handle_t program,
  hsa_ext_error_message_callback_t error_message_callback);

/**
 * @brief Validates specified HSAIL module with specified HSAIL module handle
 * for specified HSAIL program. Checks if BRIG for specified module is legal:
 * operation operand type rules, etc. For more information about BRIG see HSA
 * Programmer's Reference Manual. The caller decides when to call validation
 * routines. For example, it can be done in the debug mode.
 *
 * @param[in] program HSAIL program to validate HSAIL module in.
 *
 * @param[in] module HSAIL module handle to validate.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified HSAIL module is a valid module..
 *
 * @retval ::HSA_STATUS_ERROR If the module is not valid, refer to the error
 * call back function to get string representation of the failure.
 */
hsa_status_t HSA_API hsa_ext_validate_program_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_error_message_callback_t error_message_callback);

/**
 * @brief Serializes specified HSAIL program. Used for offline compilation.
 *
 * @param[in] caller Opaque pointer to the caller of this function.
 *
 * @param[in] program HSAIL program to be serialized.
 *
 * @param[in] alloc_serialize_data_callback Call back function for memory
 * allocation.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message (if any).
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization. 0 - exclude debug information, 1
 * - include debug information.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and specified program is serialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program is not a valid
 * program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure to
 * allocate resources required for serialization. The @a error_message_callback
 * can be used to get the string representation of the error.
 */
hsa_status_t HSA_API hsa_ext_serialize_program(
  hsa_runtime_caller_t caller,
  hsa_ext_program_handle_t program,
  hsa_runtime_alloc_data_callback_t alloc_serialize_data_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  void *serialized_object);

/**
 * @brief Call back function to get program's address of global segment
 * variables, kernel table variable, indirect function table variable based
 * on the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_program_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Call back function to get agents's address of global segment
 * variables, kernel table variable, indirect function table variable based
 * on the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_agent_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  hsa_agent_t agent,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Deserializes the HSAIL program from a given serialized object. Used
 * for offline compilation. Includes call back functions, where call back
 * functions take symbolic name, this allows symbols defined by application
 * to be relocated.
 *
 * @param[in] caller Opaque pointer to the caller of this function.
 *
 * @param[in] serialized_object Serialized HSAIL program.
 *
 * @param[in] program_allocation_symbol_address Call back function to get
 * program's address of global segment variables, kernel table variable,
 * indirect function table variable based on the symbolic name. Allows symbols
 * defined by application to be relocated.
 *
 * @param[in] agent_allocation_symbol_address Call back function to get agent's
 * address of global segment variables, kernel table variable, indirect
 * function table variable based on the symbolic name. Allows symbols defined
 * by application to be relocated.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] program Deserialized HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully,
 * and HSAIL program is deserialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a serialized_object is
 * either NULL, or is not valid, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure to
 * allocate resources required for deserialization. The
 * @a error_message_callback can be used to get the string representation
 * of the error.
 */
hsa_status_t HSA_API hsa_ext_deserialize_program(
  hsa_runtime_caller_t caller,
  void *serialized_object,
  hsa_ext_program_allocation_symbol_address_t program_allocation_symbol_address,
  hsa_ext_agent_allocation_symbol_address_t agent_allocation_symbol_address,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  hsa_ext_program_handle_t **program);
/** @} */


/** \defgroup images Images API
 *  @{
 */

/**
 * @brief Image handle, populated by ::hsa_ext_image_create_handle. Images
 * handles are only unique within an agent, not across agents.
 *
 */
typedef struct hsa_ext_image_handle_s {
   /**
    * HSA component specific handle to the image.
    */
    uint64_t handle;

} hsa_ext_image_handle_t;

/**
 * @brief Image format capability returned by ::hsa_ext_image_get_format_capability
 */
typedef enum  {
   /**
    * Images of this format are not supported.
    */
    HSA_EXT_IMAGE_FORMAT_NOT_SUPPORTED = 0x0,

   /**
    * Images of this format can be accessed for read operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_ONLY = 0x1,

   /**
    * Images of this format can be accessed for write operations.
    */
    HSA_EXT_IMAGE_FORMAT_WRITE_ONLY = 0x2,

    /**
    * Images of this format can be accessed for read and write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_WRITE = 0x4,

    /**
    * Images of this format can be accessed for read-modify-write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this format are guaranteed to have consistent data layout
    * regardless of the how it is accessed by the HSA agent.
    */
    HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA = 0x10

} hsa_ext_image_format_capability_t;

/**
 * @brief Agent-specific image size and alignment requirements. This structure
 * stores the agent-dependent image data sizes and alignment, and populated by
 * ::hsa_ext_image_get_info.
 */
typedef struct hsa_ext_image_info_s {
  /**
   * Component specific image data size in bytes.
   */
  size_t image_size;

  /**
   * Component specific image data alignment in bytes.
   */
  size_t image_alignment;

} hsa_ext_image_info_t;

/**
 * @brief Defines how the HSA device expects to access the image. The access
 * pattern used by the HSA agent specified in ::hsa_ext_image_create_handle.
 *
 */
typedef enum {
  /**
   * Image handle is to be used by the HSA agent as read-only using an HSAIL
   * roimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_ONLY,

  /**
   * Image handle is to be used by the HSA agent as write-only using an HSAIL
   * woimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_WRITE_ONLY,

  /**
   * Image handle is to be used by the HSA agent as read and/or write using an
   * HSAIL rwimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_WRITE

} hsa_ext_image_access_permission_t;

/**
 * @brief Geometry associated with the HSA image (image dimensions allowed in
 * HSA). The enumeration values match the HSAIL BRIG type BrigImageGeometry.
 */
typedef enum {
/**
   * One-dimensional image addressed by width coordinate.
   */
  HSA_EXT_IMAGE_GEOMETRY_1D = 0,

  /**
   * Two-dimensional image addressed by width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2D = 1,

  /**
   * Three-dimensional image addressed by width, height, and depth coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_3D = 2,

  /**
   * Array of one-dimensional images with the same size and format. 1D arrays
   * are addressed by index and width coordinate.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DA = 3,

  /**
   * Array of two-dimensional images with the same size and format. 2D arrays
   * are addressed by index and width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DA = 4,

  /**
   * One-dimensional image interpreted as a buffer with specific restrictions.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DB = 5,

  /**
   * Two-dimensional depth image addressed by width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DDEPTH = 6,

  /**
   * Array of two-dimensional depth images with the same size and format.  2D
   * arrays are addressed by index and width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DADEPTH = 7
} hsa_ext_image_geometry_t;

/**
* @brief Component type associated with the image. See Image section in HSA
* Programming Reference Manual for definitions on each component type. The
* enumeration values match the HSAIL BRIG type BrigImageChannelType.
*/
typedef enum
{
    HSA_EXT_IMAGE_CHANNEL_TYPE_SNORM_INT8 = 0,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SNORM_INT16 = 1,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT8 = 2,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT16 = 3,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT24 = 4,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_555 = 5,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_565 = 6,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_101010 = 7,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT8 = 8,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT16 = 9,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT32 = 10,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT8 = 11,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT16 = 12,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT32 = 13,
    HSA_EXT_IMAGE_CHANNEL_TYPE_HALF_FLOAT = 14,
    HSA_EXT_IMAGE_CHANNEL_TYPE_FLOAT = 15

} hsa_ext_image_channel_type_t;

/**
 *
* @brief Image component order associated with the image. See Image section in
* HSA Programming Reference Manual for definitions on each component order. The
* enumeration values match the HSAIL BRIG type BrigImageChannelOrder.
*
*/
typedef enum {

    HSA_EXT_IMAGE_CHANNEL_ORDER_A = 0,
    HSA_EXT_IMAGE_CHANNEL_ORDER_R = 1,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RX = 2,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RG = 3,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGX = 4,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RA = 5,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGB = 6,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGBX = 7,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGBA = 8,
    HSA_EXT_IMAGE_CHANNEL_ORDER_BGRA = 9,
    HSA_EXT_IMAGE_CHANNEL_ORDER_ARGB = 10,
    HSA_EXT_IMAGE_CHANNEL_ORDER_ABGR = 11,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGB = 12,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGBX = 13,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGBA = 14,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SBGRA = 15,
    HSA_EXT_IMAGE_CHANNEL_ORDER_INTENSITY = 16,
    HSA_EXT_IMAGE_CHANNEL_ORDER_LUMINANCE = 17,
    HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH = 18,
    HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH_STENCIL = 19

} hsa_ext_image_channel_order_t;

/**
 * @brief Image format descriptor (attributes of the image format).
 */
typedef struct hsa_ext_image_format_s {
/**
    * Channel type of the image.
    */
    hsa_ext_image_channel_type_t channel_type;

   /**
    * Channel order of the image.
    */
    hsa_ext_image_channel_order_t channel_order;
} hsa_ext_image_format_t;

/**
 * @brief Implementation-independent HSA Image descriptor.
 */
typedef struct hsa_ext_image_descriptor_s {
  /**
   * Geometry of the image.
   */
  hsa_ext_image_geometry_t geometry;

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
  hsa_ext_image_format_t format;


} hsa_ext_image_descriptor_t;

/**
 * @brief Three-dimensional image range description.
 */
typedef struct hsa_ext_image_range_s {
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

} hsa_ext_image_range_t;

/**
 * @brief Image region description. Used by image operations such as import,
 * export, copy, and clear.
 */
typedef struct hsa_ext_image_region_s {
   /**
    * Offset in the image (in coordinates).
    */
    hsa_dim3_t image_offset;

   /**
    * Dimensions of the image range (in coordinates).
    */
    hsa_ext_image_range_t image_range;

} hsa_ext_image_region_t;

/**
 * @brief Sampler handle. Samplers are populated by
 * ::hsa_ext_sampler_create_handle. Sampler handles are only unique within an
 * agent, not across agents.
 */
typedef struct hsa_ext_sampler_handle_s {
   /**
    * Component-specific HSA sampler.
    */
    uint64_t handle;

} hsa_ext_sampler_handle_t;

/**
 * @brief Sampler address modes. The sampler address mode describes the
 * processing of out-of-range image coordinates. The values match the HSAIL BRIG
 * type BrigSamplerAddressing.
 */
typedef enum {

  /**
   * Out-of-range coordinates are not handled.
   */
  HSA_EXT_SAMPLER_ADDRESSING_UNDEFINED = 0,

  /**
   * Clamp out-of-range coordinates to the image edge.
   */
  HSA_EXT_SAMPLER_ADDRESSING_CLAMP_TO_EDGE = 1,

  /**
   * Clamp out-of-range coordinates to the image border.
   */
  HSA_EXT_SAMPLER_ADDRESSING_CLAMP_TO_BORDER = 2,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_REPEAT = 3,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MIRRORED_REPEAT = 4

} hsa_ext_sampler_addressing_mode_t;

/**
 * @brief Sampler coordinate modes. The enumeration values match the HSAIL BRIG
 * BRIG_SAMPLER_COORD bit in the type BrigSamplerModifier.
 */
typedef enum {
  /**
   * Coordinates are all in the range of 0.0 to 1.0.
   */
  HSA_EXT_SAMPLER_COORD_NORMALIZED = 0,

  /**
   * Coordinates are all in the range of 0 to (dimension-1).
   */
  HSA_EXT_SAMPLER_COORD_UNNORMALIZED = 1

} hsa_ext_sampler_coordinate_mode_t;

/**
 * @brief Sampler filter modes. The enumeration values match the HSAIL BRIG type
 * BrigSamplerFilter.
 */
typedef enum {
  /**
   * Filter to the image element nearest (in Manhattan distance) to the
   * specified coordinate.
   */
  HSA_EXT_SAMPLER_FILTER_NEAREST = 0,

  /**
   * Filter to the image element calculated by combining the elements in a 2x2
   * square block or 2x2x2 cube block around the specified coordinate. The
   * elements are combined using linear interpolation.
   */
  HSA_EXT_SAMPLER_FILTER_LINEAR = 1

} hsa_ext_sampler_filter_mode_t;

/**
 * @brief Implementation-independent sampler descriptor.
 */
typedef struct hsa_ext_sampler_descriptor_s {
  /**
   * Sampler coordinate mode describes the normalization of image coordinates.
   */
  hsa_ext_sampler_coordinate_mode_t coordinate_mode;

  /**
   * Sampler filter type describes the type of sampling performed.
   */
  hsa_ext_sampler_filter_mode_t filter_mode;

  /**
   * Sampler address mode describes the processing of out-of-range image
   * coordinates.
   */
  hsa_ext_sampler_addressing_mode_t address_mode;

} hsa_ext_sampler_descriptor_t;

/**
 * @brief Retrieve image format capabilities for the specified image format on
 * the specified HSA component.
 *
 * @details If successful, the queried image format's capabilities bit-mask is
 * written to the location specified by @a capability_mask. See
 * ::hsa_ext_image_format_capability_t to determine all possible capabilities that
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a image_format, or
 *       @a capability_mask are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_format_capability(hsa_agent_t agent,
                         const hsa_ext_image_format_t *image_format,
                         hsa_ext_image_geometry_t image_geometry,
                         uint32_t *capability_mask);
/**
 *
 * @brief Inquires the required HSA component-specific image data details from a
 * implementation independent image descriptor.
 *
 * @details If successful, the queried HSA agent-specific image data info is
 * written to the location specified by @a image_info. Based on the
 * implementation the optimal image data size and alignment requirements could
 * vary depending on the image attributes specified in @a image_descriptor.
 *
 * The implementation must return the same image info requirements for different
 * access permissions with exactly the same image descriptor as long as
 * ::hsa_ext_image_get_format_capability reports
 * ::HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA for the image format
 * specified in the image descriptor.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[in] access_permission Access permission of the image by the HSA agent.
 *
 * @param[out] image_info Image info size and alignment requirements that the
 * HSA agent requires.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED If the HSA agent does
 * not support the image format specified by the descriptor.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED If the HSA agent does
 * not support the image dimensions specified by the format descriptor.
 */
hsa_status_t HSA_API hsa_ext_image_get_info(
                         hsa_agent_t agent,
                         const hsa_ext_image_descriptor_t *image_descriptor,
                         hsa_ext_image_access_permission_t access_permission,
                         hsa_ext_image_info_t *image_info);

/**
 * @brief Creates a agent-defined image handle from an
 * implementation-independent image descriptor and a agent-specific image
 * data. The image access defines how the HSA agent expects to use the image and
 * must match the HSAIL image handle type used by the agent.
 *
 * @details If successful, the image handle is written to the location specified
 * by @a image_handle. The image data memory must be allocated using the
 * previously queried ::hsa_ext_image_get_info memory requirements with the same
 * HSA agent and implementation-independent image descriptor.
 *
 * The image data is not initialized and any previous memory contents is
 * preserved. The memory management of image data is the application's
 * responsibility and can only be freed until the memory is no longer needed and
 * any image handles using it are destroyed.
 *
 * @a access_permission defines how the HSA agent expects to use the image
 * handle. The image format specified in the image descriptor must be capable by
 * the HSA agent for the intended permission.
 *
 * Image handles with different permissions can be created using the same image
 * data with exactly the same image descriptor as long as
 * ::HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA is reported by
 * ::hsa_ext_image_get_format_capability for the image format specified in the
 * image descriptor. Images of non-linear s-form channel order can share the
 * same image data with its equivalent linear non-s form channel order, provided
 * the rest of the image descriptor parameters are identical.
 *
 * If necessary, an application can use image operations (import, export, copy,
 * clear) to prepare the image for the intended use regardless of the access
 * permissions.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[in] image_data Address of the component-specific image data.
 *
 * @param[in] access_permission Access permission of the image by the HSA agent.
 *
 * @param[out] image_handle Agent-specific image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED If the HSA agent does
 * not have the capability to support the image format using the specified @a
 * agent_access.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 *
 */
hsa_status_t HSA_API hsa_ext_image_create_handle(
                         hsa_agent_t agent,
                         const hsa_ext_image_descriptor_t *image_descriptor,
                         const void *image_data,
                         hsa_ext_image_access_permission_t access_permission,
                         hsa_ext_image_handle_t *image_handle);

/**
 * @brief Imports a linearly organized image data from memory directly to an
 * image handle.
 *
 * @details This operation updates the image data referenced by the image handle
 * from the source memory. The size of the data imported from memory is
 * implicitly derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
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
 * ::hsa_ext_image_create_handle image handle can overlap.  Overlapping of any
 * of the source and destination memory within the import operation produces
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
 * @param[in] dst_image_handle Destination Image handle.
 *
 * @param[in] image_region Image region to be updated.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a src_memory or @a
 * image_region are NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_image_import (
                         hsa_agent_t agent,
                         const void *src_memory,
                         size_t src_row_pitch,
                         size_t src_slice_pitch,
                         hsa_ext_image_handle_t dst_image_handle,
                         const hsa_ext_image_region_t *image_region,
                         const hsa_signal_t *completion_signal);

/**
 * @brief Export image data from the image handle directly to memory organized
 * linearly.
 *
 * @details The operation updates the destination memory with the image data in
 * the image handle. The size of the data exported to memory is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
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
 * ::hsa_ext_image_create_handle image handle can overlap. Overlapping of any of
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a dst_memory or @a
 * image_region are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_export(
                          hsa_agent_t agent,
                          hsa_ext_image_handle_t src_image_handle,
                          void *dst_memory,
                          size_t dst_row_pitch,
                          size_t dst_slice_pitch,
                          const hsa_ext_image_region_t *image_region,
                          const hsa_signal_t *completion_signal);

/**
 * @brief Copies a region from one image to another.
 *
 * @details The operation copies the image data from the source image handle to
 * the destination image handle. The size of the image data copied is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * The source and destination handles must have been previously created using
 * ::hsa_ext_image_create_handle. The source and destination image data memory
 * are not allowed to be the same. Overlapping any of the source and destination
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_region
 * are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_copy(
                       hsa_agent_t agent,
                       hsa_ext_image_handle_t src_image_handle,
                       hsa_ext_image_handle_t dst_image_handle,
                       const hsa_ext_image_region_t *image_region,
                       const hsa_signal_t *completion_signal);

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
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a
 * image_region are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_clear(
                        hsa_agent_t agent,
                        hsa_ext_image_handle_t image_handle,
                        const float data[4],
                        const hsa_ext_image_region_t *image_region,
                        const hsa_signal_t *completion_signal);

/**
 * @brief Destroys the specified image handle.
 *
 * @details If successful, the image handle previously created using
 *  ::hsa_ext_image_create_handle is destroyed.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_handle is
 * NULL.
 */
hsa_status_t HSA_API hsa_ext_image_destroy_handle (
                        hsa_agent_t agent,
                        hsa_ext_image_handle_t *image_handle);

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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 */
hsa_status_t HSA_API hsa_ext_sampler_create_handle(
                     hsa_agent_t agent,
                     const hsa_ext_sampler_descriptor_t *sampler_descriptor,
                     hsa_ext_sampler_handle_t *sampler_handle);

/**
 * @brief Destroys the specified sampler handle.
 *
 * @details If successful, the sampler handle previously created using
 * ::hsa_ext_sampler_create_handle is destroyed.
 *
 * The sampler handle should not be destroyed while there are references to it
 * queued for execution or currently being used in a dispatch.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_handle Sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 */
hsa_status_t HSA_API hsa_ext_sampler_destroy_handle(
                     hsa_agent_t agent,
                     hsa_ext_sampler_handle_t *sampler_handle);
/** @} */


/** \defgroup extensions TODO
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a extension is not a valid
 * value for a single vendor extension or @a result is NULL.
 */
hsa_status_t HSA_API hsa_vendor_extension_query(hsa_extension_t extension,
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a extension is not a valid
 * value for a HSA extension or @a result is NULL.
 */
  hsa_status_t HSA_API hsa_extension_query(hsa_extension_t extension,
                                           int* result);
/** @} */


#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif
