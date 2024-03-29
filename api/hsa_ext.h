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

struct BrigModuleHeader;
typedef struct BrigModuleHeader* BrigModule_t;


/** \defgroup ext-alt-finalizer-extensions Finalization Extensions
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 *
 * @remark Additions to hsa_status_t
 */
enum {
  /**
   * The HSAIL program is invalid.
   */
  HSA_EXT_STATUS_ERROR_INVALID_PROGRAM = 0x2000,
  /**
   * The HSAIL module is invalid.
   */
  HSA_EXT_STATUS_ERROR_INVALID_MODULE = 0x2001,
  /**
   * Machine model or profile of the HSAIL module do not match the machine model
   * or profile of the HSAIL program.
   */
  HSA_EXT_STATUS_ERROR_INCOMPATIBLE_MODULE = 0x2002,
  /**
   * The HSAIL module is already a part of the HSAIL program.
   */
  HSA_EXT_STATUS_ERROR_MODULE_ALREADY_INCLUDED = 0x2003,
  /**
   * Compatibility mismatch between symbol declaration and symbol definition.
   */
  HSA_EXT_STATUS_ERROR_SYMBOL_MISMATCH = 0x2004,
  /**
   * The finalization encountered an error while finalizing a kernel or
   * indirect function.
   */
  HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED = 0x2005,
  /**
   * @deprecated Control directives are no longer used.
   *
   * Mismatch between a directive in the control directive structure and in
   * the HSAIL kernel.
   */
  HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH = 0x2006,
  /**
   * The code object writer is invalid.
   */
  HSA_EXT_STATUS_ERROR_INVALID_CODE_OBJECT_WRITER = 0x2007
};
/**
 * @brief Iterate over the instruction set architectures supported by the
 * finalizer extension, and invoke an application-defined callback on every
 * iteration.
 *
 * @param[in] callback Callback to be invoked once per ISA. The HSA
 * runtime passes two arguments to the callback, the ISA and the
 * application data.  If @p callback returns a status other than
 * ::HSA_STATUS_SUCCESS for a particular iteration, the traversal stops and
 * that status value is returned.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
*/
hsa_status_t HSA_API hsa_ext_finalizer_iterate_isa(
    hsa_status_t (*callback)(hsa_isa_t isa, void* data),
    void* data);

/**
 * @copydoc hsa_isa_from_name
 */
hsa_status_t HSA_API hsa_ext_isa_from_name(
    const char *name,
    hsa_isa_t *isa);

/**
 * @copydoc hsa_isa_get_info
 */
hsa_status_t HSA_API hsa_ext_isa_get_info(
    hsa_isa_t isa,
    hsa_isa_info_t attribute,
    void *value);

/** @} */

/** \defgroup ext-alt-finalizer-program Finalization Program
 *  @{
 */

/**
 * @brief Opaque handle to a code object writer. A code object writer is used by
 * the finalizer to output the finalized code object to a file (if the code
 * object writer is created using
 * ::hsa_ext_code_object_writer_create_from_file), or to memory (if the code
 * object writer is created using
 * ::hsa_ext_code_object_writer_create_from_memory).
 */
typedef struct hsa_ext_code_object_writer_s {
  /**
   *  Opaque handle. Two handles reference the same object of the enclosing type
   *  if and only if they are equal.
   */
  uint64_t handle;
} hsa_ext_code_object_writer_t;

/**
 * @brief Create an empty code object writer to operate on a file.
 *
 * @details File must be opened by application with at least write permissions
 * prior calling this function. POSIX file descriptor for opened file must be
 * provided. If file descriptor points to non-empty file, file will be
 * truncated. File is owned and managed by application, code object writer is
 * only used for populating it. Lifetime of file descriptor must exceed lifetime
 * of its code object writer.
 *
 * @param[in] file File descriptor for opened file. File must be
 * opened with at least write permissions. If file is non-empty, file will be
 * truncated.
 *
 * @param[in] code_object_writer Memory location to store newly created code
 * object writer handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_FILE @p file is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES Failure to allocate resources
 * required.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p code_object_writer is NULL.
 */
hsa_status_t HSA_API hsa_ext_code_object_writer_create_from_file(
    hsa_file_t file,
    hsa_ext_code_object_writer_t *code_object_writer);

/**
 * @brief Create an empty code object writer to operate on memory.
 *
 * @details Memory is allocated by application through a callback function.
 * Memory must be deallocated by application in case of failure. Allocated
 * memory is owned and must be managed by application, code object writer is
 * only used for populating it. Lifetime of memory that is allocated must exceed
 * lifetime of its code object writer.
 *
 * @param[in] memory_allocate Callback function to be invoked once per
 * finalization to allocate memory needed for outputting of code object.
 * Callback function takes in four arguments: requested size, requested
 * alignment, pointer to memory location where application stores pointer to
 * allocated memory, application-provided data. If callback function returns
 * status code other than ::HSA_STATUS_SUCCESS, then finalization function
 * returns same code.
 *
 * @param[in] data Application-provided data to pass into @p memory_allocate.
 * May be NULL.
 *
 * @param[in] code_object_writer Memory location to store newly created code
 * object writer handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES Failure to allocate resources
 * required.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p memory_allocate is NULL.
 * @p code_object_writer is NULL.
 */
hsa_status_t HSA_API hsa_ext_code_object_writer_create_from_memory(
    hsa_status_t (*memory_allocate)(size_t size, size_t align, void **ptr,
                                    void *data),
    void *data,
    hsa_ext_code_object_writer_t *code_object_writer);

/**
 * @brief Destroy a code object writer.
 *
 * @details Code object writer handle becomes invalid after completion of this
 * function. File/memory populated by code object writer is not closed, removed,
 * or deallocated during execution of this function, and can be used as
 * application sees fit.
 *
 * @param[in] code_object_writer Valid code object writer handle to destroy.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_CODE_OBJECT_WRITER @p
 * code_object_writer is invalid.
 */
hsa_status_t HSA_API hsa_ext_code_object_writer_destroy(
    hsa_ext_code_object_writer_t code_object_writer);

/**
 * @brief HSAIL (BRIG) module. The HSA Programmer's Reference Manual contains
 * the definition of the BrigModule_t type.
 */
typedef BrigModule_t hsa_ext_module_t;

/**
 * @brief An opaque handle to a HSAIL program, which groups a set of HSAIL
 * modules that collectively define functions and variables used by kernels and
 * indirect functions.
 */
typedef struct hsa_ext_program_s {
  /**
   *  Opaque handle. Two handles reference the same object of the enclosing type
   *  if and only if they are equal.
   */
  uint64_t handle;
} hsa_ext_program_t;

/**
 * @brief Create an empty HSAIL program.
 *
 * @param[in] machine_model Machine model used in the HSAIL program.
 *
 * @param[in] profile Profile used in the HSAIL program.
 *
 * @param[in] default_float_rounding_mode Default floating-point rounding mode
 * used in the HSAIL program.
 *
 * @param[in] options Standard and vendor-specific options. Unknown options are
 * ignored. A standard option begins with the "-hsa_" prefix. May be NULL.
 *
 * @param[out] program Memory location where the HSA runtime stores the newly
 * created HSAIL program handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p machine_model is invalid,
 * @p profile is invalid, @p default_float_rounding_mode is invalid, or
 * @p program is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_create(
    hsa_machine_model_t machine_model,
    hsa_profile_t profile,
    hsa_default_float_rounding_mode_t default_float_rounding_mode,
    const char *options,
    hsa_ext_program_t *program);

/**
 * @brief Destroy a HSAIL program.
 *
 * @details The HSAIL program handle becomes invalid after it has been
 * destroyed. Code object handles produced by ::hsa_ext_program_finalize are
 * still valid after the HSAIL program has been destroyed, and can be used as
 * intended. Resources allocated outside and associated with the HSAIL program
 * (such as HSAIL modules that are added to the HSAIL program) can be released
 * after the HSAIL program has been destroyed.
 *
 * @param[in] program HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_program_destroy(
    hsa_ext_program_t program);

/**
 * @brief Add a HSAIL module to an existing HSAIL program.
 *
 * @details The HSA runtime does not perform a deep copy of the HSAIL module
 * upon addition. Instead, it stores a pointer to the HSAIL module. The
 * ownership of the HSAIL module belongs to the application, which must ensure
 * that @p module is not released before destroying the HSAIL program.
 *
 * The HSAIL module is successfully added to the HSAIL program if @p module is
 * valid, if all the declarations and definitions for the same symbol are
 * compatible, and if @p module specify machine model and profile that matches
 * the HSAIL program.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] module HSAIL module. The application can add the same HSAIL module
 * to @p program at most once. The HSAIL module must specify the same machine
 * model and profile as @p program. If the default floating-point rounding mode
 * of @p module is not default, then it should match that of @p program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE The HSAIL module is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INCOMPATIBLE_MODULE The machine model of @p
 * module does not match machine model of @p program, or the profile of @p
 * module does not match profile of @p program.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_MODULE_ALREADY_INCLUDED The HSAIL module is
 * already a part of the HSAIL program.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_SYMBOL_MISMATCH Symbol declaration and symbol
 * definition compatibility mismatch. See the symbol compatibility rules in the
 * HSA Programming Reference Manual.
 */
hsa_status_t HSA_API hsa_ext_program_add_module(
    hsa_ext_program_t program,
    hsa_ext_module_t module);

/**
 * @brief Iterate over the HSAIL modules in a program, and invoke an
 * application-defined callback on every iteration.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] callback Callback to be invoked once per HSAIL module in the
 * program. The HSA runtime passes three arguments to the callback: the program,
 * a HSAIL module, and the application data.  If @p callback returns a status
 * other than ::HSA_STATUS_SUCCESS for a particular iteration, the traversal
 * stops and ::hsa_ext_program_iterate_modules returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
 */
hsa_status_t HSA_API hsa_ext_program_iterate_modules(
    hsa_ext_program_t program,
    hsa_status_t (*callback)(hsa_ext_program_t program, hsa_ext_module_t module,
                             void* data),
    void* data);

/**
 * @brief HSAIL program attributes.
 */
