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
#include "hsa.h"
#include <stdbool.h> /* bool */


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


typedef uint64_t BrigModule;
typedef uint32_t BrigCodeOffset32_t;

/** \defgroup ext-finalizer-brig-adapters BRIG naming adapters
 *  @{
 */

/**
 * @brief Temporary type renaming to match HSA runtime code conventions.
 */
typedef BrigModule hsa_ext_brig_module_t;

/**
 * @brief Temporary type renaming to match HSA runtime code conventions.
 */
typedef BrigCodeOffset32_t hsa_ext_brig_code_offset32_t;

/** @} */


/** \defgroup ext-finalizer-extensions Finalization Extensions
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 */
typedef enum {
    /**
     * Mismatch between a directive in the control directive structure and in
     * the HSAIL kernel.
     */
    HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH = 0x2000,
    /**
     * The HSAIL program is invalid.
     */
    HSA_EXT_STATUS_ERROR_INVALID_PROGRAM = 0x2001,
    /**
     * The HSAIL program module is invalid.
     */
    HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE = 0x2002,
    /**
     * The HSAIL module is invalid.
     */
    HSA_EXT_STATUS_ERROR_INVALID_MODULE = 0x2003,
    /**
     * The HSAIL module is already part of the HSAIL program.
     */
    HSA_EXT_STATUS_ERROR_MODULE_ALREADY_INCLUDED = 0x2004,
    /**
     * There is no symbol at the specified offset.
     */
    HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET = 0x2005,
    /**
     * There is no variable with the given name.
     */
    HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_NAME = 0x2006,
    /**
     * The finalization request is invalid.
     */
    HSA_EXT_STATUS_ERROR_INVALID_FINALIZATION_REQUEST = 0x2007,
    /**
     * The finalization encountered an error while finalizing a kernel or
     * indirect function.
     */
    HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED = 0x2008,
    /**
     * The variable is already defined.
     */
    HSA_EXT_STATUS_ERROR_VARIABLE_ALREADY_DEFINED = 0x2009,
    /**
     * The variable is undefined.
     */
    HSA_EXT_STATUS_ERROR_VARIABLE_UNDEFINED = 0x200A
} hsa_ext_fnz_status_t;

/**
 * @brief Enumeration constants added to ::hsa_agent_info_t by this extension.
 */
typedef enum {
    /**
     * Number of call conventions supported by the HSA agent. The type of this
     * attribute is uint32_t.
     */
    HSA_EXT_AGENT_INFO_NUM_CALL_CONVENTIONS = 0x2000,
    /**
     * Flag indicating that the f16 HSAIL operation is at least as fast as the
     * f32 operation in the current HSA agent. The value of this attribute is
     * undefined if the HSA agent is not an HSA component. The type of this
     * attribute is bool.
     */
    HSA_EXT_AGENT_INFO_FAST_F16_OPERATION = 0x2001
} hsa_ext_fnz_agent_info_t;


/** @} */


/** \defgroup ext-finalizer-program HSAIL Finalization
 *  @{
 */

/**
 * @brief An opaque handle to an HSAIL program. An application can use the HSA
 * runtime to create an HSAIL program, and add HSAIL modules to it. An HSAIL
 * program manages the linking of symbol declarations to their definitions
 * across HSAIL modules, performs allocation of global variables defined by the
 * HSAIL modules and manages the finalized machine code ensuring it is
 * executable on the corresponding HSA agents.
 */
typedef struct hsa_ext_program_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_ext_program_t;

/**
 * @brief Create an HSAIL program.
 *
 * @param[in] num_agents Size of @p agents. Must be greater than 0.
 *
 * @param[in] agents List of HSA agents to associate with the HSAIL program. An
 * HSA agent can only appear in the list at most once. Must not be NULL. The
 * memory associated with @p agents can be reused or freed after the function
 * returns. The HSA runtime can store the list of HSA agents in any order, so
 * the application must not assume that the index of an HSA agent in @p program
 * (see ::hsa_ext_program_get_agent) is identical to its index in @p agents.
 *
 * @param[in] machine_model Machine model used in the HSAIL program.
 *
 * @param[in] profile Profile type used in the HSAIL program.
 *
 * @param[in] options Vendor-specific options. May be NULL.
 *
 * @param[out] program Memory location where the HSA runtime stores the newly
 * created HSAIL program handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INCOMPATIBLE_ARGUMENTS An HSA agent in @p agents
 * does not support the specified profile or machine model.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * the resources required for HSAIL program creation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_agents is 0, @p agents is
 * NULL or contains duplicates, @p machine_model is invalid, @p profile is
 * invalid, or @p program is NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_program_create(
    uint32_t num_agents,
    const hsa_agent_t *agents,
    hsa_machine_model_t machine_model,
    hsa_profile_t profile,
    const char *options,
    hsa_ext_program_t *program);

/**
 * @brief Destroy an HSAIL program. The resources associated with the HSAIL
 * program (such as kernel and indirect code descriptors, and finalized machine
 * mode) are also released.
 *
 * @param[in] program HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 */
hsa_status_t HSA_API hsa_ext_program_destroy(
    hsa_ext_program_t program);

/**
 * @brief HSAIL program attributes.
 */
typedef enum {
  /**
   * Machine model specified when the HSAIL program was created. The type of
   * this attribute is ::hsa_machine_model_t.
   */
  HSA_EXT_PROGRAM_INFO_MACHINE_MODEL = 1,
  /**
   * Profile specified when the HSAIL program was created. The type of this
   * attribute is ::hsa_profile_t.
   */
  HSA_EXT_PROGRAM_INFO_PROFILE = 2
} hsa_ext_program_info_t;

/**
 * @brief Get the current value of an attribute for a given HSAIL program.
 *
 * @param[in] program HSAIL program.
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
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid HSAIL
 * program attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_get_info(
    hsa_ext_program_t program,
    hsa_ext_program_info_t attribute,
    void* value);

/**
 * @brief Get the number of HSA agents associated with an HSAIL program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[out] num_agents Memory location where the HSA runtime stores the number
 * of HSA agents in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_agents is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_get_num_agents(
    hsa_ext_program_t program,
    uint32_t* num_agents);

/**
 * @brief Retrieve an HSA agent associated with an HSAIL program. The index of
 * an HSA agent in a program is identical to the value returned by the HSAIL
 * instruction agentid_u32 in that program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] index Position of the HSA agent within the HSAIL program. The
 * index of an HSA agent remains constant during the lifetime of the HSAIL
 * program.
 *
 * @param[out] agent Memory location where the HSA runtime stores the agent at
 * index @p index in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX The specified index is too large.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_get_agent(
    hsa_ext_program_t program,
    uint32_t index,
    hsa_agent_t* agent);

/**
 * @brief Validate an HSAIL program by checking that all the declarations and
 * definitions for the same symbol are compatible, all HSAIL modules specify a
 * machine model and profile that matches the HSAIL program, and that the HSAIL
 * modules added to the program (see ::hsa_ext_program_add_module) are legal.
 *
 * @param[in] program HSAIL program.
 *
 * @param[out] result Memory location where the HSA runtime stores the
 * validation result. If the HSAIL program is valid, the result is 0.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_validate(
    hsa_ext_program_t program,
    uint32_t* result);

/** @} */

