/**
 * Copyright (c) 2014, HSA Foundation, Inc
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimers.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimers in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   * Neither the names of the HSA Team, HSA Foundation,  University of
 *     Illinois at Urbana-Champaign, nor the names of its contributors may be
 *     used to endorse or promote products derived from this Software without
 *     specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH
 * THE SOFTWARE.
 *
 */
#ifndef HSA_H
#define HSA_H

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uintXX_t */


#define HSA_VERSION_1_0                              1

/* placeholder for calling convention */
#define HSA_API

#define HSA_LITTLE_ENDIAN

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** \defgroup status Runtime Notifications
 *  @{
 */

/**
 * @brief Status codes.
 */
typedef enum {
    /**
     * The function has been executed successfully.
     */
    HSA_STATUS_SUCCESS = 0x0,
    /**
     * A traversal over a list of elements has been interrupted by the
     * application before completing.
     */
    HSA_STATUS_INFO_BREAK = 0x1,
    /**
     * A generic error has occurred.
     */
    HSA_STATUS_ERROR = 0x1000,
    /**
     * One of the actual arguments does not meet a precondition stated in the
     * documentation of the corresponding formal argument.
     */
    HSA_STATUS_ERROR_INVALID_ARGUMENT = 0x1001,
    /**
     * The requested queue creation is not valid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE_CREATION = 0x1002,
    /**
     * The requested allocation is not valid.
     */
    HSA_STATUS_ERROR_INVALID_ALLOCATION = 0x1003,
    /**
     * The HSA agent is invalid.
     */
    HSA_STATUS_ERROR_INVALID_AGENT = 0x1004,
    /**
     * The memory region is invalid.
     */
    HSA_STATUS_ERROR_INVALID_REGION = 0x1005,
    /**
     * The signal is invalid.
     */
    HSA_STATUS_ERROR_INVALID_SIGNAL = 0x1006,
    /**
     * The queue is invalid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE = 0x1007,
    /**
     * The HSA runtime failed to allocate the necessary resources. This error
     * may also occur when the HSA runtime needs to spawn threads or create
     * internal OS-specific events.
     */
    HSA_STATUS_ERROR_OUT_OF_RESOURCES = 0x1008,
    /**
     * The AQL packet is malformed.
     */
    HSA_STATUS_ERROR_INVALID_PACKET_FORMAT = 0x1009,
    /**
     * An error has been detected while releasing a resource.
     */
    HSA_STATUS_ERROR_RESOURCE_FREE = 0x100A,
    /**
     * An API other than ::hsa_init has been invoked while the reference count
     * of the HSA runtime is 0.
     */
    HSA_STATUS_ERROR_NOT_INITIALIZED = 0x100B,
    /**
     * The maximum reference count for the object has been reached.
     */
    HSA_STATUS_ERROR_REFCOUNT_OVERFLOW = 0x100C,
    /**
     * The arguments passed to a functions are not compatible.
     */
    HSA_STATUS_ERROR_INCOMPATIBLE_ARGUMENTS = 0x100D,
    /**
     * The index is invalid.
     */
    HSA_STATUS_ERROR_INVALID_INDEX = 0x100E
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p status is an invalid
 * status code, or @p status_string is NULL.
 */
hsa_status_t HSA_API hsa_status_string(
    hsa_status_t status,
    const char ** status_string);

/** @} */

/** \defgroup common Common Definitions
 *  @{
 */

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
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_REFCOUNT_OVERFLOW The HSA runtime reference
 * count reaches INT32_MAX.
 */
hsa_status_t HSA_API hsa_init();

/**
 * @brief Shut down the HSA runtime.
 *
 * @details Decreases the reference count of the HSA runtime instance. When the
 * reference count reaches 0, the HSA runtime is no longer considered valid
 * but the application might call ::hsa_init to initialize the HSA runtime
 * again.
 *
 * Once the reference count of the HSA runtime reaches 0, all the resources
 * associated with it (queues, signals, HSA agent information, etc.) are
 * considered invalid and any attempt to reference them in subsequent API calls
 * results in undefined behavior. When the reference count reaches 0, the HSA
 * runtime may release resources associated with it.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_shut_down();

/** @} **/

/** \defgroup agentinfo System and HSA Agent Information
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
  HSA_SYSTEM_INFO_VERSION_MAJOR = 0,
  /**
   * Minor version of the HSA runtime specification supported by the
   * implementation. The type of this attribute is uint16_t.
   */
  HSA_SYSTEM_INFO_VERSION_MINOR = 1,
  /**
   * Current timestamp. The value of this attribute monotonically increases at a
   * constant rate. The type of this attribute is uint64_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP = 2,
  /**
   * Timestamp value increase rate, in Hz. The timestamp (clock) frequency is
   * in the range 1-400MHz. The type of this attribute is uint64_t.
   */
  HSA_SYSTEM_INFO_TIMESTAMP_FREQUENCY = 3,
  /**
   * Maximum duration of a signal wait operation. Expressed as a count based on
   * the timestamp frequency. The type of this attribute is uint64_t.
   */
  HSA_SYSTEM_INFO_SIGNAL_MAX_WAIT = 4
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid
 * system attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_system_get_info(
    hsa_system_info_t attribute,
    void* value);

/**
 * @brief Opaque handle representing an HSA agent, a device that participates in the
 * HSA memory model. An HSA agent can submit AQL packets for execution, and may
 * also accept AQL packets for execution (Agent Dispatch packets or Kernel Dispatch
 * packets launching HSAIL-derived binaries).
 */
typedef struct hsa_agent_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_agent_t;

/**
 * @brief HSA agent features.
 */
typedef enum {
    /**
     * The HSA agent supports AQL packets of Kernel Dispatch type. If this
     * feature is enabled, the HSA agent is also an HSA component.
     */
    HSA_AGENT_FEATURE_KERNEL_DISPATCH = 1,
    /**
     * The HSA agent supports AQL packets of Agent Dispatch type.
     */
    HSA_AGENT_FEATURE_AGENT_DISPATCH = 2
} hsa_agent_feature_t;

/**
 * @brief Profile. A profile indicates a particular level of feature
 * support. For example, in the base profile the application must use the HSA
 * runtime allocator to reserve Shared Virtual Memory, while in the full profile
 * any host pointer can be shared across all the HSA agents.
 */
typedef enum {
    /**
     * Base profile.
     */
    HSA_PROFILE_BASE = 0,
    /**
     * Full profile.
     */
    HSA_PROFILE_FULL = 1
} hsa_profile_t;

/**
 * @brief Machine model. A machine model determines the size of certain data
 * types in a HSA agent.
 */
typedef enum {
    /**
     * Small machine model. Addresses use 32 bits.
     */
    HSA_MACHINE_MODEL_SMALL = 0,
    /**
     * Large machine model. Addresses use 64 bits.
     */
    HSA_MACHINE_MODEL_LARGE = 1
} hsa_machine_model_t;

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
 * @brief HSA agent attributes.
 */
typedef enum {
  /**
   * HSA agent name. The type of this attribute is a NUL-terminated char[64]. If
   * the name of the agent uses less than 63 characters, the rest of the
   * array must be filled with NULs.
   */
  HSA_AGENT_INFO_NAME = 0,
  /**
   * Name of vendor. The type of this attribute is a NUL-terminated char[64]. If
   * the name of the vendor uses less than 63 characters, the rest of the array
   * must be filled with NULs.
   */
  HSA_AGENT_INFO_VENDOR_NAME = 1,
  /**
   * HSA agent capability. The type of this attribute is ::hsa_agent_feature_t.
   */
  HSA_AGENT_INFO_FEATURE = 2,
  /**
   * Machine model supported by the HSA agent. The type of this attribute is
   * ::hsa_machine_model_t.
   */
  HSA_AGENT_INFO_MACHINE_MODEL = 3,
  /**
   * Profile supported by the HSA agent. The type of this attribute is
   * ::hsa_profile_t.
   */
  HSA_AGENT_INFO_PROFILE = 4,
  /**
   * Number of work-items in a wavefront. Must be a power of 2 in the range
   * [1,64]. The value of this attribute is undefined if the HSA agent is not
   * an HSA component. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_WAVEFRONT_SIZE = 5,
  /**
   * Maximum number of work-items of each dimension of a work-group.  Each
   * maximum must be greater than 0. No maximum can exceed the value of
   * ::HSA_AGENT_INFO_WORKGROUP_MAX_SIZE. The value of this attribute is
   * undefined if the HSA agent is not an HSA component. The type of this
   * attribute is uint16_t[3].
   */
  HSA_AGENT_INFO_WORKGROUP_MAX_DIM = 6,
  /**
   * Maximum total number of work-items in a work-group. The value of this
   * attribute is undefined if the HSA agent is not an HSA component. The type
   * of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_WORKGROUP_MAX_SIZE = 7,
  /**
   * Maximum number of work-items of each dimension of a grid. Each maximum must
   * be greater than 0, and must not be smaller than the corresponding value in
   * ::HSA_AGENT_INFO_WORKGROUP_MAX_DIM. No maximum can exceed the value of
   * ::HSA_AGENT_INFO_GRID_MAX_SIZE. The value of this attribute is undefined if
   * the HSA agent is not an HSA component. The type of this attribute is
   * ::hsa_dim3_t.
   */
  HSA_AGENT_INFO_GRID_MAX_DIM = 8,
  /**
   * Maximum total number of work-items in a grid. The value of this attribute
   * is undefined if the HSA agent is not an HSA component. The type of this
   * attribute is uint32_t.
   */
  HSA_AGENT_INFO_GRID_MAX_SIZE = 9,
  /**
   * Maximum number of fbarriers per work-group. Must be at least 32. The value
   * of this attribute is undefined if the HSA agent is not an HSA
   * component. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_FBARRIER_MAX_SIZE = 10,
  /**
   * Maximum number of queues that can be active (created but not destroyed) at
   * one time in the HSA agent. The type of this attribute is uint32_t.
   */
  HSA_AGENT_INFO_QUEUES_MAX = 11,
  /**
   * Minimum number of packets that a queue created in the HSA agent
   * can hold. Must be a power of 2 greater than 0. Must not exceed
   * the value of ::HSA_AGENT_INFO_QUEUE_MAX_SIZE. The type of this
   * attribute is uint32_t.
   */
  HSA_AGENT_INFO_QUEUE_MIN_SIZE = 12,
  /**
   * Maximum number of packets that a queue created in the HSA agent can
   * hold. Must be a power of 2 greater than 0. The type of this attribute
   * is uint32_t.
   */
  HSA_AGENT_INFO_QUEUE_MAX_SIZE = 13,
  /**
   * Type of a queue created in the HSA agent. The type of this attribute is
   * ::hsa_queue_type_t.
   */
  HSA_AGENT_INFO_QUEUE_TYPE = 14,
  /**
   * Identifier of the NUMA node associated with the HSA agent. The type of this
   * attribute is uint32_t.
   */
  HSA_AGENT_INFO_NODE = 15,
  /**
   * Type of hardware device associated with the HSA agent. The type of this
   * attribute is ::hsa_device_type_t.
   */
  HSA_AGENT_INFO_DEVICE = 16,
  /**
   * Array of data cache sizes (L1..L4). Each size is expressed in bytes. A size
   * of 0 for a particular level indicates that there is no cache information
   * for that level. The type of this attribute is uint32_t[4].
   */
  HSA_AGENT_INFO_CACHE_SIZE = 17
} hsa_agent_info_t;

/**
 * @brief Get the current value of an attribute for a given HSA agent.
 *
 * @param[in] agent A valid HSA agent.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of @p attribute, the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The HSA agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid HSA
 * agent attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_agent_get_info(
    hsa_agent_t agent,
    hsa_agent_info_t attribute,
    void* value);

/**
 * @brief Iterate over the available HSA agents, and invoke an
 * application-defined callback on every iteration.
 *
 * @param[in] callback Callback to be invoked once per HSA agent. The HSA
 * runtime passes two arguments to the callback, the HSA agent and the
 * application data.  If @p callback returns a status other than
 * ::HSA_STATUS_SUCCESS for a particular iteration, the traversal stops and
 * ::hsa_iterate_agents returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
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
 * @brief Signal handle.
 */
typedef struct hsa_signal_s {
  /**
   * Opaque handle. The value 0 is reserved.
   */
  uint64_t handle;
} hsa_signal_t;

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
 * @param[in] num_consumers Size of @p consumers. A value of 0 indicates that
 * any HSA agent might wait on the signal.
 *
 * @param[in] consumers List of HSA agents that might consume (wait on) the
 * signal. If @p num_consumers is 0, this argument is ignored; otherwise, the
 * HSA runtime might use the list to optimize the handling of the signal
 * object. If an HSA agent not listed in @p consumers waits on the returned
 * signal, the behavior is undefined. The memory associated with @p consumers
 * can be reused or freed after the function returns.
 *
 * @param[out] signal Pointer to a memory location where the HSA runtime will
 * store the newly created signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate the
 * resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p signal is NULL, @p
 * num_consumers is greater than 0 but @p consumers is NULL, or @p consumers
 * contains duplicates.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL @p signal is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT The handle in @p signal is 0.
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
 * @details If the value of the signal is changed, all the HSA agents waiting
 * on @p signal for which @p value satisfies their wait condition are awakened.
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
 * @details If the value of the signal is changed, all the HSA agents waiting
 * on @p signal for which @p value satisfies their wait condition are awakened.
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
 * @details If the value of the signal is changed, all the HSA agents waiting
 * on @p signal for which @p value satisfies their wait condition are awakened.
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
 * @details If the value of the signal is changed, all the HSA agents waiting on
 * @p signal for which @p value satisfies their wait condition are awakened.
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
 * @details If the value of the signal is changed, all the HSA agents waiting on
 * @p signal for which @p value satisfies their wait condition are awakened.
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
 * @brief Atomically perform a bitwise AND operation between the value of a
 * signal and a given value.
 *
 * @details If the value of the signal is changed, all the HSA agents waiting on
 * @p signal for which @p value satisfies their wait condition are awakened.
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
 * @brief Atomically perform a bitwise OR operation between the value of a
 * signal and a given value.
 *
 * @details If the value of the signal is changed, all the HSA agents waiting on
 * @p signal for which @p value satisfies their wait condition are awakened.
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
 * @brief Atomically perform a bitwise XOR operation between the value of a
 * signal and a given value.
 *
 * @details If the value of the signal is changed, all the HSA agents waiting on
 * @p signal for which @p value satisfies their wait condition are awakened.
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
    HSA_EQ = 0,
    /**
     * The two operands are not equal.
     */
    HSA_NE = 1,
    /**
     * The first operand is less than the second operand.
     */
    HSA_LT = 2,
    /**
     * The first operand is greater than or equal to the second operand.
     */
    HSA_GTE = 3
} hsa_signal_condition_t;

/**
 * @brief Expected duration of a wait call.
 */
typedef enum {
    /**
     * The signal value is expected to meet the specified condition in a short
     * period of time.
     */
    HSA_WAIT_EXPECTANCY_SHORT = 0,
    /**
     * The signal value is expected to meet the specified condition after a
     * longer period of time.
     */
    HSA_WAIT_EXPECTANCY_LONG = 1,
    /**
     * The expected duration of the wait call is not known.
     */
    HSA_WAIT_EXPECTANCY_UNKNOWN = 2
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
   * Queue supports Kernel Dispatch packets.
   */
  HSA_QUEUE_FEATURE_KERNEL_DISPATCH = 1,

  /**
   * Queue supports Agent Dispatch packets.
   */
  HSA_QUEUE_FEATURE_AGENT_DISPATCH = 2
} hsa_queue_feature_t;

/**
 * @brief User mode queue.
 *
 * @details Queues are read-only, but HSA agents can directly modify the
 * contents of the buffer pointed by @a base_address, or use HSA runtime APIs to
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

#ifdef HSA_LARGE_MODEL
  void* base_address;
#elif defined HSA_LITTLE_ENDIAN
  /**
   * Starting address of the HSA runtime-allocated buffer used to store the AQL
   * packets. Must be aligned to the size of an AQL packet.
   */
  void* base_address;
  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved0;
#else
  uint32_t reserved0;
  void* base_address;
#endif

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
   * Queue identifier, which is unique over the lifetime of the application.
   */
  uint64_t id;

#ifdef HSA_LARGE_MODEL
  struct hsa_queue_s* service_queue;
#elif defined HSA_LITTLE_ENDIAN
  /**
   * A pointer to another user mode queue that can be used by an HSAIL kernel
   * to request application-defined or vendor-specific services.
   */
  struct hsa_queue_s* service_queue;
  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;
#else
  uint32_t reserved1;
  struct hsa_queue_s* service_queue;
#endif

} hsa_queue_t;

