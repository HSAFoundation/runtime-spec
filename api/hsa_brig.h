/**
 * Copyright (c) 2016, HSA Foundation, Inc
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

#ifndef HSA_BRIG_H
#define HSA_BRIG_H

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uintXX_t */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*****************************************************************************/

typedef uint16_t hsa_brig_kind16_t;
typedef uint32_t hsa_brig_version32_t;

typedef uint32_t hsa_brig_data_offset32_t;
typedef uint32_t hsa_brig_code_offset32_t;
typedef uint32_t hsa_brig_operand_offset32_t;
typedef hsa_brig_data_offset32_t hsa_brig_data_offset_string32_t;
typedef hsa_brig_data_offset32_t hsa_brig_data_offset_code_list32_t;
typedef hsa_brig_data_offset32_t hsa_brig_data_offset_operand_list32_t;

typedef uint8_t hsa_brig_alignment8_t;
typedef enum {
    HSA_BRIG_ALIGNMENT_NONE = 0,
    HSA_BRIG_ALIGNMENT_1 = 1,
    HSA_BRIG_ALIGNMENT_2 = 2,
    HSA_BRIG_ALIGNMENT_4 = 3,
    HSA_BRIG_ALIGNMENT_8 = 4,
    HSA_BRIG_ALIGNMENT_16 = 5,
    HSA_BRIG_ALIGNMENT_32 = 6,
    HSA_BRIG_ALIGNMENT_64 = 7,
    HSA_BRIG_ALIGNMENT_128 = 8,
    HSA_BRIG_ALIGNMENT_256 = 9,
    HSA_BRIG_ALIGNMENT_MAX = HSA_BRIG_ALIGNMENT_256
} hsa_brig_alignment_t;

typedef uint8_t hsa_brig_allocation8_t;
typedef enum {
    HSA_BRIG_ALLOCATION_NONE = 0,
    HSA_BRIG_ALLOCATION_PROGRAM = 1,
    HSA_BRIG_ALLOCATION_AGENT = 2,
    HSA_BRIG_ALLOCATION_AUTOMATIC = 3
} hsa_brig_allocation_t;

typedef uint8_t hsa_brig_alu_modifier8_t;
typedef enum {
    HSA_BRIG_ALU_MODIFIER_FTZ = 1
} hsa_brig_alu_modifier_t;

typedef uint8_t hsa_brig_atomic_operation8_t;
typedef enum {
    HSA_BRIG_ATOMIC_OPERATION_ADD = 0,
    HSA_BRIG_ATOMIC_OPERATION_AND = 1,
    HSA_BRIG_ATOMIC_OPERATION_CAS = 2,
    HSA_BRIG_ATOMIC_OPERATION_EXCH = 3,
    HSA_BRIG_ATOMIC_OPERATION_LD = 4,
    HSA_BRIG_ATOMIC_OPERATION_MAX = 5,
    HSA_BRIG_ATOMIC_OPERATION_MIN = 6,
    HSA_BRIG_ATOMIC_OPERATION_OR = 7,
    HSA_BRIG_ATOMIC_OPERATION_ST = 8,
    HSA_BRIG_ATOMIC_OPERATION_SUB = 9,
    HSA_BRIG_ATOMIC_OPERATION_WRAPDEC = 10,
    HSA_BRIG_ATOMIC_OPERATION_WRAPINC = 11,
    HSA_BRIG_ATOMIC_OPERATION_XOR = 12,
    HSA_BRIG_ATOMIC_OPERATION_WAIT_EQ = 13,
    HSA_BRIG_ATOMIC_OPERATION_WAIT_NE = 14,
    HSA_BRIG_ATOMIC_OPERATION_WAIT_LT = 15,
    HSA_BRIG_ATOMIC_OPERATION_WAIT_GTE = 16,
    HSA_BRIG_ATOMIC_OPERATION_WAITTIMEOUT_EQ = 17,
    HSA_BRIG_ATOMIC_OPERATION_WAITTIMEOUT_NE = 18,
    HSA_BRIG_ATOMIC_OPERATION_WAITTIMEOUT_LT = 19,
    HSA_BRIG_ATOMIC_OPERATION_WAITTIMEOUT_GTE = 20,
    HSA_BRIG_ATOMIC_OPERATION_FIRST_USER_DEFINED = 128
} hsa_brig_atomic_operation_t;

typedef struct hsa_brig_base_s {
    uint16_t byte_count;
    hsa_brig_kind16_t kind;
} hsa_brig_base_t;

typedef uint8_t hsa_brig_compare_operation8_t;
typedef enum {
    HSA_BRIG_COMPARE_OPERATION_EQ = 0,
    HSA_BRIG_COMPARE_OPERATION_NE = 1,
    HSA_BRIG_COMPARE_OPERATION_LT = 2,
    HSA_BRIG_COMPARE_OPERATION_LE = 3,
    HSA_BRIG_COMPARE_OPERATION_GT = 4,
    HSA_BRIG_COMPARE_OPERATION_GE = 5,
    HSA_BRIG_COMPARE_OPERATION_EQU = 6,
    HSA_BRIG_COMPARE_OPERATION_NEU = 7,
    HSA_BRIG_COMPARE_OPERATION_LTU = 8,
    HSA_BRIG_COMPARE_OPERATION_LEU = 9,
    HSA_BRIG_COMPARE_OPERATION_GTU = 10,
    HSA_BRIG_COMPARE_OPERATION_GEU = 11,
    HSA_BRIG_COMPARE_OPERATION_NUM = 12,
    HSA_BRIG_COMPARE_OPERATION_NAN = 13,
    HSA_BRIG_COMPARE_OPERATION_SEQ = 14,
    HSA_BRIG_COMPARE_OPERATION_SNE = 15,
    HSA_BRIG_COMPARE_OPERATION_SLT = 16,
    HSA_BRIG_COMPARE_OPERATION_SLE = 17,
    HSA_BRIG_COMPARE_OPERATION_SGT = 18,
    HSA_BRIG_COMPARE_OPERATION_SGE = 19,
    HSA_BRIG_COMPARE_OPERATION_SGEU = 20,
    HSA_BRIG_COMPARE_OPERATION_SEQU = 21,
    HSA_BRIG_COMPARE_OPERATION_SNEU = 22,
    HSA_BRIG_COMPARE_OPERATION_SLTU = 23,
    HSA_BRIG_COMPARE_OPERATION_SLEU = 24,
    HSA_BRIG_COMPARE_OPERATION_SNUM = 25,
    HSA_BRIG_COMPARE_OPERATION_SNAN = 26,
    HSA_BRIG_COMPARE_OPERATION_SGTU = 27,
    HSA_BRIG_COMPARE_OPERATION_FIRST_USER_DEFINED = 128
} hsa_brig_compare_operation_t;

typedef uint16_t hsa_brig_control_directive16_t;
typedef enum {
    HSA_BRIG_CONTROL_DIRECTIVE_NONE = 0,
    HSA_BRIG_CONTROL_DIRECTIVE_ENABLEBREAKEXCEPTIONS = 1,
    HSA_BRIG_CONTROL_DIRECTIVE_ENABLEDETECTEXCEPTIONS = 2,
    HSA_BRIG_CONTROL_DIRECTIVE_MAXDYNAMICGROUPSIZE = 3,
    HSA_BRIG_CONTROL_DIRECTIVE_MAXFLATGRIDSIZE = 4,
    HSA_BRIG_CONTROL_DIRECTIVE_MAXFLATWORKGROUPSIZE = 5,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIREDDIM = 6,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIREDGRIDSIZE = 7,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIREDWORKGROUPSIZE = 8,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIRENOPARTIALWORKGROUPS = 9,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIRENOPARTIALWAVEFRONTS = 10,
    HSA_BRIG_CONTROL_DIRECTIVE_REQUIREDGROUPBASEPTRALIGN = 11,
    HSA_BRIG_CONTROL_DIRECTIVE_FIRST_USER_DEFINED = 32768
} hsa_brig_control_directive_t;