/** \defgroup ext-finalizer-module HSAIL program modules
 *  @{
 */

/**
 * @brief Opaque handle an HSAIL program module. An HSAIL module can be
 * associated with multiple HSAIL program modules, but an HSAIL program module
 * is associated with at most one HSAIL module. An HSAIL program module may not
 * be associated with an HSAIL module if it is in deserialized state, or if it
 * is the application program module. An HSAIL program module is associated with
 * a single HSAIL program.
 */
typedef struct hsa_ext_program_module_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_ext_program_module_t;

/**
 *  @brief Add an HSAIL module to an existing HSAIL program, and create an HSAIL
 *  program module that uniquely identifies the HSAIL module within the HSAIL
 *  program.
 *
 *  @details The HSA runtime does not perform a deep copy of the HSAIL module
 *  upon addition. Instead, it stores a pointer to the HSAIL module. The
 *  ownership of the HSAIL module belongs to the application, which must ensure
 *  that @p brig_module is not deallocated before finalizing any of its
 *  associated kernels of indirect functions.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] brig_module Pointer to an HSAIL module. Must not be NULL. The
 * application can add the same HSAIL module to @p program at most once. The
 * HSAIL module must specify the same machine model and profile as @p program.
 *
 * @param[out] program_module Memory location where the HSA runtime stores the
 * newly created HSAIL program module handle. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE The HSAIL module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_MODULE_ALREADY_INCLUDED The HSAIL module is
 * already part of the HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p brig_module is NULL, or @p
 * program_module is NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_program_add_module(
    hsa_ext_program_t program,
    const hsa_ext_brig_module_t *brig_module,
    hsa_ext_program_module_t *program_module);

/**
 * @brief Get the number of HSAIL program modules in a given HSAIL program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[out] num_program_modules Memory location where the HSA runtime stores
 * the number of HSAIL program modules. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_program_modules is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_get_num_program_modules(
    hsa_ext_program_t program,
    uint32_t* num_program_modules);

/**
 * @brief Get the HSAIL program module at a specific index within an HSAIL
 * program. The application HSAIL program module is always present at index 0.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] index Position of the HSAIL program module to be retrieved. The
 * index remains constant during the lifetime of the HSAIL program.
 *
 * @param[out] program_module Memory location where the HSA runtime stores the
 * HSAIL program module at index @p index in the given HSAIL program. Must not
 * be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX The specified index is too large.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program_module is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_get_program_module(
    hsa_ext_program_t program,
    uint32_t index,
    hsa_ext_program_module_t* program_module);

/**
 * @brief HSAIL program module attributes.
 */
typedef enum {
  /**
   * HSAIL program associated with the HSAIL program module. The type of this
   * attribute is ::hsa_ext_program_t.
   */
  HSA_EXT_PROGRAM_MODULE_INFO_PROGRAM = 1,
  /**
   * HSAIL module associated with the HSAIL program module. The type of this
   * attribute is *::hsa_ext_brig_module_t. A value of NULL is returned if the
   * HSAIL program module does not have BRIG associated with it (for example, it
   * denotes a symbol defined by the application, or it is a deserialized HSAIL
   * program module with no HSAIL module reattached to it).
   */
  HSA_EXT_PROGRAM_MODULE_INFO_BRIG_MODULE = 2
} hsa_ext_program_module_info_t;

/**
 * @brief Get the current value of an attribute for a given HSAIL program
 * module.
 *
 * @param[in] program_module HSAIL program module.
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store
 * the value of the attribute. If the buffer passed by the application is not
 * large enough to hold the value of @p attribute, the behavior is
 * undefined. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p program_module is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid HSAIL
 * program module attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_module_get_info(
    hsa_ext_program_module_t program_module,
    hsa_ext_program_module_info_t attribute,
    void* value);

/** @} */

/** \defgroup ext-finalizer-program-queries TODO
 *  @{
 */

/**
 * @brief Query the HSAIL program module and offset where a symbol (for example,
 * a variable, a kernel or an indirect function) is defined. The module and
 * offset passed by an application must refer to the variable declaration or
 * definition. If the module and offset passed by the application already refer
 * to a definition, the function returns the same module and offset values.
 *
 * @param[in] program_module HSAIL program module.
 *
 * @param[in] offset Symbol offset.
 *
 * @param[out] definition_program_module Memory location where the HSA runtime stores the
 * HSAIL program module where the symbol is defined.
 *
 * @param[out] definition_offset Memory location where the HSA runtime stores the
 * code offset at which the symbol is defined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p program_module
 * is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to a declaration or a definition.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p definition_program_module is NULL, or
 * @p definition_offset is NULL.
 */
hsa_status_t HSA_API hsa_ext_symbol_get_definition(
    hsa_ext_program_module_t program_module,
    hsa_ext_brig_code_offset32_t offset,
    hsa_ext_program_module_t *definition_program_module,
    hsa_ext_brig_code_offset32_t *definition_offset);

/**
 * @brief Query the address of a variable in the global segment. The variable
 * must be declared with program allocation.
 *
 * @param[in] definition_program_module HSAIL program module where the variable is
 * defined.
 *
 * @param[in] definition_offset Code offset corresponding to the variable
 * definition.
 *
 * @param[out] address Memory location where the HSA runtime stores the address of
 * the variable.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p definition_program_module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to an global variable definition.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_UNDEFINED The specified variable is
 * not defined.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_ext_global_variable_get_address(
    hsa_ext_program_module_t definition_program_module,
    hsa_ext_brig_code_offset32_t definition_offset,
    void** address);

/**
 * @brief Query the address of a variable in the global segment for a given HSA
 * agent. The variable must be declared with agent allocation.
 *
 * @param[in] definition_program_module HSAIL program module where the variable is
 * defined.
 *
 * @param[in] definition_offset Code offset corresponding to the variable
 * definition.
 *
 * @param[in] agent HSA agent.
 *
 * @param[out] address Memory location where the HSA runtime stores the address of
 * the variable. The returned pointer is not guaranteed to be directly
 * accessible by any HSA agent other than @p agent.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p definition_program_module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to the definition of a global variable with agent
 * allocation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_UNDEFINED The specified variable is
 * not defined for the given HSA agent.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_ext_global_variable_get_agent_address(
    hsa_ext_program_module_t definition_program_module,
    hsa_ext_brig_code_offset32_t definition_offset,
    hsa_agent_t agent,
    void** address);

/**
 * @brief Query the address of a variable in the readonly segment for a given
 * HSA agent.
 *
 * @param[in] definition_program_module HSAIL program module where the variable is
 * defined.
 *
 * @param[in] definition_offset Code offset corresponding to the variable
 * definition.
 *
 * @param[in] agent HSA agent.
 *
 * @param[out] address Memory location where the HSA runtime stores the address of
 * the variable. The returned pointer is not guaranteed to be directly
 * accessible by any HSA agent other than @p agent.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p definition_program_module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to a readonly variable definition.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_UNDEFINED The specified variable is
 * not defined for the given HSA agent.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_ext_readonly_variable_get_address(
    hsa_ext_program_module_t definition_program_module,
    hsa_ext_brig_code_offset32_t definition_offset,
    hsa_agent_t agent,
    void** address);

/** @} */