/**
 * @brief Create a user mode queue.
 *
 * @details When a queue is created, the HSA runtime creates the packet buffer,
 * the completion signal, and the write and read indexes. The initial value of
 * the write and read indexes is 0. The type of every packet in the buffer is
 * initialized to ::HSA_PACKET_TYPE_ALWAYS_RESERVED.
 *
 * The application should only rely on the error code returned to determine if
 * the queue is valid.
 *
 * @param[in] agent HSA agent where to create the queue.
 *
 * @param[in] size Number of packets the queue is expected to
 * hold. Must be a power of 2 between 1 and the value of
 * ::HSA_AGENT_INFO_QUEUE_MAX_SIZE in @p agent. The size of the newly
 * created queue is the maximum of @p size and the value of
 * ::HSA_AGENT_INFO_QUEUE_MIN_SIZE in @p agent.
 *
 * @param[in] type Type of the queue. If the value of
 * ::HSA_AGENT_INFO_QUEUE_TYPE in @p agent is ::HSA_QUEUE_TYPE_SINGLE, then @p
 * type must also be ::HSA_QUEUE_TYPE_SINGLE.
 *
 * @param[in] callback Callback invoked by the HSA runtime for every
 * asynchronous event related to the newly created queue. May be NULL. The HSA
 * runtime passes two arguments to the callback: a code identifying the event
 * that triggered the invocation, and a pointer to the queue where the event
 * originated.
 *
 * @param[in] service_queue Pointer to a service queue to be associated with the
 * newly created queue. May be NULL. If not NULL, the queue pointed by @p
 * service_queue must support the ::HSA_QUEUE_FEATURE_AGENT_DISPATCH feature.
 *
 * @param[in] private_segment_size Hint indicating the maximum
 * expected private segment usage per work-item, in bytes. There may
 * be performance degradation if the application places a Kernel
 * Dispatch packet in the queue and the corresponding private segment
 * usage exceeds @p private_segment_size. If the application does not
 * want to specify any particular value for this argument, @p
 * private_segment_size must be UINT32_MAX. If the queue does not
 * support Kernel Dispatch packets, this argument is ignored.
 *
 * @param[in] group_segment_size Hint indicating the maximum expected
 * group segment usage per work-group, in bytes. There may be
 * performance degradation if the application places a Kernel Dispatch
 * packet in the queue and the corresponding group segment usage
 * exceeds @p group_segment_size. If the application does not want to
 * specify any particular value for this argument, @p
 * group_segment_size must be UINT32_MAX. If the queue does not
 * support Kernel Dispatch packets, this argument is ignored.
 *
 * @param[out] queue Memory location where the HSA runtime stores a pointer to
 * the newly created queue.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The HSA agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE_CREATION @p agent does not
 * support queues of the given type.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p size is not a power of two,
 * @p size is 0, @p type is an invalid queue type, or @p queue is NULL.
 *
 */
