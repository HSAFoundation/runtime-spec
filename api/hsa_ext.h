#include "hsa.h"
#include <stdbool.h> /* bool */


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** \defgroup ext-finalizer HSAIL Finalization
 *  @{
 */

/**
 * @brief Profile kinds. For more information see the HSA Programmer's
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
 * @brief Profile is used to specify the kind of profile. This controls what
 * features of HSAIL are supported. For more information see the HSA
 * Programmer's Reference Manual.
 */
typedef uint8_t hsa_ext_brig_profile8_t;

/**
 * @brief Machine model kinds. For more information see the HSA Programmer's
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
 * @brief Machine model type. This controls the size of addresses used for
 * segment and flat addresses. For more information see the HSA Programmer's
 * Reference Manual.
 */
typedef uint8_t hsa_ext_brig_machine_model8_t;


/**
 * @brief Predefined BRIG section kinds.
 */
typedef enum {
  /**
   * Textual character strings and byte data used in the module.  Also contains
   * variable length arrays of offsets into other sections that are used by
   * entries in the hsa_code and hsa_operand sections. For more information see
   * the HSA Programmer's Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_DATA = 0,

  /**
   * All of the directives and instructions of the module. Most entries contain
   * offsets to the hsa_operand or hsa_data sections. Directives provide
   * information to the finalizer, and instructions correspond to HSAIL
   * operations which the finalizer uses to generate executable ISA code.  For
   * more information see the HSA Programmer's Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_CODE = 1,

  /**
   * The operands of directives and instructions in the code section. For
   * example, immediate constants, registers and address expressions. For more
   * information see the HSA Programmer's Reference Manual.
   */
  HSA_EXT_BRIG_SECTION_OPERAND = 2
} hsa_ext_brig_section_id_t;

/**
 * @brief BRIG section ID. The index into the array of sections in a BRIG
 * module.
 */
typedef uint32_t hsa_ext_brig_section_id32_t;

/**
 * @brief BRIG section header. Every section starts with a
 * ::hsa_ext_brig_section_header_t which contains the section size, name and
 * offset to the first entry. For more information see the HSA Programmer's
 * Reference Manual.
 */
typedef struct hsa_ext_brig_section_header_s {
  /**
   * Size in bytes of the section, including the size of the
   * ::hsa_ext_brig_section_header_t. Must be a multiple of 4.
   */
  uint32_t size;

  /**
   * Size of the header in bytes, which is also equal to the offset of the first
   * entry in the section. Must be a multiple of 4.
   */
  uint32_t header_size;

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
 * @brief A module is the basic building block for HSAIL programs. When HSAIL is
 * generated it is represented as a module.
 */
typedef struct hsa_ext_brig_module_s {
  /**
   * Number of sections in the module. Must be at least 3.
   */
  uint32_t section_count;

  /**
   * A variable-sized array containing pointers to the BRIG sections. Must have
   * @a section_count elements. Indexed by ::hsa_ext_brig_section_id32_t. The
   * first three elements must be for the following predefined sections in the
   * following order: ::HSA_EXT_BRIG_SECTION_DATA, ::HSA_EXT_BRIG_SECTION_CODE,
   * ::HSA_EXT_BRIG_SECTION_OPERAND.
   */
  hsa_ext_brig_section_header_t *section[1];
} hsa_ext_brig_module_t;

/**
 * @brief An opaque handle to the ::hsa_ext_brig_module_t.
 */
typedef struct hsa_ext_brig_module_handle_s {
  /**
   * HSA component-specific handle to the brig module.
   */
  uint64_t handle;
} hsa_ext_brig_module_handle_t;

/**
 * @brief An entry offset into the code section of the BRIG module. The value is
 * the byte offset relative to the start of the section to the beginning of the
 * referenced entry. The value 0 is reserved to indicate that the offset does
 * not reference any entry.
 */
typedef uint32_t hsa_ext_brig_code_section_offset32_t;

/**
 * @brief HSAIL exception kinds. For more information see the HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Operations are performed on values for which the results are not defined.
   * These are:
   *   - Operations on signaling NaN (sNaN) floating-point values.
   *   - Signalling comparisons: comparisons on quiet NaN (qNaN)
   *     floating-point values.
   *   - Multiplication: mul(0.0, &infin;) or mul(&infin;, 0.0).
   *   - Fused multiply add: fma(0.0, &infin;, c) or fma(&infin;, 0.0, c)
   *     unless c is a quiet NaN, in which case it is implementation-defined
   *     if an exception is generated.
   *   - Addition, subtraction, or fused multiply add: magnitude subtraction
   *     of infinities, such as: add(+&infin;, -&infin;),
   *     sub(+&infin;, +&infin;).
   *   - Division: div(0.0, 0.0) or div(&infin;, &infin;).
   *   - Square root: sqrt(negative).
   *   - Conversion: A cvt with a floating-point source type, an integer
   *     destination type, and a nonsaturating rounding mode, when the source
   *     value is a NaN, &infin;, or the rounded value, after any flush to
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
 * @brief The set of exceptions supported by HSAIL. This is represented as a bit
 * set.
 */
typedef uint16_t hsa_ext_exception_kind16_t;

/**
 * @brief HSAIL control directive kinds. For more information see the HSA
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
   * the set is empty. However, an implementation should endeavor to make the
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
   * is dispatched. For more information see the HSA Programmer's Reference
   * Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  /**
   * If not enabled then must be 0, otherwise must be greater than 0. Specifies
   * the maximum number of work-items that will be in the work-group when the
   * kernel is dispatched. For more information see the HSA Programmer's
   * Reference Manual.
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
   * when the kernel is dispatched. For more information see the HSA
   * Programmer's Reference Manual.
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
   * optimize work-group ID operations, and if the number of work-items in the
   * work-group is less than the WAVESIZE then barrier operations can be
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
   * compute the absolute and flat work-group and work-item ID, and the dim
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
   * bit set in enabledControlDirectives indicates that the control directive is
   * present.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256
} hsa_ext_control_directive_present_t;

/**
 * @brief Bit set of control directives supported in HSAIL. See the HSA
 * Programmer's Reference Manual description of control directives with the same
 * name for more information. For control directives that have an associated
 * value, the value is given by the field in hsa_ext_control_directives_t. For
 * control directives that are only present or absent (such as
 * requirenopartialworkgroups) they have no corresponding field as the presence
 * of the bit in this mask is sufficient.
 */
typedef uint64_t hsa_ext_control_directive_present64_t;

/**
 * @brief The hsa_ext_control_directives_t specifies the values for the HSAIL
 * control directives. These control how the finalizer generates code. This
 * struct is used both as an argument to ::hsa_ext_finalize to specify values
 * for the control directives, and is used in ::hsa_ext_code_descriptor_t to
 * record the values of the control directives that the finalizer used when
 * generating the code which either came from the finalizer argument or explicit
 * HSAIL control directives. See the definition of the control directives in the
 * HSA Programmer's Reference Manual, which also defines how the values
 * specified as finalizer arguments have to agree with the control directives in
 * the HSAIL code.
 */
typedef struct hsa_ext_control_directives_s {
  /**
   * This is a bit set indicating which control directives have been
   * specified. If the value is 0 then there are no control directives specified
   * and the rest of the fields can be ignored. The bits are accessed using
   * ::hsa_ext_control_directive_present_t. Any control directive that is
   * not enabled in this bit set must have the value of all 0s.
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
   * should endeavor to make the performance impact small. If the kernel being
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
   * than 0. See the HSA Programmer's Reference Manual description of
   * maxflatgridsize control directive.
   */
  uint32_t max_flat_grid_size;

  /**
   * If maxFlatWorkgroupSize is not enabled then must be 0, otherwise must be
   * greater than 0. See the HSA Programmer's Reference Manual description of
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
   * element must be greater than 0. See the HSA Programmer's Reference Manual
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
   * optimize work-group ID operations, and if the number of work-items in the
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
   * compute the absolute and flat work-group and work-item ID, and the dim
   * HSAIL operations.
   */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];
} hsa_ext_control_directives_t;

/**
 * @brief Kinds of code object. For more information see the HSA Programmer's
 * Reference Manual.
 */
typedef enum {
  /**
   * Not a code object.
   */
  HSA_EXT_CODE_NONE = 0,

  /**
   * HSAIL kernel that can be used with an AQL Kernel Dispatch packet.
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
   * Vendor-specific code objects.
   */
  HSA_EXT_CODE_VENDOR_FIRST = 0x80000000,
  HSA_EXT_CODE_VENDOR_LAST = 0xffffffff
} hsa_ext_code_kind_t;

/**
 * @brief The kinds of code objects that can be contained in
 * ::hsa_ext_code_descriptor_t.
 */
typedef uint32_t hsa_ext_code_kind32_t;

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
 * @brief Each HSA component can support one or more call conventions. For
 * example, an HSA component may have different call conventions that each use a
 * different number of ISA registers to allow different numbers of wavefronts to
 * execute on a compute unit.
 */
typedef uint32_t hsa_ext_program_call_convention_id32_t;

/**
 * @brief The 64-bit opaque code handle to the finalized code that includes the
 * executable ISA for the HSA component. It can be used for the Kernel Dispatch
 * packet kernel object address field.
 */
typedef struct hsa_ext_code_handle_s {
  /**
   * HSA component-specific handle to the code.
   */
  uint64_t handle;
} hsa_ext_code_handle_t;

/**
 * @brief An opaque handle to the debug information.
 */
typedef struct hsa_ext_debug_information_handle_s {
  /**
   * HSA component-specific handle to the debug information.
   */
  uint64_t handle;
} hsa_ext_debug_information_handle_t;

/**
 * @brief Provides the information about a finalization of the kernel or
 * indirect function for a specific HSA component, and for indirect functions, a
 * specific call convention of that HSA component
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
   * executable ISA for the HSA component. It can be used for the Kernel
   * Dispatch packet kernel object address field.
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
   * which may be used by application to allocate dynamic group memory and set
   * the dispatch work-group size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   * Program call convention ID this code descriptor holds.
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
   * generated from an ::hsa_ext_finalize then it must still indicate what
   * profile is being used.
   */
  hsa_ext_brig_profile8_t hsail_profile;

  /**
   * The HSAIL machine model gives the address sizes used by the code. This
   * information is from the HSAIL version directive. If this
   * ::hsa_ext_code_descriptor_t is not generated from an ::hsa_ext_finalize
   * then it must still indicate for what machine mode the code is generated.
   */
  hsa_ext_brig_machine_model8_t hsail_machine_model;

  /**
   * Reserved for BRIG target options if any are defined in the future. Must be
   * 0.
   */
  uint16_t reserved1;

  /**
   * Opaque handle to debug information.
   */
  hsa_ext_debug_information_handle_t debug_information;

  /**
   * Name of vendor. The type of this attribute is a NUL-terminated
   * char[24]. ISO/IEC 646 character encoding must be used. If the name of the
   * vendor uses less than 23 characters, the rest of the array must be filled
   * with NULs.
   */
  char agent_vendor[24];

  /**
   * Name of agent. The type of this attribute is a NUL-terminated
   * char[24]. ISO/IEC 646 character encoding must be used. If the name of the
   * agent uses less than 23 characters, the rest of the array must be filled
   * with NULs.
   */
  char agent_name[24];

  /**
   * The HSAIL major version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then it must be 0.
   */
  uint32_t hsail_version_major;

  /**
   * The HSAIL minor version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then it must be 0.
   */
  uint32_t hsail_version_minor;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * The values should be the actual values used by the finalizer in generating
   * the code. This may be the union of values specified as finalizer arguments
   * and explicit HSAIL control directives. If the finalizer chooses to ignore a
   * control directive and not generate constrained code, then the control
   * directive should not be marked as enabled even though it was present in the
   * HSAIL or finalizer argument. The values are intended to reflect the
   * constraints that the code actually requires to correctly execute, not the
   * values that were actually specified at finalize time.
   */
  hsa_ext_control_directives_t control_directive;
} hsa_ext_code_descriptor_t;