/** \defgroup ext-finalizer-finalization HSAIL Finalization
 *  @{
 */

/**
 * @brief Control directives specify low-level information about the
 * finalization process.
 */
typedef struct hsa_ext_control_directives_s {
  /**
   * Bitset indicating which control directives are enabled. The bit assigned to
   * a control directive is determined by the corresponding value in
   * BrigControlDirective.
   *
   * If a control directive is disabled, its corresponding field value (if any)
   * must be 0. Control directives that are only present or absent (such as
   * partial workgroups) have no corresponding field as the presence of the bit
   * in this mask is sufficient.
   */
   uint64_t control_directives_mask;

  /**
   * Bitset of HSAIL exceptions that must have the BREAK policy enabled. The bit
   * assigned to an HSAIL exception is determined by the corresponding value
   * in BrigExceptionsMask. If the kernel contains a enablebreakexceptions
   * control directive, the finalizer uses the union of the two masks.
   */
  uint16_t break_exceptions_mask;

  /**
   * Bitset of HSAIL exceptions that must have the DETECT policy enabled. The
   * bit assigned to an HSAIL exception is determined by the corresponding value
   * in BrigExceptionsMask. If the kernel contains a enabledetectexceptions
   * control directive, the finalizer uses the union of the two masks.
   */
  uint16_t detect_exceptions_mask;

  /**
   * Maximum size (in bytes) of dynamic group memory that will be allocated by
   * the application for any dispatch of the kernel.  If the kernel contains a
   * maxdynamicsize control directive, the two values should match.
   */
    uint32_t max_dynamic_group_size;

  /**
   * Maximum number of grid work-items that will be used by the application to
   * launch the kernel. If the kernel contains a maxflatgridsize control
   * directive, the value of @a max_flat_grid_size must not be greater than the
   * value of the directive, and takes precedence.
   *
   * The value specified for maximum absolute grid size must be greater than or
   * equal to the product of the values specified by @a required_grid_size.
   *
   * If the bit at position BRIG_CONTROL_MAXFLATGRIDSIZE is set in @a
   * control_directives_mask, this field must be greater than 0.
   */
  uint64_t max_flat_grid_size;

  /**
   * Maximum number of work-group work-items that will be used by the
   * application to launch the kernel. If the kernel contains a
   * maxflatworkgroupsize control directive, the value of @a
   * max_flat_workgroup_size must not be greater than the value of the
   * directive, and takes precedence.
   *
   * The value specified for maximum absolute grid size must be greater than or
   * equal to the product of the values specified by @a required_workgroup_size.
   *
   * If the bit at position BRIG_CONTROL_MAXFLATWORKGROUPSIZE is set in @a
   * control_directives_mask, this field must be greater than 0.
   */
  uint32_t max_flat_workgroup_size;

  /**
   * Number of work-groups per compute unit that will be used by the application
   * to launch the kernel. If the kernel contains a requestedworkgroupspercu
   * control directive, the two values should match.
   *
   * If the bit at position BRIG_CONTROL_REQUESTEDWORKGROUPSPERCU is set in @a
   * control_directives_mask, this field must be greater than 0.
   */
  uint32_t requested_workgroups_per_cu;

  /**
   * Grid size that will be used by the application in any dispatch of the
   * kernel. If the kernel contains a requiredgridsize control directive, the
   * dimensions should match.
   *
   * The specified grid size must be consistent with @a required_workgroup_size
   * and @a required_dim. Also, the product of the three dimensions must not
   * exceed @a max_flat_grid_size. Note that the listed invariants must hold
   * only if all the corresponding control directives are enabled.
   *
   * If the bit at position BRIG_CONTROL_REQUIREDGRIDSIZE is set in @a
   * control_directives_mask, the three dimension values must be greater than 0.
   */
  uint64_t required_grid_size[3];

  /**
   * Work-group size that will be used by the application in any dispatch of the
   * kernel. If the kernel contains a requiredworkgroupsize control directive,
   * the dimensions should match.
   *
   * The specified work-group size must be consistent with @a required_grid_size
   * and @a required_dim. Also, the product of the three dimensions must not
   * exceed @a max_flat_workgroup_size. Note that the listed invariants must
   * hold only if all the corresponding control directives are enabled.
   *
   * If the bit at position BRIG_CONTROL_REQUIREDWORKGROUPSIZE is set in @a
   * control_directives_mask, the three dimension values must be greater than 0.
   */
  hsa_dim3_t required_workgroup_size;

  /**
   * Number of dimensions that will be used by the application to launch the
   * kernel. If the kernel contains a requireddim control directive, the two
   * values should match.
   *
   * The specified dimensions must be consistent with @a required_grid_size and
   * @a required_workgroup_size. This invariant must hold only if all the
   * corresponding control directives are enabled.
   *
   * If the bit at position BRIG_CONTROL_REQUIREDDIM is set in @a
   * control_directives_mask, this field must be 1, 2, or 3.
   */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];
} hsa_ext_control_directives_t;

/** @} */


/** \defgroup ext-finalizer-descriptors TODO
 *  @{
 */

/**
 * @brief Opaque handle to the finalized code that includes the executable
 * ISA for the HSA agent. Used to populate the kernel object field in the Kernel
 * Dispatch packet.
 */
typedef struct hsa_ext_code_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_ext_code_t;

/**
 * @brief Code object types.
 */
typedef enum {
  /**
   * Not a code object.
   */
  HSA_EXT_CODE_NONE = 0,

  /**
   * Kernel.
   */
  HSA_EXT_CODE_KERNEL = 1,

  /**
   * Indirect function.
   */
  HSA_EXT_CODE_INDIRECT_FUNCTION = 2
} hsa_ext_code_type_t;

/**
 * @brief Sub-fields within the information member of the code descriptor. The
 * application can use the sub-field enumerator constant to find its offset with
 * respect to the field starting address. The width of each sub-field is
 * determined by the corresponding value in
 * ::hsa_ext_code_descriptor_info_width_t. The offset and the width are
 * expressed in bits
 */
 typedef enum {
  /**
   * Dynamic callstack flag. The finalizer sets the value of this sub-field to 1
   * to indicate that the generated ISA uses a dynamically sized call
   * stack. This can happen if recursive calls, calls to indirect functions, or
   * the HSAIL alloca instruction are present in the finalized code.
   */
   HSA_EXT_CODE_DESCRIPTOR_INFO_DYNAMIC_CALLSTACK = 0
 } hsa_ext_code_descriptor_info_t;

/**
 * @brief Width of the sub-fields in ::hsa_ext_code_descriptor_info_t.
 */
 typedef enum {
   HSA_EXT_CODE_DESCRIPTOR_INFO_WIDTH_DYNAMIC_CALLSTACK_WIDTH = 1
 } hsa_ext_code_descriptor_info_width_t;

/**
 * @brief Maximum segment alignment, which is in the range [16, 256] bytes.
 */
typedef enum {
  HSA_EXT_SEGMENT_ALIGNMENT_16 = 4,
  HSA_EXT_SEGMENT_ALIGNMENT_32 = 5,
  HSA_EXT_SEGMENT_ALIGNMENT_64 = 6,
  HSA_EXT_SEGMENT_ALIGNMENT_128 = 7,
  HSA_EXT_SEGMENT_ALIGNMENT_256 = 8
} hsa_ext_segment_alignment_t;

/**
 * @brief A code descriptor contains information about a kernel or indirect
 * function: a handle to the executable code, the memory required by the kernel
 * or function to run, and the control directives used by the finalizer during
 * the compilation process. Note that a code descriptor can also correspond to
 * a kernel or indirect function that has not been finalized, in which case the
 * @a code_type field must be ::HSA_EXT_CODE_NONE.
 */
typedef struct hsa_ext_code_descriptor_s {
  /**
   * HSA agent.
   */
  hsa_agent_t agent;

  /**
   * HSAIL program module where the kernel or indirect function is defined.
   */
  hsa_ext_program_module_t program_module;

  /**
   * Opaque handle to the finalized code.
   */
  hsa_ext_code_t code_object;

  /**
   * Code offset at which the kernel or indirect function is defined.
   */
  hsa_ext_brig_code_offset32_t offset;

  /**
   * Type of code object associated with this code descriptor. The value of this
   * field must be one of the enumeration constants in ::hsa_ext_code_type_t. If
   * the type is ::HSA_EXT_CODE_NONE, this kernel or indirect function has not
   * been finalized and the application should ignore the rest of the fields.
   */
  uint32_t code_type;

  /**
   * Call convention used in this kernel or indirect function. Must have a value
   * between 0 (inclusive) and the value of
   * ::HSA_EXT_AGENT_INFO_NUM_CALL_CONVENTIONS in @a agent (not inclusive).
  */
  uint32_t call_convention;

  /**
   * Size of static group segment memory required by this code object (per
   * work-group), in bytes. The reported amount includes any group memory used
   * by the finalizer to allocate fbarriers, but it not include any dynamically
   * allocated group segment memory that may be requested by the application
   * when a kernel is dispatched.
   */
  uint32_t group_segment_size;

  /**
   * Size of kernarg segment memory that is required to hold the values of the
   * kernel arguments, in bytes.
   */
  uint64_t kernarg_segment_size;

  /**
   * Size of static private, spill, and arg segment memory required by this code
   * object (per work-item), in bytes. If the value of
   * ::HSA_EXT_CODE_DESCRIPTOR_INFO_DYNAMIC_CALLSTACK is 1, the code object may
   * use more private memory than @a private_segment_size, and the application
   * must add the dynamic call stack usage to @a private_segment_size when
   * populating a Kernel Dispatch packet.
   */
  uint32_t private_segment_size;

  /**
   * The maximum byte alignment of variables used by the kernel in the group
   * memory segment. The value of this field must be one of the enumeration
   * constants in ::hsa_ext_segment_alignment_t.
   */
  uint8_t group_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the kernarg
   * memory segment.  The value of this field must be one of the enumeration
   * constants in ::hsa_ext_segment_alignment_t.
   */
  uint8_t kernarg_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the private
   * memory segment. The value of this field must be one of the enumeration
   * constants in ::hsa_ext_segment_alignment_t.
   */
  uint8_t private_segment_alignment;

  /**
   * Wavefront size. Must be a power of 2 in the [1,64] range. The application
   * can use this value to allocate dynamic group memory and set the work-group
   * size of a Kernel Dispatch packet.
   */
  uint8_t wavefront_size;

  /**
   * Number of fbarriers used by the kernel and all the functions it
   * transitively calls. Must be 0 if @a code_type is
   * ::HSA_EXT_CODE_INDIRECT_FUNCTION.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   * Profile. The value of this field must be one of the enumeration constants
   * in ::hsa_profile_t.
   */
  uint8_t profile;

  /**
   * Machine model. The value of this field must be one of the enumeration
   * constants in ::hsa_machine_model_t.
   */
  uint8_t machine_model;

  /**
   * Additional code descriptor information. The offset and width of the
   * sub-fields within @a info is determined by
   * ::hsa_ext_code_descriptor_info_t.
   */
  uint16_t info;

  /**
   * HSAIL major version.
   */
  uint32_t hsail_version_major;

  /**
   * HSAIL minor version.
   */
  uint32_t hsail_version_minor;

  /**
   * Reserved. Must be all 0s.
   */
  uint64_t reserved[7];

  /**
   * Control directives used by the finalizer to generate the code. If the
   * finalizer ignores a control directive, @a control_directives is a subset of
   * the union between the finalization directives and the HSAIL directives. The
   * reported directives reflect the constraints required by the code to
   * correctly execute.
   */
  hsa_ext_control_directives_t control_directives;
} hsa_ext_code_descriptor_t;

/** @} */

/** \addtogroup ext-finalizer-finalization
 *  @{
 */
/**
 * @brief Finalizer-determined call convention.
 */
typedef enum {
 /**
  * Finalizer-determined call convention.
  */
  HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO = -1
} hsa_ext_finalizer_call_convention_t;

/**
 * @brief Kernel or indirect function to be finalized. The request can also
 * include finalization options, such as control directives.
 */
typedef struct hsa_ext_finalization_request_s {
  /**
   * HSAIL program module where the kernel or indirect function definition is
   * located.
   */
  hsa_ext_program_module_t definition_program_module;
  /**
   * Entry offset (with respect to the start of the code section) of the
   * the kernel or indirect function definition.
   */
  hsa_ext_brig_code_offset32_t definition_offset;
  /**
   * Calling convention. A value of ::HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO
   * indicates that the finalizer can decide which call convention to use.
   */
  int32_t call_convention;
  /**
   * Low-level control directives that influence the finalization process.
   */
  hsa_ext_control_directives_t control_directives;
  /**
   * Pointer to a code descriptor that is populated by the HSA runtime upon
   * finalization. The code descriptor includes the machine code corresponding
   * to the kernel or indirect function.
   */
  hsa_ext_code_descriptor_t* descriptor;
} hsa_ext_finalization_request_t;

/**
 * @brief Finalize a list of kernels and indirect functions for a particular HSA
 * agent.
 *
 * @details Finalize a set of kernels and indirect functions that belong to the
 * same HSAIL program for a specific HSA agent. A kernel can be finalized at
 * most once for a given HSAIL program and HSA agent. An indirect function can
 * finalized at most once for a given HSAIL program, HSA agent, and calling
 * convention.
 *
 * The HSAIL program associated with the finalization list must contain
 * definitions for all the variables, fbarriers, kernels, and functions
 * referenced by operations in the code blocks being finalized.
 *
 * If the function succeeds, the HSA runtime updates each code descriptor
 * associated with a finalization request before performing a store release
 * (with system scope) of the descriptor's type with a value of
 * ::HSA_EXT_CODE_KERNEL or ::HSA_EXT_CODE_INDIRECT_FUNCTION. If the
 * finalization process fails, all of the code descriptors associated with the
 * finalization requests remain non-finalized (their type is
 * ::HSA_EXT_CODE_NONE).
 *
 * @param[in] num_finalization_requests Size of @p finalization_requests. Must
 * not be 0.
 *
 * @param[in,out] finalization_requests
 * \parblock
 * List of kernels and indirect functions to be finalized. Must not be NULL. The
 * kernels and indirect functions in the list must be part of the same HSAIL
 * program.
 *
 * A kernel in @p finalization_requests cannot have been already finalized for
 * the same HSA agent. An indirect function in @p finalization_requests cannot
 * have been already finalized for the given HSA agent and calling convention.
 *
 * A call convention used in a finalization request must have a value between
 * ::HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO (inclusive) and the value of the
 * attribute ::HSA_EXT_AGENT_INFO_NUM_CALL_CONVENTIONS in @p agent (not
 * inclusive).
 *
 * If this function returns successfully, the code descriptor pointed by every
 * finalization request in @p finalization_requests contains the machine code
 * and finalization metadata.
 * \endparblock
 *
 * @param[in] agent HSA agent to finalize for.
 *
 * @param[in] options Vendor-specific options. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The HSA agent is invalid
 *
 * @retval ::HSA_STATUS_ERROR_INCOMPATIBLE_ARGUMENTS @p agent is not one of
 * the HSA agents associated with the HSAIL program.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_FINALIZATION_REQUEST The finalization
 * request is invalid. For example, the HSAIL program module is invalid or does
 * not have any HSAIL module associated with it, or the offset does not
 * correspond to a kernel or indirect function.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED The finalizer encountered
 * an error while compiling a kernel or an indirect function.
 *
 *  @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH The directive in
 * the control directive structure and in the HSAIL kernel mismatch, or if the
 * same directive is used with a different value in one of the functions used by
 * this kernel.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for finalization.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p finalization_requests is NULL,
 * or @p num_finalization_requests is 0.
 *
 */
hsa_status_t HSA_API hsa_ext_finalize(
    size_t num_finalization_requests,
    hsa_ext_finalization_request_t *finalization_requests,
    hsa_agent_t agent,
    const char *options);

/** @} */


/** \addtogroup ext-finalizer-descriptors TODO
 *  @{
 */

/**
 * @brief Get the code descriptor list of a given kernel.
 *
 * @details Code descriptors associated with the same kernel are stored
 * contiguosly in memory. If the application wants to locate the code
 * descriptor associated with a particular agent, it should use the program
 * agent index (see ::hsa_ext_program_get_agent) as the offset with respect to
 * the pointer returned by this function.
 *
 * @param[in] definition_program_module HSAIL program module containing the kernel
 * definition.
 *
 * @param[in] definition_offset Code offset at which the kernel is defined.
 *
 * @param[out] descriptors Memory location where the HSA runtime stores a pointer to
 * the first kernel descriptor in the list.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p definition_program_module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to a kernel definition.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p descriptors is NULL.
 */
hsa_status_t HSA_API hsa_ext_kernel_get_descriptors(
    hsa_ext_program_module_t definition_program_module,
    hsa_ext_brig_code_offset32_t definition_offset,
    hsa_ext_code_descriptor_t** descriptors);

/**
 * @brief Get the code descriptor list for a given indirect function.
 *
 * @details Code descriptors associated with the same indirect function are
 * stored contiguosly in memory. If the application wants to locate the first
 * code descriptor associated with a particular HSA agent, it can use the value
 * returned by ::hsa_ext_agent_get_indirect_function_descriptor_index as an
 * index in the list. An HSA agent is associated with as many indirect function
 * descriptors as call conventions it supports
 * (::HSA_EXT_AGENT_INFO_NUM_CALL_CONVENTIONS).
 *
 * @param[in] definition_program_module HSAIL program module containing the indirect
 * function definition.
 *
 * @param[in] definition_offset Code offset at which the indirect function is
 * defined.
 *
 * @param[out] descriptors Memory location where the HSA runtime stores a pointer to
 * the first indirect function descriptor in the list.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE @p definition_program_module is
 * invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SYMBOL_OFFSET The specified offset
 * does not correspond to an indirect function definition.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p descriptors is NULL.
 */
hsa_status_t HSA_API hsa_ext_indirect_function_get_descriptors(
    hsa_ext_program_module_t definition_program_module,
    hsa_ext_brig_code_offset32_t definition_offset,
    hsa_ext_code_descriptor_t** descriptors);

/**
 * @brief Retrieve the offset of the first indirect function descriptor
 * associated with the given HSAIL program and HSA agent.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] agent HSA agent.
 *
 * @param[out] index Memory location where the HSA runtime stores the index of
 * the first indirect function descriptor for the HSA agent. The returned offset
 * can be used to index any indirect function descriptor list in @p program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p index is NULL.
 */
hsa_status_t HSA_API hsa_ext_agent_get_indirect_function_descriptor_index(
    hsa_ext_program_t program,
    hsa_agent_t agent,
    uint32_t* index);

/**
 * @brief Get the code descriptor associated with the given code object and HSA
 * agent.
 *
 * @param[in] code Code object.
 *
 * @param[in] agent HSA agent.
 *
 * @param[out] descriptor Memory location where the HSA runtime stores a pointer to
 * the code descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p descriptor is NULL.
 */
hsa_status_t hsa_ext_code_get_descriptor(
    hsa_ext_code_t code,
    hsa_agent_t agent,
    hsa_ext_code_descriptor_t **descriptor);

/** @} */


/** \defgroup ext-host-linkage TODO
 *  @{
 */
/**
 * @brief Define a global variable with program linkage and program allocation.
 *
 * @details This function allows the application to provide the definition
 * of a variable in the global segment with program linkage and program
 * allocation. The variable must not be defined in the HSAIL program. Note that
 * all the variables used by a finalization request must be defined.
 *
 * A variable can be defined before an HSAIL module containing a declaration
 * of the variable is added to @p program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] variable_name Name of the variable.
 *
 * @param[in] address Address where the variable is defined. The buffer pointed
 * by @p address is owned by the application, and cannot be deallocated before
 * @p program is destroyed.
 *
 * @param[out] definition_program_module Memory location where the HSA runtime
 * stores the HSAIL program module assigned to the definition, which is the
 * application HSAIL program module associated with @p program.
 *
 * @param[out] definition_offset Memory location where the HSA runtime stores
 * the variable definition offset. Application-defined variables are assigned
 * offsets based on the order they are defined, starting at 0.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_ALREADY_DEFINED The variable is
 * already defined.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL, or @p
 * definition_program_module is NULL, or @p definition_offset is NULL.
 */
hsa_status_t HSA_API hsa_ext_global_variable_define(
    hsa_ext_program_t program,
    const char* variable_name,
    void *address,
    hsa_ext_program_module_t* definition_program_module,
    hsa_ext_brig_code_offset32_t* definition_offset);

/**
 * @brief Define a global variable (with program linkage and agent allocation)
 * for a given HSA agent.
 *
 * @details This function allows the application to provide the definition (for
 * a specific HSA agent) of a variable in the global segment with program
 * linkage and agent allocation. The variable must not be defined in the HSAIL
 * program. Note that all the variables used by a finalization request must be
 * defined.
 *
 * A variable can be defined before an HSAIL module containing a declaration
 * of the variable is added to @p program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] variable_name Name of the variable.
 *
 * @param[in] agent HSA agent for which the variable is being defined.
 *
 * @param[in] address Address where the variable is defined. The buffer pointed
 * by @p address is owned by the application, and cannot be deallocated before
 * @p program is destroyed.
 *
 * @param[out] definition_program_module Memory location where the HSA runtime
 * stores the HSAIL program module assigned to the definition, which is the
 * application HSAIL program module associated with @p program.
 *
 * @param[out] definition_offset Memory location where the HSA runtime stores
 * the variable definition offset. Application-defined variables are assigned
 * offsets based on the order they are defined, starting at 0.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_ALREADY_DEFINED The variable is
 * already defined.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL, or @p
 * definition_program_module is NULL, or @p definition_offset is NULL.
 */
hsa_status_t HSA_API hsa_ext_global_variable_agent_define(
    hsa_ext_program_t program,
    const char* variable_name,
    hsa_agent_t agent,
    void *address,
    hsa_ext_program_module_t* definition_program_module,
    hsa_ext_brig_code_offset32_t* definition_offset);

/**
 * @brief Defines a readonly variable (with program linkage) for a given HSA
 * agent.
 *
 * @details This function allows the application to provide the definition (for
 * a specific HSA agent) of a variable in the readonly segment with program
 * linkage. The variable must not be defined in the HSAIL program. Note that all
 * the variables used by a finalization request must be defined.
 *
 * A variable can be defined before an HSAIL module containing a declaration
 * of the variable is added to @p program.
 *
 * @param[in] program HSAIL program
 *
 * @param[in] variable_name Name of the variable.
 *
 * @param[in] agent HSA agent for which the variable is being defined.
 *
 * @param[in] address Address where the variable is defined for the specified
 * agent. The buffer pointed by @p address is owned by the application, and
 * cannot be deallocated before @p program is destroyed.
 *
 * @param[out] definition_program_module Memory location where the HSA runtime
 * stores the HSAIL program module assigned to the definition, which is the
 * application HSAIL program module associated with @p program.
 *
 * @param[out] definition_offset Memory location where the HSA runtime stores
 * the variable definition offset. Application-defined variables are assigned
 * offsets based on the order they are defined, starting at 0.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_VARIABLE_ALREADY_DEFINED The variable is
 * already defined.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL, or @p
 * definition_program_module is NULL, or @p definition_offset is NULL.
 */
  hsa_status_t HSA_API hsa_ext_readonly_variable_define(
    hsa_ext_program_t program,
    const char* variable_name,
    hsa_agent_t agent,
    void* address,
    hsa_ext_program_module_t* definition_program_module,
    hsa_ext_brig_code_offset32_t* definition_offset);

/** @} */

/** \defgroup ext-finalizer-serialization HSAIL Finalization
 *  @{
 */

/**
 * @brief Opaque handle to application data that is passed to the serialization
 * and deserialization functions.
 */
typedef struct hsa_ext_callback_data_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_ext_callback_data_t;

/**
 * @brief Callback function invoked by the HSA runtime to allocate memory. The
 * HSA runtime passes three arguments to the callback: the allocation size, the
 * application data, and a pointer to a memory location where the application
 * stores the allocation result.
 */
typedef hsa_status_t (*hsa_ext_alloc_callback_t)(
    size_t size,
    hsa_ext_callback_data_t data,
    void **address);

/**
 * @brief Serialize an HSAIL program. Used for offline compilation.
 *
 * @details The function does not serialize any HSAIL modules added to @p
 * program.
 *
 * @param[in] program HSAIL program
 *
 * @param[in] alloc_callback Callback function for memory allocation. The HSA
 * runtime invokes @p alloc_callback once to allocate a buffer that contains the
 * serialized version of @p program. Must not be NULL. If the callback returns a
 * status code other than ::HSA_STATUS_SUCCESS, this function returns the same
 * code.
 *
 * @param[in] data Application data that is passed to @p alloc_callback. May be
 * NULL.
 *
 * @param[in] options Vendor-specific options. May be NULL.
 *
 * @param[out] serialized_program_size Memory location where the HSA runtime
 * stores the size (in bytes) of @p serialized_program. The returned value
 * matches the allocation size passed by the HSA runtime to @p
 * alloc_callback. Must not be NULL.
 *
 * @param[out] serialized_program Memory location where the HSA runtime stores a
 * pointer to the serialized program. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for serialization.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p alloc_callback, @p
 * serialized_program_size, or @p serialized_program are NULL.
 */
hsa_status_t HSA_API hsa_ext_program_serialize(
    hsa_ext_program_t program,
    hsa_ext_alloc_callback_t alloc_callback,
    hsa_ext_callback_data_t data,
    const char* options,
    size_t* serialized_program_size,
    void** serialized_program);

/**
 * @brief Callback function invoked by the HSA runtime to retrieve the address
 * of a variable with the given name. The variable must be declared with program
 * linkage and program allocation.
 */
typedef hsa_status_t (*hsa_ext_variable_define_callback_t)(
    const char *variable_name,
    hsa_ext_callback_data_t data,
    void** address);

/**
 * @brief Callback function invoked by the HSA runtime to retrieve the address
 * of a variable (for a specific HSA agent) with the given name. The variable
 * must be declared with program linkage and agent allocation.
 */
typedef hsa_status_t (*hsa_ext_variable_agent_define_callback_t)(
    const char *variable_name,
    hsa_agent_t agent,
    hsa_ext_callback_data_t data,
    void** address);

/**
 * @brief \parblock Matching callback that is invoked by the HSA runtime in
 * order to determine the mapping between machine code compatible HSA
 * agents.
 *
 * The callback receives four input arguments. The first two are the number of
 * agents at the time of deserialization and serialization. The number of
 * serialized agents matches that passed by the application to
 * ::hsa_ext_program_create. The number of deserialized agents matches that
 * passed by the application to ::hsa_ext_program_deserialize.
 *
 * The @p agent_compatibility_matrix (input) argument is an array with @p
 * num_deserialized_agents * @p num_serialized_agents elements. Cell @p i * @p
 * num_deserialized_agents + @p j is set to true if the HSA runtime determines
 * that the deserialized HSA agent with program agent ID @p i is machine code
 * compatible with the serialized HSA agent with a program agent ID of @p j, and
 * set to false otherwise.  A deserialized HSA agent can be compatible with any
 * number of serialized HSA agents between 0 and @p num_serialized_agents.
 *
 * The application populates the @p agent_map output argument with the agent
 * mapping that the HSA runtime should use during the deserialization
 * process. The array has @p num_deserialized_agents elements. For each element
 * @p i (program agent ID of a deserialized HSA agent) of @p agent_map, the
 * application must choose a matching out of the available options in @p
 * agent_compatibility_matrix, and write the program agent ID of the selected,
 * serialized HSA agent at cell @p i.  \endparblock
 */
typedef hsa_status_t (*hsa_ext_agent_map_callback_t)(
    uint32_t num_deserialized_agents,
    uint32_t num_serialized_agents,
    const bool *agent_compatibility_matrix,
    hsa_ext_callback_data_t data,
    uint32_t *agent_map);

/**
 * @brief Deserialize an HSAIL program.
 *
 * @details Creates an HSAIL program that has the same HSAIL program modules as
 * the original HSAIL program at the time of serialization. Deserialized HSAIL
 * programs can be used in the same way that the original HSAIL programs. This
 * includes performing additional finalizations (if the application reattaches
 * the original HSAIL modules, see below), and serializing.
 *
 * A deserialized HSAIL program module has no associated HSAIL module. The
 * application must restore the association invoking
 * ::hsa_ext_program_module_reattach_module before the deserialized HSAIL
 * program module can be used in a finalization request. An HSAIL program module
 * is assigned the same program index (see ::hsa_ext_program_get_program_module)
 * as its deserialized version.
 *
 * The application determines which finalizations need to be restored using @p
 * agent_map_callback. Not all the original finalizations need to be
 * restored.
 *
 * All variables defined by the HSAIL program are allocated and initialized to
 * the value defined by their HSAIL initializers, which may not be the same
 * value of the variables at the time of serialization. The kernel and indirect
 * function code descriptors are allocated and initialized according to the
 * restored finalizations.
 *
 * The HSA runtime invokes the definition callbacks to obtain the address of
 * variables that were originally defined by the application using functions
 * such as ::hsa_ext_global_variable_define. These variables must be allocated
 * and initialized appropriately. If any of the callback returns a status code
 * other than ::HSA_STATUS_SUCCESS, the deserialization process is interrupted
 * and the same status code is returned to the application.

 * @param[in] serialized_program A serialized HSAIL program. Must not be NULL.
 *
 * @param[in] num_agents Size of @p agents. Must not be 0.
 *
 * @param[in] agents List of HSA agents for which the HSAIL program should be
 * deserialized. An HSA agent can appear in the list at most once. Must not be
 * NULL. The memory associated with @p agents can be reused or freed after the
 * function returns.
 *
 * @param[in] global_variable_define_callback Callback function invoked by the
 * HSA runtime to retrieve the definition of a variable in the global segment
 * with program linkage and program allocation.
 *
 * @param[in] global_variable_agent_define_callback Callback function invoked by
 * the HSA runtime to retrieve the definition (for a specific HSA agent) of a
 * variable in the global segment with program linkage and program allocation.
 *
 * @param[in] readonly_variable_define_callback Callback function invoked by the
 * HSA runtime to retrieve the definition (for a specific HSA agent) of a
 * variable in the readonly segment with program linkage and program allocation.
 *
 * @param[in] agent_map_callback Callback function invoked by the HSA runtime to
 * retrieve a mapping between the HSA agents at the time of serialization and
 * the HSA agents present in @p agents. Must not be NULL.
 *
 * @param[in] data Application data that is passed to the callback
 * functions. May be NULL.
 *
 * @param[in] options Vendor-specific options. May be NULL.
 *
 * @param[out] program Memory location where the HSA runtime stores the
 * deserialized HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for deserialization.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p serialized_program is NULL, @p
 * num_agents is 0, @p agents is NULL or contains duplicates, or @p program is
 * NULL.
 */
hsa_status_t HSA_API hsa_ext_program_deserialize(
    void *serialized_program,
    uint32_t num_agents,
    const hsa_agent_t *agents,
    hsa_ext_variable_define_callback_t global_variable_define_callback,
    hsa_ext_variable_agent_define_callback_t global_variable_agent_define_callback,
    hsa_ext_variable_agent_define_callback_t readonly_variable_define_callback,
    hsa_ext_agent_map_callback_t agent_map_callback,
    hsa_ext_callback_data_t data,
    const char *options,
    hsa_ext_program_t *program);

/**
 * @brief Attach an HSAIL module back to a deserialized HSAIL program module.
 *
 * @param[in] program_module An HSAIL program module other than the application
 * HSAIL program module. The value of ::HSA_EXT_PROGRAM_MODULE_INFO_BRIG_MODULE
 * in this HSAIL program module must be NULL, indicating that @p program_module
 * is in deserialized state and that the application has not attached any HSAIL
 * module to it.
 *
 * @param[in] brig_module Pointer to the HSAIL module associated with @p
 * program_module at the time of serialization (the value of
 * ::HSA_EXT_PROGRAM_MODULE_INFO_BRIG_MODULE in @p program_module was @p
 * brig_module). If the application reattaches any other HSAIL module, the
 * behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM_MODULE The HSAIL program
 * module is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE The HSAIL module is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p brig_module is NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_program_module_reattach_module(
    hsa_ext_program_module_t program_module,
    const hsa_ext_brig_module_t* brig_module);

/** @} */


/** \defgroup ext-images Images and Samplers
 *  @{
 */

/**
 * @brief Image handle, populated by ::hsa_ext_image_create_handle. Images
 * handles are only unique within an HSA agent, not across HSA agents.
 *
 */
typedef struct hsa_ext_image_handle_s {
  /**
   * Opaque handle.
   */
    uint64_t handle;

} hsa_ext_image_handle_t;

/**
 * @brief Image format capability returned by
 * ::hsa_ext_image_get_format_capability
 */
typedef enum  {
   /**
    * Images of this format are not supported.
    */
    HSA_EXT_IMAGE_FORMAT_NOT_SUPPORTED = 0x0,

   /**
    * Images of this format can be accessed only from read operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_ONLY = 0x1,

   /**
    * Images of this format can be accessed only from write operations.
    */
    HSA_EXT_IMAGE_FORMAT_WRITE_ONLY = 0x2,

    /**
    * Images of this format can be accessed from read and write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_WRITE = 0x4,

    /**
    * Images of this format can be accessed from read-modify-write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this format are guaranteed to have consistent data layout
    * regardless of the how it is accessed by the HSA agent.
    */
    HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA = 0x10

} hsa_ext_image_format_capability_t;

/**
 * @brief HSA agent-specific image size and alignment requirements. This
 * structure stores the HSA agent-dependent image data sizes and alignment, and
 * populated by ::hsa_ext_image_get_info.
 */