hsa_status_t HSA_API hsa_queue_create(
    hsa_agent_t agent,
    uint32_t size,
    hsa_queue_type_t type,
    void (*callback)(hsa_status_t status, hsa_queue_t *source),
    const hsa_queue_t* service_queue,
    uint32_t private_segment_size,
    uint32_t group_segment_size,
    hsa_queue_t **queue);

/**
 * @brief Destroy a user mode queue.
 *
 * @details When a queue is destroyed, the state of the AQL packets that have
 * not been yet fully processed (their completion phase has not finished)
 * becomes undefined. It is the responsibility of the application to ensure that
 * all pending queue operations are finished if their results are required.
 *
 * The resources allocated by the HSA runtime during queue creation (queue
 * structure, ring buffer, doorbell signal) are released.  The queue should not
 * be accessed after being destroyed.
 *
 * @param[in] queue Pointer to a queue created using ::hsa_queue_create.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE The queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_destroy(
    hsa_queue_t *queue);

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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE The queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p queue is NULL.
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
    const hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_read_index_acquire
 */
uint64_t HSA_API hsa_queue_load_read_index_relaxed(
    const hsa_queue_t *queue);

/**
 * @brief Atomically load the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Write index of the queue pointed by @p queue.
 */
uint64_t HSA_API hsa_queue_load_write_index_acquire(
    const hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_write_index_acquire
 */
uint64_t HSA_API hsa_queue_load_write_index_relaxed(
    const hsa_queue_t *queue);

/**
 * @brief Atomically set the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the write index.
 *
 */
void HSA_API hsa_queue_store_write_index_relaxed(
    const hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_write_index_relaxed
 */
void HSA_API hsa_queue_store_write_index_release(
    const hsa_queue_t *queue,
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
    const hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_acquire(
    const hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_relaxed(
    const hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_cas_write_index_release(
    const hsa_queue_t *queue,
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
    const hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_acquire(
    const hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_relaxed(
    const hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_acq_rel
 */
uint64_t HSA_API hsa_queue_add_write_index_release(
    const hsa_queue_t *queue,
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
    const hsa_queue_t *queue,
    uint64_t value);

/**
 * @copydoc hsa_queue_store_read_index_relaxed
 */
void HSA_API hsa_queue_store_read_index_release(
   const hsa_queue_t *queue,
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
  HSA_PACKET_TYPE_KERNEL_DISPATCH = 2,
  /**
   * Packet used by HSA agents to delay processing of subsequent packets, and to
   * express complex dependencies between multiple packets. All queues support
   * this packet type.
   */
  HSA_PACKET_TYPE_BARRIER_AND = 3,
  /**
   * Packet used by HSA agents for dispatching jobs to HSA agents.  Not all
   * queues support packets of this type (see ::hsa_queue_feature_t).
   */
  HSA_PACKET_TYPE_AGENT_DISPATCH = 4,
  /**
   * Packet used by HSA agents to delay processing of subsequent packets, and to
   * express complex dependencies between multiple packets. All queues support
   * this packet type.
   */
  HSA_PACKET_TYPE_BARRIER_OR = 5
} hsa_packet_type_t;

/**
 * @brief Scope of the memory fence operation associated with a packet.
 */
typedef enum {
  /**
   * No scope. Must only be used as the acquire fence scope of a Barrier-AND or
   * Barrier-OR packet.
   */
  HSA_FENCE_SCOPE_NONE = 0,
  /**
   * The fence is applied with HSA component scope for the global segment.
   */
  HSA_FENCE_SCOPE_COMPONENT = 1,
  /**
   * The fence is applied with system scope for the global segment.
   */
  HSA_FENCE_SCOPE_SYSTEM = 2
} hsa_fence_scope_t;

/**
 * @brief Sub-fields of the @a header field that is present in any AQL
 * packet. The offset (with respect to the address of @a header) of a sub-field
 * is identical to its enumeration constant. The width of each sub-field is
 * determined by the corresponding value in ::hsa_packet_header_width_t. The
 * offset and the width are expressed in bits.
 */
 typedef enum {
  /**
   * Packet type. The value of this sub-field must be one of
   * ::hsa_packet_type_t.
   */
   HSA_PACKET_HEADER_TYPE = 0,
  /**
   * Barrier bit. If the barrier bit is set, the processing of the current
   * packet only launches when all preceding packets (within the same queue) are
   * complete.
   */
   HSA_PACKET_HEADER_BARRIER = 8,
  /**
   * Acquire fence scope. The value of this sub-field determines the scope and
   * type of the memory fence operation applied before the packet enters the
   * active phase. Must be ::HSA_FENCE_SCOPE_NONE for Barrier-AND and Barrier-OR
   * packets. For all the other packets, the value of this sub-field must be one
   * of ::hsa_fence_scope_t.
   */
   HSA_PACKET_HEADER_ACQUIRE_FENCE_SCOPE = 9,
  /**
   * Release fence scope, The value of this sub-field determines the scope and
   * type of the memory fence operation applied after kernel completion but
   * before the packet is completed. The value of this sub-field must be one of
   * ::hsa_fence_scope_t.
   */
   HSA_PACKET_HEADER_RELEASE_FENCE_SCOPE = 11
 } hsa_packet_header_t;

/**
 * @brief Width (in bits) of the sub-fields in ::hsa_packet_header_t.
 */
 typedef enum {
   HSA_PACKET_HEADER_WIDTH_TYPE = 8,
   HSA_PACKET_HEADER_WIDTH_BARRIER = 1,
   HSA_PACKET_HEADER_WIDTH_ACQUIRE_FENCE_SCOPE = 2,
   HSA_PACKET_HEADER_WIDTH_RELEASE_FENCE_SCOPE = 2
 } hsa_packet_header_width_t;

/**
 * @brief Sub-fields of the Kernel Dispatch packet @a setup field. The offset
 * (with respect to the address of @a setup) of a sub-field is identical to its
 * enumeration constant. The width of each sub-field is determined by the
 * corresponding value in ::hsa_kernel_dispatch_packet_setup_width_t. The
 * offset and the width are expressed in bits.
 */
 typedef enum {
  /**
   * Number of dimensions of the grid. Valid values are 1, 2, or 3.
   *
   */
   HSA_KERNEL_DISPATCH_PACKET_SETUP_DIMENSIONS = 0
 } hsa_kernel_dispatch_packet_setup_t;

/**
 * @brief Width (in bits) of the sub-fields in
 * ::hsa_kernel_dispatch_packet_setup_t.
 */
 typedef enum {
   HSA_KERNEL_DISPATCH_PACKET_SETUP_WIDTH_DIMENSIONS = 2
 } hsa_kernel_dispatch_packet_setup_width_t;

/**
 * @brief AQL Kernel Dispatch packet
 */
typedef struct hsa_kernel_dispatch_packet_s {
  /**
   * Packet header. Used to configure multiple packet parameters such as the
   * packet type. The parameters are described by ::hsa_packet_header_t.
   */
  uint16_t header;

  /**
   * Dispatch setup parameters. Used to configure kernel dispatch parameters
   * such as the number of dimensions in the grid. The parameters are described
   * by ::hsa_kernel_dispatch_packet_setup_t.
   */
  uint16_t setup;

  /**
   * X dimension of work-group, in work-items. Must be greater than 0.
   */
  uint16_t workgroup_size_x;

  /**
   * Y dimension of work-group, in work-items. Must be greater than
   * 0. If the grid has 1 dimension, the only valid value is 1.
   */
  uint16_t workgroup_size_y;

  /**
   * Z dimension of work-group, in work-items. Must be greater than
   * 0. If the grid has 1 or 2 dimensions, the only valid value is 1.
   */
  uint16_t workgroup_size_z;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved0;

  /**
   * X dimension of grid, in work-items. Must be greater than 0. Must
   * not be smaller than @a workgroup_size_x.
   */
  uint32_t grid_size_x;

  /**
   * Y dimension of grid, in work-items. Must be greater than 0. If the grid has
   * 1 dimension, the only valid value is 1. Must not be smaller than @a
   * workgroup_size_y.
   */
  uint32_t grid_size_y;

  /**
   * Z dimension of grid, in work-items. Must be greater than 0. If the grid has
   * 1 or 2 dimensions, the only valid value is 1. Must not be smaller than @a
   * workgroup_size_z.
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
   * Opaque handle to a code object that includes an implementation-defined
   * executable code for the kernel.
   */
  uint64_t kernel_object;

#ifdef HSA_LARGE_MODEL
  void* kernarg_address;
#elif defined HSA_LITTLE_ENDIAN
  /**
   * Pointer to a buffer containing the kernel arguments. May be NULL.
   *
   * The buffer must be allocated using ::hsa_memory_allocate, and must not be
   * modified once the Kernel Dispatch packet is enqueued until the dispatch has
   * completed execution.
   */
  void* kernarg_address;
  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;
#else
  uint32_t reserved1;
  void* kernarg_address;
#endif

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * Signal used to indicate completion of the job. The application can use the
   * special signal handle 0 to indicate that no signal is used.
   */
  hsa_signal_t completion_signal;

} hsa_kernel_dispatch_packet_t;

/**
 * @brief Intervals allowed for Agent Dispatch services.
 */
typedef enum {
  /**
   * Smallest type ID for a function that is specific to a single vendor.
   */
  HSA_AGENT_DISPATCH_TYPE_VENDOR_START = 0x0000,
  /**
   * Largest type ID for a function that is specific to a single vendor.
   */
  HSA_AGENT_DISPATCH_TYPE_VENDOR_END = 0x7FFF,
  /**
   * Smallest type ID for a function that is defined by an application.
   */
  HSA_AGENT_DISPATCH_TYPE_APPLICATION_START = 0x8000,
  /**
   * Largest type ID for a function that is defined by an application.
   */
  HSA_AGENT_DISPATCH_TYPE_APPLICATION_END = 0xFFFF
} hsa_agent_dispatch_type_limits_t;

/**
 * @brief Agent Dispatch packet.
 */
typedef struct hsa_agent_dispatch_packet_s {
  /**
   * Packet header. Used to configure multiple packet parameters such as the
   * packet type. The parameters are described by ::hsa_packet_header_t.
   */
  uint16_t header;

  /**
   * The function to be performed by the destination HSA agent. The limits in
   * ::hsa_agent_dispatch_type_limits_t determine whether the type corresponds
   * to an application-defined or a vendor-specific function.
   */
  uint16_t type;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved0;

#ifdef HSA_LARGE_MODEL
  void* return_address;
#elif defined HSA_LITTLE_ENDIAN
  /**
   * Address where to store the function return values, if any.
   */
  void* return_address;
  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;
#else
  uint32_t reserved1;
  void* return_address;
#endif

  /**
   * Function arguments.
   */
  uint64_t arg[4];

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * Signal used to indicate completion of the job. The application can use the
   * special signal handle 0 to indicate that no signal is used.
   */
  hsa_signal_t completion_signal;

} hsa_agent_dispatch_packet_t;

/**
 * @brief Barrier-AND packet.
 */
typedef struct hsa_barrier_and_packet_s {
  /**
   * Packet header. Used to configure multiple packet parameters such as the
   * packet type. The parameters are described by ::hsa_packet_header_t.
   */
  uint16_t header;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved0;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;

  /**
   * Array of dependent signal objects. Signals with a handle value of 0 are
   * allowed and are interpreted by the packet processor as satisfied
   * dependencies.
   */
  hsa_signal_t dep_signal[5];

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * Signal used to indicate completion of the job. The application can use the
   * special signal handle 0 to indicate that no signal is used.
   */
  hsa_signal_t completion_signal;

} hsa_barrier_and_packet_t;

/**
 * @brief Barrier-OR packet.
 */
typedef struct hsa_barrier_or_packet_s {
  /**
   * Packet header. Used to configure multiple packet parameters such as the
   * packet type. The parameters are described by ::hsa_packet_header_t.
   */
  uint16_t header;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved0;

  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;

  /**
   * Array of dependent signal objects. Signals with a handle value of 0 are
   * allowed and are interpreted by the packet processor as dependencies not
   * satisfied.
   */
  hsa_signal_t dep_signal[5];

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * Signal used to indicate completion of the job. The application can use the
   * special signal handle 0 to indicate that no signal is used.
   */
  hsa_signal_t completion_signal;

} hsa_barrier_or_packet_t;

/** @} */

/** \defgroup memory Memory
 *  @{
 */

/**
 * @brief A memory region represents a block of contiguous memory that is
 * directly accessible by an HSA agent, and exposes properties about the block of
 * memory and how it is accessed from that particular HSA agent.
 */
typedef struct hsa_region_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_region_t;

/**
 * @brief Types of memory segments.
 */
typedef enum {
  /**
   * Global segment. Used to hold data that is shared by all HSA agents.
   */
  HSA_SEGMENT_GLOBAL = 0,
  /**
   * Private segment. Used to hold data that is local to a single work-item.
   */
  HSA_SEGMENT_PRIVATE = 1,
  /**
   * Group segment. Used to hold data that is shared by the work-items of a
   * work-group.
  */
  HSA_SEGMENT_GROUP = 2,
  /**
   * Kernarg segment. Used to pass arguments into a kernel. Memory in this
   * segment is visible to all work-items of the kernel dispatch with which it
   * is associated.
   */
  HSA_SEGMENT_KERNARG = 3,
  /**
   * Read-only segment. Used to hold data that remains constant during the
   * execution of a kernel dispatch.
   */
  HSA_SEGMENT_READONLY = 4,
  /**
   * Spill segment. Used to load or store register spills.
   */
  HSA_SEGMENT_SPILL = 5,
  /**
   * Arg segment. Used to pass arguments into and out of functions.
   */
  HSA_SEGMENT_ARG = 6
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
   * region's HSA agent. If the HSA agent does not have an L1 cache (as reported
   * by the attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L1 = 2,
  /**
   * Accesses to data in the region are cached in the L2 data cache of the
   * region's HSA agent. If the HSA agent does not have an L2 cache (as reported
   * by the attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L2 = 4,
  /**
   * Accesses to data in the region are cached in the L3 data cache of the
   * region's HSA agent. If the HSA agent does not have an L3 cache (as reported
   * by the attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
   */
  HSA_REGION_FLAG_CACHED_L3 = 8,
  /**
   * Accesses to data in the region are cached in the L4 data cache of the
   * region's HSA agent. If the HSA agent does not have an L4 cache (as reported
   * by the attribute ::HSA_AGENT_INFO_CACHE_SIZE), this flag must not be set.
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
  HSA_REGION_INFO_BASE = 0,
  /**
   * Size, in bytes. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_SIZE = 1,
  /**
   * HSA agent associated with this region. The type of this attribute is
   * hsa_agent_t.
   */
  HSA_REGION_INFO_AGENT = 2,
  /**
   * Flag mask.  The type of this attribute is uint32_t, a bit-field of
   * ::hsa_region_flag_t values.
   */
  HSA_REGION_INFO_FLAGS = 3,
  /**
   * Segment where memory in the region can be used. The type of this
   * attribute is ::hsa_segment_t.
   */
  HSA_REGION_INFO_SEGMENT = 4,
  /**
   * Maximum allocation size in this region, in bytes. A value of 0 indicates
   * that the host cannot allocate memory in the region using
   * ::hsa_memory_allocate. If the value of ::HSA_REGION_INFO_SEGMENT is other
   * than ::HSA_SEGMENT_GLOBAL, the maximum allocation size must be 0. The type
   * of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_MAX_SIZE = 5,
  /**
   * Allocation granularity of buffers allocated by ::hsa_memory_allocate in
   * this region. The size of a buffer allocated in this region is a multiple of
   * the value of this attribute. If ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the
   * allocation granularity must be 0. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_GRANULE = 6,
  /**
   * Alignment of buffers allocated by ::hsa_memory_allocate in this region. If
   * ::HSA_REGION_INFO_ALLOC_MAX_SIZE is 0, the alignment must be 0. Otherwise,
   * it must be a power of 2. The type of this attribute is size_t.
   */
  HSA_REGION_INFO_ALLOC_ALIGNMENT = 7,
  /**
   * Peak bandwidth, in MB/s. The type of this attribute is uint32_t.
   */
  HSA_REGION_INFO_BANDWIDTH = 8,
  /**
   * NUMA node associated with this region. The type of this attribute is
   * uint32_t.
   */
  HSA_REGION_INFO_NODE = 9
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_REGION The region is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid
 * region attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_region_get_info(
    hsa_region_t region,
    hsa_region_info_t attribute,
    void* value);

/**
 * @brief Iterate over the memory regions associated with a given HSA agent, and
 * invoke an application-defined callback on every iteration.
 *
 * @param[in] agent A valid HSA agent.
 *
 * @param[in] callback Callback to be invoked once per region that is directly
 * accessible from the HSA agent.  The HSA runtime passes two arguments to the
 * callback, the region and the application data.  If @p callback returns a
 * status other than ::HSA_STATUS_SUCCESS for a particular iteration, the
 * traversal stops and ::hsa_agent_iterate_regions returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The HSA agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES No memory is available.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_REGION The region is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ALLOCATION The host is not allowed to
 * allocate memory in @p region, or @p size is greater than the value of
 * HSA_REGION_INFO_ALLOC_MAX_SIZE in @p region.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p ptr is NULL.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 */
hsa_status_t HSA_API hsa_memory_free(void* ptr);

/**
 * @brief Copy block of memory.
 *
 * @details Copying a number of bytes larger than the size of the memory regions
 * pointed by @p dst or @p src results in undefined behavior.
 *
 * @param[out] dst A valid pointer to the destination array where the content is
 * to be copied.
 *
 * @param[in] src A valid pointer to the source of data to be copied.
 *
 * @param[in] size Number of bytes to copy. If @p size is 0, no copy is
 * performed and the function returns success.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT The source or destination
 * pointers are NULL.
 */
hsa_status_t HSA_API hsa_memory_copy(
    void *dst,
    const void *src,
    size_t size);

/**
 *
 * @brief Register memory.
 *
 * @details Registering a buffer serves as an indication to the HSA runtime that
 * the passed buffer might be accessed from an HSA component other than the
 * host. If any HSA component with no Full profile support accesses a host
 * buffer, the application must register the buffer beforehand. If all the HSA
 * components accessing the host buffer support the Full profile, registration
 * is a performance hint that allows the HSA runtime implementation to know
 * which buffers will be accessed by some of the HSA components ahead of time.
 *
 * Registrations should not overlap.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * registered. If a NULL pointer is passed, no operation is performed.
 *
 * @param[in] size Requested registration size in bytes. A size of 0 is
 * only allowed if @p address is NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure in
 * allocating the necessary resources.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p size is 0 but @p address
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p extension is an invalid
 * value for a single vendor extension, or @p result is NULL.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p extension is an invalid value
 * for an HSA extension, or @p result is NULL.
 */
hsa_status_t HSA_API hsa_extension_query(
    hsa_extension_t extension,
    int* result);
/** @} */


#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif
