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


typedef void* BrigModule_t;

/** \defgroup ext-alt-finalizer-extensions Finalization Extensions
 *  @{
 */

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
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
   * Mismatch between a directive in the control directive structure and in
   * the HSAIL kernel.
   */
  HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH = 0x2006
};

/** @} */

/** \defgroup ext-alt-finalizer-program Finalization Program
 *  @{
 */

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
   * Opaque handle.
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
 * after the finalization program has been destroyed.
 *
 * @param[in] program HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
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
 * @brief Finalizer-determined call convention.
 */
typedef enum {
 /**
  * Finalizer-determined call convention.
  */
  HSA_EXT_FINALIZER_CALL_CONVENTION_AUTO = -1
} hsa_ext_finalizer_call_convention_t;

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
 * @param[in] options Vendor-specific options. May be NULL.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The HSA runtime has not been
 * initialized.
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

/** @} */

#define hsa_ext_finalizer_1_00

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

/** \defgroup ext-images Images and Samplers
 *  @{
 */

/**
 * @brief Image handle, populated by ::hsa_ext_image_create. Images
 * handles are only unique within an agent, not across agents.
 *
 */
typedef struct hsa_ext_image_s {
  /**
   * Opaque handle.
   */
    uint64_t handle;

} hsa_ext_image_t;

/**
 * @brief Geometry associated with the HSA image (image dimensions allowed in
 * HSA). The enumeration values match the BRIG type BrigImageGeometry.
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
 * @brief Channel type associated with the elements of an image. See the Image
 * section in the HSA Programming Reference Manual for definitions on each
 * component type. The enumeration values match the BRIG type
 * BrigImageChannelType.
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
 * @brief Channel order associated with the elements of an image. See the
 * Image section in the HSA Programming Reference Manual for definitions on each
 * component order. The enumeration values match the BRIG type
 * BrigImageChannelOrder.
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
 * @brief Image format.
 */
typedef struct hsa_ext_image_format_s {
  /**
    * Channel type.
    */
    hsa_ext_image_channel_type_t channel_type;

   /**
    * Channel order.
    */
    hsa_ext_image_channel_order_t channel_order;
} hsa_ext_image_format_t;

/**
 * @brief Implementation-independent image descriptor.
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
     * Height of the image, in components. Only defined if the geometry is 2D or
     * higher.
     */
    size_t height;
    /**
     * Depth of the image, in components. Only defined if @a geometry is
     * ::HSA_EXT_IMAGE_GEOMETRY_3D. A depth of 0 is same as a depth of 1.
     */
    size_t depth;
    /**
     * Number of images in the image array. Only defined if @a geometry is
     * ::HSA_EXT_IMAGE_GEOMETRY_1DA, ::HSA_EXT_IMAGE_GEOMETRY_2DA, or
     * HSA_EXT_IMAGE_GEOMETRY_2DADEPTH.
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
    * Images of this geometry and format are not supported in the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_NOT_SUPPORTED = 0x0,
   /**
    * Read-only images of this geometry and format are supported by the
    * agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_ONLY = 0x1,
   /**
    * Write-only images of this geometry and format are supported by the
    * agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_WRITE_ONLY = 0x2,
   /**
    * Read-write images of this geometry and format are supported by the
    * agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_WRITE = 0x4,
   /**
    * Images of this geometry and format can be accessed from read-modify-write
    * operations in the agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this geometry and format are guaranteed to have a consistent
    * data layout regardless of how they are accessed by the associated
    * agent.
    */
    HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT = 0x10
} hsa_ext_image_capability_t;

/**
 * @brief Retrieve the supported image capabilities for a given combination of
 * agent, image format and geometry.
 *
 * @param[in] agent Agent to be associated with the image.
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p geometry is not a valid image
 * geometry value, @p image_format is NULL, or @p capability_mask is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_capability(
    hsa_agent_t agent,
    hsa_ext_image_geometry_t geometry,
    const hsa_ext_image_format_t *image_format,
    uint32_t *capability_mask);

/**
 * @brief Agent-specific image size and alignment requirements, populated by
 * ::hsa_ext_image_data_get_info.
 */
typedef struct hsa_ext_image_data_info_s {
  /**
   * Image data size, in bytes.
   */
  size_t size;

  /**
   * Image data alignment, in bytes.
   */
  size_t alignment;

} hsa_ext_image_data_info_t;

/**
 * @brief Retrieve the image data requirements for a given combination of image
 * descriptor, access permission, and agent.
 *
 * @details The optimal image data size and alignment requirements may vary
 * depending on the image attributes specified in @p image_descriptor. Also,
 * different implementation of the HSA runtime may return different requirements
 * for the same input values.
 *
 * The implementation must return the same image data requirements for different
 * access permissions with exactly the same image descriptor as long as
 * ::hsa_ext_image_get_capability reports
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT for the geometry
 * and image format contained in the image descriptor.
 *
 * @param[in] agent Agent to be associated with the image.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] access_permission Image access mode for @a agent.
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
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The agent does
 * not support the image format specified by the descriptor.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The agent does
 * not support the image dimensions specified by the format descriptor.
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
 * @brief Creates a agent-defined image handle from an
 * implementation-independent image descriptor and a agent-specific image
 * data.
 *
 * @details Image created with different access permissions but the same image
 * descriptor can share the same image data if
 * ::HSA_EXT_IMAGE_CAPABILITY_ACCESS_INVARIANT_DATA_LAYOUT is reported by
 * ::hsa_ext_image_get_capability for the image format specified in the image
 * descriptor. Images with a s-form channel order can share the same image data
 * with other images that have the corresponding non-s-form channel order,
 * provided the rest of their image descriptors are identical.
 *
 * If necessary, an application can use image operations (import, export, copy,
 * clear) to prepare the image for the intended use regardless of the access
 * permissions.
 *
 * @param[in] agent agent to be associated with the image.
 *
 * @param[in] image_descriptor Pointer to an image descriptor. Must not be NULL.
 *
 * @param[in] image_data Image data buffer that must have been allocated
 * according to the size and alignment requirements dictated by
 * ::hsa_ext_image_data_get_info.  Must not be NULL.
 *
 * Any previous memory contents are preserved upon creation. The application is
 * responsible for ensuring that the lifetime of the image data exceeds that of
 * all the associated images.
 *
 * @param[in] access_permission Access permission of the image by the
 * agent. The access permission defines how the agent expects to use the
 * image and must match the corresponding HSAIL image handle type. The agent
 * must support the image format specified in @p image_descriptor for the given
 * permission.
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
 * descriptor using the specified access permission.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The HSA runtime cannot create the
 * image because it is out of resources (for example, the agent does not
 * support the creation of more image handles with the given access permission).
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p image_descriptor is NULL, @p
 * image_data is NULL, @p access_permission is not a valid access permission
 * value, or @p image is NULL.
 */
hsa_status_t HSA_API hsa_ext_image_create(
    hsa_agent_t agent,
    const hsa_ext_image_descriptor_t *image_descriptor,
    const void *image_data,
    hsa_access_permission_t access_permission,
    hsa_ext_image_t *image);

/**
 * @brief Destroy an image previously created using ::hsa_ext_image_create.
 *
 * @details Destroying the image handle does not free the associated image data,
 * or modify its contents. The application should not destroy an image while
 * there are references to it queued for execution or currently being used in a
 * kernel.
 *
 * @param[in] agent Agent associated with the image.
 *
 * @param[in] image Image.
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
 * @details The source and destination image formats should match, except if the
 * channel type of one of the images is the standard form of the channel type of
 * the other image. For example, it is allowed to copy a source image with a
 * channel type of HSA_EXT_IMAGE_CHANNEL_ORDER_SRGB to a destination image with
 * a channel type of HSA_EXT_IMAGE_CHANNEL_ORDER_RGB.
 *
 * The source and destination images do not have to be of the same geometry and
 * appropriate scaling is performed by the HSA runtime. It is possible to copy
 * subregions between any combinations of source and destination types, provided
 * that the dimensions of the subregions are the same. For example, it is
 * allowed to copy a rectangular region from a 2D image to a slice of a 3D
 * image.
 *
 * If the source and destination image data overlap, or the combination of
 * offset and range references an out-out-bounds element in any of the images,
 * the behavior is undefined.
 *
 * @param[in] agent Agent associated with both images.
 *
 * @param[in] src_image Source image. The agent associated with the source
 * image must be identical to that of the destination image.
 *
 * @param[in] src_offset Pointer to the offset within the source image where to
 * copy the data from. Must not be NULL.
 *
 * @param[in] dst_image Destination image.
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
    * Dimensions of the image range (in coordinates). The x, y, and z dimensions
    * correspond to width, height, and depth respectively.
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
 * If @p src_row_pitch is smaller than the destination region width (in bytes),
 * then @p src_row_pitch = region width.
 *
 * If @p src_slice_pitch is smaller than the destination region width * region
 * height (in bytes), then @p src_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the source memory or image data memory in the previously created
 * ::hsa_ext_image_create image handle can overlap.  Overlapping of any
 * of the source and destination memory within the import operation produces
 * undefined results.
 *
 * @param[in] agent Agent associated with the image.
 *
 * @param[in] src_memory Source memory. Must not be NULL.
 *
 * @param[in] src_row_pitch Number of bytes in one row of the source memory.
 *
 * @param[in] src_slice_pitch Number of bytes in one slice of the source memory.
 *
 * @param[in] dst_image Destination image.
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
 * If @p dst_row_pitch is smaller than the source region width (in bytes), then
 * @p dst_row_pitch = region width.
 *
 * If @p dst_slice_pitch is smaller than the source region width * region height
 * (in bytes), then @p dst_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the destination memory or image data memory in the previously created
 * ::hsa_ext_image_create image handle can overlap. Overlapping of any of
 * the source and destination memory within the export operation produces
 * undefined results.
 *
 * @param[in] agent Agent associated with the image.
 *
 * @param[in] src_image Source image.
 *
 * @param[in] dst_memory Destination memory. Must not be NULL.
 *
 * @param[in] dst_row_pitch Number of bytes in one row of the destination
 * memory.
 *
 * @param[in] dst_slice_pitch Number of bytes in one slice of the destination
 * memory.
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
 * @brief Clear an image to the specified value.
 *
 * @details Clearing an image does not perform any format conversion and the
 * provided clear data is directly stored regardless of the image format. The
 * lowest bits of the data (number of bits depending on the image component
 * type) stored in the cleared image are based on the image component order.
 *
 * The number of elements in @p data should match the number of access
 * components for the channel order of @p image, as determined by the HSA
 * Programmer's Reference Manual. A single element is required for
 * HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH and
 * HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH_STENCIL, while any other channel order
 * requires 4 elements.
 *
 * Each element in @p data is a 32-bit value. The type of each element
 * should match the access type associated with the channel type of @p image,
 * as determined by the HSA Programmer's Reference Manual:
 *  - HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT8, HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT16, and HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT32 map to int32_t.
 *  - HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT8, HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT16, and HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT32 map to uint32_t.
 *  - Any other channel type maps to a 32-bit float.
 *
 * @param[in] agent Agent associated with the image.
 *
 * @param[in] image Image to be cleared.
 *
 * @param[in] data Clear value array. Specifying a clear value outside of the
 * range that can be represented by an image format results in undefined
 * behavior. Must not be NULL.
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
   * Opaque handle.
   */
    uint64_t handle;
} hsa_ext_sampler_t;

/**
 * @brief Sampler address modes. The sampler address mode describes the
 * processing of out-of-range image coordinates. The values match the  BRIG
 * type BrigSamplerAddressing.
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
   * Clamp out-of-range coordinates to the image border.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_CLAMP_TO_BORDER = 2,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_REPEAT = 3,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MODE_MIRRORED_REPEAT = 4

} hsa_ext_sampler_addressing_mode_t;

/**
 * @brief Sampler coordinate modes. The enumeration values match the BRIG
 * BRIG_SAMPLER_COORD bit in BrigSamplerModifier.
 */
typedef enum {

  /**
   * Coordinates are all in the range of 0 to (dimension-1).
   */
  HSA_EXT_SAMPLER_COORDINATE_MODE_UNNORMALIZED = 0,

  /**
   * Coordinates are all in the range of 0.0 to 1.0.
   */
  HSA_EXT_SAMPLER_COORDINATE_MODE_NORMALIZED = 1

} hsa_ext_sampler_coordinate_mode_t;

/**
 * @brief Sampler filter modes. The enumeration values match the BRIG type
 * BrigSamplerFilter.
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
 * @brief Create a kernel agent defined sampler handle for a given combination
 * of a (agent-independent) sampler descriptor and agent.
 *
 * @param[in] agent Agent to be associated with the sampler.
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
 * @brief Destroy a sampler previously created using ::hsa_ext_sampler_create.
 *
 * @param[in] agent Agent associated with the sampler.
 *
 * @param[in] sampler Sampler.  The sampler handle should not be destroyed while
 * there are references to it queued for execution or currently being used in a
 * dispatch.
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

/**
 * @brief Enumeration constants added to ::hsa_status_t by this extension.
 */
enum {
    /**
     * Image format is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED = 0x3000,
    /**
     * Image size is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED = 0x3001
};

/**
 * @brief Enumeration constants added to ::hsa_agent_info_t by this
 * extension. The value of any of these attributes is undefined if the
 * agent is not a kernel agent, or the implementation does not support images.
 */
enum {
  /**
   * Maximum number of elements in 1D images. Must be at most 16384. The type
   * of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1D_MAX_ELEMENTS = 0x3000,
  /**
   * Maximum number of elements in 1DA images. Must be at most 16384. The type
   * of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1DA_MAX_ELEMENTS = 0x3001,
  /**
   * Maximum number of elements in 1DB images. Must be at most 65536.  The type
   * of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_1DB_MAX_ELEMENTS = 0x3002,
  /**
   * Maximum dimensions (width, height) of 2D images, in image elements. The X
   * and Y maximums must be at most 16384. The type of this attribute is
   * uint32_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2D_MAX_ELEMENTS = 0x3003,
  /**
   * Maximum dimensions (width, height) of 2DA images, in image elements. The X
   * and Y maximums must be at most 16384. The type of this attribute is
   * uint32_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DA_MAX_ELEMENTS = 0x3004,
  /**
   * Maximum dimensions (width, height) of 2DDEPTH images, in image
   * elements. The X and Y maximums must be at most 16384. The type of this
   * attribute is uint32_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DDEPTH_MAX_ELEMENTS = 0x3005,
  /**
   * Maximum dimensions (width, height) of 2DADEPTH images, in image
   * elements. The X and Y maximums must be at most 16384. The type of this
   * attribute is uint32_t[2].
   */
  HSA_EXT_AGENT_INFO_IMAGE_2DADEPTH_MAX_ELEMENTS = 0x3006,
  /**
   * Maximum dimensions (width, height, depth) of 3D images, in image
   * elements. The maximum along any dimension cannot exceed 2048. The type of
   * this attribute is uint32_t[3].
   */
  HSA_EXT_AGENT_INFO_IMAGE_3D_MAX_ELEMENTS = 0x3007,
  /**
   * Maximum number of image layers in a image array. Must not exceed 2048. The
   * type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_IMAGE_ARRAY_MAX_LAYERS = 0x3008,
  /**
   * Maximum number of read-only image handles that can be created at any one
   * time. Must be at least 128. The type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_MAX_IMAGE_RD_HANDLES = 0x3009,
  /**
   * Maximum number of write-only and read-write image handles (combined) that
   * can be created at any one time. Must be at least 64. The type of this
   * attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_MAX_IMAGE_RORW_HANDLES = 0x300A,
  /**
   * Maximum number of sampler handlers that can be created at any one
   * time. Must be at least 16. The type of this attribute is uint32_t.
   */
  HSA_EXT_AGENT_INFO_MAX_SAMPLER_HANDLERS = 0x300B
};

/** @} */


#define hsa_ext_images_1_00

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
    const void *image_data,
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


#ifdef __cplusplus
}
#endif  /*__cplusplus*/