/**
 * @brief Finalization request. Holds information about the module needed to be
 * finalized. If the module contains an indirect function, also holds
 * information about call convention ID.
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
   * If this finalization request is for indirect function, desired program call
   * convention.
   */
  hsa_ext_program_call_convention_id32_t program_call_convention;
} hsa_ext_finalization_request_t;

/**
 * @brief Finalization handle is the handle to the object produced by the
 * finalizer that contains the ISA code and related information needed to
 * execute that code for a specific HSA agent for the set of kernels/indirect
 * functions specified in the finalization request.
 */
typedef struct hsa_ext_finalization_handle_s {
  /**
   * HSA component-specific handle to the finalization information.
   */
  uint64_t handle;
} hsa_ext_finalization_handle_t;

/**
 * ID of an HSA agent within a program that it is a member. It is used to index
 * a kernel descriptor to access the code descriptor for the HSA agent. An HSA
 * agent can be a member of multiple programs and can have different
 * ::hsa_ext_program_agent_id_t in each program.
 */
typedef uint32_t hsa_ext_program_agent_id_t;

/**
 * @brief Callback function to get the definition of a module scope
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
 * @brief Callback function to get the address of global segment variables,
 * kernel table variable, indirect function table variable.
 */
typedef hsa_status_t (*hsa_ext_symbol_address_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  uint64_t *symbol_address);

/**
 * @brief Callback function to get the string representation of the error
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
 * functions. A kernel can only be finalized once per program per HSA agent.  An
 * indirect function can only be finalized once per program per HSA agent per
 * call convention. Only code for the HSA components specified when the program
 * was created can be requested. The program must contain a definition for the
 * requested kernels and indirect functions among the modules that have been
 * added to the program.
 *
 * @param[in] caller Opaque pointer and will be passed to all callback functions
 * made by this call.
 *
 * @param[in] agent HSA agents for which code must be produced.
 *
 * @param[in] program_agent_id Program agent ID.
 *
 * @param[in] program_agent_count Number of program agents.
 *
 * @param[in] finalization_request_count The number of kernels and/or indirect
 * needed to be finalized.
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
 * @param[in] symbol_definition_callback Callback function to get the definition
 * of a module scope variable/fbarrier or kernel/function.
 *
 * @param[in] symbol_address_callback Callback function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that controls
 * the level of optimization performed by the finalizer.
 *
 * @param[in] options Implementation defined options that can be specified to
 * the finalizer.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @p finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] finalization Handle to the object produced that contains the ISA
 * code and related information needed to execute that code for the specific @p
 * agent for the set of kernels/indirect functions specified in the @p
 * finalization_request_list.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH The directive in the
 * control directive structure and in the HSAIL kernel do not match, or if the
 * same directive is used with a different value in one of the functions used by
 * this kernel.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p finalization_request_list
 * is NULL or invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES The finalize API cannot
 * allocate memory for @p finalization.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS The options are not
 * recognized, no error is returned, just an info status is used to indicate
 * invalid options.
 */
hsa_status_t HSA_API hsa_ext_finalize(
    hsa_runtime_caller_t caller,
    hsa_agent_t agent,
    hsa_ext_program_agent_id_t  program_agent_id,
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
 * @brief Queries the total number of kernel and indirect functions that have
 * been finalized as part of the finalization object.
 *
 * @param[in] agent HSA agent for which the finalization object contains code.
 *
 * @param[in] finalization Finalization handle that references the finalization
 * object for @p agent.
 *
 * @param[out] code_descriptor_count Number of kernel and indirect functions
 * that have been finalized as part of the finalization object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * number of code descriptors is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent is NULL or invalid, @p
 * finalization points to invalid finalization, or @p code_descriptor_count is
 * NULL.
 */
hsa_status_t HSA_API hsa_ext_query_finalization_code_descriptor_count(
    hsa_agent_t agent,
    hsa_ext_finalization_handle_t finalization,
    uint32_t *code_descriptor_count);

/**
 * @brief Queries information about one of the kernel or indirect functions that
 * have been finalized as part of a finalization object.
 *
 * @param[in] agent HSA agent for which the finalization object contains code.
 *
 * @param[in] finalization Finalization handle that references the finalization
 * object for @p agent.
 *
 * @param[in] index Specifies which kernel or indirect function information is
 * being requested. Must be in the range 0 to
 * ::hsa_ext_query_finalization_code_descriptor_count - 1.
 *
 * @param[out] code_descriptor The information about the requested kernel or
 * indirect function.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * code descriptor is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent is NULL or invalid, @p
 * finalization points to invalid finalization, or @p code_descriptor is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_finalization_code_descriptor(
    hsa_agent_t agent,
    hsa_ext_finalization_handle_t finalization,
    uint32_t index,
    hsa_ext_code_descriptor_t *code_descriptor);

/**
 * @brief Destroys a finalization. This may reclaim the memory occupied by the
 * finalization object, and remove corresponding ISA code from the associated
 * HSA agent. Once destroyed, all code that is part of the finalization object
 * is invalidated. It is undefined if any dispatch is executing, or will
 * subsequently be executed, when the finalization containing its code is
 * destroyed.
 *
 * @param[in] agent HSA agent for which the finalization object contains code.
 *
 * @param[in] finalization Handle to the finalization to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p finalization is NULL or
 * does not point to a valid finalization structure.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE Some of the resources consumed
 * during initialization by the runtime could not be freed.
 */
hsa_status_t HSA_API hsa_ext_destroy_finalization(
    hsa_agent_t agent,
    hsa_ext_finalization_handle_t finalization);

/**
 * @brief Serializes the finalization.
 *
 * @details Serializes finalization descriptor for specified @p agent. The
 * caller can set @p debug_information to 1 in order to include debug
 * information of this finalization descriptor in the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all callback functions
 * made by this call.
 *
 * @param[in] agent The HSA agent for which @p finalization must be serialized.
 *
 * @param[in] finalization Handle to the finalization to be serialized.
 *
 * @param[in] alloc_serialize_data_callback Callback function for allocation.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @p finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p finalization
 * is either NULL or does not point to a valid finalization descriptor object.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES No memory can be allocated for @p
 * serialized_object.
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
 * @details Deserializes finalization descriptor for specified @p agent. The
 * caller can set @p debug_information to 1 in order to include debug
 * information of this finalization descriptor from the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all callback functions
 * made by this call.
 *
 * @param[in] serialized_object Serialized object to be deserialized.
 *
 * @param[in] agent The HSA agent for which @p finalization must be deserialized.
 *
 * @param[in] program_agent_id ID of the HSA agent to deserialize the
 * finalization for. Used to implement agentid_u32 operation.
 *
 * @param[in] program_agent_count Number of HSA agents in the program. Used to
 * implement agentcount_u32 operation.
 *
 * @param[in] symbol_address_callback Callback function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @p finalization. 0 - exclude debug information, 1 - include
 * debug information.
 *
 * @param[out] finalization Handle to the deserialized finalization.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p serialized_object is
 * NULL, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES No memory can be allocated for
 * @p finalization.
 */
hsa_status_t HSA_API hsa_ext_deserialize_finalization(
    hsa_runtime_caller_t caller,
    void *serialized_object,
    hsa_agent_t agent,
    hsa_ext_program_agent_id_t program_agent_id,
    uint32_t program_agent_count,
    hsa_ext_symbol_address_callback_t symbol_address_callback,
    hsa_ext_error_message_callback_t error_message_callback,
    int debug_information,
    hsa_ext_finalization_handle_t *finalization);
/** @} */

/** \defgroup ext-linker HSAIL Linking
 *  @{
 */

/**
 * @brief An opaque handle to the HSAIL program.
 *
 * @details An application can use the HSA runtime to create zero or more HSAIL
 * programs, to which it can add zero or more HSAIL modules. HSAIL program
 * manages linking of symbol declaration to symbol definitions between
 * modules. In addition, the application can provide symbol definitions to an
 * HSAIL program, and can obtain the address of symbols defined by the HSAIL
 * program using the HSA runtime. An HSAIL program can be created with
 * ::hsa_ext_program_create, which returns a handle to the created program
 * ::hsa_ext_program_handle_t. A program handle has to be further used to add
 * particular modules to the program using ::hsa_ext_add_module, perform various
 * define, query and validation operations, and finalize the program using
 * ::hsa_ext_finalize_program. A program has to be destroyed once not needed any
 * more using ::hsa_ext_program_destroy.
 */
typedef struct hsa_ext_program_handle_s {
  /**
   * HSA component-specific handle to the program.
   */
  uint64_t handle;
} hsa_ext_program_handle_t;

/**
 * @brief Creates an HSAIL program.
 *
 * @details Creates an HSAIL program. When an HSAIL program is created, one or
 * more HSA components that are part of the HSA platform must be specified
 * (::hsa_agent_t), together with the machine model
 * (::hsa_ext_brig_machine_model8_t) and profile (::hsa_ext_brig_profile8_t).
 * The set of HSA agents associated with the HSAIL being created cannot be changed
 * after the program is created. The machine model address size for the global
 * segment must match the size used by the application. All modules added to the
 * program must have the same machine model and profile as the program.  Once
 * the program is created, the program handle (::hsa_ext_program_handle_t) is
 * returned. See ::hsa_ext_program_handle_t for more details.
 *
 * @param[in] agents One or more HSA components that are part of the HSA
 * platform to create a program for.
 *
 * @param[in] agent_count Number of HSA components to create an HSAIL program
 * for.
 *
 * @param[in] machine_model The kind of machine model this HSAIL program is
 * created for. The machine model address size for the global segment must match
 * the size used by the applications. All module added to the program must have
 * the same machine model.
 *
 * @param[in] profile The kind of profile this HSAIL program is created for.
 * All modules added to the program must have the same profile as the program.
 *
 * @param[out] program A valid pointer to a program handle for the HSAIL program
 * created.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * an HSAIL program is created.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agents is NULL or invalid, @p
 * agent_count is 0, @p machine_model is invalid, or @p profile is invalid.  In
 * this case @p program will be NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for program creation.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED @p program is already a
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
 * @details Destroys an HSAIL program pointed to by program handle @p program.
 * When the program is destroyed, member code objects are destroyed as well.
 *
 * @param[in] program Program handle for the HSAIL program to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * an HSAIL program is destroyed.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE @p program is already destroyed or
 * has never been created.
 */
hsa_status_t HSA_API hsa_ext_program_destroy(
    hsa_ext_program_handle_t program);

/**
 * @brief Adds an existing HSAIL module to an existing HSAIL program.
 *
 * @details Adds an existing HSAIL module to an existing HSAIL program. An HSAIL
 * module is the unit of HSAIL generation, and can contain multiple symbol
 * declarations and definitions. An HSAIL module can be added to zero or more
 * HSAIL programs. Distinct instances of the symbols it defines are created
 * within each program, and symbol declarations are only linked to the
 * definitions provided by other modules in the same program. The same HSAIL
 * module can be added to multiple HSAIL programs, which allows multiple
 * instances of the same kernel and indirect functions that reference distinct
 * allocations of global segment variables. The same HSAIL module cannot be
 * added to the same HSAIL program more than once. The machine model and profile
 * of the HSAIL module that is being added has to match the machine model and
 * profile of the HSAIL program it is added to. HSAIL modules and their handles
 * can be queried from the program using several query operations.
 *
 * @param[in] program HSAIL program to add HSAIL module to.
 *
 * @param[in] brig_module HSAIL module to add to the HSAIL program.
 *
 * @param[out] module The handle for the @p brig_module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the module is added successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is an invalid HSAIL
 * program, @p brig_module is an invalid HSAIL module, or the machine model
 * and/or profile of the @p brig_module do not match the machine model and/or
 * profile of @p program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for module addition.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED @p brig_module is already
 * added to the HSAIL program. No error is returned, just an info status is used
 * to indicate invalid options.
 */
hsa_status_t HSA_API hsa_ext_add_module(
    hsa_ext_program_handle_t program,
    hsa_ext_brig_module_t *brig_module,
    hsa_ext_brig_module_handle_t *module);

/**
 * @brief Finalizes provided HSAIL program.
 *
 * @details Finalizes provided HSAIL program. The HSA runtime finalizer can be
 * used to generate code for kernels and indirect functions from a specific
 * program for a specific HSA component. A kernel can only be finalized once per
 * program per HSA agent. An indirect function can only finalized once per
 * program per HSA agent per call convention. Only code for HSA components
 * specified when the program was created can be requested. The program must
 * contain a definition for the requested kernels and indirect functions among
 * the modules that have been added to the program. The modules of the program
 * must collectively define all variables, fbarriers, kernels and functions
 * referenced by operations in the code block. In addition, the caller of this
 * function can specify control directives as an input argument, which will be
 * passed to the finalizer. These control directives can be used for low-level
 * performance tuning, for more information on control directives see the HSA
 * Programmer's Reference Manual.
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
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that controls
 * the level of optimization performed by the finalizer. For more information
 * see the HSA Programmer's Reference Manual.
 *
 * @param[in] options Implementation defined options that can be passed to the
 * finalizer. For more information see the HSA Programmer's Reference Manual.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information. 0 - exclude debug information, 1 - include debug
 * information.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the requested list of kernels/functions is finalized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH The directive in the
 * control directive structure and in the HSAIL kernel mismatch, or if the same
 * directive is used with a different value in one of the functions used by this
 * kernel. The @p error_message_callback can be used to get the string
 * representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent is NULL or invalid (if
 * one of the specified HSA components is not the part of the HSAIL program), @p
 * program is an invalid HSAIL program, @p finalization_request_list is NULL or
 * invalid, or @p finalization_request_count is 0. The @p error_message_callback
 * can be used to get the string representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for finalization. The @p error_message_callback can be
 * used to get the string representation of the error.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS @p options or @p
 * optimization_level are not recognized. No error is returned, just an info
 * status is used to indicate invalid options.
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
 * @brief Queries HSA component's ID for specified HSA component contained in
 * specified HSAIL program.
 *
 * @param[in] program HSAIL program to query HSA component's ID from.
 *
 * @param[in] agent HSA component for which the ID is queried.
 *
 * @param[out] program_agent_id HSA component's ID contained in specified HSAIL
 * program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * HSA component's ID is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program or @p agent is
 * invalid, or @p program_agent_id is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_agent_id(
    hsa_ext_program_handle_t program,
    hsa_agent_t agent,
    hsa_ext_program_agent_id_t *program_agent_id);

/**
 * @brief Queries the number of HSA components contained in specified HSAIL
 * program.
 *
 * @param[in] program HSAIL program to query number of HSA components from.
 *
 * @param[out] program_agent_count Number of HSA components contained in
 * specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * number of HSA components in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * program_agent_count is NULL.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * HSA agents contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the number of HSAIL modules contained in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * program_module_count is NULL.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * HSAIL module handles contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * modules is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_program_modules(
    hsa_ext_program_handle_t program,
    uint32_t program_module_count,
    hsa_ext_brig_module_handle_t *modules);

/**
 * @brief Queries HSAIL module with specified handle that is contained in
 * the specified HSAIL program.
 *
 * @param[in] program HSAIL program to query HSAIL modules from.
 *
 * @param[in] module HSAIL module handle for which to query the HSAIL module.
 *
 * @param[out] brig_module HSAIL module contained in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * HSAIL module contained in the HSAIL program is queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * brig_module is invalid.
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
 * @param[out] first_call_convention_id Set of call convention IDs for specified
 * HSA agent of a specified HSAIL program.
 *
 * @param[out] call_convention_count Number of call convention IDs available for
 * specified HSA agent of a specified program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * call convention IDs contained in the HSAIL program are queried.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p agent
 * is invalid or NULL, @p first_call_convention_id is NULL, or @p
 * call_convention_count is NULL.
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
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid.
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
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified global variable address is defined in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p module
 * is invalid, or @p address is NULL.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the global variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * module is invalid.
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
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified global variable address is defined for specified HSA agent in
 * specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p agent
 * is NULL or invalid, @p module is invalid, or @p address is NULL.
 */
hsa_status_t HSA_API hsa_ext_define_agent_allocation_global_variable_address(
    hsa_ext_program_handle_t program,
    hsa_agent_t agent,
    hsa_ext_brig_module_handle_t module,
    hsa_ext_brig_code_section_offset32_t symbol,
    hsa_ext_error_message_callback_t error_message_callback,
    void *address);

/**
 * @brief Queries global variable address for specified HSA agent from specified
 * HSAIL program. Allows host program to directly access variables.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the global variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p agent
 * is NULL or invalid, @p module is invalid, or @p address is NULL.
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
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified readonly variable address is defined for specified HSA agent in
 * specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p agent
 * is NULL or invalid, @p module is invalid, or @p address is NULL.
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
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the readonly variable address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, @p agent
 * is NULL or invalid, @p module is invalid, or @p address is NULL.
 */
hsa_status_t HSA_API hsa_ext_query_readonly_variable_address(
    hsa_ext_program_handle_t program,
    hsa_agent_t agent,
    hsa_ext_brig_module_handle_t module,
    hsa_ext_brig_code_section_offset32_t symbol,
    void** address);

/**
 * @brief Queries kernel descriptor address from specified HSAIL program.
 * Needed to create a Kernel Dispatch packet.
 *
 * @param[in] program HSAIL program to query kernel descriptor address from.
 *
 * @param[in] module HSAIL module to query kernel descriptor address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] kernel_descriptor The address of the kernel descriptor for the
 * requested kernel, which is an array of ::hsa_ext_code_descriptor_t indexed by
 * ::hsa_ext_program_agent_id_t.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the kernel descriptor address is queried from specified HSAIL program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * module is invalid.
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
 * @param[out] indirect_function_descriptor The address of the indirect function
 * descriptor for the requested indirect function, which is an array of
 * ::hsa_ext_code_descriptor_t indexed by
 * ::hsa_ext_program_call_convention_id32_t.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * the indirect function descriptor address is queried from specified HSAIL
 * program.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p
 * module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_indirect_function_descriptor_address(
    hsa_ext_program_handle_t program,
    hsa_ext_brig_module_handle_t module,
    hsa_ext_brig_code_section_offset32_t symbol,
    hsa_ext_code_descriptor_t** indirect_function_descriptor);

/**
 * @brief Validates HSAIL program with specified HSAIL program handle. Checks if
 * all declarations and definitions match, if there is at most one
 * definition. The caller decides when to call validation routines. For example,
 * it can be done in the debug mode.
 *
 * @param[in] program HSAIL program to validate.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified HSAIL program is a valid program.
 *
 * @retval ::HSA_STATUS_ERROR @p program is invalid. Refer to the error callback
 * function to get string representation of the failure.
 */
hsa_status_t HSA_API hsa_ext_validate_program(
    hsa_ext_program_handle_t program,
    hsa_ext_error_message_callback_t error_message_callback);

/**
 * @brief Validates specified HSAIL module with specified HSAIL module handle
 * for specified HSAIL program. Checks if BRIG for specified module is legal:
 * operation operand type rules, etc. For more information about BRIG see the
 * HSA Programmer's Reference Manual. The caller decides when to call validation
 * routines. For example, it can be done in the debug mode.
 *
 * @param[in] program HSAIL program to validate HSAIL module in.
 *
 * @param[in] module HSAIL module handle to validate.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified HSAIL module is a valid module.
 *
 * @retval ::HSA_STATUS_ERROR The module is invalid. Refer to the error
 * callback function to get string representation of the failure.
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
 * @param[in] alloc_serialize_data_callback Callback function for memory
 * allocation.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message (if any).
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information. 0 - exclude debug information, 1 - include debug information.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * specified program is serialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for serialization. The @p error_message_callback can be
 * used to get the string representation of the error.
 */
hsa_status_t HSA_API hsa_ext_serialize_program(
    hsa_runtime_caller_t caller,
    hsa_ext_program_handle_t program,
    hsa_runtime_alloc_data_callback_t alloc_serialize_data_callback,
    hsa_ext_error_message_callback_t error_message_callback,
    int debug_information,
    void *serialized_object);

/**
 * @brief Callback function to get program's address of global segment
 * variables, kernel table variable, indirect function table variable based on
 * the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_program_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Callback function to get HSA agent's address of global segment
 * variables, kernel table variable, indirect function table variable based on
 * the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_agent_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  hsa_agent_t agent,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Deserializes the HSAIL program from a given serialized object. Used
 * for offline compilation. Includes callback functions, where callback
 * functions take symbolic name. This allows symbols defined by application to
 * be relocated.
 *
 * @param[in] caller Opaque pointer to the caller of this function.
 *
 * @param[in] serialized_object Serialized HSAIL program.
 *
 * @param[in] program_allocation_symbol_address Callback function to get
 * program's address of global segment variables, kernel table variable,
 * indirect function table variable based on the symbolic name. Allows symbols
 * defined by application to be relocated.
 *
 * @param[in] agent_allocation_symbol_address Callback function to get HSA
 * agent's address of global segment variables, kernel table variable, indirect
 * function table variable based on the symbolic name. Allows symbols defined by
 * application to be relocated.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information. 0 - exclude debug information, 1 - include debug information.
 *
 * @param[out] program Deserialized HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully, and
 * HSAIL program is deserialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p serialized_object is
 * NULL or invalid, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for deserialization. The @p error_message_callback can be
 * used to get the string representation of the error.
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

/** \defgroup alt-finalizer-program HSAIL Finalization
 *  @{
 */

/**
 * @brief Profile types. Each profile dictates a set of HSAIL features that the
 * HSA implementation must support. The finalizer can choose to support one or
 * more profiles, but a program is not allowed to mix profiles. The application
 * selects the profile type of a program at creation time.
 */
typedef enum {
  /**
   * Base profile. A program uses this profile when precision can be traded for
   * power efficiency.
   */
  HSA_ALT_BRIG_PROFILE_BASE = 0,

  /**
   * Full profile. A program uses this profile when the hardware can guarantee
   * high-precision results without sacrificing performance.
   */
  HSA_ALT_BRIG_PROFILE_FULL = 1
} hsa_alt_brig_profile_t;

/**
 * @brief A fixed-width type used to represent values in
 * ::hsa_alt_brig_profile_t.
 */
typedef uint8_t hsa_alt_brig_profile8_t;

/**
 * @brief Machine models. A machine model determines the size of certain data
 * values in HSAIL code.
 */
typedef enum {
  /**
   * Small machine model. Addresses, atomic values, and signal values use 32
   * bits.
   */
  HSA_ALT_BRIG_MACHINE_SMALL = 0,

  /**
   * Large machine model. Flat, global segment, readonly segment, and kernarg
   * segment addresses use 64 bits. Address expression offsets and signal values
   * use 64 bits. Addresses in the group, arg, private, and spill segments use
   * 32 bits. Fbarrier addresses use 32 bits. Atomic values can use 32 or 64
   * bits.
   */
  HSA_ALT_BRIG_MACHINE_LARGE = 1
} hsa_alt_brig_machine_model_t;

/**
 * @brief A fixed-width type used to represent values in
 * ::hsa_alt_brig_machine_model_t.
 */
typedef uint8_t hsa_alt_brig_machine_model8_t;

/**
 * @brief An opaque handle to an HSAIL program. An application can use the HSA
 * runtime to create an HSAIL program, and add HSAIL modules to it. An HSAIL
 * program manages the linking of symbol declaration to symbol definitions
 * between modules. In addition, the application can provide symbol definitions
 * to an HSAIL program, and can obtain the address of symbols defined by the
 * HSAIL program using the HSA runtime.
 */
 typedef uint64_t hsa_alt_program_t;

/**
 * @brief Create an HSAIL program.
 *
 * @param[in] num_agents Size of @p agents. Must be greater than 0.
 *
 * @param[in] agents List of HSA components to associate with the HSAIL
 * program. Must not be NULL. The memory associated with @p agents can be reused
 * or freed after the function returns.
 *
 * @param[in] machine_model Machine model used in the HSAIL program. The address
 * size of the global segment implied by the machine model must match the size
 * used by the host application. All the modules added to the program must use
 * this machine model.
 *
 * @param[in] profile Profile type used in the HSAIL program. All the modules
 * added to the program must use this profile type.
 *
 * @param[out] program Memory location where the runtime stores the newly
 * created HSAIL program handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * the resources required for program creation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_agents is 0, @p agents is
 * NULL, @p machine_model is invalid, @p profile is invalid, or @p program is
 * NULL.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED @p program is already a
 * valid program.
 */
hsa_status_t HSA_API hsa_alt_program_create(
    uint32_t num_agents,
    const hsa_agent_t *agents,
    hsa_alt_brig_machine_model8_t machine_model,
    hsa_alt_brig_profile8_t profile,
    hsa_alt_program_t *program);

/**
 * @brief Destroy an HSAIL program. When the program is destroyed, its member
 * code objects are destroyed as well.
 *
 * @param[in] program Program to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The program is invalid.
 */
hsa_status_t HSA_API hsa_alt_program_destroy(
    hsa_alt_program_t program);

/**
 * @brief Get number of agents in a given HSAIL program.
 *
 * @param[in] program Program.
 *
 * @param[out] num_agents Memory location where the runtime stores the number
 * of agents in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_agents is NULL.
 */
hsa_status_t HSA_API hsa_alt_program_get_num_agents(
    hsa_alt_program_t program,
    size_t* num_agents);

/**
 * @brief Get a specific agent within an HSAIL program.
 *
 * @param[in] program Program.
 *
 * @param[in] index Position of the agent within the program.
 *
 * @param[out] agent Memory location where the runtime stores the agent at
 * index @p index in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p agent is NULL.
 */
hsa_status_t HSA_API hsa_alt_program_get_agent(
    hsa_alt_program_t program,
    size_t index,
    hsa_agent_t* agent);

/* /\** */
/*  * @brief Iterate over the agents associated with a given HSA program, and */
/*  * invoke an application-defined callback on every iteration. */
/*  * */
/*  * @param[in] program A valid HSA program. */
/*  * */
/*  * @param[in] callback Callback to be invoked once per HSA agent in the program. */
/*  * The runtime passes three arguments to the callback: the HSA agent, the ID of */
/*  * the HSA agent in the program, and the application data.  If @p callback */
/*  * returns a status other than ::HSA_STATUS_SUCCESS for a particular iteration, */
/*  * the traversal stops and ::hsa_alt_program_iterate_agents returns that status */
/*  * value. */
/*  * */
/*  * @param[in] data Application data that is passed to @p callback on every */
/*  * iteration. Might be NULL. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been */
/*  * initialized. */
/*  * */
/*  * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The program is invalid. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL. */
/* *\/ */
/* hsa_status_t HSA_API hsa_alt_program_iterate_agents( */
/*     hsa_alt_program_t program, */
/*     hsa_status_t (*callback)(hsa_agent_t agent, hsa_ext_program_agent_id_t program_agent_id, void* data), */
/*     void* data); */

/** @} */

/** \defgroup alt-finalizer-module HSAIL Finalization
 *  @{
 */

/**
 * @brief Type of BRIG section.
 */
typedef enum {
  /**
   * Data section. Contains the textual character strings and byte data used in
   * the module.  Also contains variable length arrays of offsets into other
   * sections that are used by entries in code and operand sections.
   */
  HSA_ALT_BRIG_SECTION_DATA = 0,

  /**
   * Code section. Contains the directives and instructions of the module. Most
   * entries contain offsets to the operand or data sections. Directives provide
   * information to the finalizer, and instructions correspond to HSAIL
   * operations which the finalizer uses to generate executable ISA code.
   */
  HSA_ALT_BRIG_SECTION_CODE = 1,

  /**
   * Operand section. Contains the operands of directives and instructions in
   * the code section. For example, immediate constants, registers and address
   * expressions.
   */
  HSA_ALT_BRIG_SECTION_OPERAND = 2
} hsa_ext_brig_section_type_t;

/**
 * @brief BRIG section header. Every section starts with a
 * ::hsa_alt_brig_section_header_t which contains the section size, name and
 * offset to the first entry.
 */
typedef struct hsa_alt_brig_section_header_s {
  /**
   * Size of the section, in bytes. Must be a multiple of 4.
   */
  uint32_t size;

  /**
   * Size of the header in bytes, which is also equal to the offset of the first
   * entry in the section. Must be a multiple of 4.
   */
  uint32_t header_size;

  /**
   * Size of @a name.
   */
  uint32_t name_size;

  /**
   * Section name.
   */
  uint8_t name[1];
} hsa_alt_brig_section_header_t;

/**
 * @brief HSAIL module.
 */
typedef struct hsa_alt_module_s {
  /**
   * Size of @a sections. Must be greater than 2.
   */
  uint32_t num_sections;

  /**
   * List of pointers to BRIG sections. Indexed by
   * ::hsa_ext_brig_section_type_t. The first three elements are reserved for
   * the following sections in the following order: ::HSA_ALT_BRIG_SECTION_DATA,
   * ::HSA_ALT_BRIG_SECTION_CODE, and ::HSA_ALT_BRIG_SECTION_OPERAND.
   */
  hsa_alt_brig_section_header_t *sections[1];
} hsa_alt_brig_module_t;

/**
 * @brief Opaque handle to an HSAIL module within a specific HSAIL program. An
 * HSAIL module can be associated with multiple program module handles.
 */
typedef uint64_t hsa_alt_module_t;

/**
 * @brief Add an HSAIL module to a given HSAIL program.
 *
 * @details Adds an existing HSAIL module to an existing HSAIL program. An HSAIL
 * module is the unit of HSAIL generation, and can contain multiple symbol
 * declarations and definitions. An HSAIL module can be added to zero or more
 * HSAIL programs. Distinct instances of the symbols it defines are created
 * within each program, and symbol declarations are only linked to the
 * definitions provided by other modules in the same program. The same HSAIL
 * module can be added to multiple HSAIL programs, which allows multiple
 * instances of the same kernel and indirect functions that reference distinct
 * allocations of global segment variables. The same HSAIL module cannot be
 * added to the same HSAIL program more than once. The machine model and profile
 * of the HSAIL module that is being added has to match the machine model and
 * profile of the HSAIL program it is added to. HSAIL modules and their handles
 * can be queried from the program using several query operations.
 *
 * @param[in] program HSAIL program.
 *
 * @param[in] brig_module HSAIL module to add.
 *
 * @param[out] module Memory location where the runtime stores the newly
 * created program module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_BRIG_MODULE The HSAIL module is
 * invalid, or the machine model and/or profile of @p brig_module do not match
 * the machine model and/or profile of @p program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for the operation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p brig_module is NULL, or @p
 * module is NULL.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED @p brig_module is already
 * included in the HSAIL program.
 */
hsa_status_t HSA_API hsa_alt_program_add_module(
    hsa_alt_program_t program,
    hsa_alt_brig_module_t *brig_module,
    hsa_alt_module_t *module);

/**
 * @brief Get number of modules in a given HSAIL program.
 *
 * @param[in] program Program.
 *
 * @param[out] num_modules Memory location where the runtime stores the number
 * of modules in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p num_modules is NULL.
 */
hsa_status_t HSA_API hsa_alt_program_get_num_modules(
    hsa_alt_program_t program,
    size_t* num_modules);

/**
 * @brief Get a specific module within an HSAIL program.
 *
 * @param[in] program Program.
 *
 * @param[in] index Position of the module to be retrieved.
 *
 * @param[out] module Memory location where the runtime stores the module at
 * index @p index in the given HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p module is NULL.
 */
hsa_status_t HSA_API hsa_alt_program_get_module(
    hsa_alt_program_t program,
    size_t index,
    hsa_module_t* module);

/* /\** */
/*  * @brief Iterate over the program modules associated with a given HSAIL */
/*  * program, and invoke an application-defined callback on every iteration. */
/*  * */
/*  * @param[in] program A valid HSAIL program. */
/*  * */
/*  * @param[in] callback Callback to be invoked once per program module in the */
/*  * program.  The runtime passes two arguments to the callback, the module and */
/*  * the application data.  If @p callback returns a status other than */
/*  * ::HSA_STATUS_SUCCESS for a particular iteration, the traversal stops and */
/*  * ::hsa_alt_program_iterate_modules returns that status value. */
/*  * */
/*  * @param[in] data Application data that is passed to @p callback on every */
/*  * iteration. Can be NULL. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been */
/*  * initialized. */
/*  * */
/*  * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The program is invalid. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL. */
/* *\/ */
/* hsa_status_t HSA_API hsa_alt_program_iterate_modules( */
/*     hsa_alt_program_t program, */
/*     hsa_status_t (*callback)(hsa_alt_module_t module, void* data), */
/*     void* data); */

/**
 * @brief Queries HSAIL module with specified handle that is contained in
 * the specified HSAIL program.
 *
 * @param[in] module HSAIL module handle for which to query the HSAIL module.
 *
 * @param[out] brig_module HSAIL module contained in specified HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE The HSAIL module is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p brig_module is NULL.
 */
hsa_status_t HSA_API hsa_alt_module_get_brig_module(
    hsa_alt_module_t module,
    hsa_alt_brig_module_t *brig_module);

/**
 * @brief Retrieve the program associated with the program module.
 *
 * @param[in] module HSAIL module handle for which to query the HSAIL
 * module.
 *
 * @param[out] program HSAIL program containing the specified module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE The HSAIL module is
 * invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is NULL.
 */
hsa_status_t HSA_API hsa_alt_module_get_program(
    hsa_alt_module_t module,
    hsa_alt_program_t *program);

/** @} */

/** \defgroup alt-finalizer-finalization HSAIL Finalization
 *  @{
 */

/**
 * @brief HSAIL exception kinds.
 */
typedef enum {
  /**
   * Operations are performed on values for which the results are not defined.
   * These are:
   *   - Operations on signaling NaN (sNaN) floating-point values.
   *   - Signalling comparisons: comparisons on quiet NaN (qNaN)
   *     floating-point values.
   *   - Multiplication: mul(0.0, &infin;) or mul(&infin;, 0.0).
   *   - Fused multiply add: fma(0.0, &infin;, c) or fma(&infin;, 0.0, c)
   *     unless c is a quiet NaN, in which case it is implementation-defined
   *     if an exception is generated.
   *   - Addition, subtraction, or fused multiply add: magnitude subtraction
   *     of infinities, such as: add(+&infin;, -&infin;),
   *     sub(+&infin;, +&infin;).
   *   - Division: div(0.0, 0.0) or div(&infin;, &infin;).
   *   - Square root: sqrt(negative).
   *   - Conversion: A cvt with a floating-point source type, an integer
   *     destination type, and a nonsaturating rounding mode, when the source
   *     value is a NaN, &infin;, or the rounded value, after any flush to
   *     zero, cannot be represented precisely in the integer type of the
   *     destination.
   */
  HSA_ALT_EXCEPTION_INVALID_OPERATION = 1,

  /**
   * A finite non-zero floating-point value is divided by zero. It is
   * implementation defined if integer div or rem operations with a divisor of
   * zero will generate a divide by zero exception.
   */
  HSA_ALT_EXCEPTION_DIVIDE_BY_ZERO = 2,

  /**
   * The floating-point exponent of a value is too large to be represented.
   */
  HSA_ALT_EXCEPTION_OVERFLOW = 4,

  /**
   * A non-zero tiny floating-point value is computed and either the ftz
   * modifier is specified, or the ftz modifier was not specified and the value
   * cannot be represented exactly.
   */
  HSA_ALT_EXCEPTION_UNDERFLOW = 8,

  /**
   * A computed floating-point value is not represented exactly in the
   * destination. This can occur due to rounding. In addition, it is
   * implementation defined if operations with the ftz modifier that cause a
   * value to be flushed to zero generate the inexact exception.
   */
  HSA_ALT_EXCEPTION_INEXACT = 16
} hsa_alt_exception_kind_t;

/**
 * @brief Bit positions for HSAIL control directives.
 */
typedef enum {
  HSA_ALT_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,

  HSA_ALT_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,

  HSA_ALT_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,

  HSA_ALT_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  HSA_ALT_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,

  HSA_ALT_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,

  HSA_ALT_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,

  HSA_ALT_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,

  HSA_ALT_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,

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
   */
  HSA_ALT_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256
} hsa_alt_control_directive_present_t;

/**
 * @brief The hsa_alt_control_directives_t specifies the values for the HSAIL
 * control directives. These control how the finalizer generates code. This
 * struct is used both as an argument to ::hsa_alt_finalize to specify
 * values for the control directives, and is used in ::hsa_alt_code_descriptor_t
 * to record the values of the control directives that the finalizer used when
 * generating the code which either came from the finalizer argument or explicit
 * HSAIL control directives.
 */
typedef struct hsa_alt_control_directives_s {
  /**
   * Bitset indicating which control directives are enabled. If the value is 0,
   * the rest of the fields can be ignored. The bits are accessed using
   * ::hsa_alt_control_directive_present_t. Disabled control directives must be
   * 0. Control directives that are only present or absent (such as partial
   * workgroups) have no corresponding field as the presence of the bit in this
   * mask is sufficient.
   */
   uint64_t control_directives_mask;

  /**
   * Bitset of HSAIL exceptions (::hsa_alt_exception_kind_t) that must have the
   * BREAK policy enabled. If this set is not empty then the generated code may
   * have lower performance than if the set is empty. If the kernel being
   * finalized has any 'enable break exceptions' control directives, then the
   * values specified by this argument are unioned with the values in these
   * control directives. If any of the functions the kernel calls have an
   * 'enable break exceptions' control directive, then they must be equal or a
   * subset of, this union. Must be 0 if the bit
   * ::HSA_ALT_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS is not set in @a
   * control_directives_mask.
   */
  uint16_t break_exceptions_mask;

  /**
   * If enableDetectExceptions is not enabled then must be 0, otherwise must be
   * non-0 and specifies the set of HSAIL exceptions that must have the DETECT
   * policy enabled. If this set is not empty then the generated code may have
   * lower performance than if the set is empty. However, an implementation
   * should endeavor to make the performance impact small. If the kernel being
   * finalized has any enabledetectexceptions control directives, then the
   * values specified by this argument are unioned with the values in these
   * control directives. If any of the functions the kernel calls have an
   * enabledetectexceptions control directive, then they must be equal or a
   * subset of, this union.
   */
  uint16_t detect_exceptions_mask;

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
   * than 0.
   */
  uint32_t max_flat_grid_size;

  /**
   * If maxFlatWorkgroupSize is not enabled then must be 0, otherwise must be
   * greater than 0.
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
   * element must be greater than 0.
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
   * optimize work-group ID operations, and if the number of work-items in the
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
   * compute the absolute and flat work-group and work-item ID, and the dim
   * HSAIL operations.
   */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];
} hsa_alt_control_directives_t;

/**
 * @brief Code object types.
 */
typedef enum {
  /**
   * Not a code object.
   */
  HSA_ALT_CODE_NONE = 0,

  /**
   * HSAIL kernel that can be used with an AQL Kernel Dispatch packet.
   */
  HSA_ALT_CODE_KERNEL = 1,

  /**
   * HSAIL indirect function.
   */
  HSA_ALT_CODE_INDIRECT_FUNCTION = 2,

  /**
   * HSA runtime code objects. For example, partially linked code objects.
   */
  HSA_ALT_CODE_RUNTIME_FIRST = 0x40000000,
  HSA_ALT_CODE_RUNTIME_LAST = 0x7fffffff,

  /**
   * Vendor-specific code objects.
   */
  HSA_ALT_CODE_VENDOR_FIRST = 0x80000000,
  HSA_ALT_CODE_VENDOR_LAST = 0xffffffff
} hsa_alt_code_type_t;

/**
 * @brief  A fixed-width type used to represent values in
 * ::hsa_alt_code_type_t.
 */
typedef uint32_t hsa_alt_code_type32_t;

/**
 * @brief Each HSA component can support one or more call conventions. For
 * example, an HSA component may have different call conventions that each use a
 * different number of ISA registers to allow different numbers of wavefronts to
 * execute on a compute unit.
 */
typedef enum {
 /**
  * Finalizer-determined call convention.
  */
  HSA_ALT_CALL_CONVENTION_FINALIZER_DETERMINED = -1
} hsa_alt_call_convention_t;

/**
 * @brief  A fixed-width type used to represent values in
 * ::hsa_alt_call_convention_t.
 */
typedef uint32_t hsa_alt_call_convention32_t;

/**
 * @brief An entry offset into the code section of the BRIG module. The value is
 * the byte offset relative to the start of the section to the beginning of the
 * referenced entry. The value 0 is reserved to indicate that the offset does
 * not reference any entry.
 */
typedef uint32_t hsa_alt_brig_code_section_offset32_t;

/**
 * @brief Opaque code handle to the finalized code that includes the executable
 * ISA for the HSA component. It is used to populate the kernel object address
 * field in Kernel Dispatch packets.
 */
typedef uint64_t hsa_alt_code_t;

/**
 * @brief An opaque handle to the debug information.
 */
typedef uint64_t hsa_alt_debug_info_t;

/**
 * @brief Provides the information about a finalization of the kernel or
 * indirect function for a specific HSA component, and for indirect functions, a
 * specific call convention of that HSA component
 */
typedef struct hsa_alt_code_descriptor_s {
  /**
   * Module associated with the code descriptor.
   */
  hsa_alt_module_t module;

  /**
   * BRIG directive offset associated with the code descriptor.
   */
  hsa_alt_brig_code_section_offset32_t symbol;

  /**
   * Opaque code handle to the finalized code.
   */
  hsa_alt_code_t code;

  /**
   * Agent associated with the code descriptor.
   */
  hsa_agent_t agent;

  /**
   * Type of code object this code descriptor associated with.
   */
  hsa_alt_code_type32_t type;

  /**
   * Size of static group segment memory required by this code object (per
   * work-group), in bytes. Does not include any dynamically allocated group
   * segment memory that may be requested by the application when a kernel is
   * dispatched.
   */
  uint32_t group_segment_size;

  /**
   * Size of kernarg segment memory that is required to hold the values of the
   * kernel arguments, in bytes.
   */
  uint64_t kernarg_segment_size;

  /**
   * Size of private, spill, and arg segment memory required by this code object
   * (per work-item), in bytes.
   */
  uint32_t private_segment_size;

  /**
   * The maximum byte alignment of variables used by the kernel in the group
   * memory segment. Expressed as a power of 2. Must be at least HSA_POWERTWO_16
   */
  hsa_powertwo8_t group_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the kernarg
   * memory segment. Expressed as a power of 2. Must be at least HSA_POWERTWO_16
   */
  hsa_powertwo8_t kernarg_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the private
   * memory segment. Expressed as a power of 2. Must be at least HSA_POWERTWO_16
   */
  hsa_powertwo8_t private_segment_alignment;

  /**
   * Wavefront size. Must be a power of 2 in the [1,64] range. Used to support
   * runtime query that obtains wavefront size, which may be used by application
   * to allocate dynamic group memory and set the dispatch work-group size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   * Number of fbarriers used by the kernel and all the functions it
   * transitively calls. If the implementation uses group memory to allocate the
   * fbarriers then that amount must already be included in @a
   * group_segment_size.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   * Program call convention used in this code descriptor.
   */
  hsa_alt_call_convention32_t call_convention;

  /**
   * HSAIL profile.
   */
  hsa_alt_brig_profile8_t profile;

  /**
   * Machine model.
   */
  hsa_alt_brig_machine_model8_t machine_model;

  /**
   * Reserved. Must be 0.
   */
  uint16_t reserved1;

  /**
   * Debug information.
   */
  hsa_alt_debug_info_t debug_info;

  /**
   * HSAIL major version. This information is from the HSAIL version directive.
   */
  uint32_t hsail_version_major;

  /**
   * HSAIL minor version. This information is from the HSAIL version directive.
   */
  uint32_t hsail_version_minor;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * The values should be the actual values used by the finalizer in generating
   * the code. This may be the union of values specified as finalizer arguments
   * and explicit HSAIL control directives. If the finalizer chooses to ignore a
   * control directive and not generate constrained code, then the control
   * directive should not be marked as enabled even though it was present in the
   * HSAIL or finalizer argument. The values are intended to reflect the
   * constraints that the code actually requires to correctly execute, not the
   * values that were actually specified at finalize time.
   */
  hsa_alt_control_directives_t control_directives;
} hsa_alt_code_descriptor_t;

/**
 * @brief Kernels or indirect function to be finalized. The request can also
 * include finalization options, such as control directives.
 */
typedef struct hsa_alt_finalization_request_s {
  /**
   * Module where the kernel or indirection function is located.
   */
  hsa_alt_module_t module;
  /**
   * Entry offset (with respect to the start of the code section) of the kernel
   * or indirect function.
   */
  hsa_alt_brig_code_section_offset32_t symbol;
  /**
   * Program call convention used by the finalizer when compiling an indirect
   * function. Ignored when finalizing a kernel.
   */
  hsa_alt_call_convention32_t call_convention;
  /**
   * Control directives that influence how the finalizer generates code.
   */
  hsa_alt_control_directives_t control_directives;
} hsa_alt_finalization_request_t;

/**
 * @brief Callback function to get the string representation of the error
 * message.
 */
typedef hsa_status_t (*hsa_alt_error_message_callback_t)(
    void* callback_data,
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t statement,
    uint32_t indent_level,
    const char *message);

/**
 * @brief Finalizes provided HSAIL program.
 *
 * @details Finalizes provided HSAIL program. The HSA runtime finalizer can be
 * used to generate code for kernels and indirect functions from a specific
 * program for a specific HSA component. A kernel can only be finalized once per
 * program per HSA agent. An indirect function can only finalized once per
 * program per HSA agent per call convention. Only code for HSA components
 * specified when the program was created can be requested. The program must
 * contain a definition for the requested kernels and indirect functions among
 * the modules that have been added to the program. The modules of the program
 * must collectively define all variables, fbarriers, kernels and functions
 * referenced by operations in the code block. In addition, the caller of this
 * function can specify control directives as an input argument, which will be
 * passed to the finalizer. These control directives can be used for low-level
 * performance tuning.
 *
 * @param[in] num_finalization_requests Size of @p finalization_requests.
 *
 * @param[in] finalization_requests List of kernels and indirect functions that
 * are requested to be finalized. Must not be NULL.
 *
 * @param[in] agent HSA agent to finalize for.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @param[in] optimization_level An implementation defined value that controls
 * the level of optimization performed by the finalizer.
 *
 * @param[in] options Implementation defined options that can be passed to the
 * finalizer. For more information see the HSA Programmer's Reference Manual.
 *
 * @param[in] debug_info If true, debug information is included in the output.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid, or is not one
 * of the agents associated with the HSAIL program.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH The directive in the
 * control directive structure and in the HSAIL kernel mismatch, or if the same
 * directive is used with a different value in one of the functions used by this
 * kernel. The @p error_message_callback can be used to get the string
 * representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for finalization. The @p error_message_callback can be
 * used to get the string representation of the error.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p finalization_requests is NULL,
 * or @p num_finalization_requests is 0.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS @p options or @p
 * optimization_level are not recognized. No error is returned, just an info
 * status is used to indicate invalid options.
 */
hsa_status_t HSA_API hsa_alt_finalize(
    size_t num_finalization_requests,
    const hsa_alt_finalization_request_t *finalization_requests,
    hsa_agent_t agent,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data,
    uint8_t optimization_level,
    const char *options,
    bool debug_info);

/* /\** */
/*  * @brief Identifier of an HSA agent within a program. It is used to index a */
/*  * kernel descriptor to access the code descriptor for the HSA agent. An HSA */
/*  * agent can be associated with multiple programs, but have different program */
/*  * agent IDs in each program. */
/*  *\/ */
/* typedef uint32_t hsa_alt_program_agent_id_t; */

/* /\** */
/*  * @brief Get the identifier of an agent within a given program. */
/*  * */
/*  * @param[in] agent HSA agent. */
/*  * */
/*  * @param[in] program HSAIL program. */
/*  * */
/*  * @param[out] program_agent_id Memory location where the runtime stores the */
/*  * program agent ID. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been */
/*  * initialized. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The HSA agent is invalid. */
/*  * */
/*  * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program_agent_id is NULL. */
/*  *\/ */
/* hsa_status_t HSA_API hsa_alt_agent_get_program_agent_id( */
/*     hsa_agent_t agent, */
/*     hsa_alt_program_t program, */
/*     hsa_alt_program_agent_id_t* program_agent_id); */

/**
 * @brief Get the kernel descriptor of a finalized kernel for a given agent.
 *
 * @param[in] module HSAIL module to query kernel descriptor address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[in] agent Agent.
 *
 * @param[out] descriptor Memory location where the runtime stores the kernel
 * descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p descriptor is NULL.
 */
hsa_status_t HSA_API hsa_alt_kernel_get_descriptor(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    hsa_alt_code_descriptor_t** descriptor);

/**
 * @brief Get the kernel descriptor of a finalized indirect function for a given
 * calling convention.
 *
 * @param[in] module HSAIL module to query kernel descriptor address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[in] agent Agent.
 *
 * @param[in] call_convention Calling convention
 *
 * @param[out] descriptor Memory location where the runtime stores the indirect
 * function descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The HSAIL program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p descriptor is NULL.
 */
hsa_status_t HSA_API hsa_alt_indirect_function_get_descriptor(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    hsa_alt_call_convention32_t call_convention,
    hsa_alt_code_descriptor_t** descriptor);


/* /\** */
/*  * @brief Queries kernel descriptor address from specified HSAIL program. */
/*  * Needed to create a Kernel Dispatch packet. */
/*  * */
/*  * @param[in] module HSAIL module to query kernel descriptor address from. */
/*  * */
/*  * @param[in] symbol Offset in the HSAIL module to get the address from. */
/*  * */
/*  * @param[out] kernel_descriptors The address of the kernel descriptor for the */
/*  * requested kernel, which is an array of ::hsa_alt_code_descriptor_t indexed by */
/*  * ::hsa_alt_program_agent_id_t. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been */
/*  * initialized. */
/*  * */
/*  * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p kernel_descriptors is NULL. */
/*  *\/ */
/* hsa_status_t HSA_API hsa_alt_kernel_get_descriptors( */
/*     hsa_alt_module_t module, */
/*     hsa_alt_brig_code_section_offset32_t symbol, */
/*     hsa_alt_code_descriptor_t** kernel_descriptors); */

/* /\** */
/*  * @brief Queries indirect function descriptor address from specified HSAIL */
/*  * program. Allows host program to perform indirect function table variable */
/*  * initialization. */
/*  * */
/*  * @param[in] module HSAIL module to query indirect function descriptor address */
/*  * from. */
/*  * */
/*  * @param[in] symbol Offset in the HSAIL module to get the address from. */
/*  * */
/*  * @param[out] indirect_function_descriptors The address of the indirect function */
/*  * descriptor for the requested indirect function, which is an array of */
/*  * ::hsa_alt_code_descriptor_t indexed by */
/*  * ::hsa_alt_call_convention32_t. */
/*  * */
/*  * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully. */
/*  * */
/*  * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid. */
/*  * */
/*  * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid, or @p */
/*  * module is invalid. */
/*  *\/ */
/* hsa_status_t HSA_API hsa_alt_indirect_function_get_descriptors( */
/*     hsa_alt_module_t module, */
/*     hsa_alt_brig_code_section_offset32_t symbol, */
/*     hsa_alt_code_descriptor_t** indirect_function_descriptors); */

/**
 * @brief Queries the definition of a module scope variable/fbarrier or
 * kernel/function.
 *
 * @param[in] module HSAIL module to query symbol definition from.
 *
 * @param[in] symbol Offset to query symbol definition from.
 *
 * @param[out] definition_module Queried HSAIL module handle.
 *
 * @param[out] definition_symbol Queried symbol.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p definition_module is NULL, or
 * @p definition_symbol is NULL.
 */
hsa_status_t HSA_API hsa_alt_symbol_get_definition(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_alt_module_t *definition_module,
    hsa_alt_brig_code_section_offset32_t *definition_symbol);

/**
 * @brief Iterate over the call conventions associated with a given HSA program
 * and HSA agent, and invoke an application-defined callback on every iteration.
 *
 * @param[in] program A valid HSA program.
 *
 * @param[in] agent A valid HSA agent.
 *
 * @param[in] callback Callback to be invoked once per call convention used for
 * the specified HSA agent in the given HSA program.  The runtime passes two
 * arguments to the callback, the call convention and the application data.  If
 * @p callback returns a status other than ::HSA_STATUS_SUCCESS for a particular
 * iteration, the traversal stops and
 * ::hsa_alt_program_iterate_agent_call_conventions returns that status value.
 *
 * @param[in] data Application data that is passed to @p callback on every
 * iteration. Might be NULL.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT The agent is invalid.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM The program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p callback is NULL.
*/
hsa_status_t HSA_API hsa_alt_program_iterate_agent_call_conventions(
    hsa_alt_program_t program,
    hsa_agent_t agent,
    hsa_status_t (*callback)(hsa_alt_call_convention32_t call_convention, void* data),
    void* data);

/**
 * @brief Defines global variable address. Allows direct access to host
 * variables from HSAIL.
 *
 * @param[in] module HSAIL module to define global variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] address Address to define in HSAIL program.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_global_variable_set_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    void *address,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data);

/**
 * @brief Queries global variable address. Allows host program to directly
 * access variables.
 *
 * @param[in] module HSAIL module to query global variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_global_variable_get_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    void** address);

/**
 * @brief Defines global variable address for specified HSA agent. Allows direct
 * access to host variables from HSAIL.
 *
 * @param[in] module HSAIL module to define global variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] agent HSA agent to define global variable address for.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_global_variable_set_agent_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    void *address,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data);

/**
 * @brief Queries global variable address for a given HSA agent. Allows host
 * program to directly access variables.
 *
 * @param[in] module HSAIL module to query global variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[in] agent HSA agent to query global variable address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_global_variable_get_agent_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    void** address);

/**
 * @brief Defines readonly variable address for a given HSA agent. Allows direct
 * access to host variables from HSAIL.
 *
 * @param[in] module HSAIL module to define readonly variable address for.
 *
 * @param[in] symbol Offset in the HSAIL module to put the address on.
 *
 * @param[in] agent HSA agent to define readonly variable address for.
 *
 * @param[in] address Address to define for HSA agent in HSAIL program.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_readonly_variable_set_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    void* address,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data);

/**
 * @brief Queries readonly variable address for a given HSA agent. Allows host
 * program to directly access variables.
 *
 * @param[in] agent HSA agent to query readonly variable address from.
 *
 * @param[in] module HSAIL module to query readonly variable address from.
 *
 * @param[in] symbol Offset in the HSAIL module to get the address from.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_AGENT @p agent is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p address is NULL.
 */
hsa_status_t HSA_API hsa_alt_readonly_variable_get_address(
    hsa_alt_module_t module,
    hsa_alt_brig_code_section_offset32_t symbol,
    hsa_agent_t agent,
    void** address);


/** @} */

/** \defgroup alt-finalizer-miscellaneous HSAIL Finalization
 *  @{
 */

/**
 * @brief Validates HSAIL program with specified HSAIL program handle. Checks if
 * all declarations and definitions match, if there is at most one
 * definition. The caller decides when to call validation routines. For example,
 * it can be done in the debug mode.
 *
 * @param[in] program HSAIL program to validate.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @param[out] result Validation result. A value of 0 indicates that the module
 * is invalid.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 */
hsa_status_t HSA_API hsa_alt_program_validate(
    hsa_alt_program_t program,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data,
    int* result);

/**
 * @brief Validates specified HSAIL module with specified HSAIL module handle
 * for specified HSAIL program. Checks if BRIG for specified module is legal:
 * operation operand type rules, etc. The caller decides when to call validation
 * routines. For example, it can be done in the debug mode.
 *
 * @param[in] module HSAIL module to validate.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to @p
 * error_message_callback. Might be NULL. If @p error_message_callback is NULL,
 * this argument is ignored.
 *
 * @param[out] result Validation result. A value of 0 indicates that the module
 * is invalid.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_MODULE @p module is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p result is NULL.
 */
hsa_status_t HSA_API hsa_alt_module_validate(
    hsa_alt_module_t module,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data,
    int* result);

/**
 * @brief Call back function for allocating data.
 */
typedef hsa_status_t (*hsa_alt_alloc_data_callback_t)(
  void* callback_data,
  size_t size,
  void **address);

/**
 * @brief Serializes specified HSAIL program. Used for offline compilation.
 *
 * @param[in] program HSAIL program to be serialized.
 *
 * @param[in] alloc_data_callback Callback function for memory
 * allocation.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message (if any). Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to the callback
 * functions. Might be NULL.
 *
 * @param[in] debug_info If true, debug information is included in the output.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_INVALID_PROGRAM @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p program is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for serialization. The @p error_message_callback can be
 * used to get the string representation of the error.
 */
hsa_status_t HSA_API hsa_alt_program_serialize(
    hsa_alt_program_t program,
    hsa_alt_alloc_data_callback_t alloc_data_callback,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data,
    bool debug_info,
    void** serialized_object);

/**
 * @brief Callback function to get program's address of global segment
 * variables, kernel table variable, indirect function table variable based on
 * the symbolic name.
 */
typedef hsa_status_t (*hsa_alt_program_symbol_address_callback_t)(
    void* callback_data,
    const char *name,
    uint64_t *symbol_address);

/**
 * @brief Callback function to get HSA agent's address of global segment
 * variables, kernel table variable, indirect function table variable based on
 * the symbolic name.
 */
typedef hsa_status_t (*hsa_alt_agent_symbol_address_callback_t)(
    void* callback_data,
    hsa_agent_t agent,
    const char *name,
    uint64_t *symbol_address);

/**
 * @brief Deserializes the HSAIL program from a given serialized object. Used
 * for offline compilation. Includes callback functions, where callback
 * functions take symbolic name. This allows symbols defined by application to
 * be relocated.
 *
 * @param[in] serialized_object Serialized HSAIL program. Must not be NULL.
 *
 * @param[in] program_symbol_address_callback Callback function to
 * get program's address of global segment variables, kernel table variable,
 * indirect function table variable based on the symbolic name. Allows symbols
 * defined by application to be relocated.
 *
 * @param[in] agent_symbol_address_callback Callback function to get
 * HSA agent's address of global segment variables, kernel table variable,
 * indirect function table variable based on the symbolic name. Allows symbols
 * defined by application to be relocated.
 *
 * @param[in] error_message_callback Callback function to get the string
 * representation of the error message. Might be NULL.
 *
 * @param[in] callback_data Application data that is passed to the callback
 * functions. Might be NULL.
 *
 * @param[in] debug_info If true, debug information is included in the output.
 *
 * @param[out] program Deserialized HSAIL program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT @p serialized_object is
 * NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES There is a failure to allocate
 * resources required for deserialization. The @p error_message_callback can be
 * used to get the string representation of the error.
 */
hsa_status_t HSA_API hsa_alt_program_deserialize(
    void *serialized_object,
    hsa_alt_program_symbol_address_callback_t program_symbol_address_callback,
    hsa_alt_agent_symbol_address_callback_t agent_symbol_address_callback,
    hsa_alt_error_message_callback_t error_message_callback,
    void* callback_data,
    bool debug_info,
    hsa_alt_program_t *program);
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
    * HSA component-specific handle to the image.
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
    * HSA component-specific sampler.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The HSA agent does
 * not support the image format specified by the descriptor.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED The HSA agent does
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED The HSA agent does
 * not have the capability to support the image format using the specified @p
 * access_permission.
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * specified in a linear space, which is appropriately converted by the runtime
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
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
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT Any of the arguments are NULL.
 */
hsa_status_t HSA_API hsa_ext_sampler_destroy_handle(
    hsa_agent_t agent,
    hsa_ext_sampler_handle_t *sampler_handle);

/** @} */
#ifdef __cplusplus
}
#endif  /*__cplusplus*/