typedef enum {
  /**
   * Machine model specified when the HSAIL program was created. The type
   * of this attribute is ::hsa_machine_model_t.
   */
  HSA_EXT_PROGRAM_INFO_MACHINE_MODEL = 0,
  /**
   * Profile specified when the HSAIL program was created. The type of
   * this attribute is ::hsa_profile_t.
   */
  HSA_EXT_PROGRAM_INFO_PROFILE = 1,
  /**
   * Default floating-point rounding mode specified when the HSAIL program was
   * created. The type of this attribute is ::hsa_default_float_rounding_mode_t.
   */
  HSA_EXT_PROGRAM_INFO_DEFAULT_FLOAT_ROUNDING_MODE = 2
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
 * large enough to hold the value of @p attribute, the behaviour is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid
 * HSAIL program attribute, or @p value is NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_program_get_info(
    hsa_ext_program_t program,
    hsa_ext_program_info_t attribute,
    void *value);

/**
 * @brief Generate program code object from given program.
 *
 * @details Generate program code object from given program by finalizing all
 * defined program allocation variables in given program. Generated code object
 * is written by provided code object writer [which operates on either file or
 * memory], therefore lifetime of code object writer [and lifetime of underlying
 * file or memory] must exceed execution of this function.
 *
 * @param[in] program Valid program handle to finalize.
 *
 * @param[in] options Standard and vendor-specific options. Unknown options are
 * ignored. A standard option begins with the "-hsa_" prefix. May be NULL.
 *
 * @param[out] code_object_writer Valid code object writer handle.
 *
 * @retval ::HSA_STATUS_SUCCESS Function is executed succesfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_CODE_OBJECT_WRITER
 * @p code_object_writer is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES Failure to allocate resources
 * required.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED Failure to finalize
 * @p program.
 */
hsa_status_t HSA_API hsa_ext_program_code_object_finalize(
    hsa_ext_program_t program,
    const char *options,
    hsa_ext_code_object_writer_t code_object_writer);

/**
 * @brief Generate agent code object from given program for given instruction
 * set architecture.
 *
 * @details Generate agent code object from given program for given instruction
 * set architecture by finalizing all defined agent allocation variables,
 * functions, indirect functions, and kernels in given program for given
 * instruction set architecture. Generated code object is written by provided
 * code object writer [which operates on either file or memory], therefore
 * lifetime of code object writer [and lifetime of underlying file or memory]
 * must exceed execution of this function.
 *
 * @param[in] program Valid program handle to finalize.
 *
 * @param[in] isa Valid instruction set architecture handle to finalize for.
 *
 * @param[in] options Standard and vendor-specific options. Unknown options are
 * ignored. A standard option begins with the "-hsa_" prefix. May be NULL.
 *
 * @param[out] code_object_writer Valid code object writer handle.
 *
 * @retval ::HSA_STATUS_SUCCESS Function is executed succesfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ISA @p isa is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH @p options do not match
 * one or more control directives in one or more BRIG modules in @p program.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_CODE_OBJECT_WRITER
 * @p code_object_writer is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES Failure to allocate resources
 * required.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED Failure to finalize
 * @p program.
 */
hsa_status_t HSA_API hsa_ext_agent_code_object_finalize(
    hsa_ext_program_t program,
    hsa_isa_t isa,
    const char *options,
    hsa_ext_code_object_writer_t code_object_writer);

/**
 * @deprecated
 *
 * @brief Finalizer-determined call convention.
 */
typedef enum {
 /**
  * Finalizer-determined call convention.
  */
  HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO = -1
} hsa_ext_finalizer_call_convention_t;

/**
 * @deprecated

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
   * Reserved. Must be 0.
   */
  uint32_t reserved1;
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
  uint8_t reserved2[75];
} hsa_ext_control_directives_t;

/**
 * @deprecated
 *
 * @brief Finalize an HSAIL program for a given instruction set architecture.
 *
 * @details Finalize all of the kernels and indirect functions that belong to
 * the same HSAIL program for a specific instruction set architecture (ISA). The
 * transitive closure of all functions specified by call or scall must be
 * defined. Kernels and indirect functions that are being finalized must be
 * defined. Kernels and indirect functions that are referenced in kernels and
 * indirect functions being finalized may or may not be defined, but must be
 * declared. All the global/readonly segment variables that are referenced in
 * kernels and indirect functions being finalized may or may not be defined, but
 * must be declared.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] isa Instruction set architecture to finalize for.
 *
 * @param[in] call_convention A call convention used in a finalization. Must
 * have a value between ::HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO (inclusive)
 * and the value of the attribute ::HSA_ISA_INFO_CALL_CONVENTION_COUNT in @p
 * isa (not inclusive).
 *
 * @param[in] control_directives Low-level control directives that influence
 * the finalization process.
 *
 * @param[in] options Standard and vendor-specific options. Unknown options are
 * ignored. A standard option begins with the "-hsa_" prefix. May be NULL.
 *
 * @param[in] code_object_type Type of code object to produce.
 *
 * @param[out] code_object Code object generated by the Finalizer, which
 * contains the machine code for the kernels and indirect functions in the HSAIL
 * program. The code object is independent of the HSAIL module that was used to
 * generate it.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ISA @p isa is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH The directive in
 * the control directive structure and in the HSAIL kernel mismatch, or if the
 * same directive is used with a different value in one of the functions used by
 * this kernel.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_FINALIZATION_FAILED The Finalizer
 * encountered an error while compiling a kernel or an indirect function.
 */
hsa_status_t HSA_API hsa_ext_program_finalize(
    hsa_ext_program_t program,
    hsa_isa_t isa,
    int32_t call_convention,
    hsa_ext_control_directives_t control_directives,
    const char *options,
    hsa_code_object_type_t code_object_type,
    hsa_code_object_t *code_object);


#define hsa_ext_finalizer_1_00

/**
 * @brief The function pointer table for the finalizer v1.00 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_finalizer_1_00_pfn_s {
  hsa_status_t (*hsa_ext_program_create)(
    hsa_machine_model_t machine_model,
    hsa_profile_t profile,
    hsa_default_float_rounding_mode_t default_float_rounding_mode,
    const char *options,
    hsa_ext_program_t *program);

  hsa_status_t (*hsa_ext_program_destroy)(
    hsa_ext_program_t program);

  hsa_status_t (*hsa_ext_program_add_module)(
    hsa_ext_program_t program,
    hsa_ext_module_t module);

  hsa_status_t (*hsa_ext_program_iterate_modules)(
    hsa_ext_program_t program,
    hsa_status_t (*callback)(hsa_ext_program_t program, hsa_ext_module_t module,
                             void* data),
    void* data);

  hsa_status_t (*hsa_ext_program_get_info)(
    hsa_ext_program_t program,
    hsa_ext_program_info_t attribute,
    void *value);

  hsa_status_t (*hsa_ext_program_finalize)(
    hsa_ext_program_t program,
    hsa_isa_t isa,
    int32_t call_convention,
    hsa_ext_control_directives_t control_directives,
    const char *options,
    hsa_code_object_type_t code_object_type,
    hsa_code_object_t *code_object);
} hsa_ext_finalizer_1_00_pfn_t;

#define hsa_ext_finalizer_1

/**
 * @brief The function pointer table for the finalizer v1 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_finalizer_1_pfn_s {
  hsa_status_t (*hsa_ext_program_create)(
    hsa_machine_model_t machine_model,
    hsa_profile_t profile,
    hsa_default_float_rounding_mode_t default_float_rounding_mode,
    const char *options,
    hsa_ext_program_t *program);

  hsa_status_t (*hsa_ext_program_destroy)(
    hsa_ext_program_t program);

  hsa_status_t (*hsa_ext_program_add_module)(
    hsa_ext_program_t program,
    hsa_ext_module_t module);

  hsa_status_t (*hsa_ext_program_iterate_modules)(
    hsa_ext_program_t program,
    hsa_status_t (*callback)(hsa_ext_program_t program, hsa_ext_module_t module,
                             void* data),
    void* data);

  hsa_status_t (*hsa_ext_program_get_info)(
    hsa_ext_program_t program,
    hsa_ext_program_info_t attribute,
    void *value);

  hsa_status_t (*hsa_ext_program_finalize)(
    hsa_ext_program_t program,
    hsa_isa_t isa,
    int32_t call_convention,
    hsa_ext_control_directives_t control_directives,
    const char *options,
    hsa_code_object_type_t code_object_type,
    hsa_code_object_t *code_object);
    
  hsa_status_t (*hsa_ext_code_object_writer_create_from_file)(
    hsa_file_t file,
    hsa_ext_code_object_writer_t *code_object_writer);

  hsa_status_t (*hsa_ext_code_object_writer_create_from_memory)(
    hsa_status_t (*memory_allocate)(size_t size, size_t align, void **ptr,
                                    void *data),
    void *data,
    hsa_ext_code_object_writer_t *code_object_writer);
    
  hsa_status_t (*hsa_ext_code_object_writer_destroy)(
    hsa_ext_code_object_writer_t code_object_writer);
    
  hsa_status_t (*hsa_ext_program_code_object_finalize)(
    hsa_ext_program_t program,
    const char *options,
    hsa_ext_code_object_writer_t code_object_writer);
    
  hsa_status_t (*hsa_ext_agent_code_object_finalize)(
    hsa_ext_program_t program,
    hsa_isa_t isa,
    const char *options,
    hsa_ext_code_object_writer_t code_object_writer);
} hsa_ext_finalizer_1_pfn_t;

/** @} */

/** \defgroup ext-images Images and Samplers
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 *
 * @remark Additions to hsa_status_t
 */
enum {
    /**
     * Image format is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED = 0x3000,
    /**
     * Image size is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED = 0x3001,
    /**
     * Image pitch is not supported or invalid.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_PITCH_UNSUPPORTED = 0x3002,
    /**
     * Sampler descriptor is not supported or invalid.
     */
    HSA_EXT_STATUS_ERROR_SAMPLER_DESCRIPTOR_UNSUPPORTED = 0x3003
};

/**
 * @brief Enumeration constants added to ::hsa_agent_info_t by this
 * extension.
 *
 * @remark Additions to hsa_agent_info_t
 */