typedef uint32_t hsa_brig_exceptions32_t;
typedef enum {
    HSA_BRIG_EXCEPTIONS_INVALID_OPERATION = 1 << 0,
    HSA_BRIG_EXCEPTIONS_DIVIDE_BY_ZERO = 1 << 1,
    HSA_BRIG_EXCEPTIONS_OVERFLOW = 1 << 2,
    HSA_BRIG_EXCEPTIONS_UNDERFLOW = 1 << 3,
    HSA_BRIG_EXCEPTIONS_INEXACT = 1 << 4,
    HSA_BRIG_EXCEPTIONS_FIRST_USER_DEFINED = 1 << 16
} hsa_brig_exceptions_t;

typedef uint8_t hsa_brig_executable_modifier8_t;
typedef enum {
    HSA_BRIG_EXECUTABLE_MODIFIER_DEFINITION = 1
} hsa_brig_executable_modifier_t;

typedef uint16_t hsa_brig_expression_operation16_t;
typedef enum {
    HSA_BRIG_EXPRESSION_OPERATION_NULLPTR_FLAT = 0,
    HSA_BRIG_EXPRESSION_OPERATION_NULLPTR_GROUP = 1,
    HSA_BRIG_EXPRESSION_OPERATION_NULLPTR_PRIVATE = 2,
    HSA_BRIG_EXPRESSION_OPERATION_NULLPTR_KERNARG = 3,
    HSA_BRIG_EXPRESSION_OPERATION_ADDR = 4,
    HSA_BRIG_EXPRESSION_OPERATION_FIRST_USER_DEFINED = 32768
} hsa_brig_expression_operation_t;

typedef uint8_t hsa_brig_image_channel_order8_t;
typedef enum {
    HSA_BRIG_IMAGE_CHANNEL_ORDER_A = 0,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_R = 1,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RX = 2,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RG = 3,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RGX = 4,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RA = 5,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RGB = 6,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RGBX = 7,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_RGBA = 8,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_BGRA = 9,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_ARGB = 10,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_ABGR = 11,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_SRGB = 12,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_SRGBX = 13,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_SRGBA = 14,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_SBGRA = 15,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_INTENSITY = 16,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_LUMINANCE = 17,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_DEPTH = 18,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_DEPTH_STENCIL = 19,
    HSA_BRIG_IMAGE_CHANNEL_ORDER_FIRST_USER_DEFINED = 128
} hsa_brig_image_channel_order_t;

typedef uint8_t hsa_brig_image_channel_type8_t;
typedef enum {
    HSA_BRIG_IMAGE_CHANNEL_TYPE_SNORM_INT8 = 0,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_SNORM_INT16 = 1,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_INT8 = 2,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_INT16 = 3,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_INT24 = 4,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_SHORT_555 = 5,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_SHORT_565 = 6,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNORM_INT_101010 = 7,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_SIGNED_INT8 = 8,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_SIGNED_INT16 = 9,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_SIGNED_INT32 = 10,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNSIGNED_INT8 = 11,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNSIGNED_INT16 = 12,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_UNSIGNED_INT32 = 13,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_HALF_FLOAT = 14,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_FLOAT = 15,
    HSA_BRIG_IMAGE_CHANNEL_TYPE_FIRST_USER_DEFINED = 128
} hsa_brig_image_channel_type_t;

typedef uint8_t hsa_brig_image_geometry8_t;
typedef enum {
    HSA_BRIG_IMAGE_GEOMETRY_1D = 0,
    HSA_BRIG_IMAGE_GEOMETRY_2D = 1,
    HSA_BRIG_IMAGE_GEOMETRY_3D = 2,
    HSA_BRIG_IMAGE_GEOMETRY_1DA = 3,
    HSA_BRIG_IMAGE_GEOMETRY_2DA = 4,
    HSA_BRIG_IMAGE_GEOMETRY_1DB = 5,
    HSA_BRIG_IMAGE_GEOMETRY_2DDEPTH = 6,
    HSA_BRIG_IMAGE_GEOMETRY_2DADEPTH = 7,
    HSA_BRIG_IMAGE_GEOMETRY_FIRST_USER_DEFINED = 128
} hsa_brig_image_geometry_t;

typedef uint8_t hsa_brig_image_query8_t;
typedef enum {
    HSA_BRIG_IMAGE_QUERY_WIDTH = 0,
    HSA_BRIG_IMAGE_QUERY_HEIGHT = 1,
    HSA_BRIG_IMAGE_QUERY_DEPTH = 2,
    HSA_BRIG_IMAGE_QUERY_ARRAY = 3,
    HSA_BRIG_IMAGE_QUERY_CHANNELORDER = 4,
    HSA_BRIG_IMAGE_QUERY_CHANNELTYPE = 5,
    HSA_BRIG_IMAGE_QUERY_FIRST_USER_DEFINED = 128
} hsa_brig_image_query_t;

typedef enum {
    HSA_BRIG_KIND_NONE = 0x0000,
    HSA_BRIG_KIND_DIRECTIVE_BEGIN = 0x1000,
        HSA_BRIG_KIND_DIRECTIVE_ARG_BLOCK_END = 0x1000,
        HSA_BRIG_KIND_DIRECTIVE_ARG_BLOCK_START = 0x1001,
        HSA_BRIG_KIND_DIRECTIVE_COMMENT = 0x1002,
        HSA_BRIG_KIND_DIRECTIVE_CONTROL = 0x1003,
        HSA_BRIG_KIND_DIRECTIVE_EXTENSION = 0x1004,
        HSA_BRIG_KIND_DIRECTIVE_FBARRIER = 0x1005,
        HSA_BRIG_KIND_DIRECTIVE_FUNCTION = 0x1006,
        HSA_BRIG_KIND_DIRECTIVE_INDIRECT_FUNCTION = 0x1007,
        HSA_BRIG_KIND_DIRECTIVE_KERNEL = 0x1008,
        HSA_BRIG_KIND_DIRECTIVE_LABEL = 0x1009,
        HSA_BRIG_KIND_DIRECTIVE_LOC = 0x100a,
        HSA_BRIG_KIND_DIRECTIVE_MODULE = 0x100b,
        HSA_BRIG_KIND_DIRECTIVE_PRAGMA = 0x100c,
        HSA_BRIG_KIND_DIRECTIVE_SIGNATURE = 0x100d,
        HSA_BRIG_KIND_DIRECTIVE_VARIABLE = 0x100e,
        HSA_BRIG_KIND_DIRECTIVE_EXTENSION_VERSION = 0x100f,
    HSA_BRIG_KIND_DIRECTIVE_END = 0x1010,
    HSA_BRIG_KIND_INST_BEGIN = 0x2000,
        HSA_BRIG_KIND_INST_ADDR = 0x2000,
        HSA_BRIG_KIND_INST_ATOMIC = 0x2001,
        HSA_BRIG_KIND_INST_BASIC = 0x2002,
        HSA_BRIG_KIND_INST_BR = 0x2003,
        HSA_BRIG_KIND_INST_CMP = 0x2004,
        HSA_BRIG_KIND_INST_CVT = 0x2005,
        HSA_BRIG_KIND_INST_IMAGE = 0x2006,
        HSA_BRIG_KIND_INST_LANE = 0x2007,
        HSA_BRIG_KIND_INST_MEM = 0x2008,
        HSA_BRIG_KIND_INST_MEM_FENCE = 0x2009,
        HSA_BRIG_KIND_INST_MOD = 0x200a,
        HSA_BRIG_KIND_INST_QUERY_IMAGE = 0x200b,
        HSA_BRIG_KIND_INST_QUERY_SAMPLER = 0x200c,
        HSA_BRIG_KIND_INST_QUEUE = 0x200d,
        HSA_BRIG_KIND_INST_SEG = 0x200e,
        HSA_BRIG_KIND_INST_SEG_CVT = 0x200f,
        HSA_BRIG_KIND_INST_SIGNAL = 0x2010,
        HSA_BRIG_KIND_INST_SOURCE_TYPE = 0x2011,
    HSA_BRIG_KIND_INST_END = 0x2012,
    HSA_BRIG_KIND_OPERAND_BEGIN = 0x3000,
        HSA_BRIG_KIND_OPERAND_ADDRESS = 0x3000,
        HSA_BRIG_KIND_OPERAND_ALIGN = 0x3001,
        HSA_BRIG_KIND_OPERAND_CODE_LIST = 0x3002,
        HSA_BRIG_KIND_OPERAND_CODE_REF = 0x3003,
        HSA_BRIG_KIND_OPERAND_CONSTANT_BYTES = 0x3004,
        HSA_BRIG_KIND_OPERAND_CONSTANT_EXPRESSION = 0x3005,
        HSA_BRIG_KIND_OPERAND_CONSTANT_IMAGE = 0x3006,
        HSA_BRIG_KIND_OPERAND_CONSTANT_OPERAND_LIST = 0x3007,
        HSA_BRIG_KIND_OPERAND_CONSTANT_SAMPLER = 0x3008,
        HSA_BRIG_KIND_OPERAND_OPERAND_LIST = 0x3009,
        HSA_BRIG_KIND_OPERAND_REGISTER = 0x300a,
        HSA_BRIG_KIND_OPERAND_STRING = 0x300b,
        HSA_BRIG_KIND_OPERAND_WAVESIZE = 0x3009c,
        HSA_BRIG_KIND_OPERAND_ZERO = 0x3009d,
    HSA_BRIG_KIND_OPERAND_END = 0x300e
} hsa_brig_kind_t;

typedef uint8_t hsa_brig_linkage8_t;
typedef enum {
    HSA_BRIG_LINKAGE_NONE = 0,
    HSA_BRIG_LINKAGE_PROGRAM = 1,
    HSA_BRIG_LINKAGE_MODULE = 2,
    HSA_BRIG_LINKAGE_FUNCTION = 3,
    HSA_BRIG_LINKAGE_ARG = 4
} hsa_brig_linkage_t;

typedef uint8_t hsa_brig_machine_model8_t;
typedef enum {
    HSA_BRIG_MACHINE_MODEL_SMALL = 0,
    HSA_BRIG_MACHINE_MODEL_LARGE = 1
} hsa_brig_machine_model_t;

typedef uint8_t hsa_brig_memory_modifier8_t;
typedef enum {
    HSA_BRIG_MEMORY_MODIFIER_CONST = 1,
    HSA_BRIG_MEMORY_MODIFIER_NONTEMPORAL = 2
} hsa_brig_memory_modifier_t;

typedef uint8_t hsa_brig_memory_order8_t;
typedef enum {
    HSA_BRIG_MEMORY_ORDER_NONE = 0,
    HSA_BRIG_MEMORY_ORDER_RELAXED = 1,
    HSA_BRIG_MEMORY_ORDER_SC_ACQUIRE = 2,
    HSA_BRIG_MEMORY_ORDER_SC_RELEASE = 3,
    HSA_BRIG_MEMORY_ORDER_SC_ACQUIRE_RELEASE = 4
} hsa_brig_memory_order_t;

typedef uint8_t hsa_brig_memory_scope8_t;
typedef enum {
    HSA_BRIG_MEMORY_SCOPE_NONE = 0,
    HSA_BRIG_MEMORY_SCOPE_WORKITEM = 1,
    HSA_BRIG_MEMORY_SCOPE_WAVEFRONT = 2,
    HSA_BRIG_MEMORY_SCOPE_WORKGROUP = 3,
    HSA_BRIG_MEMORY_SCOPE_AGENT = 4,
    HSA_BRIG_MEMORY_SCOPE_SYSTEM = 5
} hsa_brig_memory_scope_t;

typedef struct hsa_brig_module_header_s {
    char identification[8];
    hsa_brig_version32_t brig_major;
    hsa_brig_version32_t brig_minor;
    uint64_t byte_count;
    uint8_t hash[64];
    uint32_t reserved;
    uint32_t section_count;
    uint64_t section_index;
} hsa_brig_module_header_t;

typedef hsa_brig_module_header_t* hsa_brig_module_t;

typedef uint16_t hsa_brig_opcode16_t;
typedef enum {
    HSA_BRIG_OPCODE_NOP = 0,
    HSA_BRIG_OPCODE_ABS = 1,
    HSA_BRIG_OPCODE_ADD = 2,
    HSA_BRIG_OPCODE_BORROW = 3,
    HSA_BRIG_OPCODE_CARRY = 4,
    HSA_BRIG_OPCODE_CEIL = 5,
    HSA_BRIG_OPCODE_COPYSIGN = 6,
    HSA_BRIG_OPCODE_DIV = 7,
    HSA_BRIG_OPCODE_FLOOR = 8,
    HSA_BRIG_OPCODE_FMA = 9,
    HSA_BRIG_OPCODE_FRACT = 10,
    HSA_BRIG_OPCODE_MAD = 11,
    HSA_BRIG_OPCODE_MAX = 12,
    HSA_BRIG_OPCODE_MIN = 13,
    HSA_BRIG_OPCODE_MUL = 14,
    HSA_BRIG_OPCODE_MULHI = 15,
    HSA_BRIG_OPCODE_NEG = 16,
    HSA_BRIG_OPCODE_REM = 17,
    HSA_BRIG_OPCODE_RINT = 18,
    HSA_BRIG_OPCODE_SQRT = 19,
    HSA_BRIG_OPCODE_SUB = 20,
    HSA_BRIG_OPCODE_TRUNC = 21,
    HSA_BRIG_OPCODE_MAD24 = 22,
    HSA_BRIG_OPCODE_MAD24HI = 23,
    HSA_BRIG_OPCODE_MUL24 = 24,
    HSA_BRIG_OPCODE_MUL24HI = 25,
    HSA_BRIG_OPCODE_SHL = 26,
    HSA_BRIG_OPCODE_SHR = 27,
    HSA_BRIG_OPCODE_AND = 28,
    HSA_BRIG_OPCODE_NOT = 29,
    HSA_BRIG_OPCODE_OR = 30,
    HSA_BRIG_OPCODE_POPCOUNT = 31,
    HSA_BRIG_OPCODE_XOR = 32,
    HSA_BRIG_OPCODE_BITEXTRACT = 33,
    HSA_BRIG_OPCODE_BITINSERT = 34,
    HSA_BRIG_OPCODE_BITMASK = 35,
    HSA_BRIG_OPCODE_BITREV = 36,
    HSA_BRIG_OPCODE_BITSELECT = 37,
    HSA_BRIG_OPCODE_FIRSTBIT = 38,
    HSA_BRIG_OPCODE_LASTBIT = 39,
    HSA_BRIG_OPCODE_COMBINE = 40,
    HSA_BRIG_OPCODE_EXPAND = 41,
    HSA_BRIG_OPCODE_LDA = 42,
    HSA_BRIG_OPCODE_MOV = 43,
    HSA_BRIG_OPCODE_SHUFFLE = 44,
    HSA_BRIG_OPCODE_UNPACKHI = 45,
    HSA_BRIG_OPCODE_UNPACKLO = 46,
    HSA_BRIG_OPCODE_PACK = 47,
    HSA_BRIG_OPCODE_UNPACK = 48,
    HSA_BRIG_OPCODE_CMOV = 49,
    HSA_BRIG_OPCODE_CLASS = 50,
    HSA_BRIG_OPCODE_NCOS = 51,
    HSA_BRIG_OPCODE_NEXP2 = 52,
    HSA_BRIG_OPCODE_NFMA = 53,
    HSA_BRIG_OPCODE_NLOG2 = 54,
    HSA_BRIG_OPCODE_NRCP = 55,
    HSA_BRIG_OPCODE_NRSQRT = 56,
    HSA_BRIG_OPCODE_NSIN = 57,
    HSA_BRIG_OPCODE_NSQRT = 58,
    HSA_BRIG_OPCODE_BITALIGN = 59,
    HSA_BRIG_OPCODE_BYTEALIGN = 60,
    HSA_BRIG_OPCODE_PACKCVT = 61,
    HSA_BRIG_OPCODE_UNPACKCVT = 62,
    HSA_BRIG_OPCODE_LERP = 63,
    HSA_BRIG_OPCODE_SAD = 64,
    HSA_BRIG_OPCODE_SADHI = 65,
    HSA_BRIG_OPCODE_SEGMENTP = 66,
    HSA_BRIG_OPCODE_FTOS = 67,
    HSA_BRIG_OPCODE_STOF = 68,
    HSA_BRIG_OPCODE_CMP = 69,
    HSA_BRIG_OPCODE_CVT = 70,
    HSA_BRIG_OPCODE_LD = 71,
    HSA_BRIG_OPCODE_ST = 72,
    HSA_BRIG_OPCODE_ATOMIC = 73,
    HSA_BRIG_OPCODE_ATOMICNORET = 74,
    HSA_BRIG_OPCODE_SIGNAL = 75,
    HSA_BRIG_OPCODE_SIGNALNORET = 76,
    HSA_BRIG_OPCODE_MEMFENCE = 77,
    HSA_BRIG_OPCODE_RDIMAGE = 78,
    HSA_BRIG_OPCODE_LDIMAGE = 79,
    HSA_BRIG_OPCODE_STIMAGE = 80,
    HSA_BRIG_OPCODE_IMAGEFENCE = 81,
    HSA_BRIG_OPCODE_QUERYIMAGE = 82,
    HSA_BRIG_OPCODE_QUERYSAMPLER = 83,
    HSA_BRIG_OPCODE_CBR = 84,
    HSA_BRIG_OPCODE_BR = 85,
    HSA_BRIG_OPCODE_SBR = 86,
    HSA_BRIG_OPCODE_BARRIER = 87,
    HSA_BRIG_OPCODE_WAVEBARRIER = 88,
    HSA_BRIG_OPCODE_ARRIVEFBAR = 89,
    HSA_BRIG_OPCODE_INITFBAR = 90,
    HSA_BRIG_OPCODE_JOINFBAR = 91,
    HSA_BRIG_OPCODE_LEAVEFBAR = 92,
    HSA_BRIG_OPCODE_RELEASEFBAR = 93,
    HSA_BRIG_OPCODE_WAITFBAR = 94,
    HSA_BRIG_OPCODE_LDF = 95,
    HSA_BRIG_OPCODE_ACTIVELANECOUNT = 96,
    HSA_BRIG_OPCODE_ACTIVELANEID = 97,
    HSA_BRIG_OPCODE_ACTIVELANEMASK = 98,
    HSA_BRIG_OPCODE_ACTIVELANEPERMUTE = 99,
    HSA_BRIG_OPCODE_CALL = 100,
    HSA_BRIG_OPCODE_SCALL = 101,
    HSA_BRIG_OPCODE_ICALL = 102,
    HSA_BRIG_OPCODE_RET = 103,
    HSA_BRIG_OPCODE_ALLOCA = 104,
    HSA_BRIG_OPCODE_CURRENTWORKGROUPSIZE = 105,
    HSA_BRIG_OPCODE_CURRENTWORKITEMFLATID = 106,
    HSA_BRIG_OPCODE_DIM = 107,
    HSA_BRIG_OPCODE_GRIDGROUPS = 108,
    HSA_BRIG_OPCODE_GRIDSIZE = 109,
    HSA_BRIG_OPCODE_PACKETCOMPLETIONSIG = 110,
    HSA_BRIG_OPCODE_PACKETID = 111,
    HSA_BRIG_OPCODE_WORKGROUPID = 112,
    HSA_BRIG_OPCODE_WORKGROUPSIZE = 113,
    HSA_BRIG_OPCODE_WORKITEMABSID = 114,
    HSA_BRIG_OPCODE_WORKITEMFLATABSID = 115,
    HSA_BRIG_OPCODE_WORKITEMFLATID = 116,
    HSA_BRIG_OPCODE_WORKITEMID = 117,
    HSA_BRIG_OPCODE_CLEARDETECTEXCEPT = 118,
    HSA_BRIG_OPCODE_GETDETECTEXCEPT = 119,
    HSA_BRIG_OPCODE_SETDETECTEXCEPT = 120,
    HSA_BRIG_OPCODE_ADDQUEUEWRITEINDEX = 121,
    HSA_BRIG_OPCODE_CASQUEUEWRITEINDEX = 122,
    HSA_BRIG_OPCODE_LDQUEUEREADINDEX = 123,
    HSA_BRIG_OPCODE_LDQUEUEWRITEINDEX = 124,
    HSA_BRIG_OPCODE_STQUEUEREADINDEX = 125,
    HSA_BRIG_OPCODE_STQUEUEWRITEINDEX = 126,
    HSA_BRIG_OPCODE_CLOCK = 127,
    HSA_BRIG_OPCODE_CUID = 128,
    HSA_BRIG_OPCODE_DEBUGTRAP = 129,
    HSA_BRIG_OPCODE_GROUPBASEPTR = 130,
    HSA_BRIG_OPCODE_KERNARGBASEPTR = 131,
    HSA_BRIG_OPCODE_LANEID = 132,
    HSA_BRIG_OPCODE_MAXCUID = 133,
    HSA_BRIG_OPCODE_MAXWAVEID = 134,
    HSA_BRIG_OPCODE_NULLPTR = 135,
    HSA_BRIG_OPCODE_WAVEID = 136,
    HSA_BRIG_OPCODE_GROUPSTATICSIZE = 137,
    HSA_BRIG_OPCODE_GROUPTOTALSIZE = 138,
    HSA_BRIG_OPCODE_FIRST_USER_DEFINED = 32768
} hsa_brig_opcode_t;

typedef uint8_t hsa_brig_pack8_t;
typedef enum {
    HSA_BRIG_PACK_NONE = 0,
    HSA_BRIG_PACK_PP = 1,
    HSA_BRIG_PACK_PS = 2,
    HSA_BRIG_PACK_SP = 3,
    HSA_BRIG_PACK_SS = 4,
    HSA_BRIG_PACK_S = 5,
    HSA_BRIG_PACK_P = 6,
    HSA_BRIG_PACK_PPSAT = 7,
    HSA_BRIG_PACK_PSSAT = 8,
    HSA_BRIG_PACK_SPSAT = 9,
    HSA_BRIG_PACK_SSSAT = 10,
    HSA_BRIG_PACK_SSAT = 11,
    HSA_BRIG_PACK_PSAT = 12
} hsa_brig_pack_t;

typedef uint8_t hsa_brig_profile8_t;
typedef enum {
    HSA_BRIG_PROFILE_BASE = 0,
    HSA_BRIG_PROFILE_FULL = 1
} hsa_brig_profile_t;

typedef uint16_t hsa_brig_register_kind16_t;
typedef enum {
    HSA_BRIG_REGISTER_KIND_CONTROL = 0,
    HSA_BRIG_REGISTER_KIND_SINGLE = 1,
    HSA_BRIG_REGISTER_KIND_DOUBLE = 2,
    HSA_BRIG_REGISTER_KIND_QUAD = 3
} hsa_brig_register_kind_t;

typedef uint8_t hsa_brig_round8_t;
typedef enum {
    HSA_BRIG_ROUND_NONE = 0,
    HSA_BRIG_ROUND_FLOAT_DEFAULT = 1,
    HSA_BRIG_ROUND_FLOAT_NEAR_EVEN = 2,
    HSA_BRIG_ROUND_FLOAT_ZERO = 3,
    HSA_BRIG_ROUND_FLOAT_PLUS_INFINITY = 4,
    HSA_BRIG_ROUND_FLOAT_MINUS_INFINITY = 5,
    HSA_BRIG_ROUND_INTEGER_NEAR_EVEN = 6,
    HSA_BRIG_ROUND_INTEGER_ZERO = 7,
    HSA_BRIG_ROUND_INTEGER_PLUS_INFINITY = 8,
    HSA_BRIG_ROUND_INTEGER_MINUS_INFINITY = 9,
    HSA_BRIG_ROUND_INTEGER_NEAR_EVEN_SAT = 10,
    HSA_BRIG_ROUND_INTEGER_ZERO_SAT = 11,
    HSA_BRIG_ROUND_INTEGER_PLUS_INFINITY_SAT = 12,
    HSA_BRIG_ROUND_INTEGER_MINUS_INFINITY_SAT = 13,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_NEAR_EVEN = 14,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_ZERO = 15,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_PLUS_INFINITY = 16,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_MINUS_INFINITY = 17,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_NEAR_EVEN_SAT = 18,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_ZERO_SAT = 19,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_PLUS_INFINITY_SAT = 20,
    HSA_BRIG_ROUND_INTEGER_SIGNALING_MINUS_INFINITY_SAT = 21
} hsa_brig_round_t;

typedef uint8_t hsa_brig_sampler_addressing8_t;
typedef enum {
    HSA_BRIG_SAMPLER_ADDRESSING_UNDEFINED = 0,
    HSA_BRIG_SAMPLER_ADDRESSING_CLAMP_TO_EDGE = 1,
    HSA_BRIG_SAMPLER_ADDRESSING_CLAMP_TO_BORDER = 2,
    HSA_BRIG_SAMPLER_ADDRESSING_REPEAT = 3,
    HSA_BRIG_SAMPLER_ADDRESSING_MIRRORED_REPEAT = 4,
    HSA_BRIG_SAMPLER_ADDRESSING_FIRST_USER_DEFINED = 128
} hsa_brig_sampler_addressing_t;

typedef uint8_t hsa_brig_sampler_coord_normalization8_t;
typedef enum {
    HSA_BRIG_SAMPLER_COORD_NORMALIZATION_UNNORMALIZED = 0,
    HSA_BRIG_SAMPLER_COORD_NORMALIZATION_NORMALIZED = 1
} hsa_brig_sampler_coord_normalization_t;

typedef uint8_t hsa_brig_sampler_filter8_t;
typedef enum {
    HSA_BRIG_SAMPLER_FILTER_NEAREST = 0,
    HSA_BRIG_SAMPLER_FILTER_LINEAR = 1,
    HSA_BRIG_SAMPLER_FILTER_FIRST_USER_DEFINED = 128
} hsa_brig_sampler_filter_t;

typedef uint8_t hsa_brig_sampler_query8_t;
typedef enum {
    HSA_BRIG_SAMPLER_QUERY_ADDRESSING = 0,
    HSA_BRIG_SAMPLER_QUERY_COORD = 1,
    HSA_BRIG_SAMPLER_QUERY_FILTER = 2,
    HSA_BRIG_SAMPLER_QUERY_FIRST_USER_DEFINED = 128
} hsa_brig_sampler_query_t;

typedef uint32_t hsa_brig_section_index32_t;
typedef enum {
    HSA_BRIG_SECTION_INDEX_DATA = 0,
    HSA_BRIG_SECTION_INDEX_CODE = 1,
    HSA_BRIG_SECTION_INDEX_OPERAND = 2,
    HSA_BRIG_SECTION_INDEX_FIRST_USER_DEFINED = 3
} hsa_brig_section_index_t;

typedef struct hsa_brig_section_header_s {
    uint64_t byte_count;
    uint32_t header_byte_count;
    uint32_t name_length;
    uint8_t name[1];
} hsa_brig_section_header_t;

typedef uint8_t hsa_brig_seg_cvt_modifier8_t;
typedef enum {
    HSA_BRIG_SEG_CVT_MODIFIER_NONULL = 1
} hsa_brig_seg_cvt_modifier_t;

typedef uint8_t hsa_brig_segment8_t;
typedef enum {
    HSA_BRIG_SEGMENT_NONE = 0,
    HSA_BRIG_SEGMENT_FLAT = 1,
    HSA_BRIG_SEGMENT_GLOBAL = 2,
    HSA_BRIG_SEGMENT_READONLY = 3,
    HSA_BRIG_SEGMENT_KERNARG = 4,
    HSA_BRIG_SEGMENT_GROUP = 5,
    HSA_BRIG_SEGMENT_PRIVATE = 6,
    HSA_BRIG_SEGMENT_SPILL = 7,
    HSA_BRIG_SEGMENT_ARG = 8,
    HSA_BRIG_SEGMENT_FIRST_USER_DEFINED = 128
} hsa_brig_segment_t;

typedef enum {
    HSA_BRIG_TYPE_CLASS_BASE_SIZE = 5,
    HSA_BRIG_TYPE_CLASS_PACK_SIZE = 2,
    HSA_BRIG_TYPE_CLASS_ARRAY_SIZE = 1,

    HSA_BRIG_TYPE_CLASS_BASE_SHIFT = 0,
    HSA_BRIG_TYPE_CLASS_PACK_SHIFT = HSA_BRIG_TYPE_CLASS_BASE_SHIFT + HSA_BRIG_TYPE_CLASS_BASE_SIZE,
    HSA_BRIG_TYPE_CLASS_ARRAY_SHIFT = HSA_BRIG_TYPE_CLASS_PACK_SHIFT + HSA_BRIG_TYPE_CLASS_PACK_SIZE,

    HSA_BRIG_TYPE_CLASS_BASE_MASK = ((1 << HSA_BRIG_TYPE_CLASS_BASE_SIZE) - 1)
                                        << HSA_BRIG_TYPE_CLASS_BASE_SHIFT,
    HSA_BRIG_TYPE_CLASS_PACK_MASK = ((1 << HSA_BRIG_TYPE_CLASS_PACK_SIZE) - 1)
                                        << HSA_BRIG_TYPE_CLASS_PACK_SHIFT,
    HSA_BRIG_TYPE_CLASS_ARRAY_MASK = ((1 << HSA_BRIG_TYPE_CLASS_ARRAY_SIZE) - 1)
                                        << HSA_BRIG_TYPE_CLASS_ARRAY_SHIFT,

    HSA_BRIG_TYPE_CLASS_PACK_NONE = 0 << HSA_BRIG_TYPE_CLASS_PACK_SHIFT,
    HSA_BRIG_TYPE_CLASS_PACK_32 = 1 << HSA_BRIG_TYPE_CLASS_PACK_SHIFT,
    HSA_BRIG_TYPE_CLASS_PACK_64 = 2 << HSA_BRIG_TYPE_CLASS_PACK_SHIFT,
    HSA_BRIG_TYPE_CLASS_PACK_128 = 3 << HSA_BRIG_TYPE_CLASS_PACK_SHIFT,

    HSA_BRIG_TYPE_CLASS_ARRAY = 1 << HSA_BRIG_TYPE_CLASS_ARRAY_SHIFT
} hsa_brig_type_class_t;

typedef uint16_t hsa_brig_type16_t;
typedef enum {
    HSA_BRIG_TYPE_NONE = 0,

    HSA_BRIG_TYPE_U8 = 1,
    HSA_BRIG_TYPE_U16 = 2,
    HSA_BRIG_TYPE_U32 = 3,
    HSA_BRIG_TYPE_U64 = 4,

    HSA_BRIG_TYPE_S8 = 5,
    HSA_BRIG_TYPE_S16 = 6,
    HSA_BRIG_TYPE_S32 = 7,
    HSA_BRIG_TYPE_S64 = 8,

    HSA_BRIG_TYPE_F16 = 9,
    HSA_BRIG_TYPE_F32 = 10,
    HSA_BRIG_TYPE_F64 = 11,

    HSA_BRIG_TYPE_B1 = 12,
    HSA_BRIG_TYPE_B8 = 13,
    HSA_BRIG_TYPE_B16 = 14,
    HSA_BRIG_TYPE_B32 = 15,
    HSA_BRIG_TYPE_B64 = 16,
    HSA_BRIG_TYPE_B128 = 17,

    HSA_BRIG_TYPE_SAMP = 18,
    HSA_BRIG_TYPE_ROIMG = 19,
    HSA_BRIG_TYPE_WOIMG = 20,
    HSA_BRIG_TYPE_RWIMG = 21,

    HSA_BRIG_TYPE_SIG32 = 22,
    HSA_BRIG_TYPE_SIG64 = 23,

    HSA_BRIG_TYPE_U8X4 = HSA_BRIG_TYPE_U8 | HSA_BRIG_TYPE_CLASS_PACK_32,
    HSA_BRIG_TYPE_U8X8 = HSA_BRIG_TYPE_U8 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_U8X16 = HSA_BRIG_TYPE_U8 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_U16X2 = HSA_BRIG_TYPE_U16 | HSA_BRIG_TYPE_CLASS_PACK_32,
    HSA_BRIG_TYPE_U16X4 = HSA_BRIG_TYPE_U16 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_U16X8 = HSA_BRIG_TYPE_U16 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_U32X2 = HSA_BRIG_TYPE_U32 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_U32X4 = HSA_BRIG_TYPE_U32 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_U64X2 = HSA_BRIG_TYPE_U64 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_S8X4 = HSA_BRIG_TYPE_S8 | HSA_BRIG_TYPE_CLASS_PACK_32,
    HSA_BRIG_TYPE_S8X8 = HSA_BRIG_TYPE_S8 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_S8X16 = HSA_BRIG_TYPE_S8 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_S16X2 = HSA_BRIG_TYPE_S16 | HSA_BRIG_TYPE_CLASS_PACK_32,
    HSA_BRIG_TYPE_S16X4 = HSA_BRIG_TYPE_S16 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_S16X8 = HSA_BRIG_TYPE_S16 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_S32X2 = HSA_BRIG_TYPE_S32 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_S32X4 = HSA_BRIG_TYPE_S32 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_S64X2 = HSA_BRIG_TYPE_S64 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_F16X2 = HSA_BRIG_TYPE_F16 | HSA_BRIG_TYPE_CLASS_PACK_32,
    HSA_BRIG_TYPE_F16X4 = HSA_BRIG_TYPE_F16 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_F16X8 = HSA_BRIG_TYPE_F16 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_F32X2 = HSA_BRIG_TYPE_F32 | HSA_BRIG_TYPE_CLASS_PACK_64,
    HSA_BRIG_TYPE_F32X4 = HSA_BRIG_TYPE_F32 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_F64X2 = HSA_BRIG_TYPE_F64 | HSA_BRIG_TYPE_CLASS_PACK_128,

    HSA_BRIG_TYPE_U8_ARRAY = HSA_BRIG_TYPE_U8 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U16_ARRAY = HSA_BRIG_TYPE_U16 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U32_ARRAY = HSA_BRIG_TYPE_U32 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U64_ARRAY = HSA_BRIG_TYPE_U64 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_S8_ARRAY = HSA_BRIG_TYPE_S8 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S16_ARRAY = HSA_BRIG_TYPE_S16 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S32_ARRAY = HSA_BRIG_TYPE_S32 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S64_ARRAY = HSA_BRIG_TYPE_S64 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_F16_ARRAY = HSA_BRIG_TYPE_F16 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_F32_ARRAY = HSA_BRIG_TYPE_F32 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_F64_ARRAY = HSA_BRIG_TYPE_F64 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_B8_ARRAY = HSA_BRIG_TYPE_B8 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_B16_ARRAY = HSA_BRIG_TYPE_B16 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_B32_ARRAY = HSA_BRIG_TYPE_B32 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_B64_ARRAY = HSA_BRIG_TYPE_B64 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_B128_ARRAY = HSA_BRIG_TYPE_B128 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_SAMP_ARRAY = HSA_BRIG_TYPE_SAMP | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_ROIMG_ARRAY = HSA_BRIG_TYPE_ROIMG | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_WOIMG_ARRAY = HSA_BRIG_TYPE_WOIMG | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_RWIMG_ARRAY = HSA_BRIG_TYPE_RWIMG | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_SIG32_ARRAY = HSA_BRIG_TYPE_SIG32 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_SIG64_ARRAY = HSA_BRIG_TYPE_SIG64 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_U8X4_ARRAY = HSA_BRIG_TYPE_U8X4 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U8X8_ARRAY = HSA_BRIG_TYPE_U8X8 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U8X16_ARRAY = HSA_BRIG_TYPE_U8X16 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_U16X2_ARRAY = HSA_BRIG_TYPE_U16X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U16X4_ARRAY = HSA_BRIG_TYPE_U16X4 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U16X8_ARRAY = HSA_BRIG_TYPE_U16X8 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_U32X2_ARRAY = HSA_BRIG_TYPE_U32X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_U32X4_ARRAY = HSA_BRIG_TYPE_U32X4 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_U64X2_ARRAY = HSA_BRIG_TYPE_U64X2 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_S8X4_ARRAY = HSA_BRIG_TYPE_S8X4 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S8X8_ARRAY = HSA_BRIG_TYPE_S8X8 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S8X16_ARRAY = HSA_BRIG_TYPE_S8X16 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_S16X2_ARRAY = HSA_BRIG_TYPE_S16X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S16X4_ARRAY = HSA_BRIG_TYPE_S16X4 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S16X8_ARRAY = HSA_BRIG_TYPE_S16X8 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_S32X2_ARRAY = HSA_BRIG_TYPE_S32X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_S32X4_ARRAY = HSA_BRIG_TYPE_S32X4 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_S64X2_ARRAY = HSA_BRIG_TYPE_S64X2 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_F16X2_ARRAY = HSA_BRIG_TYPE_F16X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_F16X4_ARRAY = HSA_BRIG_TYPE_F16X4 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_F16X8_ARRAY = HSA_BRIG_TYPE_F16X8 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_F32X2_ARRAY = HSA_BRIG_TYPE_F32X2 | HSA_BRIG_TYPE_CLASS_ARRAY,
    HSA_BRIG_TYPE_F32X4_ARRAY = HSA_BRIG_TYPE_F32X4 | HSA_BRIG_TYPE_CLASS_ARRAY,

    HSA_BRIG_TYPE_F64X2_ARRAY = HSA_BRIG_TYPE_F64X2 | HSA_BRIG_TYPE_CLASS_ARRAY
} hsa_brig_type_t;

typedef struct hsa_brig_uint64_s {
    uint32_t lo;
    uint32_t hi;
} hsa_brig_uint64_t;

typedef uint8_t hsa_brig_variable_modifier8_t;
typedef enum {
    HSA_BRIG_VARIABLE_MODIFIER_DEFINITION = 1,
    HSA_BRIG_VARIABLE_MODIFIER_CONST = 2
} hsa_brig_variable_modifier_t;

typedef enum {
    HSA_BRIG_VERSION_HSAIL_MAJOR = 1,
    HSA_BRIG_VERSION_HSAIL_MINOR = 1,
    HSA_BRIG_VERSION_BRIG_MAJOR = 1,
    HSA_BRIG_VERSION_BRIG_MINOR = 1
} hsa_brig_version_t;

typedef uint8_t hsa_brig_width8_t;
typedef enum {
    HSA_BRIG_WIDTH_NONE = 0,
    HSA_BRIG_WIDTH_1 = 1,
    HSA_BRIG_WIDTH_2 = 2,
    HSA_BRIG_WIDTH_4 = 3,
    HSA_BRIG_WIDTH_8 = 4,
    HSA_BRIG_WIDTH_16 = 5,
    HSA_BRIG_WIDTH_32 = 6,
    HSA_BRIG_WIDTH_64 = 7,
    HSA_BRIG_WIDTH_128 = 8,
    HSA_BRIG_WIDTH_256 = 9,
    HSA_BRIG_WIDTH_512 = 10,
    HSA_BRIG_WIDTH_1024 = 11,
    HSA_BRIG_WIDTH_2048 = 12,
    HSA_BRIG_WIDTH_4096 = 13,
    HSA_BRIG_WIDTH_8192 = 14,
    HSA_BRIG_WIDTH_16384 = 15,
    HSA_BRIG_WIDTH_32768 = 16,
    HSA_BRIG_WIDTH_65536 = 17,
    HSA_BRIG_WIDTH_131072 = 18,
    HSA_BRIG_WIDTH_262144 = 19,
    HSA_BRIG_WIDTH_524288 = 20,
    HSA_BRIG_WIDTH_1048576 = 21,
    HSA_BRIG_WIDTH_2097152 = 22,
    HSA_BRIG_WIDTH_4194304 = 23,
    HSA_BRIG_WIDTH_8388608 = 24,
    HSA_BRIG_WIDTH_16777216 = 25,
    HSA_BRIG_WIDTH_33554432 = 26,
    HSA_BRIG_WIDTH_67108864 = 27,
    HSA_BRIG_WIDTH_134217728 = 28,
    HSA_BRIG_WIDTH_268435456 = 29,
    HSA_BRIG_WIDTH_536870912 = 30,
    HSA_BRIG_WIDTH_1073741824 = 31,
    HSA_BRIG_WIDTH_2147483648 = 32,
    HSA_BRIG_WIDTH_WAVESIZE = 33,
    HSA_BRIG_WIDTH_ALL = 34
} hsa_brig_width_t;

typedef struct hsa_brig_data_s {
    uint32_t byte_count;
    uint8_t bytes[1];
} hsa_brig_data_t;

typedef struct hsa_brig_directive_arg_block_s {
    hsa_brig_base_t base;
} hsa_brig_directive_arg_block_t;

typedef struct hsa_brig_directive_comment_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
} hsa_brig_directive_comment_t;

typedef struct hsa_brig_directive_control_s {
    hsa_brig_base_t base;
    hsa_brig_control_directive16_t control;
    uint16_t reserved;
    hsa_brig_data_offset_operand_list32_t operands;
} hsa_brig_directive_control_t;

typedef struct hsa_brig_directive_executable_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
    uint16_t out_arg_count;
    uint16_t in_arg_count;
    hsa_brig_code_offset32_t first_in_arg;
    hsa_brig_code_offset32_t first_code_block_entry;
    hsa_brig_code_offset32_t next_module_entry;
    hsa_brig_executable_modifier8_t modifier;
    hsa_brig_linkage8_t linkage;
    uint16_t reserved;
} hsa_brig_directive_executable_t;

typedef struct hsa_brig_directive_extension_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
} hsa_brig_directive_extension_t;

typedef struct hsa_brig_directive_extension_version_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
    hsa_brig_version32_t extension_major;
    hsa_brig_version32_t extension_minor;
} hsa_brig_directive_extension_version_t;

typedef struct hsa_brig_directive_fbarrier_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
    hsa_brig_variable_modifier8_t modifier;
    hsa_brig_linkage8_t linkage;
    uint16_t reserved;
} hsa_brig_directive_fbarrier_t;

typedef struct hsa_brig_directive_label_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
} hsa_brig_directive_label_t;

typedef struct hsa_brig_directive_loc_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t filename;
    uint32_t line;
    uint32_t column;
} hsa_brig_directive_loc_t;

typedef struct hsa_brig_directive_module_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
    hsa_brig_version32_t hsail_major;
    hsa_brig_version32_t hsail_minor;
    hsa_brig_profile8_t profile;
    hsa_brig_machine_model8_t machine_model;
    hsa_brig_round8_t default_float_round;
    uint8_t reserved;
} hsa_brig_directive_module_t;

typedef struct hsa_brig_directive_none_s {
    hsa_brig_base_t base;
} hsa_brig_directive_none_t;

typedef struct hsa_brig_directive_pragma_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_operand_list32_t operands;
} hsa_brig_directive_pragma_t;

typedef struct hsa_brig_directive_variable_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t name;
    hsa_brig_operand_offset32_t init;
    hsa_brig_type16_t type;
    hsa_brig_segment8_t segment;
    hsa_brig_alignment8_t align;
    hsa_brig_uint64_t dim;
    hsa_brig_variable_modifier8_t modifier;
    hsa_brig_linkage8_t linkage;
    hsa_brig_allocation8_t allocation;
    uint8_t reserved;
} hsa_brig_directive_variable_t;

typedef struct hsa_brig_inst_base_s {
    hsa_brig_base_t base;
    hsa_brig_opcode16_t opcode;
    hsa_brig_type16_t type;
    hsa_brig_data_offset_operand_list32_t operands;
} hsa_brig_inst_base_t;

typedef struct hsa_brig_inst_addr_s {
    hsa_brig_inst_base_t base;
    hsa_brig_segment8_t segment;
    uint8_t reserved[3];
} hsa_brig_inst_addr_t;

typedef struct hsa_brig_inst_atomic_s {
    hsa_brig_inst_base_t base;
    hsa_brig_segment8_t segment;
    hsa_brig_memory_order8_t memory_order;
    hsa_brig_memory_scope8_t memory_scope;
    hsa_brig_atomic_operation8_t atomic_operation;
    uint8_t equiv_class;
    uint8_t reserved[3];
} hsa_brig_inst_atomic_t;