typedef struct hsa_ext_image_info_s {
  /**
   * HSA component-specific image data size in bytes.
   */
  size_t image_size;

  /**
   * HSA component-specific image data alignment in bytes.
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
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_ONLY = 0,

  /**
   * Image handle is to be used by the HSA agent as write-only using an HSAIL
   * woimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_WRITE_ONLY = 1,

  /**
   * Image handle is to be used by the HSA agent as read and/or write using an
   * HSAIL rwimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_WRITE = 2

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
typedef enum {
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
 * @brief Implementation-independent HSA image descriptor.
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
   * Depth of the image in slices, only used if geometry is 3D. Depth = 0 is
   * same as depth = 1.
   */
  size_t depth;

  /**
   * Number of images in the image array, only used if geometry is 1DArray or
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
 * HSA agent, not across HSA agents.
 */
typedef struct hsa_ext_sampler_handle_s {
  /**
   * Opaque handle.
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
 * written to the location specified by @p capability_mask. See
 * ::hsa_ext_image_format_capability_t to determine all possible capabilities
 * that can be reported in the bit mask.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent, @p image_format, or
 * @p capability_mask is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_format_capability(
    hsa_agent_t agent,
    const hsa_ext_image_format_t *image_format,
    hsa_ext_image_geometry_t image_geometry,
    uint32_t *capability_mask);

/**
 *
 * @brief Inquires the required HSA component-specific image data details from a
 * implementation-independent image descriptor.
 *
 * @details If successful, the queried HSA agent-specific image data info is
 * written to the location specified by @p image_info. Based on the
 * implementation the optimal image data size and alignment requirements could
 * vary depending on the image attributes specified in @p image_descriptor.
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
 * @param[out] image_info Image info size and alignment required by the HSA
 * agent.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 *
 * @retval ::HSA_EXT_IMG_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The HSA agent does
 * not support the image format specified by the descriptor.
 *
 * @retval ::HSA_EXT_IMG_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The HSA agent does
 * not support the image dimensions specified by the format descriptor.
 */
hsa_status_t HSA_API hsa_ext_image_get_info(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_ext_image_access_permission_t access_permission,
    hsa_ext_image_info_t *image_info);

/**
 * @brief Creates a HSA agent-defined image handle from an
 * implementation-independent image descriptor and a HSA agent-specific image
 * data. The image access defines how the HSA agent expects to use the image and
 * must match the HSAIL image handle type used by the HSA agent.
 *
 * @details If successful, the image handle is written to the location specified
 * by @p image_handle. The image data memory must be allocated using the
 * previously queried ::hsa_ext_image_get_info memory requirements with the same
 * HSA agent and implementation-independent image descriptor.
 *
 * The image data is not initialized and any previous memory contents is
 * preserved. The memory management of image data is the application's
 * responsibility and can only be freed until the memory is no longer needed and
 * any image handles using it are destroyed.
 *
 * @p access_permission defines how the HSA agent expects to use the image
 * handle. The image format specified in the image descriptor must be supported
 * by the HSA agent for the intended permission.
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
 * @param[in] image_data Address of the HSA component-specific image data.
 *
 * @param[in] access_permission Access permission of the image by the HSA agent.
 *
 * @param[out] image_handle Agent-specific image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 *
 * @retval ::HSA_EXT_IMG_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The HSA agent
 * does not have the capability to support the image format using the specified
 * @p access_permission.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The HSA agent cannot create the
 * specified handle because it is out of resources.
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
 * If @p completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * If @p src_row_pitch is smaller than the destination region width (in bytes),
 * then @p src_row_pitch = region width.
 *
 * If @p src_slice_pitch is smaller than the destination region width * region
 * height (in bytes), then @p src_slice_pitch = region width * region height.
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
 * @param[in] dst_image_handle Destination image handle.
 *
 * @param[in] image_region Image region to be updated.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent, @p src_memory or @p
 * image_region is NULL.
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
 * If @p completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the completion
 * signal is signaled when the operation completes.
 *
 * If @p dst_row_pitch is smaller than the source region width (in bytes), then
 * @p dst_row_pitch = region width.
 *
 * If @p dst_slice_pitch is smaller than the source region width * region height
 * (in bytes), then @p dst_slice_pitch = region width * region height.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent, @p dst_memory or @p
 * image_region is NULL.
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
 * If @p completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the completion
 * signal is signaled when the operation completes.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent or @p image_region is
 * NULL.
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
 * component type) stored in the cleared image are based on the image
 * component order. The size of the image data cleared is implicitly derived
 * from the image region.
 *
 * If @p completion_signal is NULL, the operation occurs
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
 * specified in a linear space, which is appropriately converted by the HSA
 * runtime to sRGB color space.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent or @p image_region is
 * NULL.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent or @p image_handle is
 * NULL.
 */
hsa_status_t HSA_API hsa_ext_image_destroy_handle (
    hsa_agent_t agent,
    hsa_ext_image_handle_t *image_handle);

/**
 * @brief Create an HSA component-defined sampler handle from an HSA
 * component-independent sampler descriptor.
 *
 * @details If successful, the sampler handle is written to the location
 * specified by the sampler handle.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_descriptor Implementation-independent sampler descriptor.
 *
 * @param[out] sampler_handle HSA component-specific sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The HSA agent cannot create the
 * specified handle because it is out of resources.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 */
hsa_status_t HSA_API hsa_ext_sampler_destroy_handle(
    hsa_agent_t agent,
    hsa_ext_sampler_handle_t *sampler_handle);

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 */
typedef enum {
    /**
     * Image format is not supported.
     */
    HSA_EXT_IMG_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED = 0x3000,
    /**
     * Image size is not supported.
     */
    HSA_EXT_IMG_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED = 0x3001
} hsa_ext_img_status_t;

/**
 * @brief Enumeration constants added to ::hsa_agent_info_t by this extension.
 */
typedef enum {
    /**
   * Maximum dimensions (width, height, depth) of one-dimensional images, in
   * image elements. The Y and Z dimensions maximums must be 0. The X maximum
   * must be at most 16384.  The value of this attribute is undefined if the
   * HSA agent is not an HSA component, or the implementation does not support
   * images. The type of this attribute is ::hsa_dim3_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE1D_MAX_DIM = 0x3000,
  /**
   * Maximum dimensions (width, height, depth) of two-dimensional images, in
   * image elements. The Z dimension maximum must be 0. The X and Y maximums
   * must be at most 16384. The value of this attribute is undefined if the
   * HSA agent is not an HSA component, or the implementation does not support
   * images. The type of this attribute is ::hsa_dim3_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE2D_MAX_DIM = 0x3001,
  /**
   * Maximum dimensions (width, height, depth) of three-dimensional images, in
   * image elements. The maximum along any dimension cannot exceed 2048. The
   * value of this attribute is undefined if the HSA agent is not an HSA
   * component, or the implementation does not support images.  The type of this
   * attribute is ::hsa_dim3_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE3D_MAX_DIM = 0x3002,
  /**
   * Maximum number of image layers in a image array. Must not exceed 2048. The
   * value of this attribute is undefined if the HSA agent is not an HSA
   * component, or the implementation does not support images. The type of this
   * attribute is uint32_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE_ARRAY_MAX_SIZE = 0x3003,
  /**
   * Maximum number of read-only image handles that can be created at any one
   * time. Must be at least 128. The value of this attribute is undefined if the
   * HSA agent is not an HSA component, or the implementation does not support
   * images. The type of this attribute is uint32_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE_RD_MAX = 0x3004,
  /**
   * Maximum number of write-only and read-write image handles that can be
   * created at any one time. Must be at least 64. The value of this attribute
   * is undefined if the HSA agent is not an HSA component, or the
   * implementation does not support images. The type of this attribute is
   * uint32_t.
   */
  HSA_EXT_IMG_AGENT_INFO_IMAGE_RDWR_MAX = 0x3005,
  /**
   * Maximum number of sampler handlers that can be created at any one
   * time. Must be at least 16. The value of this attribute is undefined if the
   * HSA agent is not an HSA component, or the implementation does not support
   * images. The type of this attribute is uint32_t.
   */
  HSA_EXT_IMG_AGENT_INFO_SAMPLER_MAX = 0x3006
} hsa_ext_img_agent_info_t;

/** @} */
#ifdef __cplusplus
}
#endif  /*__cplusplus*/