enum {
  /**
   * Maximum number of elements in 1D images. Must be at least 16384. The type
   * of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1D_MAX_ELEMENTS = 0x3000,
  /**
   * Maximum number of elements in 1DA images. Must be at least 16384. The type
   * of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1DA_MAX_ELEMENTS = 0x3001,
  /**
   * Maximum number of elements in 1DB images. Must be at least 65536. The type
   * of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1DB_MAX_ELEMENTS = 0x3002,
  /**
   * Maximum dimensions (width, height) of 2D images, in image elements. The X
   * and Y maximums must be at least 16384. The type of this attribute is
   * size_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2D_MAX_ELEMENTS = 0x3003,
  /**
   * Maximum dimensions (width, height) of 2DA images, in image elements. The X
   * and Y maximums must be at least 16384. The type of this attribute is
   * size_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DA_MAX_ELEMENTS = 0x3004,
  /**
   * Maximum dimensions (width, height) of 2DDEPTH images, in image
   * elements. The X and Y maximums must be at least 16384. The type of this
   * attribute is size_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DDEPTH_MAX_ELEMENTS = 0x3005,
  /**
   * Maximum dimensions (width, height) of 2DADEPTH images, in image
   * elements. The X and Y maximums must be at least 16384. The type of this
   * attribute is size_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DADEPTH_MAX_ELEMENTS = 0x3006,
  /**
   * Maximum dimensions (width, height, depth) of 3D images, in image
   * elements. The maximum along any dimension must be at least 2048. The type
   * of this attribute is size_t[3].
   */
  HSA_EXT_AGENT_INFO_IMAGE_3D_MAX_ELEMENTS = 0x3007,
  /**
   * Maximum number of image layers in a image array. Must be at least 2048. The
   * type of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_ARRAY_MAX_LAYERS = 0x3008,
  /**
   * Maximum number of read-only image handles that can be created for an agent at any one
   * time. Must be at least 128. The type of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_MAX_IMAGE_RD_HANDLES = 0x3009,
  /**
   * Maximum number of write-only and read-write image handles (combined) that
   * can be created for an agent at any one time. Must be at least 64. The type of this
   * attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_MAX_IMAGE_RORW_HANDLES = 0x300A,
  /**
   * Maximum number of sampler handlers that can be created for an agent at any one
   * time. Must be at least 16. The type of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_MAX_SAMPLER_HANDLERS = 0x300B,
  /**
   * Image pitch alignment. The agent only supports linear image data
   * layouts with a row pitch that is a multiple of this value. Must be
   * a power of 2. The type of this attribute is size_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_LINEAR_ROW_PITCH_ALIGNMENT = 0x300C
};

/**
 * @brief Image handle, populated by ::hsa_ext_image_create or
 * ::hsa_ext_image_create_with_layout. Image
 * handles are only unique within an agent, not across agents.
 *
 */
typedef struct hsa_ext_image_s {
  /**
   *  Opaque handle. For a given agent, two handles reference the same object of
   *  the enclosing type if and only if they are equal.
   */
    uint64_t handle;

} hsa_ext_image_t;

/**
 * @brief Geometry associated with the image. This specifies the
 * number of image dimensions and whether the image is an image
 * array. See the <em>Image Geometry</em> section in the <em>HSA
 * Programming Reference Manual</em> for definitions on each
 * geometry. The enumeration values match the BRIG type @p
 * hsa_ext_brig_image_geometry_t.
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
   * are addressed by width and index coordinate.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DA = 3,

  /**
   * Array of two-dimensional images with the same size and format. 2D arrays
   * are addressed by width,  height, and index coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DA = 4,

  /**
   * One-dimensional image addressed by width coordinate. It has
   * specific restrictions compared to ::HSA_EXT_IMAGE_GEOMETRY_1D. An
   * image with an opaque image data layout will always use a linear
   * image data layout, and one with an explicit image data layout
   * must specify ::HSA_EXT_IMAGE_DATA_LAYOUT_LINEAR.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DB = 5,

  /**
   * Two-dimensional depth image addressed by width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DDEPTH = 6,

  /**
   * Array of two-dimensional depth images with the same size and format. 2D
   * arrays are addressed by width, height, and index coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DADEPTH = 7
} hsa_ext_image_geometry_t;

/**
 * @brief Channel type associated with the elements of an image. See
 * the <em>Channel Type</em> section in the <em>HSA Programming Reference
 * Manual</em> for definitions on each channel type. The
 * enumeration values and defintion match the BRIG type @p
 * hsa_ext_brig_image_channel_type_t.
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
 * @brief A fixed-size type used to represent ::hsa_ext_image_channel_type_t constants.
 */
typedef uint32_t hsa_ext_image_channel_type32_t;
    
/**
 *
 * @brief Channel order associated with the elements of an image. See
 * the <em>Channel Order</em> section in the <em>HSA Programming Reference
 * Manual</em> for definitions on each channel order. The
 * enumeration values match the BRIG type @p
 * hsa_ext_brig_image_channel_order_t.
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
 * @brief A fixed-size type used to represent ::hsa_ext_image_channel_order_t constants.
 */
typedef uint32_t hsa_ext_image_channel_order32_t;
    

/**
 * @brief Image format.
 */
typedef struct hsa_ext_image_format_s {
  /**
    * Channel type.
    */
    hsa_ext_image_channel_type32_t channel_type;

   /**
    * Channel order.
    */
    hsa_ext_image_channel_order32_t channel_order;
} hsa_ext_image_format_t;

/**
 * @brief Implementation independent image descriptor.
 */
typedef struct hsa_ext_image_descriptor_s {
    /**
     * Image geometry.
     */
    hsa_ext_image_geometry_t geometry;
    /**
     * Width of the image, in components.
     */
    size_t width;
    /**
     * Height of the image, in components. Only used if the geometry is
     * ::HSA_EXT_IMAGE_GEOMETRY_2D, ::HSA_EXT_IMAGE_GEOMETRY_3D,
     * HSA_EXT_IMAGE_GEOMETRY_2DA, HSA_EXT_IMAGE_GEOMETRY_2DDEPTH, or
     * HSA_EXT_IMAGE_GEOMETRY_2DADEPTH, otherwise must be 0.
     */
    size_t height;
    /**
     * Depth of the image, in components. Only used if the geometry is
     * ::HSA_EXT_IMAGE_GEOMETRY_3D, otherwise must be 0.
     */
    size_t depth;
    /**
     * Number of image layers in the image array. Only used if the geometry is
     * ::HSA_EXT_IMAGE_GEOMETRY_1DA, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
     * HSA_EXT_IMAGE_GEOMETRY_2DADEPTH, otherwise must be 0.
     */
    size_t array_size;
    /**
     * Image format.
     */
    hsa_ext_image_format_t format;
} hsa_ext_image_descriptor_t;

/**
 * @brief Image capability.
 */
typedef enum  {
   /**
    * Images of this geometry, format, and layout are not supported by
    * the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_NOT_SUPPORTED = 0x0,
   /**
    * Read-only images of this geometry, format and layout are
    * supported by the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_ONLY = 0x1,
   /**
    * Write-only images of this geometry, format, and layout are
    * supported by the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_WRITE_ONLY = 0x2,
   /**
    * Read-write images of this geometry, format, and layout are
    * supported by the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_WRITE = 0x4,
   /**
    * @deprecated Images of this geometry, format, and layout can be accessed from
    * read-modify-write atomic operations in the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this geometry, format and layout are guaranteed to
    * have a consistent data layout regardless of how they are
    * accessed by the associated agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT = 0x10
} hsa_ext_image_capability_t;

/**
 * @brief Image data layout.
 *
 * @details An image data layout denotes such aspects of image data
 * layout as tiling and organization of channels in memory. Some image
 * data layouts may only apply to specific image geometries, formats,
 * and access permissions. Different agents may support different
 * image layout identifiers, including vendor specific layouts. Note
 * that an agent may not support the same image data layout for
 * different access permissions to images with the same image
 * geometry, size, and format. If multiple agents support the same
 * image data layout then it is possible to use separate image handles
 * for each agent that references the same image data.
 */

typedef enum  {
   /**
    * An implementation specific opaque image data layout which can
    * vary depending on the agent, geometry, image format, image size,
    * and access permissions.
    */
    HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE = 0x0,
   /**
    * The image data layout is specified by the following rules in
    * ascending byte address order. For a 3D image, 2DA image array,
    * or 1DA image array the image data is stored as a linear sequence
    * of adjacent 2D image slices, 2D images, or 1D images
    * respectively, spaced according to the slice pitch. Each 2D image
    * is stored as a linear sequence of adjacent image rows, spaced
    * according to the row pitch. Each 1D or 1DB image is stored as a
    * single image row. Each image row is stored as a linear sequence
    * of image elements. Each image element is stored as a linear
    * sequence of image components specified by the left to right
    * channel order definition. Each image component is stored using
    * the memory type specified by the channel type.
    *
    * The 1DB image geometry always uses the linear image data layout.
    */
    HSA_EXT_IMAGE_DATA_LAYOUT_LINEAR = 0x1
} hsa_ext_image_data_layout_t;

/**
 * @brief Retrieve the supported image capabilities for a given combination of
 * agent, geometry, and image format for an image created with an opaque image
 * data layout.
 *
 * @param[in] agent Agent to be associated with the image handle.
 *
 * @param[in] geometry Geometry.
 *
 * @param[in] image_format Pointer to an image format. Must not be NULL.
 *
 * @param[out] capability_mask Pointer to a memory location where the HSA
 * runtime stores a bit-mask of supported image capability
 * (::hsa_ext_image_capability_t) values. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_format is
 * NULL, or @p capability_mask is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_capability(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    uint32_t *capability_mask);

/**
 * @brief Retrieve the supported image capabilities for a given combination of
 * agent, geometry, image format, and image layout for an image created with
 * an explicit image data layout.
 *
 * @param[in] agent Agent to be associated with the image handle.
 *
 * @param[in] geometry Geometry.
 *
 * @param[in] image_format Pointer to an image format. Must not be NULL.
 *
 * @param[in] image_data_layout The image data layout.
 * It is invalid to use ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE, use
 * ::hsa_ext_image_get_capability instead.
 *
 * @param[out] capability_mask Pointer to a memory location where the HSA
 * runtime stores a bit-mask of supported image capability
 * (::hsa_ext_image_capability_t) values. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_format is
 * NULL, @p image_data_layout is ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE,
 * or @p capability_mask is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_capability_with_layout(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    hsa_ext_image_data_layout_t image_data_layout,
    uint32_t *capability_mask);

/**
 * @brief Agent specific image size and alignment requirements, populated by
 * ::hsa_ext_image_data_get_info and ::hsa_ext_image_data_get_info_with_layout.
 */
typedef struct hsa_ext_image_data_info_s {
  /**
   * Image data size, in bytes.
   */
  size_t size;

  /**
   * Image data alignment, in bytes. Must always be a power of 2.
   */
  size_t alignment;

} hsa_ext_image_data_info_t;

/**
 * @brief Retrieve the image data requirements for a given combination of agent, image
 * descriptor, and access permission for an image created with an opaque image
 * data layout.
 *
 * @details The optimal image data size and alignment requirements may
 * vary depending on the image attributes specified in @p
 * image_descriptor, the @p access_permission, and the @p agent. Also,
 * different implementations of the HSA runtime may return different
 * requirements for the same input values.
 *
 * The implementation must return the same image data requirements for
 * different access permissions with matching image descriptors as long
 * as ::hsa_ext_image_get_capability reports
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT. Image
 * descriptors match if they have the same values, with the exception
 * that s-form channel orders match the corresponding non-s-form
 * channel order and vice versa.
 *
 * @param[in] agent Agent to be associated with the image handle.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] access_permission Access permission of the image when
 * accessed by @p agent. The access permission defines how the agent
 * is allowed to access the image and must match the corresponding
 * HSAIL image handle type. The @p agent must support the image format
 * specified in @p image_descriptor for the given @p
 * access_permission.
 *
 * @param[out] image_data_info Memory location where the runtime stores the
 * size and alignment requirements. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The @p
 * agent does not support the image format specified by @p
 * image_descriptor with the specified @p access_permission.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The agent
 * does not support the image dimensions specified by @p
 * image_descriptor with the specified @p access_permission.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_descriptor is NULL, @p
 * access_permission is not a valid access permission value, or @p
 * image_data_info is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_data_get_info(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_info_t *image_data_info);

/**
 * @brief Retrieve the image data requirements for a given combination of
 * image descriptor, access permission, image data layout, image data row pitch,
 * and image data slice pitch for an image created with an explicit image
 * data layout.
 *
 * @details The image data size and alignment requirements may vary
 * depending on the image attributes specified in @p image_descriptor,
 * the @p access_permission, and the image layout. However, different
 * implementations of the HSA runtime will return the same
 * requirements for the same input values.
 *
 * The implementation must return the same image data requirements for
 * different access permissions with matching image descriptors and
 * matching image layouts as long as ::hsa_ext_image_get_capability
 * reports
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT. Image
 * descriptors match if they have the same values, with the exception
 * that s-form channel orders match the corresponding non-s-form
 * channel order and vice versa. Image layouts match if they are the
 * same image data layout and use the same image row and slice pitch
 * values.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] access_permission Access permission of the image when
 * accessed by an agent. The access permission defines how the agent
 * is allowed to access the image and must match the corresponding
 * HSAIL image handle type.
 *
 * @param[in] image_data_layout The image data layout to use.
 * It is invalid to use ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE, use
 * ::hsa_ext_image_data_get_info instead.
 *
 * @param[in] image_data_row_pitch The size in bytes for a single row
 * of the image in the image data. If 0 is specified then the default
 * row pitch value is used: image width * image element byte size.
 * The value used must be greater than or equal to the default row
 * pitch, and be a multiple of the image element byte size. For the
 * linear image layout it must also be a multiple of the image linear
 * row pitch alignment for the agents that will access the image data
 * using image instructions.
 *
 * @param[in] image_data_slice_pitch The size in bytes of a single
 * slice of a 3D image, or the size in bytes of each image layer in an
 * image array in the image data. If 0 is specified then the default
 * slice pitch value is used: row pitch * height if geometry is
 * ::HSA_EXT_IMAGE_GEOMETRY_3D, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
 * ::HSA_EXT_IMAGE_GEOMETRY_2DADEPTH; row pitch if geometry is
 * ::HSA_EXT_IMAGE_GEOMETRY_1DA; and 0 otherwise. The value used must
 * be 0 if the default slice pitch is 0, be greater than or equal to
 * the default slice pitch, and be a multiple of the row pitch.
 *
 * @param[out] image_data_info Memory location where the runtime stores the
 * size and alignment requirements. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The image
 * format specified by @p image_descriptor is not supported for the
 * @p access_permission and @p image_data_layout specified.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The image
 * dimensions specified by @p image_descriptor are not supported for
 * the @p access_permission and @p image_data_layout specified.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_PITCH_UNSUPPORTED The row and
 * slice pitch specified by @p image_data_row_pitch and @p
 * image_data_slice_pitch are invalid or not supported.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_descriptor is
 * NULL, @p image_data_layout is ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE,
 * or @p image_data_info is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_data_get_info_with_layout(
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_layout_t image_data_layout,
    size_t image_data_row_pitch,
    size_t image_data_slice_pitch,
    hsa_ext_image_data_info_t *image_data_info);

/**
 * @brief Creates a agent specific image handle to an image with an
 * opaque image data layout.
 *
 * @details Images with an opaque image data layout created with
 * different access permissions but matching image descriptors and
 * same agent can share the same image data if
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT is reported
 * by ::hsa_ext_image_get_capability for the image format specified in
 * the image descriptor. Image descriptors match if they have the same
 * values, with the exception that s-form channel orders match the
 * corresponding non-s-form channel order and vice versa.
 *
 * If necessary, an application can use image operations (import,
 * export, copy, clear) to prepare the image for the intended use
 * regardless of the access permissions.
 *
 * @param[in] agent agent to be associated with the image handle created.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] image_data Image data buffer that must have been allocated
 * according to the size and alignment requirements dictated by
 * ::hsa_ext_image_data_get_info. Must not be NULL.
 *
 * Any previous memory contents are preserved upon creation. The application is
 * responsible for ensuring that the lifetime of the image data exceeds that of
 * all the associated images.
 *
 * @param[in] access_permission Access permission of the image when
 * accessed by agent. The access permission defines how the agent
 * is allowed to access the image using the image handle created and
 * must match the corresponding HSAIL image handle type. The agent
 * must support the image format specified in @p image_descriptor for
 * the given @p access_permission.
 *
 * @param[out] image Pointer to a memory location where the HSA runtime stores
 * the newly created image handle. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The agent
 * does not have the capability to support the image format contained
 * in @p image_descriptor using the specified @p access_permission.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The agent
 * does not support the image dimensions specified by @p
 * image_descriptor using the specified @p access_permission.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The HSA runtime cannot create the
 * image because it is out of resources (for example, the agent does not
 * support the creation of more image handles with the given @p access_permission).
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_descriptor is NULL, @p
 * image_data is NULL, @p image_data does not have a valid alignment,
 * @p access_permission is not a valid access permission
 * value, or @p image is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_create(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_t *image);

/**
 * @brief Creates a agent specific image handle to an image with an explicit
 * image data layout.
 *
 * @details Images with an explicit image data layout created with
 * different access permissions but matching image descriptors and
 * matching image layout can share the same image data if
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT is reported
 * by ::hsa_ext_image_get_capability_with_layout for the image format
 * specified in the image descriptor and specified image data
 * layout. Image descriptors match if they have the same values, with
 * the exception that s-form channel orders match the corresponding
 * non-s-form channel order and vice versa. Image layouts match if
 * they are the same image data layout and use the same image row and
 * slice values.
 *
 * If necessary, an application can use image operations (import, export, copy,
 * clear) to prepare the image for the intended use regardless of the access
 * permissions.
 *
 * @param[in] agent agent to be associated with the image handle created.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] image_data Image data buffer that must have been allocated
 * according to the size and alignment requirements dictated by
 * ::hsa_ext_image_data_get_info_with_layout. Must not be NULL.
 *
 * Any previous memory contents are preserved upon creation. The application is
 * responsible for ensuring that the lifetime of the image data exceeds that of
 * all the associated images.
 *
 * @param[in] access_permission Access permission of the image when
 * accessed by the agent. The access permission defines how the agent
 * is allowed to access the image and must match the corresponding
 * HSAIL image handle type. The agent must support the image format
 * specified in @p image_descriptor for the given @p access_permission
 * and @p image_data_layout.
 *
 * @param[in] image_data_layout The image data layout to use for the
 * @p image_data. It is invalid to use
 * ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE, use ::hsa_ext_image_create
 * instead.
 *
 * @param[in] image_data_row_pitch The size in bytes for a single row
 * of the image in the image data. If 0 is specified then the default
 * row pitch value is used: image width * image element byte size.
 * The value used must be greater than or equal to the default row
 * pitch, and be a multiple of the image element byte size. For the
 * linear image layout it must also be a multiple of the image linear
 * row pitch alignment for the agents that will access the image data
 * using image instructions.
 *
 * @param[in] image_data_slice_pitch The size in bytes of a single
 * slice of a 3D image, or the size in bytes of each image layer in an
 * image array in the image data. If 0 is specified then the default
 * slice pitch value is used: row pitch * height if geometry is
 * ::HSA_EXT_IMAGE_GEOMETRY_3D, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
 * ::HSA_EXT_IMAGE_GEOMETRY_2DADEPTH; row pitch if geometry is
 * ::HSA_EXT_IMAGE_GEOMETRY_1DA; and 0 otherwise. The value used must
 * be 0 if the default slice pitch is 0, be greater than or equal to
 * the default slice pitch, and be a multiple of the row pitch.
 *
 * @param[out] image Pointer to a memory location where the HSA runtime stores
 * the newly created image handle. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The agent does
 * not have the capability to support the image format contained in the image
 * descriptor using the specified @p access_permission and @p image_data_layout.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The agent
 * does not support the image dimensions specified by @p
 * image_descriptor using the specified @p access_permission and @p
 * image_data_layout.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_PITCH_UNSUPPORTED The agent does
 * not support the row and slice pitch specified by @p image_data_row_pitch
 * and @p image_data_slice_pitch, or the values are invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The HSA runtime cannot create the
 * image because it is out of resources (for example, the agent does not
 * support the creation of more image handles with the given @p access_permission).
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_descriptor is NULL, @p
 * image_data is NULL, @p image_data does not have a valid alignment,
 * @p image_data_layout is ::HSA_EXT_IMAGE_DATA_LAYOUT_OPAQUE,
 * or @p image is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_create_with_layout(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_layout_t image_data_layout,
    size_t image_data_row_pitch,
    size_t image_data_slice_pitch,
    hsa_ext_image_t *image);

/**
 * @brief Destroy an image handle previously created using ::hsa_ext_image_create or
 * ::hsa_ext_image_create_with_layout.
 *
 * @details Destroying the image handle does not free the associated image data,
 * or modify its contents. The application should not destroy an image handle while
 * there are references to it queued for execution or currently being used in a
 * kernel dispatch.
 *
 * @param[in] agent Agent associated with the image handle.
 *
 * @param[in] image Image handle to destroy.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_image_destroy(
    hsa_agent_t agent,
    hsa_ext_image_t image);

/**
 * @brief Copies a portion of one image (the source) to another image (the
 * destination).
 *
 * @details The source and destination image formats should be the
 * same, with the exception that s-form channel orders match the
 * corresponding non-s-form channel order and vice versa. For example,
 * it is allowed to copy a source image with a channel order of
 * HSA_EXT_IMAGE_CHANNEL_ORDER_SRGB to a destination image with a
 * channel order of HSA_EXT_IMAGE_CHANNEL_ORDER_RGB.
 *
 * The source and destination images do not have to be of the same geometry and
 * appropriate scaling is performed by the HSA runtime. It is possible to copy
 * subregions between any combinations of source and destination geometries, provided
 * that the dimensions of the subregions are the same. For example, it is
 * allowed to copy a rectangular region from a 2D image to a slice of a 3D
 * image.
 *
 * If the source and destination image data overlap, or the combination of
 * offset and range references an out-out-bounds element in any of the images,
 * the behavior is undefined.
 *
 * @param[in] agent Agent associated with both the source and destination image handles.
 *
 * @param[in] src_image Image handle of source image. The agent associated with the source
 * image handle must be identical to that of the destination image.
 *
 * @param[in] src_offset Pointer to the offset within the source image where to
 * copy the data from. Must not be NULL.
 *
 * @param[in] dst_image Image handle of destination image.
 *
 * @param[in] dst_offset Pointer to the offset within the destination
 * image where to copy the data. Must not be NULL.
 *
 * @param[in] range Dimensions of the image portion to be copied. The HSA
 * runtime computes the size of the image data to be copied using this
 * argument. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p src_offset is
 * NULL, @p dst_offset is NULL, or @p range is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_copy(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    const hsa_dim3_t* src_offset,
    hsa_ext_image_t dst_image,
    const hsa_dim3_t* dst_offset,
    const hsa_dim3_t* range);

/**
 * @brief Image region.
 */
typedef struct hsa_ext_image_region_s {
   /**
    * Offset within an image (in coordinates).
    */
    hsa_dim3_t offset;

   /**
    * Dimension size of the image range (in coordinates). The x, y, and z dimensions
    * correspond to width, height, and depth or index respectively.
    */
    hsa_dim3_t range;
} hsa_ext_image_region_t;

/**
 * @brief Import a linearly organized image data from memory directly to an
 * image handle.
 *
 * @details This operation updates the image data referenced by the image handle
 * from the source memory. The size of the data imported from memory is
 * implicitly derived from the image region.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the source memory or destination image data memory can
 * overlap. Overlapping of any of the source and destination image
 * data memory within the import operation produces undefined results.
 *
 * @param[in] agent Agent associated with the image handle.
 *
 * @param[in] src_memory Source memory. Must not be NULL.
 *
 * @param[in] src_row_pitch The size in bytes of a single row of the image in the
 * source memory. If the value is smaller than the destination image region
 * width * image element byte size, then region width * image element byte
 * size is used.
 *
 * @param[in] src_slice_pitch The size in bytes of a single 2D slice of a 3D image,
 * or the size in bytes of each image layer in an image array in the source memory.
 * If the geometry is ::HSA_EXT_IMAGE_GEOMETRY_1DA and the value is smaller than the
 * value used for @p src_row_pitch, then the value used for @p src_row_pitch is used.
 * If the geometry is ::HSA_EXT_IMAGE_GEOMETRY_3D, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
 * HSA_EXT_IMAGE_GEOMETRY_2DADEPTH and the value is smaller than the value used for
 * @p src_row_pitch * destination image region height, then the value used for
 * @p src_row_pitch * destination image region height is used.
 * Otherwise, the value is not used.
 *
 * @param[in] dst_image Image handle of destination image.
 *
 * @param[in] image_region Pointer to the image region to be updated. Must not
 * be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p src_memory is NULL, or @p
 * image_region is NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_image_import(
    hsa_agent_t agent,
    const void *src_memory,
    size_t src_row_pitch,
    size_t src_slice_pitch,
    hsa_ext_image_t dst_image,
    const hsa_ext_image_region_t *image_region);

/**
 * @brief Export the image data to linearly organized memory.
 *
 * @details The operation updates the destination memory with the image data of
 * @p src_image. The size of the data exported to memory is implicitly derived
 * from the image region.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the destination memory or source image data memory can
 * overlap. Overlapping of any of the source and destination image
 * data memory within the export operation produces undefined results.
 *
 * @param[in] agent Agent associated with the image handle.
 *
 * @param[in] src_image Image handle of source image.
 *
 * @param[in] dst_memory Destination memory. Must not be NULL.
 *
 * @param[in] dst_row_pitch The size in bytes of a single row of the image in the
 * destination memory. If the value is smaller than the source image region
 * width * image element byte size, then region width * image element byte
 * size is used.
 *
 * @param[in] dst_slice_pitch The size in bytes of a single 2D slice of a 3D image,
 * or the size in bytes of each image in an image array in the destination memory.
 * If the geometry is ::HSA_EXT_IMAGE_GEOMETRY_1DA and the value is smaller than the
 * value used for @p dst_row_pitch, then the value used for @p dst_row_pitch is used.
 * If the geometry is ::HSA_EXT_IMAGE_GEOMETRY_3D, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
 * HSA_EXT_IMAGE_GEOMETRY_2DADEPTH and the value is smaller than the value used for
 * @p dst_row_pitch * source image region height, then the value used for
 * @p dst_row_pitch * source image region height is used.
 * Otherwise, the value is not used.
 *
 * @param[in] image_region Pointer to the image region to be exported. Must not
 * be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p dst_memory is NULL, or @p
 * image_region is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_export(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    void *dst_memory,
    size_t dst_row_pitch,
    size_t dst_slice_pitch,
    const hsa_ext_image_region_t *image_region);

/**
 * @brief Clear a region of an image so that every image element has
 * the specified value.
 *
 * @param[in] agent Agent associated with the image handle.
 *
 * @param[in] image Image handle for image to be cleared.
 *
 * @param[in] data The value to which to set each image element being
 * cleared. It is specified as an array of image component values. The
 * number of array elements must match the number of access components
 * for the image channel order. The type of each array element must
 * match the image access type of the image channel type. When the
 * value is used to set the value of an image element, the conversion
 * method corresponding to the image channel type is used. See the
 * <em>Channel Order</em> section and <em>Channel Type</em> section in
 * the <em>HSA Programming Reference Manual</em> for more
 * information. Must not be NULL.
 *
 * @param[in] image_region Pointer to the image region to clear. Must not be
 * NULL. If the region references an out-out-bounds element, the behavior is
 * undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p data is NULL, or @p
 * image_region is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_clear(
    hsa_agent_t agent,
    hsa_ext_image_t image,
    const void* data,
    const hsa_ext_image_region_t *image_region);

/**
 * @brief Sampler handle. Samplers are populated by
 * ::hsa_ext_sampler_create. Sampler handles are only unique within an
 * agent, not across agents.
 */
typedef struct hsa_ext_sampler_s {
  /**
   *  Opaque handle. For a given agent, two handles reference the same object of
   *  the enclosing type if and only if they are equal.
   */
    uint64_t handle;
} hsa_ext_sampler_t;

/**
 * @brief Sampler address modes. The sampler address mode describes
 * the processing of out-of-range image coordinates. See the
 * <em>Addressing Mode</em> section in the <em>HSA Programming Reference
 * Manual</em> for definitions on each address mode. The values
 * match the BRIG type @p hsa_ext_brig_sampler_addressing_t.
 */
typedef enum {
  /**
   * Out-of-range coordinates are not handled.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_UNDEFINED = 0,

  /**
   * Clamp out-of-range coordinates to the image edge.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_CLAMP_TO_EDGE = 1,

  /**
   * Clamp out-of-range coordinates to the image border color.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_CLAMP_TO_BORDER = 2,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate
   * range so the image appears as repeated tiles.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_REPEAT = 3,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate
   * range so the image appears as repeated tiles with every other
   * tile a reflection.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_MIRRORED_REPEAT = 4

} hsa_ext_sampler_addressing_mode_t;

/**
 * @brief A fixed-size type used to represent ::hsa_ext_sampler_addressing_mode_t constants.
 */
typedef uint32_t hsa_ext_sampler_addressing_mode32_t;

/**
 * @brief Sampler coordinate normalization modes. See the
 * <em>Coordinate Normalization Mode</em> section in the <em>HSA
 * Programming Reference Manual</em> for definitions on each
 * coordinate normalization mode. The values match the BRIG type @p
 * hsa_ext_brig_sampler_coord_normalization_t.
 */
typedef enum {

  /**
   * Coordinates are used to directly address an image element.
   */
  HSA_EXT_SAMPLER_COORDINATE_MODE_UNNORMALIZED = 0,

  /**
   * Coordinates are scaled by the image dimension size before being
   * used to address an image element.
   */
  HSA_EXT_SAMPLER_COORDINATE_MODE_NORMALIZED = 1

} hsa_ext_sampler_coordinate_mode_t;

/**
 * @brief A fixed-size type used to represent ::hsa_ext_sampler_coordinate_mode_t constants.
 */
typedef uint32_t hsa_ext_sampler_coordinate_mode32_t;
    

/**
 * @brief Sampler filter modes. See the <em>Filter Mode</em> section
 * in the <em>HSA Programming Reference Manual</em> for definitions
 * on each address mode. The enumeration values match the BRIG type @p
 * hsa_ext_brig_sampler_filter_t.
 */
typedef enum {
  /**
   * Filter to the image element nearest (in Manhattan distance) to the
   * specified coordinate.
   */
  HSA_EXT_SAMPLER_FILTER_MODE_NEAREST = 0,

  /**
   * Filter to the image element calculated by combining the elements in a 2x2
   * square block or 2x2x2 cube block around the specified coordinate. The
   * elements are combined using linear interpolation.
   */
  HSA_EXT_SAMPLER_FILTER_MODE_LINEAR = 1

} hsa_ext_sampler_filter_mode_t;

/**
 * @brief A fixed-size type used to represent ::hsa_ext_sampler_filter_mode_t constants.
 */
typedef uint32_t hsa_ext_sampler_filter_mode32_t;

/**
 * @brief Implementation independent sampler descriptor.
 */
typedef struct hsa_ext_sampler_descriptor_s {
  /**
   * Sampler coordinate mode describes the normalization of image coordinates.
   */
  hsa_ext_sampler_coordinate_mode32_t coordinate_mode;

  /**
   * Sampler filter type describes the type of sampling performed.
   */
  hsa_ext_sampler_filter_mode32_t filter_mode;

  /**
   * Sampler address mode describes the processing of out-of-range image
   * coordinates.
   */
  hsa_ext_sampler_addressing_mode32_t address_mode;

} hsa_ext_sampler_descriptor_t;

/**
 * @brief Create an agent specific sampler handle for a given agent
 * independent sampler descriptor and agent.
 *
 * @param[in] agent Agent to be associated with the sampler handle created.
 *
 * @param[in] sampler_descriptor Pointer to a sampler descriptor. Must not be
 * NULL.
 *
 * @param[out] sampler Memory location where the HSA runtime stores the newly
 * created sampler handle. Must not be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_SAMPLER_DESCRIPTOR_UNSUPPORTED The
 * @p agent does not have the capability to support the properties
 * specified by @p sampler_descriptor or it is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The agent cannot create the
 * specified handle because it is out of resources.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p sampler_descriptor is NULL, or
 * @p sampler is NULL.
 */
hsa_status_t HSA_API hsa_ext_sampler_create(
    hsa_agent_t agent,
    const hsa_ext_sampler_descriptor_t *sampler_descriptor,
    hsa_ext_sampler_t *sampler);

/**
 * @brief Destroy a sampler handle previously created using ::hsa_ext_sampler_create.
 *
 * @details The sampler handle should not be destroyed while there are
 * references to it queued for execution or currently being used in a
 * kernel dispatch.
 *
 * @param[in] agent Agent associated with the sampler handle.
 *
 * @param[in] sampler Sampler handle to destroy.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_sampler_destroy(
    hsa_agent_t agent,
    hsa_ext_sampler_t sampler);


#define hsa_ext_images_1_00

/**
 * @brief The function pointer table for the images v1.00 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_images_1_00_pfn_s {

  hsa_status_t (*hsa_ext_image_get_capability)(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    uint32_t *capability_mask);

  hsa_status_t (*hsa_ext_image_data_get_info)(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_info_t *image_data_info);

  hsa_status_t (*hsa_ext_image_create)(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_t *image);

  hsa_status_t (*hsa_ext_image_destroy)(
    hsa_agent_t agent,
    hsa_ext_image_t image);

  hsa_status_t (*hsa_ext_image_copy)(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    const hsa_dim3_t* src_offset,
    hsa_ext_image_t dst_image,
    const hsa_dim3_t* dst_offset,
    const hsa_dim3_t* range);

  hsa_status_t (*hsa_ext_image_import)(
    hsa_agent_t agent,
    const void *src_memory,
    size_t src_row_pitch,
    size_t src_slice_pitch,
    hsa_ext_image_t dst_image,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_image_export)(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    void *dst_memory,
    size_t dst_row_pitch,
    size_t dst_slice_pitch,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_image_clear)(
    hsa_agent_t agent,
    hsa_ext_image_t image,
    const void* data,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_sampler_create)(
    hsa_agent_t agent,
    const hsa_ext_sampler_descriptor_t *sampler_descriptor,
    hsa_ext_sampler_t *sampler);

  hsa_status_t (*hsa_ext_sampler_destroy)(
    hsa_agent_t agent,
    hsa_ext_sampler_t sampler);

} hsa_ext_images_1_00_pfn_t;

#define hsa_ext_images_1

/**
 * @brief The function pointer table for the images v1 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_images_1_pfn_s {

  hsa_status_t (*hsa_ext_image_get_capability)(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    uint32_t *capability_mask);

  hsa_status_t (*hsa_ext_image_data_get_info)(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_info_t *image_data_info);

  hsa_status_t (*hsa_ext_image_create)(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_t *image);

  hsa_status_t (*hsa_ext_image_destroy)(
    hsa_agent_t agent,
    hsa_ext_image_t image);

  hsa_status_t (*hsa_ext_image_copy)(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    const hsa_dim3_t* src_offset,
    hsa_ext_image_t dst_image,
    const hsa_dim3_t* dst_offset,
    const hsa_dim3_t* range);

  hsa_status_t (*hsa_ext_image_import)(
    hsa_agent_t agent,
    const void *src_memory,
    size_t src_row_pitch,
    size_t src_slice_pitch,
    hsa_ext_image_t dst_image,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_image_export)(
    hsa_agent_t agent,
    hsa_ext_image_t src_image,
    void *dst_memory,
    size_t dst_row_pitch,
    size_t dst_slice_pitch,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_image_clear)(
    hsa_agent_t agent,
    hsa_ext_image_t image,
    const void* data,
    const hsa_ext_image_region_t *image_region);

  hsa_status_t (*hsa_ext_sampler_create)(
    hsa_agent_t agent,
    const hsa_ext_sampler_descriptor_t *sampler_descriptor,
    hsa_ext_sampler_t *sampler);

  hsa_status_t (*hsa_ext_sampler_destroy)(
    hsa_agent_t agent,
    hsa_ext_sampler_t sampler);

  hsa_status_t (*hsa_ext_image_get_capability_with_layout)(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    hsa_ext_image_data_layout_t image_data_layout,
    uint32_t *capability_mask);

  hsa_status_t (*hsa_ext_image_data_get_info_with_layout)(
    const hsa_ext_image_descriptor_t *image_descriptor,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_layout_t image_data_layout,
    size_t image_data_row_pitch,
    size_t image_data_slice_pitch,
    hsa_ext_image_data_info_t *image_data_info);

  hsa_status_t (*hsa_ext_image_create_with_layout)(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_data_layout_t image_data_layout,
    size_t image_data_row_pitch,
    size_t image_data_slice_pitch,
    hsa_ext_image_t *image);

} hsa_ext_images_1_pfn_t;
/** @} */
    
/** \defgroup ext-performance-counters Profiling performance counters
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 *
 * @remark Additions to hsa_status_t
 */
enum {
  /**
   * An operation was attempted on a session which is in an invalid state for that operation, i.e. attempting to enable or disable a counter during a session, or attempting to start a non-enabled session context, or attempting to stop a non-running session, or attempting to enable or disable a session which has already been enabled or disabled.
   */
    HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE = 0x4000,
  /**
   * An attempt was made to sample a counter in an invalid context.
   */
    HSA_EXT_STATUS_ERROR_INVALID_SAMPLING_CONTEXT = 0x4001,

  /**
   * An attempt was made to stop a session at a point in which the counters cannot be stopped by the system.
   */
    HSA_EXT_STATUS_ERROR_CANNOT_STOP_SESSION = 0x4002,
};

/**
 * @brief Performance counter types
 */
typedef enum {
    /**
     * This performance counter's value is an unsigned 32 bit integer.
     */
    HSA_EXT_PERF_COUNTER_TYPE_UINT32 = 0,
    /**
     * This performance counter's value is an unsigned 64 bit integer.
     */
    HSA_EXT_PERF_COUNTER_TYPE_UINT64 = 1,

    /**
     * This performance counter's value is a float.
     */
    HSA_EXT_PERF_COUNTER_TYPE_FLOAT = 2,

    /**
     * This performance counter's value is a double.
     */
    HSA_EXT_PERF_COUNTER_TYPE_DOUBLE = 3

} hsa_ext_perf_counter_type_t;


/**
 * @brief System element which a performance counter is associated with
 */
typedef enum {
    /**
     * This performance counter is associated with an agent.
     */
    HSA_EXT_PERF_COUNTER_ASSOC_AGENT_NODE = 1,

    /**
     * This performance counter is associated with a memory region.
     */
    HSA_EXT_PERF_COUNTER_ASSOC_MEMORY_NODE = 2,

    /**
     * This performance counter is associated with a cache.
     */
    HSA_EXT_PERF_COUNTER_ASSOC_CACHE_NODE = 3,
    
    /**
     * This performance counter is associated with a queue.
     */
    HSA_EXT_PERF_COUNTER_ASSOC_QUEUE = 4,
    
    /**
     * This performance counter is associated with the whole system.
     */
    HSA_EXT_PERF_COUNTER_ASSOC_SYSTEM = 5,
} hsa_ext_perf_counter_assoc_t;

/**
 * @brief Granularity of a performance counter
 */
typedef enum {
    /**
     * This performance counter applies to the whole system.
     */
    HSA_EXT_PERF_COUNTER_GRANULARITY_SYSTEM = 0,
    /**
     * This performance counter applies to a single process.
     */
    HSA_EXT_PERF_COUNTER_GRANULARITY_PROCESS = 1,

    /**
     * This performance counter applies to a single HSA kernel dispatch.
     */
    HSA_EXT_PERF_COUNTER_GRANULARITY_DISPATCH = 2,
} hsa_ext_perf_counter_granularity_t;

/**
 * @brief Persistence of a performance counter's value
 */
typedef enum {
    /**
     * This performance counter resets when a session begins.
     */
    HSA_EXT_PERF_COUNTER_VALUE_PERSISTENCE_RESETS = 0,

    /**
     * This performance counter does not reset when a session begins.
     */
    HSA_EXT_PERF_COUNTER_VALUE_PERSISTENCE_PERSISTS = 1
} hsa_ext_perf_counter_value_persistence_t;

/**
 * @brief The type of value which the performance counter exposes.
 */
typedef enum {
    /**
     * The value is a generic integer (e.g. a counter or a value explained by the performance counter description).
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_GENERIC = 0,

    /**
     * The value is a percentage
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_PERCENTAGE = 1,
    
    /**
     * The value is measured in Watts
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_WATTS = 2,

    /**
     * The value is measured in Milliwatts
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_MILLIWATTS = 3,

    /**
     * The value is measured in Bytes
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_BYTES = 4,

    /**
     * The value is measured in Kilobytes
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_KILOBYTES = 5,

    /**
     * The value is measured in Kilobits per Second
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_KBPS = 6,

    /**
     * The value is measured in Celsius
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_CELSIUS = 7,

    /**
     * The value is measured in Fahrenheit
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_FAHRENHEIT = 8,

    /**
     * The value is measured in Milliseconds
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_MILLISECONDS = 9,


    /**
     * Agents can have vendor-defined types for their performance counter values. This marks the lowest value of the range in which they can be defined.
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_AGENT_SPECIFIC_LOW = 128,

    /**
     * Agents can have vendor-defined types for their performance counter values. This marks the highest value of the range in which they can be defined.
     */
    HSA_EXT_PERF_COUNTER_VALUE_TYPE_AGENT_SPECIFIC_HIGH = 255

} hsa_ext_perf_counter_value_type_t;

/**
 * @brief Performance counter attributes.
 */

typedef enum {
    /**
     * The length of the counter name. The type of this attribute is uint32_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_NAME_LENGTH = 0,
    
    /**
     * Performance counter name. This name is vendor-specified. Values retrieved from performance counters with the same or similar names are not directly comparable unless specified in external documentation. Names are not necessarily unique in a system. The type of this attribute is a NUL-terminated character array with the length equal to the value of ::HSA_EXT_PERF_COUNTER_INFO_NAME_LENGTH attribute.
     */
    HSA_EXT_PERF_COUNTER_INFO_NAME = 1,

    /**
     * The length of the counter description. The type of this attribute is uint32_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_DESCRIPTION_LENGTH = 2,    

    /**
     * Performance counter description. This description is vendor-specified. Values retrieved from performance counters with the same or similar descriptions are not directly comparable unless specified in external documentation. The type of this attribute is a NUL-terminated character array with the length equal to the value of ::HSA_EXT_PERF_COUNTER_INFO_DESCRIPTION_LENGTH attribute.
     */
    HSA_EXT_PERF_COUNTER_INFO_DESCRIPTION = 3,
    /**
     * Performance counter type. The type of this attribute is ::hsa_ext_perf_counter_type_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_TYPE = 4,
    /**
     * Indicates whether the performance counter supports sampling while a session is running. The type of this attribute is bool.
     */
    HSA_EXT_PERF_COUNTER_INFO_SUPPORTS_ASYNC = 5,

    /**
     * Performance counter granularity. The type of this attribute is ::hsa_ext_perf_counter_granularity_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_GRANULARITY = 6,
    /**
     * The persistence of value represented by this counter. The type of this attribute is ::hsa_ext_perf_counter_value_persistence_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_VALUE_PERSISTENCE = 7,

    /**
     * The type of value represented by this counter. The type of this attribute is ::hsa_ext_perf_counter_value_type_t.
     */
    HSA_EXT_PERF_COUNTER_INFO_VALUE_TYPE = 8

} hsa_ext_perf_counter_info_t;

/**
 * @brief An opaque handle to a profiling session context, which is used to represent a set of enabled performance counters.
 */
typedef struct hsa_ext_perf_counter_session_ctx_s {
  /**
   * Opaque handle.
   */
  uint64_t handle;
} hsa_ext_perf_counter_session_ctx_t;

/**
 * @brief Initialize the performance counter system. 
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_ALREADY_INITIALIZED The performance counter system has already been initialized and has not been shut down with ::hsa_ext_perf_counter_shut_down.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_init();

/**
 * @brief Shut down the performance counter system.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The performance counter system has not been initialized.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_shut_down();


/**
 * @brief Get the number of counters available in the entire system.
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the query.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_get_num(
    uint32_t* result);

/**
 * @brief Get the current value of an attribute of a profiling counter.
 *
 * @param[in] counter_idx Performance counter index.
 * Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[in] attribute Attribute to query.
 *
 * @param[out] value Pointer to an application-allocated buffer where to store the value of the attribute.
 * If the buffer passed by the application is not large enough to hold the value of the attribute,
 * the behavior is undefined.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p attribute is an invalid performance counter attribute, or @p value is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_get_info(
    uint32_t counter_idx,
    hsa_ext_perf_counter_info_t attribute,
    void* value);

/**
 * @brief Iterate the constructs associated with the given performance counter, and invoke an application-defined callback on each iteration.
 *
 * @details This is not part of ::hsa_ext_perf_counter_get_info as counters can be associated with more than one system component.
 *
 * @param[in] counter_idx Performance counter index.
 * Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[in] callback Callback to be invoked once per construct associated with the performance counter. The HSA runtime passes three arguments to the callback: the associated construct's type, the associated construct's id, and the application data. The semantics of the id depends on the construct type. If the type is an agent, memory region, or cache, the id is an opaque handle to an agent, memory region, or cache, respectively. If the type is a queue, the id is a queue id. If the type is the whole system, the id is 0. If @p callback returns a status
 * other than ::HSA_STATUS_SUCCESS for a particular iteration, the traversal
 * stops and ::hsa_ext_perf_counter_iterate_associations returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
 */    
hsa_status_t HSA_API hsa_ext_perf_counter_iterate_associations(
    uint32_t counter_idx,
    hsa_status_t (*callback)(hsa_ext_perf_counter_assoc_t assoc_type, uint64_t assoc_id,
                             void* data),
    void* data);
    

/**
 * @brief Create a session context. This should be destroyed with a call to ::hsa_ext_perf_counter_session_context_destroy.
 *
 * @param[out] ctx Memory location where the HSA runtime stores the newly created session context handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p ctx is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_context_create(
    hsa_ext_perf_counter_session_ctx_t* ctx);

/**
 * @brief Destroy a session context.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has already been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_context_destroy(
    hsa_ext_perf_counter_session_ctx_t ctx);

/**
 * @brief Enable sampling for the performance counter at the given index. Calls to ::hsa_ext_perf_counter_session_start between this call and a corresponding successful ::hsa_ext_perf_counter_disable call will cause this performance counter to be populated.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index.
 * Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 * If the specified counter is already enabled, this function has no effect.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * Attempt to enable performance counter during a profiling session.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_enable(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx);

/**
 * @brief Disable sampling for the performance counter at the given index. Calls to ::hsa_ext_perf_counter_session_start will no longer populate this performance counter until the corresponding call to ::hsa_ext_perf_counter_enable is successfully executed.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index.
 * Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 * If the specified counter is already disabled, this function has no effect.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * Attempt to disable performance counter during a profiling session.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_disable(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx);


/**
 * @brief Check if the performance counter at the given index is currently enabled.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index.
 * Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[out] enabled Pointer to a memory location where the HSA runtime stores the result of the check. The result is true if the performance counter at the given index is currently enabled and false otherwise.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p enabled is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_is_enabled(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    bool* enabled);

/**
 * @brief Check if the set of currently enabled performance counters in a given session context can be sampled in a single profiling session. This call does not enable or disable any performance counters; the client is responsible for discovering a valid set.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 * The result is true if the enabled performance counter set can be sampled in a single profiling session and false otherwise.
 * If there are no profiling counters enabled, the result is true.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_context_valid(
    hsa_ext_perf_counter_session_ctx_t ctx,
    bool* result);

 /**
 * @brief Check if the given set of session contexts can be enabled and executed concurrently. 
 *
 * @param[in] ctxs Pointer to an array of ::hsa_ext_perf_counter_session_ctx_t objects of size @p n_ctxs
 *
 * @param[in] n_ctxs The size of the @p ctxs array.
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 * The result is true if the sessions can be executed concurrently and false otherwise.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p ctxs is NULL or @p result is NULL.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_context_set_valid(
    hsa_ext_perf_counter_session_ctx_t* ctxs,
    size_t n_ctxs,
    bool* result);

/**
 * @brief Enable a profiling session. Performance counters enabled through calls to ::hsa_ext_perf_counter_enable without an intervening call to ::hsa_ext_perf_counter_disable for the same counter index will be readied for counting and sampling. Performance counters which have the attribute ::HSA_EXT_PERF_COUNTER_VALUE_PERSISTENCE_RESETS for ::HSA_EXT_PERF_COUNTER_INFO_VALUE_PERSISTENCE will reset to 0.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INCOMPATIBLE_ARGUMENTS The set of enabled performance counters is invalid for reading in a single profiling session, or there is a session currently enabled which cannot be executed concurrently with the given session context.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * This session context has already been enabled with a call to ::hsa_ext_perf_counter_session_enable without being disabled with a call to ::hsa_ext_perf_counter_session_disable
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_enable(
    hsa_ext_perf_counter_session_ctx_t ctx);

/**
 * @brief Disable a profiling session. Reading performance counters for that session is no longer valid.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * This session context has not been enabled with a call to ::hsa_ext_perf_counter_session_enable, or has already been disabled with a call to ::hsa_ext_perf_counter_session_disable without being enabled again.
 *
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_disable(
    hsa_ext_perf_counter_session_ctx_t ctx);

/**
 * @brief Start a profiling session. Performance counters enabled through calls to ::hsa_ext_perf_counter_enable without an intervening call to ::hsa_ext_perf_counter_disable for the same counter index will count until a successful call to ::hsa_ext_perf_counter_session_stop with the same session context.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INCOMPATIBLE_ARGUMENTS The set of enabled performance counters is invalid for reading in a single profiling session, or there is a session currently running which cannot be executed concurrently with the given session context.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * This session context has not been enabled with a call to ::hsa_ext_perf_counter_session_enable, or has since been disabled with a call to ::hsa_ext_perf_counter_session_disable.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_start(
    hsa_ext_perf_counter_session_ctx_t ctx);

 /**
 * @brief Stop a profiling session, freezing the counters which were enabled. Reading of performance counters which do not support in-session reading is now valid until a call to ::hsa_ext_perf_counter_session_disable with the same session context. If the session is already stopped, this function has no effect. The session can be started again with a call to ::hsa_ext_perf_counter_session_start; the state of the counters will be carried over from the point at which this function was called.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SESSION_STATE
 * The session has already been ended or has not been started.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_STOP_SESSION The session cannot be stopped by the system at this time. 
 *
 */
hsa_status_t HSA_API hsa_ext_perf_counter_session_stop(
    hsa_ext_perf_counter_session_ctx_t ctx);

 /**
 * @brief Read the value of a given performance counter as a uint32_t. The value type of a performance counter can be queried using ::hsa_ext_perf_counter_get_info.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index. Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL or @p counter_idx points to a performance counter whose data type is not uint32_t.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SAMPLING_CONTEXT
 * The given performance counter cannot be sampled at this time. If the counter supports sampling whilst the session is running, the session must have been enabled with a call to ::hsa_ext_perf_counter_session_enable and not have been since disabled with a call to ::hsa_ext_perf_counter_session_disable. If the counter does not support sampling whilst the session is running, the session must additionally have been stopped with a call to ::hsa_ext_perf_counter_session_stop.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_read_uint32(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    uint32_t* result);

 /**
 * @brief Read the value of a given performance counter as a uint64_t. The value type of a performance counter can be queried using ::hsa_ext_perf_counter_get_info.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index. Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL or @p counter_idx points to a performance counter whose data type is not uint64_t.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SAMPLING_CONTEXT
 * The given performance counter cannot be sampled at this time. If the counter supports sampling whilst the session is running, the session must have been enabled with a call to ::hsa_ext_perf_counter_session_enable and not have been since disabled with a call to ::hsa_ext_perf_counter_session_disable. If the counter does not support sampling whilst the session is running, the session must additionally have been stopped with a call to ::hsa_ext_perf_counter_session_stop.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_read_uint64(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    uint64_t* result);

/**
 * @brief Read the value of a given performance counter as a float. The value type of a performance counter can be queried using ::hsa_ext_perf_counter_get_info.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index. Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL or @p counter_idx points to a performance counter whose data type is not float.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SAMPLING_CONTEXT
 * The given performance counter cannot be sampled at this time. If the counter supports sampling whilst the session is running, the session must have been enabled with a call to ::hsa_ext_perf_counter_session_enable and not have been since disabled with a call to ::hsa_ext_perf_counter_session_disable. If the counter does not support sampling whilst the session is running, the session must additionally have been stopped with a call to ::hsa_ext_perf_counter_session_stop.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_read_float(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    float* result);

/**
 * @brief Read the value of a given performance counter as a double. The value type of a performance counter can be queried using ::hsa_ext_perf_counter_get_info.
 *
 * @param[in] ctx Session context. Using an object which has not been created using ::hsa_ext_perf_counter_session_context_create or has been destroyed with ::hsa_ext_perf_counter_session_context_destroy is undefined behaviour.
 *
 * @param[in] counter_idx Performance counter index. Must have a value between 0 (inclusive) and the value returned by ::hsa_ext_perf_counter_get_num (not inclusive).
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the check.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_INDEX @p counter_idx is out-of-bounds.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL or @p counter_idx points to a performance counter whose data type is not double.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_SAMPLING_CONTEXT
 * The given performance counter cannot be sampled at this time. If the counter supports sampling whilst the session is running, the session must have been enabled with a call to ::hsa_ext_perf_counter_session_enable and not have been since disabled with a call to ::hsa_ext_perf_counter_session_disable. If the counter does not support sampling whilst the session is running, the session must additionally have been stopped with a call to ::hsa_ext_perf_counter_session_stop.
 */
hsa_status_t HSA_API hsa_ext_perf_counter_read_double(
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    double* result);

#define hsa_ext_perf_counter_1

/**
 * @brief The function pointer table for the performance counter v1 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_perf_counter_1_pfn_s {
  hsa_status_t (*hsa_ext_perf_counter_init) ();
    
  hsa_status_t (*hsa_ext_perf_counter_shut_down) ();
    
  hsa_status_t (*hsa_ext_perf_counter_get_num) (
    uint32_t* result);

  hsa_status_t (*hsa_ext_perf_counter_get_info) (
    uint32_t counter_idx,
    hsa_ext_perf_counter_info_t attribute,
    void* value);

  hsa_status_t (*hsa_ext_perf_counter_iterate_associations) (
    uint32_t counter_idx,
    hsa_status_t (*callback)(hsa_ext_perf_counter_assoc_t assoc_type, uint64_t assoc_id,
                             void* data),
    void* data);
    

  hsa_status_t (*hsa_ext_perf_counter_session_context_create) (
    hsa_ext_perf_counter_session_ctx_t* ctx);

  hsa_status_t (*hsa_ext_perf_counter_session_context_destroy) (
    hsa_ext_perf_counter_session_ctx_t ctx);

  hsa_status_t (*hsa_ext_perf_counter_enable) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx);

  hsa_status_t (*hsa_ext_perf_counter_disable) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx);

  hsa_status_t (*hsa_ext_perf_counter_is_enabled) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    bool* enabled);

  hsa_status_t (*hsa_ext_perf_counter_session_context_valid) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    bool* result);

  hsa_status_t (*hsa_ext_perf_counter_session_context_set_valid) (
    hsa_ext_perf_counter_session_ctx_t* ctxs,
    size_t n_ctxs,
    bool* result);

  hsa_status_t (*hsa_ext_perf_counter_session_enable) (
    hsa_ext_perf_counter_session_ctx_t ctx);

  hsa_status_t (*hsa_ext_perf_counter_session_disable) (
    hsa_ext_perf_counter_session_ctx_t ctx);

  hsa_status_t (*hsa_ext_perf_counter_session_start) (
    hsa_ext_perf_counter_session_ctx_t ctx);

  hsa_status_t (*hsa_ext_perf_counter_session_stop) (
    hsa_ext_perf_counter_session_ctx_t ctx);

  hsa_status_t (*hsa_ext_perf_counter_read_uint32) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    uint32_t* result);

  hsa_status_t (*hsa_ext_perf_counter_read_uint64) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    uint64_t* result);

  hsa_status_t (*hsa_ext_perf_counter_read_float) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    float* result);

  hsa_status_t (*hsa_ext_perf_counter_read_double) (
    hsa_ext_perf_counter_session_ctx_t ctx,
    uint32_t counter_idx,
    double* result);
} hsa_ext_perf_counter_1_pfn_t;

/** @} */

/** \defgroup ext-event-system Profiling event system
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 *
 * @remark Additions to hsa_status_t
 */
enum {
  /**
   * The HSA runtime was not initialized with the ::hsa_ext_profiling_event_init function.
   */
    HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED = 0x5000,

  /**
   * The HSA runtime has already been initialized through a call to ::hsa_init or ::hsa_ext_profiling_event_init
   */
    HSA_EXT_STATUS_ERROR_ALREADY_INITIALIZED = 0x5001,

  /**
   * An event was requested from a buffer which has no events remaining.
   */
    HSA_EXT_STATUS_ERROR_OUT_OF_EVENTS = 0x5002,

  /**
   * An HSAIL or application event was triggered which hasn't been registered yet.
   */
    HSA_EXT_STATUS_ERROR_EVENT_NOT_REGISTERED = 0x5003,

  /**
   * The producer mask was updated or some specific producers were enabled but the requested producers cannot be enabled at this point, or don't support profiling events.
   */
    HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS = 0x5004,
};

/**
 * @brief Possible event producers to collect events from.
 */
typedef enum {
    /**
     * Do not collect events from any event producers.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_NONE = 0,

    /**
     * Collect events from agent nodes.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_AGENT= 1,

    /**
     * Collect events from memory nodes.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_MEMORY = 2,

    /**
     * Collect events from cache nodes.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_CACHE = 4,

    /**
     * Collect events from applications.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_APPLICATION = 8,

    /**
     * Collect events from signals.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_SIGNAL = 16,

    /**
     * Collect events from the runtime API.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_RUNTIME_API = 32,

    /**
     * Collect events from all producers.
     */
    HSA_EXT_PROFILING_EVENT_PRODUCER_ALL = 63,
} hsa_ext_profiling_event_producer_t;

/**
 * @brief A fixed-size type used to represent ::hsa_ext_profiling_event_producer_t constants.
 */
typedef uint32_t hsa_ext_profiling_event_producer32_t;

/**
 * The type of the value which a metadata field holds.
 */
typedef enum {
    /**
     * The value is an unsigned 32 bit integer
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_UINT32 = 0,

    /**
     * The value is an unsigned 64 bit integer
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_UINT64 = 1,

    /**
     * The value is a signed 32 bit integer.
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_INT32 = 2,
    
    /**
     * The value is an signed 64 bit integer.
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_INT64 = 3,

    /**
     * The value is a 32 bit floating point value.
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_FLOAT = 4,

    /**
     * The value is a 64 bit floating point value.
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_DOUBLE = 5,

    /**
     * The value is a NUL-terminated C-like string.
     */
    HSA_EXT_PROFILING_EVENT_METADATA_TYPE_STRING = 6
} hsa_ext_profiling_event_metadata_type_t;

/**
 * @brief A fixed-size type used to represent ::hsa_ext_profiling_event_metadata_type_t constants.
 */
typedef uint32_t hsa_ext_profiling_event_metadata_type32_t;
    

/**
 * A profiling event
 */
typedef struct hsa_ext_profiling_event_s {
    /**
     * The type of the producer.
     */
    hsa_ext_profiling_event_producer32_t producer_type;

    /**
     * The identifier for the producer. This should be interpreted in a way dependent on the producer type.
     *
     *
     */
    uint64_t producer_id;

    /**
     * Producer-local event id.
     */
    uint64_t event_id;

    /**
     * Name of the event. A NUL-terminated string.
     */
    const char* name;
    /**
     * Length of the name in chars.
     */
    size_t name_length;
    
    /**
     * Description of the event. A NUL-terminated string.
     */
    const char* description;
    /**
     * Length of the description in chars.
     */
    size_t description_length;

    /**
     * HSA system timestamp at which the event was triggered.
     */
    uint64_t timestamp;

    /**
     * Pointer to the metadata associated with the event. The pointee should have the same structure as if a C struct was defined with the event metadata as member data, defined in the same order in which they were specified to ::hsa_ext_profiling_event_register_application_event.
     */
    void* metadata;

    /**
     * Size of the metadata in bytes.
     */    
    size_t metadata_size;
} hsa_ext_profiling_event_t;

/**
 * @brief Description of a metadata field
 */
typedef struct hsa_ext_profiling_event_metadata_field_desc_s {
    /**
     * Name of the metadata entry. A NUL-terminated string.
     */
    const char* data_name;

    /**
     * Length of data_name in chars. Does not include NUL-terminator.
     */
    size_t name_length;

    /**
     * Type of the metadata
     */
    hsa_ext_profiling_event_metadata_type32_t metadata_type;

} hsa_ext_profiling_event_metadata_field_desc_t;

/**
 * @brief Initialize the event producer with the given identifier and type for producing profiling events. Must be called prior to ::hsa_ext_profiling_event_init. Calling this function while the runtime is not in the configuration state is undefined behaviour.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_RUNTIME_STATE (Optional) The HSA runtime is not in the configuration state.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_ALREADY_INITIALIZED The profiling events system has already been initialized and has not been shut down with ::hsa_ext_profiling_event_shut_down.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS The producer requested cannot be initialized for profiling events.
 */
hsa_status_t hsa_ext_profiling_event_init_producer(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);

/**
 * @brief Initialize all event producers of the given type for producing profiling events. Must be called prior to ::hsa_ext_profiling_event_init. Calling this function while the runtime is not in the configuration state is undefined behaviour.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_RUNTIME_STATE (Optional) The HSA runtime is not in the configuration state.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_ALREADY_INITIALIZED The profiling events system has already been initialized and has not been shut down with ::hsa_ext_profiling_event_shut_down.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS Some of the producers requested cannot be initialized for profiling events.
 */
hsa_status_t hsa_ext_profiling_event_init_all_of_producer_type(
    hsa_ext_profiling_event_producer_t producer_type);

    
/**
 * @brief Initialize the profiling events system. Calling this function while the runtime is not in the configuration state is undefined behaviour.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_RUNTIME_STATE (Optional) The HSA runtime is not in the configuration state.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_ALREADY_INITIALIZED The profiling events system has already been initialized and has not been shut down with ::hsa_ext_profiling_event_shut_down.
 */
hsa_status_t hsa_ext_profiling_event_init();

/**
 * @brief Shut down the profiling events system.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_shut_down();
    
/**
 * @brief Register a new application event producer with a given name and description.
 *
 * @param[in] name A NUL-terminated string containing the name. Cannot be NULL. Does not need to be unique.
 *
 * @param[in] description A NUL-terminated string containing the description. May be NULL.
 *
 * @param[out] app_producer_id Pointer to a memory location where the HSA runtime stores the unique identifier for this event producer.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p name is NULL, or @p app_producer_id is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_register_application_event_producer(
    const char* name,
    const char* description,
    uint64_t* app_producer_id);

/**
 * @brief Deregister an application event producer.
 *
 * @details Deregistering an application event producer before all triggered events with that producer been destroyed is undefined behaviour.
 *
 * @param[in] app_producer_id Application event producer ID.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p app_producer_id is not currently registered or is id 0, which is reserved for HSAIL events.
 */
hsa_status_t hsa_ext_profiling_event_deregister_application_event_producer(
    uint64_t app_producer_id);    

/**
 * @brief Iterate over the available application event producers, and invoke an
 * application-defined callback on every iteration.
 *
 * @details This can be used to retrieve registered producers to display to profiler users and/or filter events.
 *
 * @param[in] callback Callback to be invoked once per producer. The HSA
 * runtime passes two arguments to the callback, the producer id and the
 * application data.  If @p callback returns a status other than
 * ::HSA_STATUS_SUCCESS for a particular iteration, the traversal stops and
 * ::hsa_ext_profiling_event_iterate_application_event_producers returns that status value.
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
hsa_status_t hsa_ext_profiling_event_iterate_application_event_producers(
    hsa_status_t (*callback)(uint64_t app_producer_id, void* data),
    void* data);


/**
 * @brief Get the name of an event producer from its identifier and type.
 *
 * @details If @p producer_type is ::HSA_EXT_PROFILING_EVENT_PRODUCER_APPLICATION and @p producer_id is 0, @p name will be set to "HSAIL"
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @param[out] name A NUL-terminated string containing the event producer name.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p name is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS The specified component does not produce profiling events.
 */
hsa_status_t hsa_ext_profiling_event_producer_get_name(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    const char** name);

/**
 * @brief Get the description of an application event producer from its identifier.
 *
 * @details If @p producer_type is ::HSA_EXT_PROFILING_EVENT_PRODUCER_APPLICATION and @p producer_id is 0, @p description will be set to "Produces events from HSAIL kernels."
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @param[out] description A NUL-terminated string containing the event producer description.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p description is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS The specified component does not produce profiling events.
 */
hsa_status_t hsa_ext_profiling_event_producer_get_description(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    const char** description);

/**
 * @brief Check if a given prospective producer supports profiling events.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @param[out] result Pointer to a memory location where the HSA runtime stores the result of the query.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_producer_supports_events(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    bool* result);    

/**
 * @brief Enable event collection from the event producer with the given identifier and type.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS The producer requested cannot be enabled at this point, or the producer requested does not support profiling events.
 */
hsa_status_t hsa_ext_profiling_event_enable_for_producer(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);

/**
 * @brief Disable event collection from the event producer with the given type and identifier.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @param[in] producer_id Event producer identifier. See section \\ref{prodid} for details.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_disable_for_producer(
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);    

/**
 * @brief Enable event collection from all registered event producers of a given type.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_CANNOT_USE_PRODUCERS Some of the producers requested cannot be enabled at this point, or the producer requested does not support profiling events.
 */
hsa_status_t hsa_ext_profiling_event_enable_all_for_producer_type(
    hsa_ext_profiling_event_producer_t producer_type);

/**
 * @brief Disable event collection from all registered event producers of a given type.
 *
 * @param[in] producer_type Type of the event producer.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_disable_all_for_producer_type(
    hsa_ext_profiling_event_producer_t producer_type);

/**
 * @brief Provide a hint to the runtime for how many bytes to reserve for buffering events.
 *
 * @param[in] size_hint Suggested number of bytes to reserve for events.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_set_buffer_size_hint(
    size_t size_hint);

/**
 * @brief Register a new application profiling event.
 *
 * @param[in] app_producer_id Application event producer identifier.
 *
 * @param[in] event_id A producer-specific event identifier.
 *
 * @param[in] name A NUL-terminated string containing the name. May be NULL.
 *
 * @param[in] name_length The length of @p name in chars.
 *
 * @param[in] description A NUL-terminated string containing the description. May be NULL.
 *
 * @param[in] description_length The length of @p description in chars.
 *
 * @param[in] metadata_field_descriptions Pointer to the first element of an array containing descriptions of the metadata fields. May be NULL.
 *
 * @param[in] n_metadata_fields The number of metadata fields.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */
hsa_status_t hsa_ext_profiling_event_register_application_event(
    uint64_t app_producer_id,
    uint64_t event_id,
    const char* name,
    size_t name_length,
    const char* description,
    size_t description_length,
    hsa_ext_event_metadata_field_desc_t* metadata_field_descriptions,
    size_t n_metadata_fields);

/**
 * @brief Deregister an application event.
 *
 * @details Deregistering an application event before all triggered events with that producer and Id have been destroyed is undefined behaviour.
 *
 * @param[in] app_producer_id Application event producer id. 
 *
 * @param[in] event_id Event id.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENT_NOT_REGISTERED The @p event_id has not been registered with ::hsa_ext_profiling_event_register_application_event
 */
hsa_status_t hsa_ext_profiling_event_deregister_application_event(
    uint64_t app_producer_id,
    uint64_t event_id);

/**
 * @brief Trigger a profiling event with an ID and any associated metadata.
 *
 * @param[in] app_producer_id Application event producer id
 *
 * @param[in] event_id Producer-specific event identifier.
 *
 * @param[in] metadata A pointer to the metadata, which should have the same structure as if a C struct was defined with the event metadata as member data, defined in the same order in which they were specified to ::hsa_ext_profiling_event_register_application_event. May be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENT_NOT_REGISTERED The @p event_id has not been registered with ::hsa_ext_profiling_event_register_application_event
 */
hsa_status_t hsa_ext_profiling_event_trigger_application_event(
    uint64_t app_producer_id,
    uint64_t event_id,
    void* metadata);

/**
 * @brief Retrieve the head event.
 *
 * @param[out] event Pointer to a memory location where the HSA runtime stores the event.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p event is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_OUT_OF_EVENTS There are no events remaining in this buffer.
 */
hsa_status_t hsa_ext_profiling_event_get_head_event(
    hsa_ext_profiling_event_t* event);

/**
 * @brief Destroy the head event, making the succeeding event the new head if one exists.
 *
 * @param[in] event Event retrieved from ::hsa_ext_profiling_event_get_head_event
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p event is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 */    
hsa_status_t hsa_ext_profiling_event_destroy_head_event(
    hsa_ext_profiling_event_t* event);

/**
 * @brief Get metadata descriptions for the given producer and event ids.
 *
 * @param[in] producer_id Producer id. See section \\ref{prodid} for details.
 *
 * @param[in] event_id Event id. 
 *
 * @param[out] metadata_descs Pointer to a memory location where the HSA runtime stores an array of metadata field descriptions.

 * @param[out] n_descs Pointer to a memory location where the HSA runtime stores the number of metadata fields.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENTS_NOT_INITIALIZED The profiling events system has not been initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_EVENT_NOT_REGISTERED The @p event_id has not been registered with ::hsa_ext_profiling_event_register_application_event.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p metadata_descs is NULL, or @p n_descs is NULL.
 */    
hsa_status_t hsa_ext_profiling_event_get_metadata_field_descs(
    uint64_t producer_id,
    uint64_t event_id,
    hsa_ext_event_metadata_field_desc_t** metadata_descs,
    size_t* n_descs);

#define hsa_ext_profiling_event_1


/**
 * @brief The function pointer table for the profiling event v1 extension. Can be returned by ::hsa_system_get_extension_table or ::hsa_system_get_major_extension_table.
 */
typedef struct hsa_ext_profiling_event_1_pfn_s {
  hsa_status_t (*hsa_ext_profiling_event_init_producer) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);

  hsa_status_t (*hsa_ext_profiling_event_init_all_of_producer_type) (
    hsa_ext_profiling_event_producer_t producer_type);

    
  hsa_status_t (*hsa_ext_profiling_event_init) ();

  hsa_status_t (*hsa_ext_profiling_event_shut_down) ();
    
  hsa_status_t (*hsa_ext_profiling_event_register_application_event_producer) (
    const char* name,
    const char* description,
    uint64_t* app_producer_id);

  hsa_status_t (*hsa_ext_profiling_event_deregister_application_event_producer) (
    uint64_t app_producer_id);    

  hsa_status_t (*hsa_ext_profiling_event_iterate_application_event_producers) (
    hsa_status_t (*callback)(uint64_t app_producer_id, void* data),
    void* data);


  hsa_status_t (*hsa_ext_profiling_event_producer_get_name) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    const char** name);

  hsa_status_t (*hsa_ext_profiling_event_producer_get_description) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    const char** description);

  hsa_status_t (*hsa_ext_profiling_event_producer_supports_events) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id,
    bool* result);    

  hsa_status_t (*hsa_ext_profiling_event_enable_for_producer) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);

  hsa_status_t (*hsa_ext_profiling_event_disable_for_producer) (
    hsa_ext_profiling_event_producer_t producer_type,
    uint64_t producer_id);    

  hsa_status_t (*hsa_ext_profiling_event_enable_all_for_producer_type) (
    hsa_ext_profiling_event_producer_t producer_type);

  hsa_status_t (*hsa_ext_profiling_event_disable_all_for_producer_type) (
    hsa_ext_profiling_event_producer_t producer_type);

  hsa_status_t (*hsa_ext_profiling_event_set_buffer_size_hint) (
    size_t size_hint);

  hsa_status_t (*hsa_ext_profiling_event_register_application_event) (
    uint64_t app_producer_id,
    uint64_t event_id,
    const char* name,
    size_t name_length,
    const char* description,
    size_t description_length,
    hsa_ext_event_metadata_field_desc_t* metadata_field_descriptions,
    size_t n_metadata_fields);

  hsa_status_t (*hsa_ext_profiling_event_deregister_application_event) (
    uint64_t app_producer_id,
    uint64_t event_id);

  hsa_status_t (*hsa_ext_profiling_event_trigger_application_event) (
    uint64_t app_producer_id,
    uint64_t event_id,
    void* metadata);

  hsa_status_t (*hsa_ext_profiling_event_get_head_event) (
    hsa_ext_profiling_event_t* event);

  hsa_status_t (*hsa_ext_profiling_event_destroy_head_event) (
    hsa_ext_profiling_event_t* event);

  hsa_status_t (*hsa_ext_profiling_event_get_metadata_field_descs) (
    uint64_t producer_id,
    uint64_t event_id,
    hsa_ext_event_metadata_field_desc_t** metadata_descs,
    size_t* n_descs);

} hsa_ext_profiling_event_1_pfn_t;
    
/** @} */

#ifdef __cplusplus
}
#endif  /*__cplusplus*/