typedef struct hsa_brig_inst_basic_s {
    hsa_brig_inst_base_t base;
} hsa_brig_inst_basic_t;

typedef struct hsa_brig_inst_br_s {
    hsa_brig_inst_base_t base;
    hsa_brig_width8_t width;
    uint8_t reserved[3];
} hsa_brig_inst_br_t;

typedef struct hsa_brig_inst_cmp_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t source_type;
    hsa_brig_alu_modifier8_t modifier;
    hsa_brig_compare_operation8_t compare;
    hsa_brig_pack8_t pack;
    uint8_t reserved[3];
} hsa_brig_inst_cmp_t;

typedef struct hsa_brig_inst_cvt_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t source_type;
    hsa_brig_alu_modifier8_t modifier;
    hsa_brig_round8_t round;
} hsa_brig_inst_cvt_t;

typedef struct hsa_brig_inst_image_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t image_type;
    hsa_brig_type16_t coord_type;
    hsa_brig_image_geometry8_t geometry;
    uint8_t equiv_class;
    uint16_t reserved;
} hsa_brig_inst_image_t;

typedef struct hsa_brig_inst_lane_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t source_type;
    hsa_brig_width8_t width;
    uint8_t reserved;
} hsa_brig_inst_lane_t;

typedef struct hsa_brig_inst_mem_s {
    hsa_brig_inst_base_t base;
    hsa_brig_segment8_t segment;
    hsa_brig_alignment8_t align;
    uint8_t equiv_class;
    hsa_brig_width8_t width;
    hsa_brig_memory_modifier8_t modifier;
    uint8_t reserved[3];
} hsa_brig_inst_mem_t;

typedef struct hsa_brig_inst_mem_fence_s {
    hsa_brig_inst_base_t base;
    hsa_brig_memory_order8_t memory_order;
    hsa_brig_memory_scope8_t global_segment_memory_scope;
    hsa_brig_memory_scope8_t group_segment_memory_scope;
    hsa_brig_memory_scope8_t image_segment_memory_scope;
} hsa_brig_inst_mem_fence_t;

typedef struct hsa_brig_inst_mod_s {
    hsa_brig_inst_base_t base;
    hsa_brig_alu_modifier8_t modifier;
    hsa_brig_round8_t round;
    hsa_brig_pack8_t pack;
    uint8_t reserved;
} hsa_brig_inst_mod_t;

typedef struct hsa_brig_inst_query_image_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t image_type;
    hsa_brig_image_geometry8_t geometry;
    hsa_brig_image_query8_t query;
} hsa_brig_inst_query_image_t;

typedef struct hsa_brig_inst_query_sampler_s {
    hsa_brig_inst_base_t base;
    hsa_brig_sampler_query8_t query;
    uint8_t reserved[3];
} hsa_brig_inst_query_sampler_t;

typedef struct hsa_brig_inst_queue_s {
    hsa_brig_inst_base_t base;
    hsa_brig_segment8_t segment;
    hsa_brig_memory_order8_t memory_order;
    uint16_t reserved;
} hsa_brig_inst_queue_t;

typedef struct hsa_brig_inst_seg_s {
    hsa_brig_inst_base_t base;
    hsa_brig_segment8_t segment;
    uint8_t reserved[3];
} hsa_brig_inst_seg_t;

typedef struct hsa_brig_inst_seg_cvt_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t source_type;
    hsa_brig_segment8_t segment;
    hsa_brig_seg_cvt_modifier8_t modifier;
} hsa_brig_inst_seg_cvt_t;

typedef struct hsa_brig_inst_signal_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t signal_type;
    hsa_brig_memory_order8_t memory_order;
    hsa_brig_atomic_operation8_t signal_operation;
} hsa_brig_inst_signal_t;

typedef struct hsa_brig_inst_source_type_s {
    hsa_brig_inst_base_t base;
    hsa_brig_type16_t source_type;
    uint16_t reserved;
} hsa_brig_inst_source_type_t;

typedef struct hsa_brig_operand_address_s {
    hsa_brig_base_t base;
    hsa_brig_code_offset32_t symbol;
    hsa_brig_operand_offset32_t reg;
    hsa_brig_uint64_t offset;
} hsa_brig_operand_address_t;

typedef struct hsa_brig_operand_align_s {
    hsa_brig_base_t base;
    hsa_brig_alignment8_t align;
    uint8_t reserved[3];
} hsa_brig_operand_align_t;

typedef struct hsa_brig_operand_code_list_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_code_list32_t elements;
} hsa_brig_operand_code_list_t;

typedef struct hsa_brig_operand_code_ref_s {
    hsa_brig_base_t base;
    hsa_brig_code_offset32_t ref;
} hsa_brig_operand_code_ref_t;

typedef struct hsa_brig_operand_constant_bytes_s {
    hsa_brig_base_t base;
    hsa_brig_type16_t type;
    uint16_t reserved;
    hsa_brig_data_offset_string32_t bytes;
} hsa_brig_operand_constant_bytes_t;

typedef struct hsa_brig_operand_constant_expression_s {
    hsa_brig_base_t base;
    hsa_brig_type16_t type;
    hsa_brig_expression_operation16_t expression_operation;
    hsa_brig_data_offset_operand_list32_t operands;
} hsa_brig_operand_constant_expression_t;

typedef struct hsa_brig_operand_constant_image_s {
    hsa_brig_base_t base;
    hsa_brig_type16_t type;
    hsa_brig_image_geometry8_t geometry;
    hsa_brig_image_channel_order8_t channel_order;
    hsa_brig_image_channel_type8_t channel_type;
    uint8_t reserved[3];
    hsa_brig_uint64_t width;
    hsa_brig_uint64_t height;
    hsa_brig_uint64_t depth;
    hsa_brig_uint64_t array;
} hsa_brig_operand_constant_image_t;

typedef struct hsa_brig_operand_constant_operand_list_s {
    hsa_brig_base_t base;
    hsa_brig_type16_t type;
    uint16_t reserved;
    hsa_brig_data_offset_operand_list32_t elements;
} hsa_brig_operand_constant_operand_list_t;

typedef struct hsa_brig_operand_constant_sampler_s {
    hsa_brig_base_t base;
    hsa_brig_type16_t type;
    hsa_brig_sampler_coord_normalization8_t coord;
    hsa_brig_sampler_filter8_t filter;
    hsa_brig_sampler_addressing8_t addressing;
    uint8_t reserved[3];
} hsa_brig_operand_constant_sampler_t;

typedef struct hsa_brig_operand_operand_list_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_operand_list32_t elements;
} hsa_brig_operand_operand_list_t;

typedef struct hsa_brig_operand_register_s {
    hsa_brig_base_t base;
    hsa_brig_register_kind16_t reg_kind;
    uint16_t reg_num;
} hsa_brig_operand_register_t;

typedef struct hsa_brig_operand_string_s {
    hsa_brig_base_t base;
    hsa_brig_data_offset_string32_t string;
} hsa_brig_operand_string_t;

typedef struct hsa_brig_operand_wavesize_s {
    hsa_brig_base_t base;
} hsa_brig_operand_wavesize_t;

typedef struct hsa_brig_operand_zero_s {
    hsa_brig_base_t base;
    hsa_brig_uint64_t byte_count;
} hsa_brig_operand_zero_t;

#ifdef __cplusplus
}
#endif  /*__cplusplus*/

#endif /* HSA_BRIG_H */
