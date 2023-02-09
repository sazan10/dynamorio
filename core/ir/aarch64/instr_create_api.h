/* **********************************************************
 * Copyright (c) 2011-2022 Google, Inc. All rights reserved.
 * Copyright (c) 2016-2018 ARM Limited. All rights reserved.
 * Copyright (c) 2002-2010 VMware, Inc. All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of ARM Limited nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL ARM LIMITED OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef DR_IR_MACROS_AARCH64_H
#define DR_IR_MACROS_AARCH64_H 1

/**
 * Used in an additional immediate source operand to a vector operation, denotes
 * 8 bit vector element width. See \ref sec_IR_AArch64.
 */
#define VECTOR_ELEM_WIDTH_BYTE 0

/**
 * Used in an additional immediate source operand to a vector operation, denotes
 * 16 bit vector element width. See \ref sec_IR_AArch64.
 */
#define VECTOR_ELEM_WIDTH_HALF 1

/**
 * Used in an additional immediate source operand to a vector operation, denotes
 * 32 bit vector element width. See \ref sec_IR_AArch64.
 */
#define VECTOR_ELEM_WIDTH_SINGLE 2

/**
 * Used in an additional immediate source operand to a vector operation, denotes
 * 64 bit vector element width. See \ref sec_IR_AArch64.
 */
#define VECTOR_ELEM_WIDTH_DOUBLE 3

/**
 * Used in an additional immediate source operand to a vector operation, denotes
 * full size 128 bit vector width. See \ref sec_IR_AArch64.
 */
#define VECTOR_ELEM_WIDTH_QUAD 4

/**
 * Operand denoting 8 bit vector element width for the other operands of
 * the containing instruction.
 */
#define OPND_CREATE_BYTE() OPND_CREATE_INT8(VECTOR_ELEM_WIDTH_BYTE)

/**
 * Operand denoting 16 bit vector element width for the other operands of
 * the containing instruction.
 */
#define OPND_CREATE_HALF() OPND_CREATE_INT8(VECTOR_ELEM_WIDTH_HALF)

/**
 * Operand denoting 32 bit vector element width for the other operands of
 * the containing instruction.
 */
#define OPND_CREATE_SINGLE() OPND_CREATE_INT8(VECTOR_ELEM_WIDTH_SINGLE)

/**
 * Operand denoting 64 bit vector element width for the other operands of
 * the containing instruction.
 */
#define OPND_CREATE_DOUBLE() OPND_CREATE_INT8(VECTOR_ELEM_WIDTH_DOUBLE)

/**
 * @file dr_ir_macros_aarch64.h
 * @brief AArch64-specific instruction creation convenience macros.
 */

/**
 * Create an absolute address operand encoded as pc-relative.
 * Encoding will fail if addr is out of the maximum signed displacement
 * reach for the architecture.
 */
#define OPND_CREATE_ABSMEM(addr, size) opnd_create_rel_addr(addr, size)

/**
 * Create an immediate integer operand. For AArch64 the size of an immediate
 * is ignored when encoding, so there is no need to specify the final size.
 */
#define OPND_CREATE_INT(val) OPND_CREATE_INTPTR(val)

/** Create a zero register operand of the same size as reg. */
#define OPND_CREATE_ZR(reg) \
    opnd_create_reg(opnd_get_size(reg) == OPSZ_4 ? DR_REG_WZR : DR_REG_XZR)

/** Create an operand specifying LSL, the default shift type when there is no shift. */
#define OPND_CREATE_LSL() opnd_add_flags(OPND_CREATE_INT(DR_SHIFT_LSL), DR_OPND_IS_SHIFT)

/** Create an operand specifying MUL, a multiplier operand. */
#define OPND_CREATE_MUL() opnd_add_flags(OPND_CREATE_INT(DR_SHIFT_MUL), DR_OPND_IS_SHIFT)

/****************************************************************************
 * Platform-independent INSTR_CREATE_* macros
 */
/** @name Platform-independent macros */
/** @{ */ /* doxygen start group */

/**
 * This platform-independent macro creates an instr_t for a debug trap
 * instruction, automatically supplying any implicit operands.
 * \param dc The void * dcontext used to allocate memory for the instr_t.
 */
#define XINST_CREATE_debug_instr(dc) INSTR_CREATE_brk((dc), OPND_CREATE_INT16(0))

/**
 * This platform-independent macro creates an instr_t for a 4-byte
 * or 8-byte memory load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load(dc, r, m)                                                    \
    ((opnd_is_base_disp(m) &&                                                          \
      (opnd_get_disp(m) < 0 ||                                                         \
       opnd_get_disp(m) % opnd_size_in_bytes(opnd_get_size(m)) != 0))                  \
         ? INSTR_CREATE_ldur(                                                          \
               dc,                                                                     \
               opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), opnd_get_size(m))), \
               m)                                                                      \
         : INSTR_CREATE_ldr(                                                           \
               dc,                                                                     \
               opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), opnd_get_size(m))), \
               m))

/**
 * This platform-independent macro creates an instr_t which loads 1 byte
 * from memory, zero-extends it to 4 bytes, and writes it to a 4 byte
 * destination register.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load_1byte_zext4(dc, r, m) INSTR_CREATE_ldrb(dc, r, m)

/**
 * This platform-independent macro creates an instr_t for a 1-byte
 * memory load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load_1byte(dc, r, m) INSTR_CREATE_ldrb(dc, r, m)

/**
 * This platform-independent macro creates an instr_t for a 2-byte
 * memory load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load_2bytes(dc, r, m) INSTR_CREATE_ldrh(dc, r, m)

/**
 * This platform-independent macro creates an instr_t for a 4-byte
 * or 8-byte memory store instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param m   The destination memory opnd.
 * \param r   The source register opnd.
 */
#define XINST_CREATE_store(dc, m, r)                                                   \
    ((opnd_is_base_disp(m) &&                                                          \
      (opnd_get_disp(m) < 0 ||                                                         \
       opnd_get_disp(m) % opnd_size_in_bytes(opnd_get_size(m)) != 0))                  \
         ? INSTR_CREATE_stur(                                                          \
               dc, m,                                                                  \
               opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), opnd_get_size(m)))) \
         : INSTR_CREATE_str(                                                           \
               dc, m,                                                                  \
               opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), opnd_get_size(m)))))

/**
 * This platform-independent macro creates an instr_t for a 1-byte
 * memory store instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param m   The destination memory opnd.
 * \param r   The source register opnd.
 */
#define XINST_CREATE_store_1byte(dc, m, r) \
    INSTR_CREATE_strb(dc, m, opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), OPSZ_4)))

/**
 * This platform-independent macro creates an instr_t for a 2-byte
 * memory store instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param m   The destination memory opnd.
 * \param r   The source register opnd.
 */
#define XINST_CREATE_store_2bytes(dc, m, r) \
    INSTR_CREATE_strh(dc, m, opnd_create_reg(reg_resize_to_opsz(opnd_get_reg(r), OPSZ_4)))

/**
 * This AArchXX-platform-independent macro creates an instr_t for a 2-register
 * memory store instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param m   The destination memory opnd.
 * \param r1  The first register opnd.
 * \param r2  The second register opnd.
 */
#define XINST_CREATE_store_pair(dc, m, r1, r2) INSTR_CREATE_stp(dc, m, r1, r2)

/**
 * This AArchXX-platform-independent macro creates an instr_t for a 2-register
 * memory load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r1  The first register opnd.
 * \param r2  The second register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load_pair(dc, r1, r2, m) INSTR_CREATE_ldp(dc, r1, r2, m)

/**
 * This platform-independent macro creates an instr_t for a register
 * to register move instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d   The destination register opnd.
 * \param s   The source register opnd.
 */
#define XINST_CREATE_move(dc, d, s)                                      \
    ((opnd_get_reg(d) == DR_REG_XSP || opnd_get_reg(s) == DR_REG_XSP ||  \
      opnd_get_reg(d) == DR_REG_WSP || opnd_get_reg(s) == DR_REG_WSP)    \
         ? instr_create_1dst_4src(dc, OP_add, d, s, OPND_CREATE_INT(0),  \
                                  OPND_CREATE_LSL(), OPND_CREATE_INT(0)) \
         : instr_create_1dst_4src(dc, OP_orr, d, OPND_CREATE_ZR(d), s,   \
                                  OPND_CREATE_LSL(), OPND_CREATE_INT(0)))

/**
 * This platform-independent macro creates an instr_t for a multimedia
 * register load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param m   The source memory opnd.
 */
#define XINST_CREATE_load_simd(dc, r, m) INSTR_CREATE_ldr((dc), (r), (m))

/**
 * This platform-independent macro creates an instr_t for a multimedia
 * register store instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param m   The destination memory opnd.
 * \param r   The source register opnd.
 */
#define XINST_CREATE_store_simd(dc, m, r) INSTR_CREATE_str((dc), (m), (r))

/**
 * This platform-independent macro creates an instr_t for an indirect
 * jump instruction through a register.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The register opnd holding the target.
 */
#define XINST_CREATE_jump_reg(dc, r) INSTR_CREATE_br((dc), (r))

/**
 * This platform-independent macro creates an instr_t for an immediate
 * integer load instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The destination register opnd.
 * \param i   The source immediate integer opnd.
 */
#define XINST_CREATE_load_int(dc, r, i)                                            \
    (opnd_get_immed_int(i) < 0                                                     \
         ? INSTR_CREATE_movn((dc), (r), OPND_CREATE_INT32(~opnd_get_immed_int(i)), \
                             OPND_CREATE_INT(0))                                   \
         : INSTR_CREATE_movz((dc), (r), (i), OPND_CREATE_INT(0)))

/**
 * This platform-independent macro creates an instr_t for a return instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 */
#define XINST_CREATE_return(dc) INSTR_CREATE_ret(dc, opnd_create_reg(DR_REG_X30))

/**
 * This platform-independent macro creates an instr_t for an unconditional
 * branch instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param t   The opnd_t target operand for the instruction, which can be
 * either a pc (opnd_create_pc)()) or an instr_t (opnd_create_instr()).
 * Be sure to ensure that the limited reach of this short branch will reach
 * the target (a pc operand is not suitable for most uses unless you know
 * precisely where this instruction will be encoded).
 */
#define XINST_CREATE_jump(dc, t) INSTR_CREATE_b((dc), (t))

/**
 * This platform-independent macro creates an instr_t for an unconditional
 * branch instruction with the smallest available reach.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param t   The opnd_t target operand for the instruction, which can be
 * either a pc (opnd_create_pc)()) or an instr_t (opnd_create_instr()).
 * Be sure to ensure that the limited reach of this short branch will reach
 * the target (a pc operand is not suitable for most uses unless you know
 * precisely where this instruction will be encoded).
 */
#define XINST_CREATE_jump_short(dc, t) INSTR_CREATE_b((dc), (t))

/**
 * This platform-independent macro creates an instr_t for an unconditional
 * branch instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param t   The opnd_t target operand for the instruction, which can be
 * either a pc (opnd_create_pc)()) or an instr_t (opnd_create_instr()).
 * Be sure to ensure that the limited reach of this short branch will reach
 * the target (a pc operand is not suitable for most uses unless you know
 * precisely where this instruction will be encoded).
 */
#define XINST_CREATE_call(dc, t) INSTR_CREATE_bl(dc, t)

/**
 * This platform-independent macro creates an instr_t for a conditional
 * branch instruction that branches if the previously-set condition codes
 * indicate the condition indicated by \p pred.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param pred  The #dr_pred_type_t condition to match.
 * \param t   The opnd_t target operand for the instruction, which can be
 * either a pc (opnd_create_pc)()) or an instr_t (opnd_create_instr()).
 * Be sure to ensure that the limited reach of this short branch will reach
 * the target (a pc operand is not suitable for most uses unless you know
 * precisely where this instruction will be encoded).
 */
#define XINST_CREATE_jump_cond(dc, pred, t) \
    (INSTR_PRED(INSTR_CREATE_bcond((dc), (t)), (pred)))

/**
 * This platform-independent macro creates an instr_t for an addition
 * instruction that does not affect the status flags.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_add(dc, d, s) INSTR_CREATE_add(dc, d, d, s)

/**
 * This platform-independent macro creates an instr_t for an addition
 * instruction that does not affect the status flags and takes two sources
 * plus a destination.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s1  The opnd_t explicit first source operand for the instruction. This
 * must be a register.
 * \param s2  The opnd_t explicit source operand for the instruction. This
 * can be either a register or an immediate integer.
 */
#define XINST_CREATE_add_2src(dc, d, s1, s2) INSTR_CREATE_add(dc, d, s1, s2)

/**
 * This platform-independent macro creates an instr_t for an addition
 * instruction that does not affect the status flags and takes two register sources
 * plus a destination, with one source being shifted logically left by
 * an immediate amount that is limited to either 0, 1, 2, or 3.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s1  The opnd_t explicit first source operand for the instruction.  This
 * must be a register.
 * \param s2_toshift  The opnd_t explicit source operand for the instruction.  This
 * must be a register.
 * \param shift_amount  An integer value that must be either 0, 1, 2, or 3.
 */
#define XINST_CREATE_add_sll(dc, d, s1, s2_toshift, shift_amount)            \
    INSTR_CREATE_add_shift((dc), (d), (s1), (s2_toshift), OPND_CREATE_LSL(), \
                           OPND_CREATE_INT8(shift_amount))

/**
 * This platform-independent macro creates an instr_t for an addition
 * instruction that does affect the status flags.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_add_s(dc, d, s) INSTR_CREATE_adds(dc, d, d, s)

/**
 * This platform-independent macro creates an instr_t for a subtraction
 * instruction that does not affect the status flags.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_sub(dc, d, s) INSTR_CREATE_sub(dc, d, d, s)

/**
 * This platform-independent macro creates an instr_t for a subtraction
 * instruction that does affect the status flags.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_sub_s(dc, d, s) INSTR_CREATE_subs(dc, d, d, s)

/**
 * This platform-independent macro creates an instr_t for a bitwise and
 * instruction that does affect the status flags.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param d  The opnd_t explicit destination operand for the instruction.
 * \param s  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_and_s(dc, d, s) INSTR_CREATE_ands(dc, d, d, s)

/**
 * This platform-independent macro creates an instr_t for a comparison
 * instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param s1  The opnd_t explicit source operand for the instruction.
 * \param s2  The opnd_t explicit source operand for the instruction.
 */
#define XINST_CREATE_cmp(dc, s1, s2) INSTR_CREATE_cmp(dc, s1, s2)

/**
 * This platform-independent macro creates an instr_t for a software
 * interrupt instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param i   The source integer constant opnd_t operand.
 */
#define XINST_CREATE_interrupt(dc, i) INSTR_CREATE_svc(dc, (i))

/**
 * This platform-independent macro creates an instr_t for a logical right shift
 * instruction that does affect the status flags.
 * \param dc         The void * dcontext used to allocate memory for the instr_t.
 * \param d          The opnd_t explicit destination operand for the instruction.
 * \param rm_or_imm  The opnd_t explicit source operand for the instruction.
 */
/* FIXME i#2440: I'm not sure this is correct.  Use INSTR_CREATE_lsr once available!
 * Also, what about writing the flags?  Most users don't want to read the flag results,
 * they just need to know whether they need to preserve the app's flags, so maybe
 * we can just document that this may not write them.
 */
#define XINST_CREATE_slr_s(dc, d, rm_or_imm)                                          \
    (opnd_is_reg(rm_or_imm)                                                           \
         ? instr_create_1dst_2src(dc, OP_lsrv, d, d, rm_or_imm)                       \
         : instr_create_1dst_3src(dc, OP_ubfm, d, d, rm_or_imm,                       \
                                  reg_is_32bit(opnd_get_reg(d)) ? OPND_CREATE_INT(31) \
                                                                : OPND_CREATE_INT(63)))

/**
 * This platform-independent macro creates an instr_t for a nop instruction.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 */
#define XINST_CREATE_nop(dc) INSTR_CREATE_nop(dc)

/**
 * This platform-independent macro creates an instr_t for an indirect call instr
 * through a register.
 * \param dc  The void * dcontext used to allocate memory for the instr_t.
 * \param r   The opnd_t explicit source operand for the instruction. This should
 * be a reg_id_t operand with the address of the subroutine.
 */
#define XINST_CREATE_call_reg(dc, r) INSTR_CREATE_blr(dc, r)

/** @} */ /* end doxygen group */

/****************************************************************************
 * Manually-added ARM-specific INSTR_CREATE_* macros
 * FIXME i#4106: Add Doxygen headers.
 * Newer doxygens give warnings causing build errors on these so we remove
 * from the docs until headers are added.
 */

/** \cond disabled_until_i4106_is_fixed */
#define INSTR_CREATE_add(dc, rd, rn, rm_or_imm)                                         \
    opnd_is_reg(rm_or_imm)                                                              \
        ? /* _extend supports sp in rn, so prefer it, but it does not support imm. */   \
        INSTR_CREATE_add_extend(dc, rd, rn, rm_or_imm, OPND_CREATE_INT(DR_EXTEND_UXTX), \
                                OPND_CREATE_INT(0))                                     \
        : INSTR_CREATE_add_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(),              \
                                 OPND_CREATE_INT(0))
#define INSTR_CREATE_add_extend(dc, rd, rn, rm, ext, exa)                             \
    instr_create_1dst_4src(dc, OP_add, rd, rn,                                        \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_EXTENDED), \
                           opnd_add_flags(ext, DR_OPND_IS_EXTEND), exa)
#define INSTR_CREATE_add_shift(dc, rd, rn, rm_or_imm, sht, sha)                \
    opnd_is_reg(rm_or_imm)                                                     \
        ? instr_create_1dst_4src(                                              \
              (dc), OP_add, (rd), (rn),                                        \
              opnd_create_reg_ex(opnd_get_reg(rm_or_imm), 0, DR_OPND_SHIFTED), \
              opnd_add_flags((sht), DR_OPND_IS_SHIFT), (sha))                  \
        : instr_create_1dst_4src((dc), OP_add, (rd), (rn), (rm_or_imm), (sht), (sha))
#define INSTR_CREATE_adds(dc, rd, rn, rm_or_imm)                             \
    (opnd_is_reg(rm_or_imm)                                                  \
         ? INSTR_CREATE_adds_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(), \
                                   OPND_CREATE_INT(0))                       \
         : INSTR_CREATE_adds_imm(dc, rd, rn, rm_or_imm, OPND_CREATE_INT(0)))
/** \endcond disabled_until_i4106_is_fixed */

/**
 * Creates an AND instruction with one output and two inputs.
 * \param dc   The void * dcontext used to allocate memory for the instr_t.
 * \param rd   The output register.
 * \param rn   The first input register.
 * \param rm_or_imm   The second input register or immediate.
 */
#define INSTR_CREATE_and(dc, rd, rn, rm_or_imm)                             \
    (opnd_is_immed(rm_or_imm)                                               \
         ? instr_create_1dst_2src((dc), OP_and, (rd), (rn), (rm_or_imm))    \
         : INSTR_CREATE_and_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(), \
                                  OPND_CREATE_INT(0)))
/** \cond disabled_until_i4106_is_fixed */
#define INSTR_CREATE_and_shift(dc, rd, rn, rm, sht, sha)                             \
    instr_create_1dst_4src((dc), OP_and, (rd), (rn),                                 \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_SHIFTED), \
                           opnd_add_flags((sht), DR_OPND_IS_SHIFT), (sha))
/** \endcond disabled_until_i4106_is_fixed */

/**
 * Creates an ANDS instruction with one output and two inputs.
 * \param dc   The void * dcontext used to allocate memory for the instr_t.
 * \param rd   The output register.
 * \param rn   The first input register.
 * \param rm_or_imm   The second input register or immediate.
 */
#define INSTR_CREATE_ands(dc, rd, rn, rm_or_imm)                             \
    (opnd_is_immed(rm_or_imm)                                                \
         ? instr_create_1dst_2src((dc), OP_ands, (rd), (rn), (rm_or_imm))    \
         : INSTR_CREATE_ands_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(), \
                                   OPND_CREATE_INT(0)))
/** \cond disabled_until_i4106_is_fixed */
#define INSTR_CREATE_ands_shift(dc, rd, rn, rm, sht, sha)                            \
    instr_create_1dst_4src((dc), OP_ands, (rd), (rn),                                \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_SHIFTED), \
                           opnd_add_flags((sht), DR_OPND_IS_SHIFT), (sha))
#define INSTR_CREATE_b(dc, pc) instr_create_0dst_1src((dc), OP_b, (pc))
/** \endcond disabled_until_i4106_is_fixed */
/**
 * This macro creates an instr_t for a conditional branch instruction. The condition
 * can be set using INSTR_PRED macro.
 * \param dc The void * dcontext used to allocate memory for the instr_t.
 * \param pc The opnd_t target operand containing the program counter to jump to.
 */
#define INSTR_CREATE_bcond(dc, pc) instr_create_0dst_1src((dc), OP_bcond, (pc))
/**
 * This macro creates an instr_t for a BL (branch and link) instruction.
 * \param dc The void * dcontext used to allocate memory for the instr_t.
 * \param pc The opnd_t target operand containing the program counter to jump to.
 */
#define INSTR_CREATE_bl(dc, pc) \
    instr_create_1dst_1src((dc), OP_bl, opnd_create_reg(DR_REG_X30), (pc))

/**
 * Creates a CCMP (Conditional Compare) instruction. Sets the NZCV flags to the
 * result of a comparison of its two source values if the named input condition
 * is true, or to an immediate value if the input condition is false.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param cond    The comparison condition specified by #dr_pred_type_t, e.g. #DR_PRED_EQ.
 * \param Rn      The GPR source register.
 * \param Op      Either a 5-bit immediate (use #opnd_create_immed_uint() to create
   the operand, e.g. opnd_create_immed_uint(val, #OPSZ_5b)) or a GPR source register.
 * \param nzcv    The 4 bit NZCV flags value used if the input condition is false.
 * (use #opnd_create_immed_uint() to create the operand, e.g.
 * opnd_create_immed_uint(val, #OPSZ_4b)).
 */
#define INSTR_CREATE_ccmp(dc, Rn, Op, nzcv, cond) \
    (INSTR_PRED(instr_create_0dst_3src(dc, OP_ccmp, Rn, Op, nzcv), (cond)))

/**
 * Creates a CCMN (Conditional Compare Negative) instruction. Sets the NZCV
 * flags to the result of a comparison of its two source values if the named
 * input condition is true, or to an immediate value if the input condition is
 * false. The comparison is based on a negated second source value (Op) if an
 * immediate, inverted if a register.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param cond    The comparison condition specified by #dr_pred_type_t, e.g. #DR_PRED_EQ.
 * \param Rn      The GPR source register.
 * \param Op      Either a 5-bit immediate (use #opnd_create_immed_uint() to create the
 * operand, e.g. opnd_create_immed_uint(val, #OPSZ_5b)) or a GPR source register.
 * \param nzcv    The 4 bit NZCV flags value used if the input condition is false.
 * (use #opnd_create_immed_uint() to create the operand, e.g.
 * opnd_create_immed_uint(val, #OPSZ_4b)).
 */
#define INSTR_CREATE_ccmn(dc, Rn, Op, nzcv, cond) \
    (INSTR_PRED(instr_create_0dst_3src(dc, OP_ccmn, Rn, Op, nzcv), (cond)))

/** \cond disabled_until_i4106_is_fixed */
#define INSTR_CREATE_adc(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src((dc), OP_adc, (Rd), (Rn), (Rm))
#define INSTR_CREATE_adcs(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src((dc), OP_adcs, (Rd), (Rn), (Rm))
#define INSTR_CREATE_adds_extend(dc, Rd, Rn, Rm, shift, imm3)                         \
    instr_create_1dst_4src((dc), OP_adds, (Rd), (Rn),                                 \
                           opnd_create_reg_ex(opnd_get_reg(Rm), 0, DR_OPND_EXTENDED), \
                           opnd_add_flags((shift), DR_OPND_IS_EXTEND), (imm3))
#define INSTR_CREATE_adds_imm(dc, Rd, Rn, imm12, shift_amt)                       \
    instr_create_1dst_4src((dc), OP_adds, (Rd), (Rn), (imm12), OPND_CREATE_LSL(), \
                           (shift_amt))
#define INSTR_CREATE_adds_shift(dc, Rd, Rn, Rm, shift, imm6)                         \
    instr_create_1dst_4src((dc), OP_adds, (Rd), (Rn),                                \
                           opnd_create_reg_ex(opnd_get_reg(Rm), 0, DR_OPND_SHIFTED), \
                           opnd_add_flags((shift), DR_OPND_IS_SHIFT), (imm6))
#define INSTR_CREATE_br(dc, xn) instr_create_0dst_1src((dc), OP_br, (xn))
#define INSTR_CREATE_blr(dc, xn) \
    instr_create_1dst_1src((dc), OP_blr, opnd_create_reg(DR_REG_X30), (xn))
#define INSTR_CREATE_brk(dc, imm) instr_create_0dst_1src((dc), OP_brk, (imm))
#define INSTR_CREATE_cbnz(dc, pc, reg) instr_create_0dst_2src((dc), OP_cbnz, (pc), (reg))
#define INSTR_CREATE_cbz(dc, pc, reg) instr_create_0dst_2src((dc), OP_cbz, (pc), (reg))
#define INSTR_CREATE_tbz(dc, pc, reg, imm) \
    instr_create_0dst_3src((dc), OP_tbz, (pc), (reg), (imm))
#define INSTR_CREATE_tbnz(dc, pc, reg, imm) \
    instr_create_0dst_3src((dc), OP_tbnz, (pc), (reg), (imm))
#define INSTR_CREATE_cmp(dc, rn, rm_or_imm) \
    INSTR_CREATE_subs(dc, OPND_CREATE_ZR(rn), rn, rm_or_imm)
#define INSTR_CREATE_eor(dc, d, s)                                      \
    INSTR_CREATE_eor_shift(dc, d, d, s, OPND_CREATE_INT8(DR_SHIFT_LSL), \
                           OPND_CREATE_INT8(0))
#define INSTR_CREATE_eor_shift(dc, rd, rn, rm, sht, sha)                             \
    instr_create_1dst_4src(dc, OP_eor, rd, rn,                                       \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_SHIFTED), \
                           opnd_add_flags(sht, DR_OPND_IS_SHIFT), sha)

#define INSTR_CREATE_ldp(dc, rt1, rt2, mem) \
    instr_create_2dst_1src(dc, OP_ldp, rt1, rt2, mem)
#define INSTR_CREATE_ldr(dc, Rd, mem) instr_create_1dst_1src((dc), OP_ldr, (Rd), (mem))
#define INSTR_CREATE_ldrb(dc, Rd, mem) instr_create_1dst_1src(dc, OP_ldrb, Rd, mem)
#define INSTR_CREATE_ldrsb(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldrsb, (Rd), (mem))
#define INSTR_CREATE_ldrh(dc, Rd, mem) instr_create_1dst_1src(dc, OP_ldrh, Rd, mem)
#define INSTR_CREATE_ldur(dc, rt, mem) instr_create_1dst_1src(dc, OP_ldur, rt, mem)
#define INSTR_CREATE_ldar(dc, Rt, mem) instr_create_1dst_1src((dc), OP_ldar, (Rt), (mem))
#define INSTR_CREATE_ldarb(dc, Rt, mem) \
    instr_create_1dst_1src((dc), OP_ldarb, (Rt), (mem))
#define INSTR_CREATE_ldarh(dc, Rt, mem) \
    instr_create_1dst_1src((dc), OP_ldarh, (Rt), (mem))
#define INSTR_CREATE_ldxr(dc, Rd, mem) instr_create_1dst_1src((dc), OP_ldxr, (Rd), (mem))
#define INSTR_CREATE_ldxrb(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldxrb, (Rd), (mem))
#define INSTR_CREATE_ldxrh(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldxrh, (Rd), (mem))
#define INSTR_CREATE_ldxp(dc, rt1, rt2, mem) \
    instr_create_2dst_1src((dc), OP_ldxp, rt1, rt2, (mem))
#define INSTR_CREATE_ldaxr(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldaxr, (Rd), (mem))
#define INSTR_CREATE_ldaxrb(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldaxrb, (Rd), (mem))
#define INSTR_CREATE_ldaxrh(dc, Rd, mem) \
    instr_create_1dst_1src((dc), OP_ldaxrh, (Rd), (mem))
#define INSTR_CREATE_ldaxp(dc, rt1, rt2, mem) \
    instr_create_2dst_1src((dc), OP_ldaxp, rt1, rt2, (mem))
#define INSTR_CREATE_movk(dc, rt, imm16, lsl) \
    instr_create_1dst_4src(dc, OP_movk, rt, rt, imm16, OPND_CREATE_LSL(), lsl)
#define INSTR_CREATE_movn(dc, rt, imm16, lsl) \
    instr_create_1dst_3src(dc, OP_movn, rt, imm16, OPND_CREATE_LSL(), lsl)
#define INSTR_CREATE_movz(dc, rt, imm16, lsl) \
    instr_create_1dst_3src(dc, OP_movz, rt, imm16, OPND_CREATE_LSL(), lsl)
#define INSTR_CREATE_mrs(dc, Xt, sysreg) \
    instr_create_1dst_1src((dc), OP_mrs, (Xt), (sysreg))
#define INSTR_CREATE_msr(dc, sysreg, Xt) \
    instr_create_1dst_1src((dc), OP_msr, (sysreg), (Xt))
#define INSTR_CREATE_nop(dc) instr_create_0dst_0src((dc), OP_nop)
#define INSTR_CREATE_ret(dc, Rn) instr_create_0dst_1src((dc), OP_ret, (Rn))
#define INSTR_CREATE_stp(dc, mem, rt1, rt2) \
    instr_create_1dst_2src(dc, OP_stp, mem, rt1, rt2)
#define INSTR_CREATE_str(dc, mem, rt) instr_create_1dst_1src(dc, OP_str, mem, rt)
#define INSTR_CREATE_strb(dc, mem, rt) instr_create_1dst_1src(dc, OP_strb, mem, rt)
#define INSTR_CREATE_strh(dc, mem, rt) instr_create_1dst_1src(dc, OP_strh, mem, rt)
#define INSTR_CREATE_stur(dc, mem, rt) instr_create_1dst_1src(dc, OP_stur, mem, rt)
#define INSTR_CREATE_sturh(dc, mem, rt) instr_create_1dst_1src(dc, OP_sturh, mem, rt)
#define INSTR_CREATE_stlr(dc, mem, rt) instr_create_1dst_1src(dc, OP_stlr, mem, rt)
/* This incorrect name was exported in official releases, so to avoid breaking
 * potential existing uses we keep it as an alias.
 */
#define INST_CREATE_stlr INSTR_CREATE_stlr
#define INSTR_CREATE_stxr(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stxr, mem, rs, rt)
#define INSTR_CREATE_stxrb(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stxrb, mem, rs, rt)
#define INSTR_CREATE_stxrh(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stxrh, mem, rs, rt)
#define INSTR_CREATE_stxp(dc, mem, rs, rt1, rt2) \
    instr_create_2dst_2src(dc, OP_stxp, mem, rs, rt1, rt2)
#define INSTR_CREATE_stlxr(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stlxr, mem, rs, rt)
#define INSTR_CREATE_stlxrb(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stlxrb, mem, rs, rt)
#define INSTR_CREATE_stlxrh(dc, mem, rs, rt) \
    instr_create_2dst_1src(dc, OP_stlxrh, mem, rs, rt)
#define INSTR_CREATE_stlxp(dc, mem, rs, rt1, rt2) \
    instr_create_2dst_2src(dc, OP_stlxp, mem, rs, rt1, rt2)
#define INSTR_CREATE_sub(dc, rd, rn, rm_or_imm)                                         \
    opnd_is_reg(rm_or_imm)                                                              \
        ? /* _extend supports sp in rn, so prefer it, but it does not support imm. */   \
        INSTR_CREATE_sub_extend(dc, rd, rn, rm_or_imm, OPND_CREATE_INT(DR_EXTEND_UXTX), \
                                OPND_CREATE_INT(0))                                     \
        : INSTR_CREATE_sub_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(),              \
                                 OPND_CREATE_INT(0))
#define INSTR_CREATE_sub_extend(dc, rd, rn, rm, ext, exa)                             \
    instr_create_1dst_4src(dc, OP_sub, rd, rn,                                        \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_EXTENDED), \
                           opnd_add_flags(ext, DR_OPND_IS_EXTEND), exa)
#define INSTR_CREATE_sub_shift(dc, rd, rn, rm_or_imm, sht, sha)                \
    opnd_is_reg(rm_or_imm)                                                     \
        ? instr_create_1dst_4src(                                              \
              (dc), OP_sub, (rd), (rn),                                        \
              opnd_create_reg_ex(opnd_get_reg(rm_or_imm), 0, DR_OPND_SHIFTED), \
              opnd_add_flags((sht), DR_OPND_IS_SHIFT), (sha))                  \
        : instr_create_1dst_4src((dc), OP_sub, (rd), (rn), (rm_or_imm), (sht), (sha))
#define INSTR_CREATE_subs(dc, rd, rn, rm_or_imm) \
    INSTR_CREATE_subs_shift(dc, rd, rn, rm_or_imm, OPND_CREATE_LSL(), OPND_CREATE_INT(0))
#define INSTR_CREATE_subs_extend(dc, rd, rn, rm, ext, exa)                            \
    instr_create_1dst_4src(dc, OP_subs, rd, rn,                                       \
                           opnd_create_reg_ex(opnd_get_reg(rm), 0, DR_OPND_EXTENDED), \
                           opnd_add_flags(ext, DR_OPND_IS_EXTEND), exa)
#define INSTR_CREATE_subs_shift(dc, rd, rn, rm_or_imm, sht, sha)               \
    opnd_is_reg(rm_or_imm)                                                     \
        ? instr_create_1dst_4src(                                              \
              (dc), OP_subs, (rd), (rn),                                       \
              opnd_create_reg_ex(opnd_get_reg(rm_or_imm), 0, DR_OPND_SHIFTED), \
              opnd_add_flags((sht), DR_OPND_IS_SHIFT), (sha))                  \
        : instr_create_1dst_4src((dc), OP_subs, (rd), (rn), (rm_or_imm), (sht), (sha))
#define INSTR_CREATE_svc(dc, imm) instr_create_0dst_1src((dc), OP_svc, (imm))
#define INSTR_CREATE_adr(dc, rt, imm) instr_create_1dst_1src(dc, OP_adr, rt, imm)
#define INSTR_CREATE_adrp(dc, rt, imm) instr_create_1dst_1src(dc, OP_adrp, rt, imm)

#define INSTR_CREATE_sys(dc, op, Rn) instr_create_0dst_2src(dc, OP_sys, op, Rn)

/* TODO i#4400: Cache instructions which behave like memory stores (e.g. DC
 * ZVA) should implement memory operand which encapsulates back-aligned start
 * address as well as cache line size (read from system regiater).
 */

/**
 * Creates a DC CISW instruction to Clean and Invalidate data cache line by Set/Way.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the Set/Way value and cache level.
 *             See reference manual for a detailed explanation.
 */
#define INSTR_CREATE_dc_cisw(dc, Rn) instr_create_0dst_1src(dc, OP_dc_cisw, Rn)

/**
 * Creates a DC CIVAC instruction to Clean and Invalidate data cache by
 * Virtual Address to point of Coherency.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             No alignment restrictions apply to this VA.
 */
#define INSTR_CREATE_dc_civac(dc, Rn)                                                   \
    instr_create_0dst_1src(dc, OP_dc_civac,                                             \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates a DC CSW instruction to Clean data cache line by Set/Way.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the Set/Way value and cache level.
 *             See reference manual for a detailed explanation.
 */
#define INSTR_CREATE_dc_csw(dc, Rn) instr_create_0dst_1src(dc, OP_dc_csw, Rn)

/**
 * Creates a DC CVAC instruction to Clean data cache by Virtual Address to
 * point of Coherency.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             No alignment restrictions apply to this VA.
 */
#define INSTR_CREATE_dc_cvac(dc, Rn)                                                    \
    instr_create_0dst_1src(dc, OP_dc_cvac,                                              \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates a DC CVAU instruction to Clean data cache by Virtual Address to
 * point of Unification.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             No alignment restrictions apply to this VA.
 */
#define INSTR_CREATE_dc_cvau(dc, Rn)                                                    \
    instr_create_0dst_1src(dc, OP_dc_cvau,                                              \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates a DC ISW instruction to Invalidate data cache line by Set/Way.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the Set/Way value and cache level.
 *             See reference manual for a detailed explanation.
 */
#define INSTR_CREATE_dc_isw(dc, Rn) instr_create_0dst_1src(dc, OP_dc_isw, Rn)

/**
 * Creates a DC ICVAC instruction to Clean and Invalidate data cache by
 * Virtual Address to point of Coherency.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             No alignment restrictions apply to this VA.
 */
#define INSTR_CREATE_dc_ivac(dc, Rn)                                                    \
    instr_create_0dst_1src(dc, OP_dc_ivac,                                              \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates a DC ZVA instruction to Zero data cache by Virtual Address.
 * Zeroes a naturally aligned block of N bytes, where N is identified in
 * DCZID_EL0 system register.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             There is no alignment restriction on the address within the
 *             block of N bytes that is used.
 */
#define INSTR_CREATE_dc_zva(dc, Rn)                                                     \
    instr_create_1dst_0src(dc, OP_dc_zva,                                               \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates an IC IVAU instruction to Invalidate instruction cache line by
 * VA to point of Unification.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The input register containing the virtual address to use.
 *             No alignment restrictions apply to this VA.
 */
#define INSTR_CREATE_ic_ivau(dc, Rn)                                                    \
    instr_create_0dst_1src(dc, OP_ic_ivau,                                              \
                           opnd_create_base_disp_aarch64(opnd_get_reg(Rn), DR_REG_NULL, \
                                                         0, false, 0, 0, OPSZ_sys))

/**
 * Creates an IC IALLU instruction to Invalidate All of instruction caches
 * to point of Unification.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 */
#define INSTR_CREATE_ic_iallu(dc) instr_create_0dst_0src(dc, OP_ic_iallu)

/**
 * Creates an IC IALLUIS instruction to Invalidate All of instruction caches
 * in Inner Shareable domain to point of Unification.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 */
#define INSTR_CREATE_ic_ialluis(dc) instr_create_0dst_0src(dc, OP_ic_ialluis)

/**
 * Creates a CLREX instruction.
 * \param dc   The void * dcontext used to allocate memory for the instr_t.
 */
#define INSTR_CREATE_clrex(dc) instr_create_0dst_1src(dc, OP_clrex, OPND_CREATE_INT(15))
#define INSTR_CREATE_clrex_imm(dc, imm) \
    instr_create_0dst_1src(dc, OP_clrex, OPND_CREATE_INT(imm))

/* FIXME i#1569: these two should perhaps not be provided */
#define INSTR_CREATE_add_shimm(dc, rd, rn, rm_or_imm, sht, sha) \
    INSTR_CREATE_add_shift(dc, rd, rn, rm_or_imm, sht, sha)
#define INSTR_CREATE_sub_shimm(dc, rd, rn, rm_or_imm, sht, sha) \
    INSTR_CREATE_sub_shift(dc, rd, rn, rm_or_imm, sht, sha)
/** \endcond disabled_until_i4106_is_fixed */

/**
 * Creates an FMOV instruction to move between GPRs and floating point registers.
 * This macro is used to encode the forms:
 * \verbatim
 *    FMOV    <Wd>, <Hn>
 *    FMOV    <Wd>, <Sn>
 *    FMOV    <Xd>, <Dn>
 *    FMOV    <Xd>, <Hn>
 *    FMOV    <Dd>, <Xn>
 *    FMOV    <Hd>, <Wn>
 *    FMOV    <Hd>, <Xn>
 *    FMOV    <Sd>, <Wn>
 *    FMOV    <Dd>, <Dn>
 *    FMOV    <Hd>, <Hn>
 *    FMOV    <Sd>, <Sn>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the instr_t.
 * \param Rd   The output register.
 * \param Rn   The first input register.
 */
#define INSTR_CREATE_fmov_general(dc, Rd, Rn) instr_create_1dst_1src(dc, OP_fmov, Rd, Rn)

/**
 * Creates an FMOV instruction to move between GPRs and floating point registers.
 * \param dc   The void * dcontext used to allocate memory for the instr_t.
 * \param Rd   The output register.
 * \param Rn   The first input vector register.
 */
#define INSTR_CREATE_fmov_upper_vec(dc, Rd, Rn)                                    \
    instr_create_2dst_2src(dc, OP_fmov, Rd, opnd_create_immed_int(1, OPSZ_2b), Rn, \
                           OPND_CREATE_DOUBLE())

/* -------- Advanced SIMD three same including fp16 versions ----------------
 * Some macros are also used for
 *   SVE Integer Arithmetic - Unpredicated Group
 *   Advanced SIMD three same (FP16)
 */

/**
 * Creates a SHADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_shadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_shadd, Rd, Rm, Rn, width)

/**
 * Creates a SQADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sqadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqadd, Rd, Rm, Rn, width)

/**
 * Creates a SRHADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_srhadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_srhadd, Rd, Rm, Rn, width)

/**
 * Creates a SHSUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_shsub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_shsub, Rd, Rm, Rn, width)

/**
 * Creates a SQSUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sqsub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqsub, Rd, Rm, Rn, width)

/**
 * Creates a CMGT vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmgt_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmgt, Rd, Rm, Rn, width)

/**
 * Creates a CMGE vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmge_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmge, Rd, Rm, Rn, width)

/**
 * Creates a SSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sshl, Rd, Rm, Rn, width)

/**
 * Creates a SQSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sqshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqshl, Rd, Rm, Rn, width)

/**
 * Creates a SRSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_srshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_srshl, Rd, Rm, Rn, width)

/**
 * Creates a SQRSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sqrshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqrshl, Rd, Rm, Rn, width)

/**
 * Creates a SMAX vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smax_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smax, Rd, Rm, Rn, width)

/**
 * Creates a SMIN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smin_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smin, Rd, Rm, Rn, width)

/**
 * Creates a SABD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sabd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sabd, Rd, Rm, Rn, width)

/**
 * Creates a SABA vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_saba_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_saba, Rd, Rm, Rn, width)

/**
 * Creates a ADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_add_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_add, Rd, Rm, Rn, width)

/**
 * Creates an ADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output SVE register (created with
 *                opnd_create_reg_element_vector).
 * \param Rm      The first input SVE register (created with
 *                opnd_create_reg_element_vector).
 * \param Rn      The second input SVE register (created with
 *                opnd_create_reg_element_vector).
 */
#define INSTR_CREATE_sve_add_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_add, Rd, Rm, Rn)

/**
 * Creates a CMTST vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmtst_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmtst, Rd, Rm, Rn, width)

/**
 * Creates a MLA vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_mla_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_mla, Rd, Rd, Rm, Rn, width)

/**
 * Creates a MUL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_mul_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_mul, Rd, Rm, Rn, width)

/**
 * Creates a SMAXP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smaxp_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smaxp, Rd, Rm, Rn, width)

/**
 * Creates a SMINP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sminp_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sminp, Rd, Rm, Rn, width)

/**
 * Creates a SQDMULH vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmulh_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmulh, Rd, Rm, Rn, width)

/**
 * Creates a ADDP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_addp_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_addp, Rd, Rm, Rn, width)

/**
 * Creates a FMAXNM vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fmaxnm_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fmaxnm, Rd, Rm, Rn, width)

/**
 * Creates a FMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLA    <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMLA    <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmla_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_fmla, Rd, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLA    <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.H[<index>]
 *    FMLA    <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Tb>[<index>]
 *    FMLA    <Hd>, <Hn>, <Hm>.H[<index>]
 *    FMLA    <V><d>, <V><n>, <Sm>.<Ts>[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register. Can be
 *             S (singleword, 32 bits), D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmla_vector_idx(dc, Rd, Rn, Rm, index, Rm_elsz) \
    instr_create_1dst_5src(dc, OP_fmla, Rd, Rd, Rn, Rm, index, Rm_elsz)

/**
 * Creates a FADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fadd, Rd, Rm, Rn, width)

/**
 * Creates a FMULX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMULX   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMULX   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmulx_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fmulx, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMULX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMULX   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.H[<index>]
 *    FMULX   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Tb>[<index>]
 *    FMULX   <Hd>, <Hn>, <Hm>.H[<index>]
 *    FMULX   <V><d>, <V><n>, <Sm>.<Ts>[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param index     The immediate index for Rm
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmulx_vector_idx(dc, Rd, Rn, Rm, index, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_fmulx, Rd, Rn, Rm, index, Rm_elsz)

/**
 * Creates a FMULX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMULX   <Hd>, <Hn>, <Hm>
 *    FMULX   <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)

 */
#define INSTR_CREATE_fmulx(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_fmulx, Rd, Rn, Rm)

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Hd>.<Ts>, <Hn>.<Ts>, #0
 *    FCMEQ   <Dd>.<Ts>, <Dn>.<Ts>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmeq_vector_zero(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_3src(dc, OP_fcmeq, Rd, Rn, opnd_create_immed_float(0), Rn_elsz)

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FCMEQ   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmeq_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fcmeq, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Hd>, <Hn>, #0
 *    FCMEQ   <V><d>, <V><n>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmeq_zero(dc, Rd, Rn) \
    instr_create_1dst_2src(dc, OP_fcmeq, Rd, Rn, opnd_create_immed_float(0))

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Hd>, <Hn>, <Hm>
 *    FCMEQ   <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmeq(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_fcmeq, Rd, Rn, Rm)

/**
 * Creates a FMLAL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLAL   <Sd>.<Ts>, <Hn>.<Tb>, <Hm>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 * \param Rm   The third source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fmlal_vector(dc, Rd, Rn, Rm) \
    instr_create_1dst_4src(dc, OP_fmlal, Rd, Rd, Rn, Rm, OPND_CREATE_HALF())

/**
 * Creates a FMLAL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLAL   <Sd>.<Ts>, <Hn>.<Tb>, <Hm>.H[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 */
#define INSTR_CREATE_fmlal_vector_idx(dc, Rd, Rn, Rm, index) \
    instr_create_1dst_5src(dc, OP_fmlal, Rd, Rd, Rn, Rm, index, OPND_CREATE_HALF())

/**
 * Creates a FMAX vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fmax_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fmax, Rd, Rm, Rn, width)

/**
 * Creates a FRECPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPE  <Hd>.<Ts>, <Hn>.<Ts>
 *    FRECPE  <Dd>.<Ts>, <Dn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_frecpe_vector(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_frecpe, Rd, Rn, Rn_elsz)

/**
 * Creates a FRECPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPE  <Hd>, <Hn>
 *    FRECPE  <V><d>, <V><n>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_frecpe(dc, Rd, Rn) instr_create_1dst_1src(dc, OP_frecpe, Rd, Rn)

/**
 * Creates a FRECPS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPS  <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FRECPS  <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_frecps_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_frecps, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FRECPS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPS  <Hd>, <Hn>, <Hm>
 *    FRECPS  <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_frecps(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_frecps, Rd, Rn, Rm)

/**
 * Creates a FRSQRTE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTE <Hd>.<Ts>, <Hn>.<Ts>
 *    FRSQRTE <Dd>.<Ts>, <Dn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_frsqrte_vector(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_frsqrte, Rd, Rn, Rn_elsz)

/**
 * Creates a FRSQRTE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTE <Hd>, <Hn>
 *    FRSQRTE <V><d>, <V><n>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_frsqrte(dc, Rd, Rn) instr_create_1dst_1src(dc, OP_frsqrte, Rd, Rn)

/**
 * Creates a AND vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_and_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_and, Rd, Rm, Rn)

/**
 * Creates a BIC vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_bic_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_bic, Rd, Rm, Rn)

/**
 * Creates a FMINNM vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fminnm_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fminnm, Rd, Rm, Rn, width)

/**
 * Creates a FMLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLS    <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMLS    <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmls_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_fmls, Rd, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLS    <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.H[<index>]
 *    FMLS    <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Tb>[<index>]
 *    FMLS    <Hd>, <Hn>, <Hm>.H[<index>]
 *    FMLS    <V><d>, <V><n>, <Sm>.<Ts>[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register. Can be
 *             S (singleword, 32 bits), D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmls_vector_idx(dc, Rd, Rn, Rm, index, Rm_elsz) \
    instr_create_1dst_5src(dc, OP_fmls, Rd, Rd, Rn, Rm, index, Rm_elsz)

/**
 * Creates a FSUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fsub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fsub, Rd, Rm, Rn, width)

/**
 * Creates a FMLSL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmlsl_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_4src(dc, OP_fmlsl, Rd, Rd, Rm, Rn, OPND_CREATE_HALF())
/**
 * Creates a FMLSL indexed vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param index   The first input register's vector element index.
 */
#define INSTR_CREATE_fmlsl_vector_idx(dc, Rd, Rm, Rn, index) \
    instr_create_1dst_5src(dc, OP_fmlsl, Rd, Rd, Rm, Rn, index, OPND_CREATE_HALF())

/**
 * Creates a FMIN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fmin_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fmin, Rd, Rm, Rn, width)

/**
 * Creates a FRSQRTS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTS <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FRSQRTS <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
               D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
               D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
               D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_frsqrts_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_frsqrts, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FRSQRTS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTS <Hd>, <Hn>, <Hm>
 *    FRSQRTS <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
               S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
               S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
               S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_frsqrts(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_frsqrts, Rd, Rn, Rm)

/**
 * Creates a ORR vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_orr_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_orr, Rd, Rm, Rn)

/**
 * Creates a ORN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_orn_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_orn, Rd, Rm, Rn)

/**
 * Creates a UHADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uhadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uhadd, Rd, Rm, Rn, width)

/**
 * Creates a UQADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_uqadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uqadd, Rd, Rm, Rn, width)

/**
 * Creates a URHADD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_urhadd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_urhadd, Rd, Rm, Rn, width)

/**
 * Creates a UHSUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uhsub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uhsub, Rd, Rm, Rn, width)

/**
 * Creates a UQSUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_uqsub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uqsub, Rd, Rm, Rn, width)

/**
 * Creates a CMHI vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmhi_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmhi, Rd, Rm, Rn, width)

/**
 * Creates a CMHS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmhs_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmhs, Rd, Rm, Rn, width)

/**
 * Creates a USHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_ushl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_ushl, Rd, Rm, Rn, width)

/**
 * Creates a UQSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_uqshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uqshl, Rd, Rm, Rn, width)

/**
 * Creates a URSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_urshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_urshl, Rd, Rm, Rn, width)

/**
 * Creates a UQRSHL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_uqrshl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uqrshl, Rd, Rm, Rn, width)

/**
 * Creates a UMAX vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umax_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_umax, Rd, Rm, Rn, width)

/**
 * Creates a UMIN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umin_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_umin, Rd, Rm, Rn, width)

/**
 * Creates a UABD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uabd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uabd, Rd, Rm, Rn, width)

/**
 * Creates a UABA vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uaba_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uaba, Rd, Rm, Rn, width)

/**
 * Creates a SUB vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_sub_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sub, Rd, Rm, Rn, width)

/**
 * Creates a CMEQ vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_cmeq_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_cmeq, Rd, Rm, Rn, width)

/**
 * Creates a MLS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_mls_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_mls, Rd, Rd, Rm, Rn, width)

/**
 * Creates a PMUL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use OPND_CREATE_BYTE().
 */
#define INSTR_CREATE_pmul_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_pmul, Rd, Rm, Rn, width)

/**
 * Creates a UMAXP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umaxp_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_umaxp, Rd, Rm, Rn, width)

/**
 * Creates a UMINP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uminp_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uminp, Rd, Rm, Rn, width)

/**
 * Creates a SQRDMULH vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqrdmulh_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqrdmulh, Rd, Rm, Rn, width)

/**
 * Creates a SQRDMLSH scalar instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_sqrdmlsh_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_3src(dc, OP_sqrdmlsh, Rd, Rd, Rm, Rn)

/**
 * Creates a SQRDMLSH scalar indexed instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param index   The first input register's vector element index.
 * \param elsz    The vector element size. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqrdmlsh_scalar_idx(dc, Rd, Rm, Rn, index, elsz) \
    instr_create_1dst_5src(dc, OP_sqrdmlsh, Rd, Rd, Rm, Rn, index, elsz)

/**
 * Creates a SQRDMLSH vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param elsz    The vector element size. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqrdmlsh_vector(dc, Rd, Rm, Rn, elsz) \
    instr_create_1dst_4src(dc, OP_sqrdmlsh, Rd, Rd, Rm, Rn, elsz)

/**
 * Creates a SQRDMLSH vector indexed instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param index   The first input register's vector element index.
 * \param elsz    The vector element size. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqrdmlsh_vector_idx(dc, Rd, Rm, Rn, index, elsz) \
    instr_create_1dst_5src(dc, OP_sqrdmlsh, Rd, Rd, Rm, Rn, index, elsz)

/**
 * Creates a FMLAL2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLAL2  <Sd>.<Ts>, <Hn>.<Tb>, <Hm>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 * \param Rm   The third source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fmlal2_vector(dc, Rd, Rn, Rm) \
    instr_create_1dst_4src(dc, OP_fmlal2, Rd, Rd, Rn, Rm, OPND_CREATE_HALF())

/**
 * Creates a FMLAL2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLAL2  <Sd>.<Ts>, <Hn>.<Tb>, <Hm>.H[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, S (singleword, 32 bits) or
 *             D (doubleword, 64 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 */
#define INSTR_CREATE_fmlal2_vector_idx(dc, Rd, Rn, Rm, index) \
    instr_create_1dst_5src(dc, OP_fmlal2, Rd, Rd, Rn, Rm, index, OPND_CREATE_HALF())

/**
 * Creates a FADDP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADDP   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FADDP   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_faddp_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_faddp, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FADDP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADDP   <Hd>, <Hn>.2H
 *    FADDP   <V><d>, <Sn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_faddp_scalar(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_faddp, Rd, Rn, Rn_elsz)

/**
 * Creates a FMUL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fmul_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fmul, Rd, Rm, Rn, width)

/**
 * Creates a FCMGE vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcmge_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fcmge, Rd, Rm, Rn, width)

/**
 * Creates a FMAXNMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXNMP <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMAXNMP <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmaxnmp_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fmaxnmp, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMAXNMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXNMP <Hd>, <Hn>.2H
 *    FMAXNMP <V><d>, <Sn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source vector register. Can be
 *             S (singleword, 32 bits), D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmaxnmp_scalar(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fmaxnmp, Rd, Rn, Rn_elsz)

/**
 * Creates a FMAXP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXP   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMAXP   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmaxp_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fmaxp, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMAXP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXP   <Hd>, <Hn>.2H
 *    FMAXP   <V><d>, <Sn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fmaxp_scalar(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fmaxp, Rd, Rn, Rn_elsz)

/**
 * Creates a FACGE vector instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGE   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FACGE   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_facge_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_facge, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FACGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGE   <Hd>, <Hn>, <Hm>
 *    FACGE   <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_facge(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_facge, Rd, Rn, Rm)

/**
 * Creates a FCMLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLE   <Hd>.<Ts>, <Hn>.<Ts>, #0
 *    FCMLE   <Dd>.<Ts>, <Dn>.<Ts>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *             OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmle_vector_zero(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_3src(dc, OP_fcmle, Rd, Rn, opnd_create_immed_float(0.0), Rn_elsz)

/**
 * Creates a FCMLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLE   <Hd>, <Hn>, #0
 *    FCMLE   <V><d>, <V><n>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmle_zero(dc, Rd, Rn) \
    instr_create_1dst_2src(dc, OP_fcmle, Rd, Rn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLT   <Hd>.<Ts>, <Hn>.<Ts>, #0
 *    FCMLT   <Dd>.<Ts>, <Dn>.<Ts>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *             OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmlt_vector_zero(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_3src(dc, OP_fcmlt, Rd, Rn, opnd_create_immed_float(0.0), Rn_elsz)

/**
 * Creates a FCMLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLT   <Hd>, <Hn>, #0
 *    FCMLT   <V><d>, <V><n>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmlt_zero(dc, Rd, Rn) \
    instr_create_1dst_2src(dc, OP_fcmlt, Rd, Rn, opnd_create_immed_float(0.0))

/**
 * Creates a FDIV vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fdiv_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fdiv, Rd, Rm, Rn, width)

/**
 * Creates a EOR vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_eor_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_eor, Rd, Rm, Rn)

/**
 * Creates a BSL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_bsl_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_bsl, Rd, Rm, Rn)

/**
 * Creates a FMINNMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNMP <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMINNMP <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fminnmp_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fminnmp, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMINNMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNMP <Hd>, <Hn>.2H
 *    FMINNMP <V><d>, <Sn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source vector register. Can be S (singleword, 32 bits),
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fminnmp_scalar(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fminnmp, Rd, Rn, Rn_elsz)

/**
 * Creates a FMINNMV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNMV <Hd>, <Hn>.<Ts>
 *    FMINNMV <Sd>, <Sn>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits) or
 *             S (singleword, 32 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF() or
 *                  OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_fminnmv_vector(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fminnmv, Rd, Rn, Rn_elsz)

/**
 * Creates a FMLSL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmlsl2_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_4src(dc, OP_fmlsl2, Rd, Rd, Rm, Rn, OPND_CREATE_HALF())
/**
 * Creates a FMLSL2 indexed vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register. The instruction also reads this register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param index   The first input register's vector element index.
 */
#define INSTR_CREATE_fmlsl2_vector_idx(dc, Rd, Rm, Rn, index) \
    instr_create_1dst_5src(dc, OP_fmlsl2, Rd, Rd, Rm, Rn, index, OPND_CREATE_HALF())

/**
 * Creates a FABD vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The vector element width. Use either OPND_CREATE_HALF(),
 *                OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fabd_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_fabd, Rd, Rm, Rn, width)

/**
 * Creates a FACGT vector instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGT   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FACGT   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_facgt_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_facgt, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FACGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGT   <Hd>, <Hn>, <Hm>
 *    FACGT   <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_facgt(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_facgt, Rd, Rn, Rm)

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Hd>.<Ts>, <Hn>.<Ts>, #0
 *    FCMGT   <Dd>.<Ts>, <Dn>.<Ts>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmgt_vector_zero(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_3src(dc, OP_fcmgt, Rd, Rn, opnd_create_immed_float(0.0), Rn_elsz)

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FCMGT   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fcmgt_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fcmgt, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Hd>, <Hn>, #0
 *    FCMGT   <V><d>, <V><n>, #0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmgt_zero(dc, Rd, Rn) \
    instr_create_1dst_2src(dc, OP_fcmgt, Rd, Rn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Hd>, <Hn>, <Hm>
 *    FCMGT   <V><d>, <V><n>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rm   The third source register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 */
#define INSTR_CREATE_fcmgt(dc, Rd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_fcmgt, Rd, Rn, Rm)

/**
 * Creates a FMINP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINP   <Hd>.<Ts>, <Hn>.<Ts>, <Hm>.<Ts>
 *    FMINP   <Dd>.<Ts>, <Dn>.<Ts>, <Dm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm   The third source vector register. Can be
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fminp_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_fminp, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a FMINP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINP   <Hd>, <Hn>.2H
 *    FMINP   <V><d>, <Sn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be H (halfword, 16 bits),
 *             S (singleword, 32 bits) or D (doubleword, 64 bits)
 * \param Rn   The second source vector register. Can be
 *             S (singleword, 32 bits), D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn. Can be OPND_CREATE_HALF(),
 *                  OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fminp_scalar(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fminp, Rd, Rn, Rn_elsz)

/**
 * Creates a BIT vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_bit_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_bit, Rd, Rm, Rn)

/**
 * Creates a BIF vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_bif_vector(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_bif, Rd, Rm, Rn)

/**
 * Creates an FCVTAS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtas_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtas, Rd, Rm, width)

/**
 * Creates an FCVTAU vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtau_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtau, Rd, Rm, width)

/**
 * Creates an FCVTMS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtms_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtms, Rd, Rm, width)

/**
 * Creates an FCVTMU vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtmu_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtmu, Rd, Rm, width)

/**
 * Creates an FCVTNS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtns_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtns, Rd, Rm, width)

/**
 * Creates an FCVTNU vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtnu_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtnu, Rd, Rm, width)

/**
 * Creates an FCVTPS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtps_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtps, Rd, Rm, width)

/**
 * Creates an FCVTPU vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtpu_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtpu, Rd, Rm, width)

/**
 * Creates an FCVTZS vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_SINGLE()
 * or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtzs_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtzs, Rd, Rm, width)

/**
 * Creates an FCVTZU vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_SINGLE()
 * or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_fcvtzu_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_fcvtzu, Rd, Rm, width)

/**
 * Creates an FCVTZU vector floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input register.
 * \param width   The vector element width. Use either OPND_CREATE_SINGLE() or
 *                OPND_CREATE_DOUBLE().
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                destination element.
 */
#define INSTR_CREATE_fcvtzu_vector_fixed(dc, Rd, Rm, width, fbits) \
    instr_create_1dst_3src(dc, OP_fcvtzu, Rd, Rm, width, fbits)

/**
 * Creates an SLI shift left and insert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rn      The input register.
 * \param width   The output vector element width. Use OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 * \param shift   The number of bits to shift the result by.
 */
#define INSTR_CREATE_sli_vector(dc, Rd, Rn, width, shift) \
    instr_create_1dst_3src(dc, OP_sli, Rd, Rn, width, shift)

/**
 * Creates an UQSHRN vector unsigned saturating shift right narrow (immediate)
 * instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rn      The input register.
 * \param width   The output vector element width. Use OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 * \param shift   The number of bits to shift the result by.
 */
#define INSTR_CREATE_uqshrn_vector(dc, Rd, Rn, width, shift) \
    instr_create_1dst_3src(dc, OP_uqshrn, Rd, Rn, width, shift)

/**
 * Creates a UCVTF vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_SINGLE()
 * or #OPND_CREATE_DOUBLE() or #OPND_CREATE_HALF().
 */
#define INSTR_CREATE_ucvtf_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_ucvtf, Rd, Rm, width)

/**
 * Creates a UCVTF vector floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input register.
 * \param width   The vector element width. Must be #OPND_CREATE_SINGLE() or
 *                #OPND_CREATE_DOUBLE() or #OPND_CREATE_HALF().
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                destination element.
 */
#define INSTR_CREATE_ucvtf_vector_fixed(dc, Rd, Rm, width, fbits) \
    instr_create_1dst_3src(dc, OP_ucvtf, Rd, Rm, width, fbits)

/**
 * Creates an SCVTF vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_SINGLE()
 * or #OPND_CREATE_DOUBLE() or #OPND_CREATE_HALF().
 */
#define INSTR_CREATE_scvtf_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_scvtf, Rd, Rm, width)

/**
 * Creates an SCVTF vector floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input register.
 * \param width   The vector element width. Must be #OPND_CREATE_SINGLE() or
 *                #OPND_CREATE_DOUBLE() or #OPND_CREATE_HALF().
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                destination element.
 */
#define INSTR_CREATE_scvtf_vector_fixed(dc, Rd, Rm, width, fbits) \
    instr_create_1dst_3src(dc, OP_scvtf, Rd, Rm, width, fbits)

/**
 * Creates a SHA512H instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SHA512H <Qd>, <Qn>, <Dm>.2D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination register, Q (quadword, 128 bits)
 * \param Rn   The second source register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_sha512h(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sha512h, Rd, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a SHA512H2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SHA512H2 <Qd>, <Qn>, <Dm>.2D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination register, Q (quadword, 128 bits)
 * \param Rn   The second source register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_sha512h2(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sha512h2, Rd, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a SHA512SU0 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SHA512SU0 <Dd>.2D, <Dn>.2D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn, OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_sha512su0(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_3src(dc, OP_sha512su0, Rd, Rd, Rn, Rn_elsz)

/**
 * Creates a SHA512SU1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SHA512SU1 <Dd>.2D, <Dn>.2D, <Dm>.2D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_sha512su1(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sha512su1, Rd, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a RAX1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RAX1    <Dd>.2D, <Dn>.2D, <Dm>.2D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 */
#define INSTR_CREATE_rax1(dc, Rd, Rn, Rm) \
    instr_create_1dst_3src(dc, OP_rax1, Rd, Rn, Rm, OPND_CREATE_DOUBLE())

/**
 * Creates a XAR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    XAR     <Dd>.2D, <Dn>.2D, <Dm>.2D, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param imm6   The immediate imm
 */
#define INSTR_CREATE_xar(dc, Rd, Rn, Rm, imm6) \
    instr_create_1dst_4src(dc, OP_xar, Rd, Rn, Rm, imm6, OPND_CREATE_DOUBLE())

/* -------- Memory Touching instructions ------------------------------- */

/**
 * Creates an LDR immediate instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt      The output register.
 * \param Xn      The input register or stack pointer
 * \param Rn      The input memory disposition.
 * \param imm     Immediate int of the input register offset
 */
#define INSTR_CREATE_ldr_imm(dc, Rt, Xn, Rn, imm) \
    instr_create_2dst_3src(dc, OP_ldr, Rt, Xn, Rn, Xn, imm)

/**
 * Creates a STR immediate instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt      The output memory disposition.
 * \param Xt      The input register or stack pointer.
 * \param Xn      The output register
 * \param imm     Immediate int of the output register offset
 */
#define INSTR_CREATE_str_imm(dc, Rt, Xt, Xn, imm) \
    instr_create_2dst_3src(dc, OP_str, Rt, Xn, Xt, Xn, imm)

/* -------- Floating-point data-processing (1 source) ------------------ */

/**
 * Creates an FMOV floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_fmov_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_fmov, Rd, Rm)

/**
 * Creates a FABS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_fabs_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_fabs, Rd, Rm)

/**
 * Creates a FNEG floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_fneg_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_fneg, Rd, Rm)

/**
 * Creates a FSQRT floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_fsqrt_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_fsqrt, Rd, Rm)

/**
 * Creates a FSQRT instruction.
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register. Can be D (doubleword, 64 bits) or Q
 * (quadword, 128 bits) \param Rn   The second source vector register. Can be D
 * (doubleword, 64 bits) or Q (quadword, 128 bits) \param Rn_elsz   The element size for
 * Rn. Can be #OPND_CREATE_HALF(), #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE()
 */
#define INSTR_CREATE_fsqrt_vector(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_fsqrt, Rd, Rn, Rn_elsz)

/**
 * Creates an FCVT floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvt_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_fcvt, Rd, Rm)

/**
 * Creates an FCVTAS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtas_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtas, Rd, Rm)

/**
 * Creates an FCVTAU floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtau_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtau, Rd, Rm)

/**
 * Creates an FCVTMS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtms_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtms, Rd, Rm)

/**
 * Creates an FCVTMU floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtmu_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtmu, Rd, Rm)

/**
 * Creates an FCVTNS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtns_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtns, Rd, Rm)

/**
 * Creates an FCVTNU floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtnu_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtnu, Rd, Rm)

/**
 * Creates an FCVTPS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtps_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtps, Rd, Rm)

/**
 * Creates an FCVTPU floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtpu_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtpu, Rd, Rm)

/**
 * Creates an FCVTZS floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtzs_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtzs, Rd, Rm)

/**
 * Creates an FCVTZS scalar floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                destination.
 */
#define INSTR_CREATE_fcvtzs_scalar_fixed(dc, Rd, Rm, fbits) \
    instr_create_1dst_2src(dc, OP_fcvtzs, Rd, Rm, fbits)

/**
 * Creates an FCVTZU floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 */
#define INSTR_CREATE_fcvtzu_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_fcvtzu, Rd, Rm)

/**
 * Creates an FCVTZU scalar floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point or integer output register.
 * \param Rm      Floating-point input register.
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                destination.
 */
#define INSTR_CREATE_fcvtzu_scalar_fixed(dc, Rd, Rm, fbits) \
    instr_create_1dst_2src(dc, OP_fcvtzu, Rd, Rm, fbits)

/**
 * Creates a UCVTF floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point output register.
 * \param Rm      Integer input register.
 */
#define INSTR_CREATE_ucvtf_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_ucvtf, Rd, Rm)

/**
 * Creates a UCVTF scalar floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point output register.
 * \param Rm      Integer input register.
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                input.
 */
#define INSTR_CREATE_ucvtf_scalar_fixed(dc, Rd, Rm, fbits) \
    instr_create_1dst_2src(dc, OP_ucvtf, Rd, Rm, fbits)

/**
 * Creates an SCVTF floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point output register.
 * \param Rm      Integer input register.
 */
#define INSTR_CREATE_scvtf_scalar(dc, Rd, Rm) instr_create_1dst_1src(dc, OP_scvtf, Rd, Rm)

/**
 * Creates an SCVTF scalar floating-point to fixed-point convert instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      Floating-point output register.
 * \param Rm      Integer input register.
 * \param fbits   The number of bits after the binary point in the fixed-point
 *                input.
 */
#define INSTR_CREATE_scvtf_scalar_fixed(dc, Rd, Rm, fbits) \
    instr_create_1dst_2src(dc, OP_scvtf, Rd, Rm, fbits)

/**
 * Creates a FRINTN floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frintn_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frintn, Rd, Rm)

/**
 * Creates a FRINTN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frintn_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frintn, Rd, Rm, width)

/**
 * Creates a FRINTP floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frintp_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frintp, Rd, Rm)

/**
 * Creates a FRINTP vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frintp_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frintp, Rd, Rm, width)

/**
 * Creates a FRINTM floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frintm_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frintm, Rd, Rm)

/**
 * Creates a FRINTM vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frintm_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frintm, Rd, Rm, width)

/**
 * Creates a FRINTZ floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frintz_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frintz, Rd, Rm)

/**
 * Creates a FRINTZ vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frintz_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frintz, Rd, Rm, width)

/**
 * Creates a FRINTA floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frinta_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frinta, Rd, Rm)

/**
 * Creates a FRINTA vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frinta_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frinta, Rd, Rm, width)

/**
 * Creates a FRINTX floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frintx_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frintx, Rd, Rm)

/**
 * Creates a FRINTX vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frintx_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frintx, Rd, Rm, width)

/**
 * Creates a FRINTI floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 */
#define INSTR_CREATE_frinti_scalar(dc, Rd, Rm) \
    instr_create_1dst_1src(dc, OP_frinti, Rd, Rm)

/**
 * Creates a FRINTI vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd      The output register.
 * \param Rm      The input vector register.
 * \param width   Immediate int of the vector element width. Must be #OPND_CREATE_HALF()
 * or #OPND_CREATE_SINGLE() or #OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_frinti_vector(dc, Rd, Rm, width) \
    instr_create_1dst_2src(dc, OP_frinti, Rd, Rm, width)

/**
 * Creates a LDPSW floating point instruction.
 * \param dc    The void * dcontext used to allocate memory for the instr_t.
 * \param Xt1   The first GPR output register.
 * \param Xt2   The second GPR output register.
 * \param Xn    The input Stack-pointer or GPR register.
 * \param Xr    The disposition of the input Stack-pointer or GPR register.
 * \param imm   The immediate integer offset.
 */

#define INSTR_CREATE_ldpsw(dc, Xt1, Xt2, Xn, Xr, imm) \
    instr_create_3dst_3src(dc, OP_ldpsw, Xt1, Xt2, Xn, Xr, Xn, imm)

/**
 * Creates a LDPSW floating point instruction.
 * \param dc    dc
 * \param Xt1   The first GPR output register.
 * \param Xt2   The second GPR output register.
 * \param Xn    The disposition of the input register.
 */
#define INSTR_CREATE_ldpsw_2(dc, Xt1, Xt2, Xn) \
    instr_create_2dst_1src(dc, OP_ldpsw, Xt1, Xt2, Xn)

/* -------- Floating-point data-processing (2 source) ------------------ */

/**
 * Creates a FMUL floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmul_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fmul, Rd, Rm, Rn)

/**
 * Creates a FDIV floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fdiv_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fdiv, Rd, Rm, Rn)

/**
 * Creates a FADD floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fadd_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fadd, Rd, Rm, Rn)

/**
 * Creates a FSUB floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fsub_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fsub, Rd, Rm, Rn)

/**
 * Creates a FMAX floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmax_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fmax, Rd, Rm, Rn)

/**
 * Creates a FMIN floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmin_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fmin, Rd, Rm, Rn)

/**
 * Creates a FMAXNM floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fmaxnm_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fmaxnm, Rd, Rm, Rn)

/**
 * Creates a FMINNM floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fminnm_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fminnm, Rd, Rm, Rn)

/**
 * Creates a FNMUL floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 */
#define INSTR_CREATE_fnmul_scalar(dc, Rd, Rm, Rn) \
    instr_create_1dst_2src(dc, OP_fnmul, Rd, Rm, Rn)

/* -------- Floating-point data-processing (3 source) ------------------ */

/**
 * Creates a FMADD floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param Ra      The third input register.
 */
#define INSTR_CREATE_fmadd_scalar(dc, Rd, Rm, Rn, Ra) \
    instr_create_1dst_3src(dc, OP_fmadd, Rd, Rm, Rn, Ra)

/**
 * Creates a FMSUB floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param Ra      The third input register.
 */
#define INSTR_CREATE_fmsub_scalar(dc, Rd, Rm, Rn, Ra) \
    instr_create_1dst_3src(dc, OP_fmsub, Rd, Rm, Rn, Ra)

/**
 * Creates a FNMADD floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param Ra      The third input register.
 */
#define INSTR_CREATE_fnmadd_scalar(dc, Rd, Rm, Rn, Ra) \
    instr_create_1dst_3src(dc, OP_fnmadd, Rd, Rm, Rn, Ra)

/**
 * Creates a FNMSUB floating point instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param Ra      The third input register.
 */
#define INSTR_CREATE_fnmsub_scalar(dc, Rd, Rm, Rn, Ra) \
    instr_create_1dst_3src(dc, OP_fnmsub, Rd, Rm, Rn, Ra)

/* Advanced SIMD (NEON) memory instructions */

#define INSTR_CREATE_ld2_multi(dc, Vt1, Vt2, Xn, elsz) \
    instr_create_2dst_2src(dc, OP_ld2, Vt1, Vt2, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD2 instruction to load multiple 2-element
 * structures to two vector registers with post-indexing, e.g. LD2 {V0.4H, V1.4H}, [X0],
 * #32.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1 and Vt2.
 * \param disp    The disposition of Xn.
 * \param offset  The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld2_multi_2(dc, Vt1, Vt2, Xn, disp, offset, elsz) \
    instr_create_3dst_4src(dc, OP_ld2, Vt1, Vt2, Xn, disp, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD2 instruction to load a 2-element
 * structure to the index of two vector registers, e.g. LD2 {V0.4H, V1.4H}[5], [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1 and Vt2.
 * \param index   The vector element index.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld2(dc, Vt1, Vt2, Xn, index, elsz) \
    instr_create_2dst_3src(dc, OP_ld2, Vt1, Vt2, Xn, index, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD2 instruction to load a 2-element
 * structure to the index of two vector registers with post-indexing,
 * e.g. LD2 {V0.4H, V1.4H}[5], [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Xn      The stack-pointer or register to load into Vt and Vt2.
 * \param Xnd     The disposition of Xn.
 * \param index   The index of the destination vectors.
 * \param offset  The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld2_2(dc, Vt1, Vt2, Xn, Xnd, index, offset, elsz) \
    instr_create_3dst_5src(dc, OP_ld2, Vt1, Vt2, Xn, Xnd, index, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD2R instruction to load and replicate a
 * single 2-element structure to all lanes of two vector registers,
 * e.g. LD2R {V0.4H, V1.4H}, [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1 and Vt2.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld2r(dc, Vt1, Vt2, Xn, elsz) \
    instr_create_2dst_2src(dc, OP_ld2r, Vt1, Vt2, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD2R instruction to load and replicate a
 * single 2-element structure to all lanes of two vector registers with post-indexing
 * , e.g. LD2R {V0.4H, V1.4H}, [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt and Vt2.
 * \param Xnd     Disposition of Xn.
 * \param Xm      The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld2r_2(dc, Vt1, Vt2, Xn, Xnd, Xm, elsz) \
    instr_create_3dst_4src(dc, OP_ld2r, Vt1, Vt2, Xn, Xnd, Xn, Xm, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load multiple 3-element
 * structures from memory to three vector register,
 * e.g. LD3 {V0.4H, V1.4H, V2.4H}, [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1, Vt2 and Vt3.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3_multi(dc, Vt1, Vt2, Vt3, Xn, elsz) \
    instr_create_3dst_2src(dc, OP_ld3, Vt1, Vt2, Vt3, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load multiple 3-element
 * structures from memory to the index of three vector registers with
 * post-index offset, e.g. LD3 {V0.4H, V1.4H, V2.4H}, [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1, Vt2 and Vt3.
 * \param Xnd     The disposition of Xn.
 * \param Xm      The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3_multi_2(dc, Vt1, Vt2, Vt3, Xn, Xnd, Xm, elsz) \
    instr_create_4dst_4src(dc, OP_ld3, Vt1, Vt2, Vt3, Xn, Xnd, Xn, Xm, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load a single 3-element
 * structure to the index of three vector registers, e.g. LD3 {V0.4H, V1.4H, V2.4H}[15],
 * [X0]. \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The GPR to load into Vt1, Vt2 and Vt3.
 * \param index   The index of the destination vectors.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3(dc, Vt1, Vt2, Vt3, Xn, index, elsz) \
    instr_create_3dst_3src(dc, OP_ld3, Vt1, Vt2, Vt3, Xn, index, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load a single 3-element
 * structure to the index of three vector registers with post-index offset,
 * e.g. LD3 {V0.4H, V1.4H, V2.4H}[15], [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The register to load into Vt, Vt2 and Vt3.
 * \param index   The immediate or GPR post-index offset.
 * \param Xnd     The disposition of Xn.
 * \param offset  The immediate or GPR post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3_2(dc, Vt1, Vt2, Vt3, Xn, Xnd, index, offset, elsz) \
    instr_create_4dst_5src(dc, OP_ld3, Vt1, Vt2, Vt3, Xn, Xnd, index, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load and replicate a single
 * 3-element structure to the index of three vector registers, e.g. LD3 {V0.4H, V1.4H,
 * V2.4H}[15], [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1, Vt2 and Vt3.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3r(dc, Vt1, Vt2, Vt3, Xn, elsz) \
    instr_create_3dst_2src(dc, OP_ld3r, Vt1, Vt2, Vt3, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD3 instruction to load and replicate a single
 * 3-element structure to the index of three vector registers with post-index offset, e.g.
 * LD3 {V0.4H, V1.4H, V2.4H}[15], [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into Vt1, Vt2 and Vt3.
 * \param Xnd     The disposition of Xn.
 * \param offset  The immediate or GPR post-index offset
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld3r_2(dc, Vt1, Vt2, Vt3, Xn, Xnd, offset, elsz) \
    instr_create_4dst_4src(dc, OP_ld3r, Vt1, Vt2, Vt3, Xn, Xnd, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4 instruction to load single or multiple 4-element
 * structures to four vector registers, e.g. LD4 {V0.4H, V1.4H, V2.4H, V3.4H}, [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or register to load into the destination vectors.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4_multi(dc, Vt1, Vt2, Vt3, Vt4, Xn, elsz) \
    instr_create_4dst_2src(dc, OP_ld4, Vt1, Vt2, Vt3, Vt4, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4 instruction to load multiple 4-element
 * structures to four vector registers with post-index,
 * e.g. LD4 {V0.4H, V1.4H, V2.4H, V3.4H}, [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into the destination vectors
 * \param Xnd     The disposition of Xn
 * \param offset  The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4_multi_2(dc, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, offset, elsz) \
    instr_create_5dst_4src(dc, OP_ld4, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4 instruction to load single or multiple 4-element
 * structures to four vector registers, e.g. LD4 {V0.4H, V1.4H, V2.4H, V3.4H}, [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or register to load into the destination vectors.
 * \param index   The immediate or GPR post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4(dc, Vt1, Vt2, Vt3, Vt4, Xn, index, elsz) \
    instr_create_4dst_3src(dc, OP_ld4, Vt1, Vt2, Vt3, Vt4, Xn, index, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4 instruction to load a single 4-element
 * structures to four vector registers with post-index,
 * e.g. LD4 {V0.4H, V1.4H, V2.4H, V3.4H}, [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into the destination vectors.
 * \param Xnd     The disposition of Xn.
 * \param index   The index of the destination vectors.
 * \param offset  The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4_2(dc, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, index, offset, elsz)       \
    instr_create_5dst_5src(dc, OP_ld4, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, index, Xn, offset, \
                           elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4R instruction to load
 * and replicate a single 4-element structure to four vector registers,
 * e.g. LD4R {V0.4H, V1.4H, V2.4H, V3.4H}, [X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or GPR to load into the destination vectors.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4r(dc, Vt1, Vt2, Vt3, Vt4, Xn, elsz) \
    instr_create_4dst_2src(dc, OP_ld4r, Vt1, Vt2, Vt3, Vt4, Xn, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD4R instruction to load and
 * replicate a single 4-element structure to four vector registers with post-indexing,
 * e.g. LD4 {V0.4H, V1.4H, V2.4H, V3.4H}, [X0], X1.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Vt1     The first destination vector register operand.
 * \param Vt2     The second destination vector register operand.
 * \param Vt3     The third destination vector register operand.
 * \param Vt4     The fourth destination vector register operand.
 * \param Xn      The stack-pointer or register to load into the destination vectors.
 * \param Xnd     The disposition of Xn.
 * \param offset  The post-index offset.
 * \param elsz    The vector element size
 */
#define INSTR_CREATE_ld4r_2(dc, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, offset, elsz) \
    instr_create_5dst_4src(dc, OP_ld4r, Vt1, Vt2, Vt3, Vt4, Xn, Xnd, Xn, offset, elsz)

/**
 * Creates an Advanced SIMD (NEON) LD1 instruction to load multiple
 * single element structures to one vector register, e.g. LD1 {V0.4H},[X0].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param q       The destination vector register operand.
 * \param r       The source memory operand.
 * \param s       The size of the vector element.
 */
#define INSTR_CREATE_ld1_multi_1(dc, q, r, s) instr_create_1dst_2src(dc, OP_ld1, q, r, s)

/**
 * Creates an Advanced SIMD (NEON) ST1 instruction to store multiple
 * single element structures from one vector register, e.g. ST1 {V1.2S},[X1].
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param r       The destination memory operand.
 * \param q       The source vector register operand.
 * \param s       The size of the vector element.
 */
#define INSTR_CREATE_st1_multi_1(dc, r, q, s) instr_create_1dst_2src(dc, OP_st1, r, q, s)

/* -------- Advanced SIMD three different ------------------------------ */

/**
 * Creates a SADDL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_saddl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_saddl, Rd, Rm, Rn, width)

/**
 * Creates a SADDL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_saddl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_saddl2, Rd, Rm, Rn, width)

/**
 * Creates a SADDW vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_saddw_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_saddw, Rd, Rm, Rn, width)

/**
 * Creates a SADDW2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_saddw2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_saddw2, Rd, Rm, Rn, width)

/**
 * Creates a SSUBL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_ssubl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_ssubl, Rd, Rm, Rn, width)

/**
 * Creates a SSUBL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_ssubl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_ssubl2, Rd, Rm, Rn, width)

/**
 * Creates a SSUBW vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_ssubw_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_ssubw, Rd, Rm, Rn, width)

/**
 * Creates a SSUBW2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_ssubw2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_ssubw2, Rd, Rm, Rn, width)

/**
 * Creates a ADDHN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_addhn_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_addhn, Rd, Rm, Rn, width)

/**
 * Creates a ADDHN2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_addhn2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_addhn2, Rd, Rm, Rn, width)

/**
 * Creates a SABAL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sabal_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sabal, Rd, Rm, Rn, width)

/**
 * Creates a SABAL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sabal2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sabal2, Rd, Rm, Rn, width)

/**
 * Creates a SUBHN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_subhn_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_subhn, Rd, Rm, Rn, width)

/**
 * Creates a SUBHN2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_subhn2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_subhn2, Rd, Rm, Rn, width)

/**
 * Creates a SABDL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sabdl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sabdl, Rd, Rm, Rn, width)

/**
 * Creates a SABDL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sabdl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sabdl2, Rd, Rm, Rn, width)

/**
 * Creates a SMLAL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smlal_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smlal, Rd, Rm, Rn, width)

/**
 * Creates a SMLAL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smlal2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smlal2, Rd, Rm, Rn, width)

/**
 * Creates a SQDMLAL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmlal_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmlal, Rd, Rm, Rn, width)

/**
 * Creates a SQDMLAL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmlal2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmlal2, Rd, Rm, Rn, width)

/**
 * Creates a SMLSL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smlsl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smlsl, Rd, Rm, Rn, width)

/**
 * Creates a SMLSL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smlsl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smlsl2, Rd, Rm, Rn, width)

/**
 * Creates a SQDMLSL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmlsl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmlsl, Rd, Rm, Rn, width)

/**
 * Creates a SQDMLSL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmlsl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmlsl2, Rd, Rm, Rn, width)

/**
 * Creates a SMULL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smull_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smull, Rd, Rm, Rn, width)

/**
 * Creates a SMULL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_smull2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_smull2, Rd, Rm, Rn, width)

/**
 * Creates a SQDMULL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmull_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmull, Rd, Rm, Rn, width)

/**
 * Creates a SQDMULL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_HALF() or
 *                OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_sqdmull2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_sqdmull2, Rd, Rm, Rn, width)

/**
 * Creates a PMULL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(), or
 *                OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_pmull_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_pmull, Rd, Rm, Rn, width)

/**
 * Creates a PMULL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(), or
 *                OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_pmull2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_pmull2, Rd, Rm, Rn, width)

/**
 * Creates a UADDL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uaddl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uaddl, Rd, Rm, Rn, width)

/**
 * Creates a UADDL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uaddl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uaddl2, Rd, Rm, Rn, width)

/**
 * Creates a UADDW vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uaddw_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uaddw, Rd, Rm, Rn, width)

/**
 * Creates a UADDW2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uaddw2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uaddw2, Rd, Rm, Rn, width)

/**
 * Creates a USUBL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_usubl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_usubl, Rd, Rm, Rn, width)

/**
 * Creates a USUBL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_usubl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_usubl2, Rd, Rm, Rn, width)

/**
 * Creates a USUBW vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_usubw_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_usubw, Rd, Rm, Rn, width)

/**
 * Creates a USUBW2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_usubw2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_usubw2, Rd, Rm, Rn, width)

/**
 * Creates a RADDHN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_raddhn_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_raddhn, Rd, Rm, Rn, width)

/**
 * Creates a RADDHN2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_raddhn2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_raddhn2, Rd, Rm, Rn, width)

/**
 * Creates a UABAL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uabal_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uabal, Rd, Rm, Rn, width)

/**
 * Creates a UABAL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uabal2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uabal2, Rd, Rm, Rn, width)

/**
 * Creates a RSUBHN vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_rsubhn_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_rsubhn, Rd, Rm, Rn, width)

/**
 * Creates a RSUBHN2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_rsubhn2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_rsubhn2, Rd, Rm, Rn, width)

/**
 * Creates a UABDL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uabdl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uabdl, Rd, Rm, Rn, width)

/**
 * Creates a UABDL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_uabdl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_uabdl2, Rd, Rm, Rn, width)

/**
 * Creates a UMLAL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umlal_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_umlal, Rd, Rd, Rm, Rn, width)

/**
 * Creates a UMLAL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umlal2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_umlal2, Rd, Rd, Rm, Rn, width)

/**
 * Creates a UMLSL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umlsl_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_umlsl, Rd, Rd, Rm, Rn, width)

/**
 * Creates a UMLSL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umlsl2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_4src(dc, OP_umlsl2, Rd, Rd, Rm, Rn, width)

/**
 * Creates a UMULL vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umull_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_umull, Rd, Rm, Rn, width)

/**
 * Creates a UMULL2 vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output register.
 * \param Rm      The first input register.
 * \param Rn      The second input register.
 * \param width   The input vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF() or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_umull2_vector(dc, Rd, Rm, Rn, width) \
    instr_create_1dst_3src(dc, OP_umull2, Rd, Rm, Rn, width)

/**
 * Creates an FMOV immediate to vector floating point move instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output vector register.
 * \param f       The source immediate floating point opnd.
 * \param width   The output vector element width. Use either OPND_CREATE_HALF()
 *                or OPND_CREATE_SINGLE().
 */
#define INSTR_CREATE_fmov_vector_imm(dc, Rd, f, width) \
    instr_create_1dst_2src(dc, OP_fmov, Rd, f, width)

/**
 * Creates an FMOV immediate to scalar floating point move instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      The output scalar register.
 * \param f       The source immediate floating point opnd.
 */
#define INSTR_CREATE_fmov_scalar_imm(dc, Rd, f) instr_create_1dst_1src(dc, OP_fmov, Rd, f)

/**
 * Creates a LDLAR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDLAR   <Wt>, [<Xn|SP>]
 *    LDLAR   <Xt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register. Can be W (Word, 32 bits) or X (Extended, 64
 * bits) \param Rn   The second source register. Can be X (Extended, 64 bits)
 */
#define INSTR_CREATE_ldlar(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_ldlar, Rt, Rn)

/**
 * Creates a LDLARB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDLARB  <Wt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register, W (Word, 32 bits)
 * \param Rn   The second source register, X (Extended, 64 bits)
 */
#define INSTR_CREATE_ldlarb(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_ldlarb, Rt, Rn)

/**
 * Creates a LDLARH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDLARH  <Wt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register, W (Word, 32 bits)
 * \param Rn   The second source register, X (Extended, 64 bits)
 */
#define INSTR_CREATE_ldlarh(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_ldlarh, Rt, Rn)

/**
 * Creates a STLLR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    STLLR   <Wt>, [<Xn|SP>]
 *    STLLR   <Xt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register. Can be W (Word, 32 bits) or X (Extended, 64
 * bits) \param Rn   The second source register. Can be X (Extended, 64 bits)
 */
#define INSTR_CREATE_stllr(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_stllr, Rt, Rn)

/**
 * Creates a STLLRB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    STLLRB  <Wt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register, W (Word, 32 bits)
 * \param Rn   The second source register, X (Extended, 64 bits)
 */
#define INSTR_CREATE_stllrb(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_stllrb, Rt, Rn)

/**
 * Creates a STLLRH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    STLLRH  <Wt>, [<Xn|SP>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The first destination register, W (Word, 32 bits)
 * \param Rn   The second source register, X (Extended, 64 bits)
 */
#define INSTR_CREATE_stllrh(dc, Rt, Rn) instr_create_1dst_1src(dc, OP_stllrh, Rt, Rn)

/**
 * Creates a LDAPR load-acquire instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDAPR <Wt>, [<Xn|SP> {,#0}]
 *    LDAPR <Xt>, [<Xn|SP> {,#0}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The destination register, W (Word, 32 bits) or X (Extended, 64 bits).
 * \param mem  The source memory address operand.
 */
#define INSTR_CREATE_ldapr(dc, Rt, mem) \
    instr_create_1dst_1src((dc), OP_ldapr, (Rt), (mem))

/**
 * Creates a LDAPRB load-acquire byte instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDAPRB <Wt>, [<Xn|SP> {,#0}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The destination register, W (Word, 32 bits).
 * \param mem  The source memory address operand.
 */
#define INSTR_CREATE_ldaprb(dc, Rt, mem) \
    instr_create_1dst_1src((dc), OP_ldaprb, (Rt), (mem))

/**
 * Creates a LDAPRH load-acquire half-word instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDAPRH <Wt>, [<Xn|SP> {,#0}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rt   The destination register, W (Word, 32 bits).
 * \param mem  The source memory address operand.
 */
#define INSTR_CREATE_ldaprh(dc, Rt, mem) \
    instr_create_1dst_1src((dc), OP_ldaprh, (Rt), (mem))

/**
 * Creates a SM3PARTW1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3PARTW1 <Sd>.4S, <Sn>.4S, <Sm>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3partw1_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_sm3partw1, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a SM3PARTW2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3PARTW2 <Sd>.4S, <Sn>.4S, <Sm>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3partw2_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_sm3partw2, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a SM3SS1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3SS1  <Sd>.4S, <Sn>.4S, <Sm>.4S, <Sa>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Ra   The fourth source vector register, Q (quadword, 128 bits)
 * \param Ra_elsz   The element size for Ra, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3ss1_vector(dc, Rd, Rn, Rm, Ra, Ra_elsz) \
    instr_create_1dst_4src(dc, OP_sm3ss1, Rd, Rn, Rm, Ra, Ra_elsz)

/**
 * Creates a SM3TT1A instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3TT1A <Sd>.4S, <Sn>.4S, <Sm>.S[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param imm2   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3tt1a_vector_indexed(dc, Rd, Rn, Rm, imm2, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sm3tt1a, Rd, Rn, Rm, imm2, Rm_elsz)

/**
 * Creates a SM3TT1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3TT1B <Sd>.4S, <Sn>.4S, <Sm>.S[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param imm2   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3tt1b_vector_indexed(dc, Rd, Rn, Rm, imm2, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sm3tt1b, Rd, Rn, Rm, imm2, Rm_elsz)

/**
 * Creates a SM3TT2A instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3TT2A <Sd>.4S, <Sn>.4S, <Sm>.S[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param imm2   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3tt2a_vector_indexed(dc, Rd, Rn, Rm, imm2, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sm3tt2a, Rd, Rn, Rm, imm2, Rm_elsz)

/**
 * Creates a SM3TT2B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM3TT2B <Sd>.4S, <Sn>.4S, <Sm>.S[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param imm2   The immediate index for Rm
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm3tt2b_vector_indexed(dc, Rd, Rn, Rm, imm2, Rm_elsz) \
    instr_create_1dst_4src(dc, OP_sm3tt2b, Rd, Rn, Rm, imm2, Rm_elsz)

/**
 * Creates a SM4E instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM4E    <Sd>.4S, <Sn>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rn_elsz   The element size for Rn, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm4e_vector(dc, Rd, Rn, Rn_elsz) \
    instr_create_1dst_2src(dc, OP_sm4e, Rd, Rn, Rn_elsz)

/**
 * Creates a SM4EKEY instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SM4EKEY <Sd>.4S, <Sn>.4S, <Sm>.4S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Rm_elsz   The element size for Rm, OPND_CREATE_SINGLE()
 */
#define INSTR_CREATE_sm4ekey_vector(dc, Rd, Rn, Rm, Rm_elsz) \
    instr_create_1dst_3src(dc, OP_sm4ekey, Rd, Rn, Rm, Rm_elsz)

/**
 * Creates a BCAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BCAX    <Bd>.16B, <Bn>.16B, <Bm>.16B, <Ba>.16B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Ra   The fourth source vector register, Q (quadword, 128 bits)
 */
#define INSTR_CREATE_bcax(dc, Rd, Rn, Rm, Ra) \
    instr_create_1dst_4src(dc, OP_bcax, Rd, Rn, Rm, Ra, OPND_CREATE_BYTE())

/**
 * Creates a EOR3 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EOR3    <Bd>.16B, <Bn>.16B, <Bm>.16B, <Ba>.16B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination vector register, Q (quadword, 128 bits)
 * \param Rn   The second source vector register, Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param Ra   The fourth source vector register, Q (quadword, 128 bits)
 */
#define INSTR_CREATE_eor3(dc, Rd, Rn, Rm, Ra) \
    instr_create_1dst_4src(dc, OP_eor3, Rd, Rn, Rm, Ra, OPND_CREATE_BYTE())

/**
 * Creates a ESB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ESB
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 */
#define INSTR_CREATE_esb(dc) instr_create_0dst_0src(dc, OP_esb)

/**
 * Creates a PSB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PSB CSYNC
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 */
#define INSTR_CREATE_psb_csync(dc) instr_create_0dst_0src(dc, OP_psb)

/**
 * Creates a FCCMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCCMP   <Dn>, <Dm>, #<imm>, <cond>
 *    FCCMP   <Hn>, <Hm>, #<imm>, <cond>
 *    FCCMP   <Sn>, <Sm>, #<imm>, <cond>
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn    The first source register. Can be D (doubleword, 64 bits),
 *              H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rm    The second source register. Can be D (doubleword, 64 bits),
 *              H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param nzcv  The 4 bit NZCV flags value used if the input condition is false.
 *              (use #opnd_create_immed_uint() to create the operand, e.g.
 *              opnd_create_immed_uint(val, #OPSZ_4b)).
 * \param condition_code   The comparison condition specified by #dr_pred_type_t,
 *              e.g. #DR_PRED_EQ.
 */
#define INSTR_CREATE_fccmp(dc, Rn, Rm, nzcv, condition_code) \
    INSTR_PRED(instr_create_0dst_3src(dc, OP_fccmp, Rn, Rm, nzcv), (condition_code))

/**
 * Creates a FCCMPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCCMPE   <Dn>, <Dm>, #<imm>, <cond>
 *    FCCMPE   <Hn>, <Hm>, #<imm>, <cond>
 *    FCCMPE   <Sn>, <Sm>, #<imm>, <cond>
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn    The first source register. Can be D (doubleword, 64 bits),
 *              H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rm    The second source register. Can be D (doubleword, 64 bits),
 *              H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param nzcv  The 4 bit NZCV flags value used if the input condition is false.
 *              (use #opnd_create_immed_uint() to create the operand, e.g.
 *              opnd_create_immed_uint(val, #OPSZ_4b)).
 * \param condition_code   The comparison condition specified by #dr_pred_type_t,
 *              e.g. #DR_PRED_EQ.
 */
#define INSTR_CREATE_fccmpe(dc, Rn, Rm, nzcv, condition_code) \
    INSTR_PRED(instr_create_0dst_3src(dc, OP_fccmpe, Rn, Rm, nzcv), (condition_code))

/**
 * Creates a FCSEL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCSEL   <Dd>, <Dn>, <Dm>, <cond>
 *    FCSEL   <Hd>, <Hn>, <Hm>, <cond>
 *    FCSEL   <Sd>, <Sn>, <Sm>, <cond>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first destination register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rn   The second source register. Can be D (doubleword, 64 bits),
               H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rm   The third source register. Can be D (doubleword, 64 bits),
               H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param condition_code   The comparison condition specified by #dr_pred_type_t,
 *                         e.g. #DR_PRED_EQ.
 */
#define INSTR_CREATE_fcsel(dc, Rd, Rn, Rm, condition_code) \
    INSTR_PRED(instr_create_1dst_2src(dc, OP_fcsel, Rd, Rn, Rm), (condition_code))

/**
 * Creates a FCMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMP    <Dn>, #0.0
 *    FCMP    <Hn>, #0.0
 *    FCMP    <Sn>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 */
#define INSTR_CREATE_fcmp_zero(dc, Rn) \
    instr_create_0dst_2src(dc, OP_fcmp, Rn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMP    <Dn>, <Dm>
 *    FCMP    <Hn>, <Hm>
 *    FCMP    <Sn>, <Sm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rm   The second source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 */
#define INSTR_CREATE_fcmp(dc, Rn, Rm) instr_create_0dst_2src(dc, OP_fcmp, Rn, Rm)

/**
 * Creates a FCMPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMPE   <Dn>, #0.0
 *    FCMPE   <Hn>, #0.0
 *    FCMPE   <Sn>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 */
#define INSTR_CREATE_fcmpe_zero(dc, Rn) \
    instr_create_0dst_2src(dc, OP_fcmpe, Rn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMPE   <Dn>, <Dm>
 *    FCMPE   <Hn>, <Hm>
 *    FCMPE   <Sn>, <Sm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 * \param Rm   The second source register. Can be D (doubleword, 64 bits),
 *             H (halfword, 16 bits) or S (singleword, 32 bits)
 */
#define INSTR_CREATE_fcmpe(dc, Rn, Rm) instr_create_0dst_2src(dc, OP_fcmpe, Rn, Rm)

/**
 * Creates a SDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SDOT    <Sd>.<Ts>, <Bn>.<Tb>, <Bm>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 */
#define INSTR_CREATE_sdot_vector(dc, Rd, Rn, Rm) \
    instr_create_1dst_4src(dc, OP_sdot, Rd, Rd, Rn, Rm, OPND_CREATE_BYTE())

/**
 * Creates a SDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SDOT    <Sd>.<Ts>, <Bn>.<Tb>, <Bm>.4B[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 */
#define INSTR_CREATE_sdot_vector_indexed(dc, Rd, Rn, Rm, index) \
    instr_create_1dst_5src(dc, OP_sdot, Rd, Rd, Rn, Rm, index, OPND_CREATE_BYTE())

/**
 * Creates a UDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UDOT    <Sd>.<Ts>, <Bn>.<Tb>, <Bm>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 */
#define INSTR_CREATE_udot_vector(dc, Rd, Rn, Rm) \
    instr_create_1dst_4src(dc, OP_udot, Rd, Rd, Rn, Rm, OPND_CREATE_BYTE())

/**
 * Creates a UDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UDOT    <Sd>.<Ts>, <Bn>.<Tb>, <Bm>.4B[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The first source and destination vector register,
 *             D (doubleword, 64 bits) or Q (quadword, 128 bits)
 * \param Rn   The second source vector register, D (doubleword, 64 bits) or
 *             Q (quadword, 128 bits)
 * \param Rm   The third source vector register, Q (quadword, 128 bits)
 * \param index   The immediate index for Rm
 */
#define INSTR_CREATE_udot_vector_indexed(dc, Rd, Rn, Rm, index) \
    instr_create_1dst_5src(dc, OP_udot, Rd, Rd, Rn, Rm, index, OPND_CREATE_BYTE())

/**
 * Creates an XPACI instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Rd      Register with PAC bits to remove.
 */
#define INSTR_CREATE_xpaci(dc, Rd) instr_create_0dst_1src((dc), OP_xpaci, (Rd))

/****************************************************************************
 *                              SVE Instructions                            *
 ****************************************************************************/

/* -------- SVE bitwise logical operations (predicated) ---------------- */

/**
 * Creates an ORR scalable vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Zd      The output SVE vector register.
 * \param Pg      Predicate register for predicated instruction, P0-P7.
 * \param Zd_     The first input SVE vector register. Must match Zd.
 * \param Zm      The second input SVE vector register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_orr_sve_pred(dc, Zd, Pg, Zd_, Zm, width) \
    instr_create_1dst_4src(dc, OP_orr, Zd, Pg, Zd_, Zm, width)

/**
 * Creates an EOR scalable vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Zd      The output SVE vector register.
 * \param Pg      Predicate register for predicated instruction, P0-P7.
 * \param Zd_     The first input SVE vector register. Must match Zd.
 * \param Zm      The second input SVE vector register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_eor_sve_pred(dc, Zd, Pg, Zd_, Zm, width) \
    instr_create_1dst_4src(dc, OP_eor, Zd, Pg, Zd_, Zm, width)

/**
 * Creates an AND scalable vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Zd      The output SVE vector register.
 * \param Pg      Predicate register for predicated instruction, P0-P7.
 * \param Zd_     The first input SVE vector register. Must match Zd.
 * \param Zm      The second input SVE vector register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_and_sve_pred(dc, Zd, Pg, Zd_, Zm, width) \
    instr_create_1dst_4src(dc, OP_and, Zd, Pg, Zd_, Zm, width)

/**
 * Creates a BIC scalable vector instruction.
 * \param dc      The void * dcontext used to allocate memory for the instr_t.
 * \param Zd      The output SVE vector register.
 * \param Pg      Predicate register for predicated instruction, P0-P7.
 * \param Zd_     The first input SVE vector register. Must match Zd.
 * \param Zm      The second input SVE vector register.
 * \param width   The vector element width. Use either OPND_CREATE_BYTE(),
 *                OPND_CREATE_HALF(), OPND_CREATE_SINGLE() or OPND_CREATE_DOUBLE().
 */
#define INSTR_CREATE_bic_sve_pred(dc, Zd, Pg, Zd_, Zm, width) \
    instr_create_1dst_4src(dc, OP_bic, Zd, Pg, Zd_, Zm, width)

/**
 * Creates a ZIP2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ZIP2    <Zd>.Q, <Zn>.Q, <Zm>.Q
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The second source vector register, Z (Scalable)
 * \param Zm   The third source vector register, Z (Scalable)
 */
#define INSTR_CREATE_zip2_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_zip2, Zd, Zn, Zm)

/**
 * Creates a MOVPRFX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MOVPRFX <Zd>, <Zn>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_movprfx_vector(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_movprfx, Zd, Zn)

/**
 * Creates a MOVPRFX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MOVPRFX <Zd>.<Ts>, <Pg>/<ZM>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_movprfx_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_movprfx, Zd, Pg, Zn)

/**
 * Creates a SQADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQADD   <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_sqadd_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_sqadd, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a SQADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQADD   <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sqadd_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_sqadd, Zd, Zn, Zm)

/**
 * Creates a SQSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQSUB   <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_sqsub_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_sqsub, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a SQSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQSUB   <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sqsub_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_sqsub, Zd, Zn, Zm)

/**
 * Creates a SUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUB     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and first destination vector register, Z (Scalable)
 * \param Pg   The first source vector register, Z (Scalable)
 * \param Zm   The third source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sub_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_sub, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUB     <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_sub_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_sub, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a SUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUB     <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sub_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_sub, Zd, Zn, Zm)

/**
 * Creates a SUBR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUBR    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and first destination vector register, Z (Scalable)
 * \param Pg   The first source vector register, Z (Scalable)
 * \param Zm   The third source vector register, Z (Scalable)
 */
#define INSTR_CREATE_subr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_subr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SUBR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUBR    <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_subr_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_subr, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a UQADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQADD   <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_uqadd_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_uqadd, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a UQADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQADD   <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uqadd_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_uqadd, Zd, Zn, Zm)

/**
 * Creates a UQSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQSUB   <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm8
 */
#define INSTR_CREATE_uqsub_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_uqsub, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a UQSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQSUB   <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uqsub_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_uqsub, Zd, Zn, Zm)

/**
 * Creates a ADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ADD     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_add_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_add, Zdn, Pg, Zdn, Zm)

/**
 * Creates a ADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ADD     <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and first destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 * \param shift   The immediate shiftOp for imm
 */
#define INSTR_CREATE_add_sve_shift(dc, Zdn, imm, shift) \
    instr_create_1dst_4src(dc, OP_add, Zdn, Zdn, imm, OPND_CREATE_LSL(), shift)

/**
 * Creates a ADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ADD     <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_add_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_add, Zd, Zn, Zm)

/**
 * Creates a CPY instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CPY     <Zd>.<Ts>, <Pg>/Z, #<simm>, <shift>
 *    CPY     <Zd>.<Ts>, <Pg>/M, #<simm>, <shift>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param simm   The signed immediate imm
 * \param shift   The immediate shiftOp for simm
 */
#define INSTR_CREATE_cpy_sve_shift_pred(dc, Zd, Pg, simm, shift) \
    instr_create_1dst_4src(dc, OP_cpy, Zd, Pg, simm, OPND_CREATE_LSL(), shift)

/**
 * Creates a CPY instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CPY     <Zd>.<T>, <Pg>/M, <R><n|SP>
 *    CPY     <Zd>.<T>, <Pg>/M, <V><n>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The first destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Rn_or_Vn The source register. Can be a general purpose register
 *                 W (Word, 32 bits) or X (Extended, 64 bits),
 *                 or a vector register B (Byte, 8 bits), H (Halfword, 16 bits),
 *                 S (Singleword, 32 bits), or D (Doubleword, 64 bits).
 */
#define INSTR_CREATE_cpy_sve_pred(dc, Zd, Pg, Rn_or_Vn) \
    instr_create_1dst_2src(dc, OP_cpy, Zd, Pg, Rn_or_Vn)

/**
 * Creates a PTEST instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PTEST   <Pg>, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Pn   The first source predicate register, P (Predicate)
 */
#define INSTR_CREATE_ptest_sve_pred(dc, Pg, Pn) \
    instr_create_0dst_2src(dc, OP_ptest, Pg, Pn)

/**
 * Creates a MAD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MAD     <Zdn>.<Ts>, <Pg>/M, <Zm>.<Ts>, <Za>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 * \param Za   The third source vector register, Z (Scalable)
 */
#define INSTR_CREATE_mad_sve_pred(dc, Zdn, Pg, Zm, Za) \
    instr_create_1dst_4src(dc, OP_mad, Zdn, Pg, Zdn, Zm, Za)

/**
 * Creates a MLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MLA     <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The third source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_mla_sve_pred(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_mla, Zda, Pg, Zn, Zm, Zda)

/**
 * Creates a MLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MLS     <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The third source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_mls_sve_pred(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_mls, Zda, Pg, Zn, Zm, Zda)

/**
 * Creates a MSB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MSB     <Zdn>.<Ts>, <Pg>/M, <Zm>.<Ts>, <Za>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 * \param Za   The third source vector register, Z (Scalable)
 */
#define INSTR_CREATE_msb_sve_pred(dc, Zdn, Pg, Zm, Za) \
    instr_create_1dst_4src(dc, OP_msb, Zdn, Pg, Zdn, Zm, Za)

/**
 * Creates a MUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MUL     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_mul_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_mul, Zdn, Pg, Zdn, Zm)

/**
 * Creates a MUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MUL     <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_mul_sve(dc, Zdn, simm) \
    instr_create_1dst_2src(dc, OP_mul, Zdn, Zdn, simm)

/**
 * Creates a SMULH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMULH   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_smulh_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_smulh, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UMULH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMULH   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_umulh_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_umulh, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FEXPA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FEXPA   <Zd>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fexpa_sve(dc, Zd, Zn) instr_create_1dst_1src(dc, OP_fexpa, Zd, Zn)

/**
 * Creates a FTMAD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FTMAD   <Zdn>.<Ts>, <Zdn>.<Ts>, <Zm>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_ftmad_sve(dc, Zdn, Zm, imm) \
    instr_create_1dst_3src(dc, OP_ftmad, Zdn, Zdn, Zm, imm)

/**
 * Creates a FTSMUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FTSMUL  <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_ftsmul_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_ftsmul, Zd, Zn, Zm)

/**
 * Creates a FTSSEL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FTSSEL  <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_ftssel_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_ftssel, Zd, Zn, Zm)

/**
 * Creates an ABS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ABS     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_abs_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_abs, Zd, Pg, Zn)

/**
 * Creates a CNOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNOT    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cnot_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_cnot, Zd, Pg, Zn)

/**
 * Creates a NEG instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NEG     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_neg_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_neg, Zd, Pg, Zn)

/**
 * Creates a SABD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SABD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sabd_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_sabd, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMAX    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_smax_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_smax, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMAX    <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_smax_sve(dc, Zdn, simm) \
    instr_create_1dst_2src(dc, OP_smax, Zdn, Zdn, simm)

/**
 * Creates a SMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMIN    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_smin_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_smin, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMIN    <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_smin_sve(dc, Zdn, simm) \
    instr_create_1dst_2src(dc, OP_smin, Zdn, Zdn, simm)

/**
 * Creates an UABD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UABD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uabd_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_uabd, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FACGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_facge_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_facge, Pd, Pg, Zn, Zm)

/**
 * Creates a FACGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FACGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_facgt_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_facgt, Pd, Pg, Zn, Zm)

/**
 * Creates a SDIV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SDIV    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sdiv_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_sdiv, Zdn, Pg, Zdn, Zm)

/**
 * Creates a SDIVR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SDIVR   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sdivr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_sdivr, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UDIV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UDIV    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_udiv_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_udiv, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UDIVR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UDIVR   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_udivr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_udivr, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMAX    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_umax_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_umax, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMAX    <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_umax_sve(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_umax, Zdn, Zdn, imm)

/**
 * Creates an UMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMIN    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_umin_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_umin, Zdn, Pg, Zdn, Zm)

/**
 * Creates an UMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMIN    <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_umin_sve(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_umin, Zdn, Zdn, imm)

/**
 * Creates a SXTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SXTB    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sxtb_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_sxtb, Zd, Pg, Zn)

/**
 * Creates a SXTH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SXTH    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sxth_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_sxth, Zd, Pg, Zn)

/**
 * Creates a SXTW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SXTW    <Zd>.D, <Pg>/M, <Zn>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_sxtw_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_sxtw, Zd, Pg, Zn)

/**
 * Creates an UXTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UXTB    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uxtb_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_uxtb, Zd, Pg, Zn)

/**
 * Creates an UXTH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UXTH    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uxth_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_uxth, Zd, Pg, Zn)

/**
 * Creates an UXTW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UXTW    <Zd>.D, <Pg>/M, <Zn>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 */
#define INSTR_CREATE_uxtw_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_uxtw, Zd, Pg, Zn)

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmeq_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmeq, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMEQ   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmeq_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_fcmeq, Pd, Pg, Zn, Zm)

/**
 * Creates a FCMGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmge_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmge, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmge_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_fcmge, Pd, Pg, Zn, Zm)

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmgt_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmgt, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmgt_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_fcmgt, Pd, Pg, Zn, Zm)

/**
 * Creates a FCMLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmle_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmle, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmlt_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmlt, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMNE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMNE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #0.0
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmne_sve_zero_pred(dc, Pd, Pg, Zn) \
    instr_create_1dst_3src(dc, OP_fcmne, Pd, Pg, Zn, opnd_create_immed_float(0.0))

/**
 * Creates a FCMNE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMNE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmne_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_fcmne, Pd, Pg, Zn, Zm)

/**
 * Creates a FCMUO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMUO   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmuo_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_fcmuo, Pd, Pg, Zn, Zm)

/**
 * Creates a FCMLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLE   <Pd>.<Ts>, <Pg>/Z, <Zm>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The first source vector register, Z (Scalable)
 * \param Zn   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmle_sve_pred(dc, Pd, Pg, Zm, Zn) \
    instr_create_1dst_3src(dc, OP_fcmle, Pd, Pg, Zm, Zn)

/**
 * Creates a FCMLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLT   <Pd>.<Ts>, <Pg>/Z, <Zm>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zm   The first source vector register, Z (Scalable)
 * \param Zn   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_fcmlt_sve_pred(dc, Pd, Pg, Zm, Zn) \
    instr_create_1dst_3src(dc, OP_fcmlt, Pd, Pg, Zm, Zn)

/**
 * Creates a CMPEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPEQ   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmpeq_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmpeq, Pd, Pg, Zn, simm)

/**
 * Creates a CMPEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPEQ   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPEQ   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmpeq_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmpeq, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmpge_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmpge, Pd, Pg, Zn, simm)

/**
 * Creates a CMPGE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPGE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmpge_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmpge, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmpgt_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmpgt, Pd, Pg, Zn, simm)

/**
 * Creates a CMPGT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPGT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmpgt_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmpgt, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPHI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPHI   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_cmphi_sve_pred_imm(dc, Pd, Pg, Zn, imm) \
    instr_create_1dst_3src(dc, OP_cmphi, Pd, Pg, Zn, imm)

/**
 * Creates a CMPHI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPHI   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPHI   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmphi_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmphi, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPHS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPHS   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_cmphs_sve_pred_imm(dc, Pd, Pg, Zn, imm) \
    instr_create_1dst_3src(dc, OP_cmphs, Pd, Pg, Zn, imm)

/**
 * Creates a CMPHS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPHS   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPHS   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmphs_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmphs, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmple_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmple, Pd, Pg, Zn, simm)

/**
 * Creates a CMPLE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmple_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmple, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPLO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLO   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_cmplo_sve_pred_imm(dc, Pd, Pg, Zn, imm) \
    instr_create_1dst_3src(dc, OP_cmplo, Pd, Pg, Zn, imm)

/**
 * Creates a CMPLO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLO   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmplo_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmplo, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLS   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param imm   The immediate imm
 */
#define INSTR_CREATE_cmpls_sve_pred_imm(dc, Pd, Pg, Zn, imm) \
    instr_create_1dst_3src(dc, OP_cmpls, Pd, Pg, Zn, imm)

/**
 * Creates a CMPLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLS   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmpls_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmpls, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmplt_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmplt, Pd, Pg, Zn, simm)

/**
 * Creates a CMPLT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPLT   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmplt_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmplt, Pd, Pg, Zn, Zm)

/**
 * Creates a CMPNE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPNE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The source vector register, Z (Scalable)
 * \param simm   The signed immediate imm
 */
#define INSTR_CREATE_cmpne_sve_pred_simm(dc, Pd, Pg, Zn, simm) \
    instr_create_1dst_3src(dc, OP_cmpne, Pd, Pg, Zn, simm)

/**
 * Creates a CMPNE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CMPNE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.D
 *    CMPNE   <Pd>.<Ts>, <Pg>/Z, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 * \param Zn   The first source vector register, Z (Scalable)
 * \param Zm   The second source vector register, Z (Scalable)
 */
#define INSTR_CREATE_cmpne_sve_pred(dc, Pd, Pg, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_cmpne, Pd, Pg, Zn, Zm)

/**
 * Creates a SETFFR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SETFFR
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 */
#define INSTR_CREATE_setffr_sve(dc) instr_create_0dst_0src(dc, OP_setffr)

/**
 * Creates a RDFFR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RDFFR   <Pd>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 */
#define INSTR_CREATE_rdffr_sve(dc, Pd) instr_create_1dst_0src(dc, OP_rdffr, Pd)

/**
 * Creates a RDFFR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RDFFR   <Pd>.B, <Pg>/Z
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 */
#define INSTR_CREATE_rdffr_sve_pred(dc, Pd, Pg) \
    instr_create_1dst_1src(dc, OP_rdffr, Pd, Pg)

/**
 * Creates a RDFFRS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RDFFRS  <Pd>.B, <Pg>/Z
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate)
 * \param Pg   The governing predicate register, P (Predicate)
 */
#define INSTR_CREATE_rdffrs_sve_pred(dc, Pd, Pg) \
    instr_create_1dst_1src(dc, OP_rdffrs, Pd, Pg)

/**
 * Creates a WRFFR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    WRFFR   <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pn   The source predicate register, P (Predicate)
 */
#define INSTR_CREATE_wrffr_sve(dc, Pn) instr_create_0dst_1src(dc, OP_wrffr, Pn)

/**
 * Creates a CNTP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNTP    <Xd>, <Pg>, <Pn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination register, X (Extended, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_cntp_sve_pred(dc, Rd, Pg, Pn) \
    instr_create_1dst_2src(dc, OP_cntp, Rd, Pg, Pn)

/**
 * Creates a DECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECP    <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register, X (Extended, 64 bits).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_decp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_decp, Rdn, Pm, Rdn)

/**
 * Creates a DECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECP    <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_decp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_decp, Zdn, Pm, Zdn)

/**
 * Creates an INCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCP    <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register, X (Extended, 64 bits).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_incp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_incp, Rdn, Pm, Rdn)

/**
 * Creates an INCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCP    <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_incp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_incp, Zdn, Pm, Zdn)

/**
 * Creates a SQDECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECP  <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The source and destination register, X (Extended, 64 bits).
 * \param Pm   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqdecp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_sqdecp, Rdn, Pm, Rdn)

/**
 * Creates a SQDECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECP  <Xdn>, <Pm>.<Ts>, <Wdn>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register, X (Extended, 64 bits).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqdecp_sve_wide(dc, Rdn, Pm)  \
    instr_create_1dst_2src(dc, OP_sqdecp, Rdn, Pm, \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0))

/**
 * Creates a SQDECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECP  <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqdecp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_sqdecp, Zdn, Pm, Zdn)

/**
 * Creates a SQINCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCP  <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register, X (Extended, 64 bits).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqincp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_sqincp, Rdn, Pm, Rdn)

/**
 * Creates a SQINCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCP  <Xdn>, <Pm>.<Ts>, <Wdn>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register, X (Extended, 64 bits).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqincp_sve_wide(dc, Rdn, Pm)  \
    instr_create_1dst_2src(dc, OP_sqincp, Rdn, Pm, \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0))

/**
 * Creates a SQINCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCP  <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sqincp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_sqincp, Zdn, Pm, Zdn)

/**
 * Creates an UQDECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECP  <Wdn>, <Pm>.<Ts>
 *    UQDECP  <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register. Can be X (Extended, 64 bits)
 * or W (Word, 32 bits). \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uqdecp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_uqdecp, Rdn, Pm, Rdn)

/**
 * Creates an UQDECP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECP  <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uqdecp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_uqdecp, Zdn, Pm, Zdn)

/**
 * Creates an UQINCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCP  <Wdn>, <Pm>.<Ts>
 *    UQINCP  <Xdn>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The second source and destination register. Can be X (Extended, 64 bits)
 * or W (Word, 32 bits). \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uqincp_sve(dc, Rdn, Pm) \
    instr_create_1dst_2src(dc, OP_uqincp, Rdn, Pm, Rdn)

/**
 * Creates an UQINCP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCP  <Zdn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Pm   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uqincp_sve_vector(dc, Zdn, Pm) \
    instr_create_1dst_2src(dc, OP_uqincp, Zdn, Pm, Zdn)

/**
 * Creates an AND instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    AND     <Zdn>.<T>, <Zdn>.<T>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_and_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_and, Zdn, Zdn, imm)

/**
 * Creates a BIC instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BIC     <Zdn>.<T>, <Zdn>.<T>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_bic_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_and, Zdn, Zdn, opnd_invert_immed_int(imm))

/**
 * Creates an EOR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EOR     <Zdn>.<T>, <Zdn>.<T>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_eor_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_eor, Zdn, Zdn, imm)

/**
 * Creates an ORR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORR     <Zdn>.<T>, <Zdn>.<T>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_orr_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_orr, Zdn, Zdn, imm)

/**
 * Creates an ORN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORN     <Zdn>.<T>, <Zdn>.<T>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_orn_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_orr, Zdn, Zdn, opnd_invert_immed_int(imm))

/**
 * Creates an AND instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    AND     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_and_sve_pred_b(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_and, Pd, Pg, Pn, Pm)

/**
 * Creates an AND instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    AND     <Zd>.D, <Zn>.D, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_and_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_and, Zd, Zn, Zm)

/**
 * Creates an ANDS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ANDS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_ands_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_ands, Pd, Pg, Pn, Pm)

/**
 * Creates a BIC instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BIC     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_bic_sve_pred_b(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_bic, Pd, Pg, Pn, Pm)

/**
 * Creates a BIC instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BIC     <Zd>.D, <Zn>.D, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bic_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_bic, Zd, Zn, Zm)

/**
 * Creates a BICS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BICS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_bics_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_bics, Pd, Pg, Pn, Pm)

/**
 * Creates an EOR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EOR     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_eor_sve_pred_b(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_eor, Pd, Pg, Pn, Pm)

/**
 * Creates an EOR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EOR     <Zd>.D, <Zn>.D, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_eor_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_eor, Zd, Zn, Zm)

/**
 * Creates an EORS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EORS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_eors_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_eors, Pd, Pg, Pn, Pm)

/**
 * Creates a NAND instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NAND    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_nand_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_nand, Pd, Pg, Pn, Pm)

/**
 * Creates a NANDS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NANDS   <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_nands_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_nands, Pd, Pg, Pn, Pm)

/**
 * Creates a NOR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NOR     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_nor_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_nor, Pd, Pg, Pn, Pm)

/**
 * Creates a NORS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NORS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_nors_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_nors, Pd, Pg, Pn, Pm)

/**
 * Creates a NOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    NOT     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_not_sve_pred_vec(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_not, Zd, Pg, Zn)

/**
 * Creates an ORN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORN     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_orn_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_orn, Pd, Pg, Pn, Pm)

/**
 * Creates an ORNS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORNS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_orns_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_orns, Pd, Pg, Pn, Pm)

/**
 * Creates an ORR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORR     <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_orr_sve_pred_b(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_orr, Pd, Pg, Pn, Pm)

/**
 * Creates an ORR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORR     <Zd>.D, <Zn>.D, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_orr_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_orr, Zd, Zn, Zm)

/**
 * Creates an ORRS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORRS    <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_orrs_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_orrs, Pd, Pg, Pn, Pm)

/**
 * Creates a CLASTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTA  <R><dn>, <Pg>, <R><dn>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn  The first source and destination register. Can be W (Word, 32 bits) or
 * X (Extended, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clasta_sve_scalar(dc, Rdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clasta, Rdn, Pg, Rdn, Zm)

/**
 * Creates a CLASTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTA  <V><dn>, <Pg>, <V><dn>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vdn  The first source and destination register. Can be D (doubleword, 64 bits),
 * S (singleword, 32 bits), H (halfword, 16 bits) or B (byte, 8 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clasta_sve_simd_fp(dc, Vdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clasta, Vdn, Pg, Vdn, Zm)

/**
 * Creates a CLASTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTA  <Zdn>.<Ts>, <Pg>, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clasta_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clasta, Zdn, Pg, Zdn, Zm)

/**
 * Creates a CLASTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTB  <R><dn>, <Pg>, <R><dn>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn  The first source and destination register. Can be W (Word, 32 bits) or
 * X (Extended, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clastb_sve_scalar(dc, Rdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clastb, Rdn, Pg, Rdn, Zm)

/**
 * Creates a CLASTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTB  <V><dn>, <Pg>, <V><dn>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vdn  The first source and destination register. Can be D (doubleword, 64 bits),
 * S (singleword, 32 bits), H (halfword, 16 bits) or B (byte, 8 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clastb_sve_simd_fp(dc, Vdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clastb, Vdn, Pg, Vdn, Zm)

/**
 * Creates a CLASTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLASTB  <Zdn>.<Ts>, <Pg>, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clastb_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_clastb, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LASTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LASTA   <R><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination vector register. Can be W (Word, 32 bits) or X (Extended,
 * 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lasta_sve_scalar(dc, Rd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_lasta, Rd, Pg, Zn)

/**
 * Creates a LASTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LASTA   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination register. Can be D (doubleword, 64 bits), B (byte, 8
 * bits), S (singleword, 32 bits) or H (halfword, 16 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lasta_sve_simd_fp(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_lasta, Vd, Pg, Zn)

/**
 * Creates a LASTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LASTB   <R><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination vector register. Can be W (Word, 32 bits) or X (Extended,
 * 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lastb_sve_scalar(dc, Rd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_lastb, Rd, Pg, Zn)

/**
 * Creates a LASTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LASTB   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination register. Can be D (doubleword, 64 bits), B (byte, 8
 * bits), S (singleword, 32 bits) or H (halfword, 16 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lastb_sve_simd_fp(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_lastb, Vd, Pg, Zn)

/**
 * Creates a CNT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNT     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_cnt_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_cnt, Zd, Pg, Zn)

/**
 * Creates a CNTB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNTB    <Xd>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination register, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_cntb(dc, Rd, pattern, imm) \
    instr_create_1dst_3src(dc, OP_cntb, Rd, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a CNTD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNTD    <Xd>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination register, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_cntd(dc, Rd, pattern, imm) \
    instr_create_1dst_3src(dc, OP_cntd, Rd, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a CNTH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNTH    <Xd>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination register, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_cnth(dc, Rd, pattern, imm) \
    instr_create_1dst_3src(dc, OP_cnth, Rd, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a CNTW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNTW    <Xd>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination register, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_cntw(dc, Rd, pattern, imm) \
    instr_create_1dst_3src(dc, OP_cntw, Rd, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECB    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The GPR register to be decremented, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_decb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_decb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECD    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The GPR register to be decremented, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_decd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_decd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECD    <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The vector register to be decremented, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_decd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_decd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECH    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The GPR register to be decremented, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_dech(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_dech, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECH    <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The vector register to be decremented, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_dech_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_dech, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECW    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The GPR register to be decremented, X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_decw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_decw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a DECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DECW    <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The vector register to be decremented, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_decw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_decw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCB    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_incb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_incb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCD    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_incd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_incd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCD    <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_incd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_incd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCH    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_inch(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_inch, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCH    <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_inch_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_inch, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCW    <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_incw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_incw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an INCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INCW    <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_incw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_incw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECB  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecb_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqdecb, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECB  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdecb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECD  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecd_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqdecd, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECD  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdecd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECD  <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdecd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECH  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdech_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqdech, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECH  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdech(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdech, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECH  <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdech_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdech, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECW  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecw_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqdecw, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECW  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdecw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQDECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQDECW  <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqdecw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqdecw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCB  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincb_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqincb, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCB  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqincb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCD  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincd_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqincd, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCD  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqincd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCD  <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqincd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCH  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqinch_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqinch, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCH  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqinch(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqinch, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCH  <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqinch_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqinch, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCW  <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits). The 32 bit result from the source
 *              register is sign extended to 64 bits.
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincw_wide(dc, Rdn, pattern, imm)                                \
    instr_create_1dst_4src(dc, OP_sqincw, Rdn,                                         \
                           opnd_create_reg(opnd_get_reg(Rdn) - DR_REG_X0 + DR_REG_W0), \
                           pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCW  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register, X (Extended, 64
 *              bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqincw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a SQINCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SQINCW  <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_sqincw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_sqincw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECB  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQDECB  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdecb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdecb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECD  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQDECD  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdecd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdecd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECD  <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdecd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdecd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECH  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQDECH  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdech(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdech, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECH  <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdech_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdech, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECW  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQDECW  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdecw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdecw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQDECW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQDECW  <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqdecw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqdecw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCB  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQINCB  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqincb(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqincb, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCD  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQINCD  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqincd(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqincd, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCD  <Zdn>.D{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqincd_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqincd, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCH  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQINCH  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqinch(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqinch, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCH  <Zdn>.H{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqinch_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqinch, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCW  <Wdn>{, <pattern>{, MUL #<imm>}}
 *    UQINCW  <Xdn>{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rdn   The first source and destination register. Can be W (Word, 32
 *              bits) or X (Extended, 64 bits).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqincw(dc, Rdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqincw, Rdn, Rdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates an UQINCW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UQINCW  <Zdn>.S{, <pattern>{, MUL #<imm>}}
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The source and destination vector register, Z (Scalable).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 * \param imm   The imm used as the predicate constraint multiplier.
 */
#define INSTR_CREATE_uqincw_sve(dc, Zdn, pattern, imm) \
    instr_create_1dst_4src(dc, OP_uqincw, Zdn, Zdn, pattern, OPND_CREATE_MUL(), imm)

/**
 * Creates a BRKA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKA    <Pd>.B, <Pg>/<ZM>, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brka_sve_pred(dc, Pd, Pg, Pn) \
    instr_create_1dst_2src(dc, OP_brka, Pd, Pg, Pn)

/**
 * Creates a BRKAS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKAS   <Pd>.B, <Pg>/Z, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkas_sve_pred(dc, Pd, Pg, Pn) \
    instr_create_1dst_2src(dc, OP_brkas, Pd, Pg, Pn)

/**
 * Creates a BRKB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKB    <Pd>.B, <Pg>/<ZM>, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkb_sve_pred(dc, Pd, Pg, Pn) \
    instr_create_1dst_2src(dc, OP_brkb, Pd, Pg, Pn)

/**
 * Creates a BRKBS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKBS   <Pd>.B, <Pg>/Z, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkbs_sve_pred(dc, Pd, Pg, Pn) \
    instr_create_1dst_2src(dc, OP_brkbs, Pd, Pg, Pn)

/**
 * Creates a BRKN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKN    <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pdm   The second source and destination predicate register, P
 *              (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkn_sve_pred(dc, Pdm, Pg, Pn) \
    instr_create_1dst_3src(dc, OP_brkn, Pdm, Pg, Pn, Pdm)

/**
 * Creates a BRKNS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKNS   <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pdm   The second source and destination predicate register, P
 *              (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkns_sve_pred(dc, Pdm, Pg, Pn) \
    instr_create_1dst_3src(dc, OP_brkns, Pdm, Pg, Pn, Pdm)

/**
 * Creates a BRKPA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKPA   <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkpa_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_brkpa, Pd, Pg, Pn, Pm)

/**
 * Creates a BRKPAS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKPAS  <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkpas_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_brkpas, Pd, Pg, Pn, Pm)

/**
 * Creates a BRKPB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKPB   <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkpb_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_brkpb, Pd, Pg, Pn, Pm)

/**
 * Creates a BRKPBS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BRKPBS  <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_brkpbs_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_brkpbs, Pd, Pg, Pn, Pm)

/**
 * Creates a WHILELE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    WHILELE <Pd>.<Ts>, <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Rn   The first source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 * \param Rm   The second source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 */
#define INSTR_CREATE_whilele_sve(dc, Pd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_whilele, Pd, Rn, Rm)

/**
 * Creates a WHILELO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    WHILELO <Pd>.<Ts>, <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Rn   The first source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 * \param Rm   The second source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 */
#define INSTR_CREATE_whilelo_sve(dc, Pd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_whilelo, Pd, Rn, Rm)

/**
 * Creates a WHILELS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    WHILELS <Pd>.<Ts>, <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Rn   The first source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 * \param Rm   The second source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 */
#define INSTR_CREATE_whilels_sve(dc, Pd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_whilels, Pd, Rn, Rm)

/**
 * Creates a WHILELT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    WHILELT <Pd>.<Ts>, <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Rn   The first source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 * \param Rm   The second source  register. Can be W (Word, 32 bits) or X
 *             (Extended, 64 bits).
 */
#define INSTR_CREATE_whilelt_sve(dc, Pd, Rn, Rm) \
    instr_create_1dst_2src(dc, OP_whilelt, Pd, Rn, Rm)

/**
 * Creates a TBL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    TBL     <Zd>.<Ts>, { <Zn>.<Ts> }, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_tbl_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_tbl, Zd, Zn, Zm)

/**
 * Creates a DUP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DUP     <Zd>.<Ts>, #<simm>, <shift>
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd    The destination vector register, Z (Scalable).
 * \param simm  The signed immediate imm.
 * \param shift Left shift to apply to the immediate, defaulting to LSL #0.  Can
 * be 0 or 8
 */
#define INSTR_CREATE_dup_sve_shift(dc, Zd, simm, shift) \
    instr_create_1dst_3src(dc, OP_dup, Zd, simm, OPND_CREATE_LSL(), shift)

/**
 * Creates a DUP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DUP     <Zd>.<Ts>, <Zn>.<Ts>[<index>]
 * \endverbatim
 * \param dc     The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd     The destination vector register, Z (Scalable).
 * \param Zn     The source vector register, Z (Scalable).
 * \param index  Immediate index of source vector.  The operand size must match the
 * encoded field size, which depends on the element size of Zd and Zn; OPSZ_6b for byte,
 * OPSZ_5b for halfword, OPSZ_4b for singleword, OPSZ_3b for doubleword, and OPSZ_2b for
 * quadword.
 */
#define INSTR_CREATE_dup_sve_idx(dc, Zd, Zn, index) \
    instr_create_1dst_2src(dc, OP_dup, Zd, Zn, index)

/**
 * Creates a DUP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DUP     <Zd>.<Ts>, <R><n|SP>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Rn   The source vector register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 */
#define INSTR_CREATE_dup_sve_scalar(dc, Zd, Rn) instr_create_1dst_1src(dc, OP_dup, Zd, Rn)

/**
 * Creates a INSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INSR    <Zdn>.<T>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The second source and destination vector register, Z (Scalable).
 * \param Rm   The source vector register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 */
#define INSTR_CREATE_insr_sve_scalar(dc, Zd, Rm) \
    instr_create_1dst_2src(dc, OP_insr, Zd, Zd, Rm)

/**
 * Creates an INSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INSR    <Zdn>.<Ts>, <V><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The second source and destination vector register, Z (Scalable).
 * \param Vm   The first source register. Can be S (singleword, 32 bits), B
 *             (byte, 8 bits), D (doubleword, 64 bits) or H
 *             (halfword, 16 bits).
 */
#define INSTR_CREATE_insr_sve_simd_fp(dc, Zdn, Vm) \
    instr_create_1dst_2src(dc, OP_insr, Zdn, Zdn, Vm)

/**
 * Creates an EXT instruction (destructive).
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EXT     <Zdn>.B, <Zdn>.B, <Zm>.B, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 * \param imm  The immediate imm.
 */
#define INSTR_CREATE_ext_sve(dc, Zdn, Zm, imm) \
    instr_create_1dst_3src(dc, OP_ext, Zdn, Zdn, Zm, imm)

/**
 * Creates a SPLICE instruction (destructive).
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SPLICE  <Zdn>.<Ts>, <Pv>, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The second source and destination vector register, Z (Scalable).
 * \param Pv   The first source predicate register, P (Predicate).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_splice_sve(dc, Zdn, Pv, Zm) \
    instr_create_1dst_3src(dc, OP_splice, Zdn, Pv, Zdn, Zm)

/**
 * Creates a REV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    REV     <Pd>.<Ts>, <Pn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_rev_sve_pred(dc, Pd, Pn) instr_create_1dst_1src(dc, OP_rev, Pd, Pn)

/**
 * Creates a REV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    REV     <Zd>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_rev_sve(dc, Zd, Zn) instr_create_1dst_1src(dc, OP_rev, Zd, Zn)

/**
 * Creates a REVB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    REVB    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_revb_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_revb, Zd, Pg, Zn)

/**
 * Creates a REVH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    REVH    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_revh_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_revh, Zd, Pg, Zn)

/**
 * Creates a REVW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    REVW    <Zd>.D, <Pg>/M, <Zn>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_revw_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_revw, Zd, Pg, Zn)

/**
 * Creates a COMPACT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    COMPACT <Zd>.<Ts>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_compact_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_compact, Zd, Pg, Zn)

/**
 * Creates a PUNPKHI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PUNPKHI <Pd>.H, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_punpkhi_sve(dc, Pd, Pn) \
    instr_create_1dst_1src(dc, OP_punpkhi, Pd, Pn)

/**
 * Creates a PUNPKLO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PUNPKLO <Pd>.H, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The source predicate register, P (Predicate).
 */
#define INSTR_CREATE_punpklo_sve(dc, Pd, Pn) \
    instr_create_1dst_1src(dc, OP_punpklo, Pd, Pn)

/**
 * Creates a SUNPKHI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUNPKHI <Zd>.<Ts>, <Zn>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).  The destination vector element
 * size, \<Ts\> (H, S, D) is twice the size of the source vector element size, \<Tb\> (B,
 * H, S).
 */
#define INSTR_CREATE_sunpkhi_sve(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_sunpkhi, Zd, Zn)

/**
 * Creates a SUNPKLO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUNPKLO <Zd>.<Ts>, <Zn>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).  The destination vector element
 * size, \<Ts\> (H, S, D) is twice the size of the source vector element size, \<Tb\> (B,
 * H, S).
 */
#define INSTR_CREATE_sunpklo_sve(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_sunpklo, Zd, Zn)

/**
 * Creates an UUNPKHI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UUNPKHI <Zd>.<Ts>, <Zn>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).  The destination vector element
 * size, \<Ts\> (H, S, D) is twice the size of the source vector element size, \<Tb\> (B,
 * H, S).
 */
#define INSTR_CREATE_uunpkhi_sve(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_uunpkhi, Zd, Zn)

/**
 * Creates an UUNPKLO instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UUNPKLO <Zd>.<Ts>, <Zn>.<Tb>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).  The destination vector element
 * size, \<Ts\> (H, S, D) is twice the size of the source vector element size, \<Tb\> (B,
 * H, S).
 */
#define INSTR_CREATE_uunpklo_sve(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_uunpklo, Zd, Zn)

/**
 * Creates an UZP1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UZP1    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uzp1_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_uzp1, Pd, Pn, Pm)

/**
 * Creates an UZP1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UZP1    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_uzp1_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_uzp1, Zd, Zn, Zm)

/**
 * Creates an UZP2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UZP2    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_uzp2_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_uzp2, Pd, Pn, Pm)

/**
 * Creates an UZP2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UZP2    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_uzp2_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_uzp2, Zd, Zn, Zm)

/**
 * Creates a ZIP1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ZIP1    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_zip1_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_zip1, Pd, Pn, Pm)

/**
 * Creates a ZIP1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ZIP1    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_zip1_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_zip1, Zd, Zn, Zm)

/**
 * Creates a ZIP2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ZIP2    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_zip2_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_zip2, Pd, Pn, Pm)

/**
 * Creates a ZIP2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ZIP2    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_zip2_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_zip2, Zd, Zn, Zm)

/**
 * Creates a TRN1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    TRN1    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_trn1_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_trn1, Pd, Pn, Pm)

/**
 * Creates a TRN1 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    TRN1    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_trn1_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_trn1, Zd, Zn, Zm)

/**
 * Creates a TRN2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    TRN2    <Pd>.<Ts>, <Pn>.<Ts>, <Pm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_trn2_sve_pred(dc, Pd, Pn, Pm) \
    instr_create_1dst_2src(dc, OP_trn2, Pd, Pn, Pm)

/**
 * Creates a TRN2 instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    TRN2    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_trn2_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_trn2, Zd, Zn, Zm)

/**
 * Creates a DUPM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    DUPM    <Zd>.<Ts>, #<const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param imm  The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_dupm_sve(dc, Zd, imm) instr_create_1dst_1src(dc, OP_dupm, Zd, imm)

/**
 * Creates an EON instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EON     <Zdn>.<Ts>, <Zdn>.<Ts>, #<imm>
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param imm   The immediate logicalImm.  The 13 bit immediate defining a 64, 32, 16 or 8
 * bit mask of 2, 4, 8, 16, 32 or 64 bit fields.
 */
#define INSTR_CREATE_eon_sve_imm(dc, Zdn, imm) \
    instr_create_1dst_2src(dc, OP_eor, Zdn, Zdn, opnd_invert_immed_int(imm))

/**
 * Creates a PFALSE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PFALSE  <Pd>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 */
#define INSTR_CREATE_pfalse_sve(dc, Pd) instr_create_1dst_0src(dc, OP_pfalse, Pd)

/**
 * Creates a PFIRST instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PFIRST  <Pdn>.B, <Pg>, <Pdn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pdn   The source and destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 */
#define INSTR_CREATE_pfirst_sve(dc, Pdn, Pg) \
    instr_create_1dst_2src(dc, OP_pfirst, Pdn, Pg, Pdn)

/**
 * Creates a SEL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SEL     <Pd>.B, <Pg>, <Pn>.B, <Pm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 * \param Pm   The second source predicate register, P (Predicate).
 */
#define INSTR_CREATE_sel_sve_pred(dc, Pd, Pg, Pn, Pm) \
    instr_create_1dst_3src(dc, OP_sel, Pd, Pg, Pn, Pm)

/**
 * Creates a SEL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SEL     <Zd>.<Ts>, <Pv>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pv   The first source predicate register, P (Predicate).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_sel_sve_vector(dc, Zd, Pv, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_sel, Zd, Pv, Zn, Zm)

/**
 * Creates an MOV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MOV     <Pd>.B, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_mov_sve_pred(dc, Pd, Pn) \
    instr_create_1dst_3src(dc, OP_orr, Pd,    \
                           opnd_create_predicate_reg(opnd_get_reg(Pn), false), Pn, Pn)

/**
 * Creates an MOVS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    MOVS    <Pd>.B, <Pg>/Z, <Pn>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd   The destination predicate register, P (Predicate).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Pn   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_movs_sve_pred(dc, Pd, Pg, Pn) \
    instr_create_1dst_3src(dc, OP_ands, Pd, Pg, Pn, Pn)

/**
 * Creates a PTRUE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PTRUE   <Pd>.<Ts>{, <pattern>}
 * \endverbatim
 * \param dc        The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd        The destination predicate register, P (Predicate).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 */
#define INSTR_CREATE_ptrue_sve(dc, Pd, pattern) \
    instr_create_1dst_1src(dc, OP_ptrue, Pd, pattern)

/**
 * Creates a PTRUES instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PTRUES  <Pd>.<Ts>{, <pattern>}
 * \endverbatim
 * \param dc        The void * dcontext used to allocate memory for the #instr_t.
 * \param Pd        The destination predicate register, P (Predicate).
 * \param pattern   The predicate constraint, see #dr_pred_constr_type_t.
 */
#define INSTR_CREATE_ptrues_sve(dc, Pd, pattern) \
    instr_create_1dst_1src(dc, OP_ptrues, Pd, pattern)

/**
 * Creates an ASR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASR     <Zd>.<Ts>, <Zn>.<Ts>, #<const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param imm   The immediate imm, one indexed.
 */
#define INSTR_CREATE_asr_sve(dc, Zd, Zn, imm) \
    instr_create_1dst_2src(dc, OP_asr, Zd, Zn, imm)

/**
 * Creates an ASR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASR     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_asr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_asr, Zdn, Pg, Zdn, Zm)

/**
 * Creates an ASR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASR     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_asr_sve_pred_wide(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_asr, Zdn, Pg, Zdn, Zm)

/**
 * Creates an ASR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASR     <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_asr_sve_wide(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_asr, Zd, Zn, Zm)

/**
 * Creates an ASRD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASRD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, #<const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm   The immediate imm, one indexed.
 */
#define INSTR_CREATE_asrd_sve_pred(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_asrd, Zdn, Pg, Zdn, imm)

/**
 * Creates an ASRR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ASRR    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_asrr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_asrr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a CLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLS     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_cls_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_cls, Zd, Pg, Zn)

/**
 * Creates a CLZ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CLZ     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_clz_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_clz, Zd, Pg, Zn)

/**
 * Creates a CNT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CNT     <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_cnt_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_cnt, Zd, Pg, Zn)

/**
 * Creates a LSL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSL     <Zd>.<Ts>, <Zn>.<Ts>, #<const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param imm   The immediate imm.
 */
#define INSTR_CREATE_lsl_sve(dc, Zd, Zn, imm) \
    instr_create_1dst_2src(dc, OP_lsl, Zd, Zn, imm)

/**
 * Creates a LSL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSL     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsl_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lsl, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LSL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSL     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsl_sve_pred_wide(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lsl, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LSL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSL     <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsl_sve_wide(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_lsl, Zd, Zn, Zm)

/**
 * Creates a LSLR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSLR    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lslr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lslr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSR     <Zd>.<Ts>, <Zn>.<Ts>, #<const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param imm   The immediate imm, one indexed.
 */
#define INSTR_CREATE_lsr_sve(dc, Zd, Zn, imm) \
    instr_create_1dst_2src(dc, OP_lsr, Zd, Zn, imm)

/**
 * Creates a LSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSR     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lsr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSR     <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsr_sve_pred_wide(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lsr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a LSR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSR     <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsr_sve_wide(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_lsr, Zd, Zn, Zm)

/**
 * Creates a LSRR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LSRR    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn   The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_lsrr_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_lsrr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a RBIT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RBIT    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_rbit_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_rbit, Zd, Pg, Zn)

/**
 * Creates an ANDV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ANDV    <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_andv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_andv, Vd, Pg, Zn)

/**
 * Creates an EORV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    EORV    <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_eorv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_eorv, Vd, Pg, Zn)

/**
 * Creates a FADDA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADDA   <V><dn>, <Pg>, <V><dn>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vdn   The first source and destination  register. Can be S
 *              (singleword, 32 bits), H (halfword, 16 bits) or
 *              D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fadda_sve_pred(dc, Vdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fadda, Vdn, Pg, Vdn, Zm)

/**
 * Creates a FADDV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADDV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be S (singleword, 32 bits), H
 *             (halfword, 16 bits) or D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_faddv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_faddv, Vd, Pg, Zn)

/**
 * Creates a FMAXNMV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXNMV <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be S (singleword, 32 bits), H
 *             (halfword, 16 bits) or D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmaxnmv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fmaxnmv, Vd, Pg, Zn)

/**
 * Creates a FMAXV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be S (singleword, 32 bits), H
 *             (halfword, 16 bits) or D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmaxv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fmaxv, Vd, Pg, Zn)

/**
 * Creates a FMINNMV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNMV <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be S (singleword, 32 bits), H
 *             (halfword, 16 bits) or D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fminnmv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fminnmv, Vd, Pg, Zn)

/**
 * Creates a FMINV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be S (singleword, 32 bits), H
 *             (halfword, 16 bits) or D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fminv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fminv, Vd, Pg, Zn)

/**
 * Creates an ORV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ORV     <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_orv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_orv, Vd, Pg, Zn)

/**
 * Creates a SADDV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SADDV   <Dd>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register, D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_saddv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_saddv, Vd, Pg, Zn)

/**
 * Creates a SMAXV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMAXV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_smaxv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_smaxv, Vd, Pg, Zn)

/**
 * Creates a SMINV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMINV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_sminv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_sminv, Vd, Pg, Zn)

/**
 * Creates an UADDV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UADDV   <Dd>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register, D (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_uaddv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_uaddv, Vd, Pg, Zn)

/**
 * Creates an UMAXV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMAXV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_umaxv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_umaxv, Vd, Pg, Zn)

/**
 * Creates an UMINV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMINV   <V><d>, <Pg>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Vd   The destination  register. Can be H (halfword, 16 bits), S
 *             (singleword, 32 bits), B (byte, 8 bits) or D
 *             (doubleword, 64 bits).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_uminv_sve_pred(dc, Vd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_uminv, Vd, Pg, Zn)

/*
 * Creates a FCPY instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCPY    <Zd>.<Ts>, <Pg>/M, #<imm>
 * \param imm  The floating-point immediate value to be copied.
 */
#define INSTR_CREATE_fcpy_sve_pred(dc, Zd, Pg, imm) \
    instr_create_1dst_2src(dc, OP_fcpy, Zd, Pg, imm)

/**
 * Creates a FDUP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FDUP    <Zd>.<Ts>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param imm  The floating-point immediate value to be copied.
 */
#define INSTR_CREATE_fdup_sve(dc, Zd, imm) instr_create_1dst_1src(dc, OP_fdup, Zd, imm)

/**
 * Creates a LD1RB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RB   { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RB   { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RB   { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RB   { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_1)
 */
#define INSTR_CREATE_ld1rb_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rb, Zt, Rn, Pg)

/**
 * Creates a LD1RH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RH   { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RH   { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RH   { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_2)
 */
#define INSTR_CREATE_ld1rh_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rh, Zt, Rn, Pg)

/**
 * Creates a LD1RW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RW   { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RW   { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_4)
 */
#define INSTR_CREATE_ld1rw_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rw, Zt, Rn, Pg)

/**
 * Creates a LD1RD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RD   { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_8)
 */
#define INSTR_CREATE_ld1rd_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rd, Zt, Rn, Pg)

/**
 * Creates a LD1RSB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RSB  { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RSB  { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RSB  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_1)
 */
#define INSTR_CREATE_ld1rsb_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rsb, Zt, Rn, Pg)

/**
 * Creates a LD1RSH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RSH  { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 *    LD1RSH  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_2)
 */
#define INSTR_CREATE_ld1rsh_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rsh, Zt, Rn, Pg)

/**
 * Creates a LD1RSW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RSW  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<pimm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with an immediate offset,
 *             constructed with the function:
 *             opnd_create_base_disp(Rn, DR_REG_NULL, 0, imm6,
 *             OPSZ_4)
 */
#define INSTR_CREATE_ld1rsw_sve(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rsw, Zt, Rn, Pg)

/**
 * Creates an INDEX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    INDEX   <Zd>.<Ts>, #<imm1>, #<imm2>
 *    INDEX   <Zd>.<Ts>, #<imm>, <R><m>
 *    INDEX   <Zd>.<Ts>, <R><n>, #<imm>
 *    INDEX   <Zd>.<Ts>, <R><n>, <R><m>
 * \endverbatim
 * \param dc         The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd         The destination vector register, Z (Scalable).
 * \param Rn_or_imm  The first source operand. Can be W (Word, 32 bits) or X
 *                   (Extended, 64 bits) register or a 5-bit signed immediate
 *                   in the range -16 to 15.
 * \param Rm_or_imm  The second source operand. Can be W (Word, 32 bits) or X
 *                   (Extended, 64 bits) register or a 5-bit signed immediate
 *                   in the range -16 to 15.
 */
#define INSTR_CREATE_index_sve(dc, Zd, Rn_or_imm, Rm_or_imm) \
    instr_create_1dst_2src(dc, OP_index, Zd, Rn_or_imm, Rm_or_imm)

/**
 * Creates a FCVT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCVT    <Zd>.H, <Pg>/M, <Zn>.D
 *    FCVT    <Zd>.S, <Pg>/M, <Zn>.D
 *    FCVT    <Zd>.D, <Pg>/M, <Zn>.H
 *    FCVT    <Zd>.S, <Pg>/M, <Zn>.H
 *    FCVT    <Zd>.D, <Pg>/M, <Zn>.S
 *    FCVT    <Zd>.H, <Pg>/M, <Zn>.S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fcvt_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fcvt, Zd, Pg, Zn)

/**
 * Creates a FCVTZS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCVTZS  <Zd>.S, <Pg>/M, <Zn>.D
 *    FCVTZS  <Zd>.D, <Pg>/M, <Zn>.D
 *    FCVTZS  <Zd>.H, <Pg>/M, <Zn>.H
 *    FCVTZS  <Zd>.S, <Pg>/M, <Zn>.H
 *    FCVTZS  <Zd>.D, <Pg>/M, <Zn>.H
 *    FCVTZS  <Zd>.S, <Pg>/M, <Zn>.S
 *    FCVTZS  <Zd>.D, <Pg>/M, <Zn>.S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fcvtzs_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fcvtzs, Zd, Pg, Zn)

/**
 * Creates a FCVTZU instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCVTZU  <Zd>.S, <Pg>/M, <Zn>.D
 *    FCVTZU  <Zd>.D, <Pg>/M, <Zn>.D
 *    FCVTZU  <Zd>.H, <Pg>/M, <Zn>.H
 *    FCVTZU  <Zd>.S, <Pg>/M, <Zn>.H
 *    FCVTZU  <Zd>.D, <Pg>/M, <Zn>.H
 *    FCVTZU  <Zd>.S, <Pg>/M, <Zn>.S
 *    FCVTZU  <Zd>.D, <Pg>/M, <Zn>.S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fcvtzu_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fcvtzu, Zd, Pg, Zn)

/**
 * Creates a FRINTA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTA  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frinta_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frinta, Zd, Pg, Zn)

/**
 * Creates a FRINTI instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTI  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frinti_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frinti, Zd, Pg, Zn)

/**
 * Creates a FRINTM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTM  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frintm_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frintm, Zd, Pg, Zn)

/**
 * Creates a FRINTN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTN  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frintn_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frintn, Zd, Pg, Zn)

/**
 * Creates a FRINTP instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTP  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frintp_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frintp, Zd, Pg, Zn)

/**
 * Creates a FRINTX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTX  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frintx_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frintx, Zd, Pg, Zn)

/**
 * Creates a FRINTZ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRINTZ  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frintz_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frintz, Zd, Pg, Zn)

/**
 * Creates a SCVTF instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SCVTF   <Zd>.H, <Pg>/M, <Zn>.H
 *    SCVTF   <Zd>.D, <Pg>/M, <Zn>.S
 *    SCVTF   <Zd>.H, <Pg>/M, <Zn>.S
 *    SCVTF   <Zd>.S, <Pg>/M, <Zn>.S
 *    SCVTF   <Zd>.D, <Pg>/M, <Zn>.D
 *    SCVTF   <Zd>.H, <Pg>/M, <Zn>.D
 *    SCVTF   <Zd>.S, <Pg>/M, <Zn>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_scvtf_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_scvtf, Zd, Pg, Zn)

/**
 * Creates an UCVTF instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UCVTF   <Zd>.H, <Pg>/M, <Zn>.H
 *    UCVTF   <Zd>.D, <Pg>/M, <Zn>.S
 *    UCVTF   <Zd>.H, <Pg>/M, <Zn>.S
 *    UCVTF   <Zd>.S, <Pg>/M, <Zn>.S
 *    UCVTF   <Zd>.D, <Pg>/M, <Zn>.D
 *    UCVTF   <Zd>.H, <Pg>/M, <Zn>.D
 *    UCVTF   <Zd>.S, <Pg>/M, <Zn>.D
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_ucvtf_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_ucvtf, Zd, Pg, Zn)

/**
 * Creates a CTERMEQ instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CTERMEQ <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source  register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 * \param Rm   The second source  register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 */
#define INSTR_CREATE_ctermeq(dc, Rn, Rm) instr_create_0dst_2src(dc, OP_ctermeq, Rn, Rm)

/**
 * Creates a CTERMNE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    CTERMNE <R><n>, <R><m>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rn   The first source  register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 * \param Rm   The second source  register. Can be X (Extended, 64 bits) or W
 *             (Word, 32 bits).
 */
#define INSTR_CREATE_ctermne(dc, Rn, Rm) instr_create_0dst_2src(dc, OP_ctermne, Rn, Rm)

/**
 * Creates a PNEXT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    PNEXT   <Pdn>.<Ts>, <Pv>, <Pdn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Pdn   The second source and destination predicate register, P
 *              (Predicate).
 * \param Pv   The first source predicate register, P (Predicate).
 */
#define INSTR_CREATE_pnext_sve(dc, Pdn, Pv) \
    instr_create_1dst_2src(dc, OP_pnext, Pdn, Pv, Pdn)

/**
 * Creates a FABD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FABD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fabd_sve(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fabd, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FABS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FABS    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fabs_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fabs, Zd, Pg, Zn)

/**
 * Creates a FDIV instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FDIV    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fdiv_sve(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fdiv, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FDIVR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FDIVR   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fdivr_sve(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fdivr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMAD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAD    <Zdn>.<Ts>, <Pg>/M, <Zm>.<Ts>, <Za>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 * \param Za   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmad_sve(dc, Zdn, Pg, Zm, Za) \
    instr_create_1dst_4src(dc, OP_fmad, Zdn, Zdn, Pg, Zm, Za)

/**
 * Creates a FMULX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMULX   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmulx_sve(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fmulx, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FNEG instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FNEG    <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fneg_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fneg, Zd, Pg, Zn)

/**
 * Creates a FNMAD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FNMAD   <Zdn>.<Ts>, <Pg>/M, <Zm>.<Ts>, <Za>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 * \param Za   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fnmad_sve(dc, Zdn, Pg, Zm, Za) \
    instr_create_1dst_4src(dc, OP_fnmad, Zdn, Zdn, Pg, Zm, Za)

/**
 * Creates a FNMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FNMLA   <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fnmla_sve(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_fnmla, Zda, Zda, Pg, Zn, Zm)

/**
 * Creates a FNMLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FNMLS   <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fnmls_sve(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_fnmls, Zda, Zda, Pg, Zn, Zm)

/**
 * Creates a FRECPE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPE  <Zd>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frecpe_sve(dc, Zd, Zn) instr_create_1dst_1src(dc, OP_frecpe, Zd, Zn)

/**
 * Creates a FRECPS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPS  <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frecps_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_frecps, Zd, Zn, Zm)

/**
 * Creates a FRECPX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRECPX  <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frecpx_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_frecpx, Zd, Pg, Zn)

/**
 * Creates a FRSQRTE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTE <Zd>.<Ts>, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frsqrte_sve(dc, Zd, Zn) \
    instr_create_1dst_1src(dc, OP_frsqrte, Zd, Zn)

/**
 * Creates a FRSQRTS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FRSQRTS <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_frsqrts_sve(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_frsqrts, Zd, Zn, Zm)

/**
 * Creates a FSCALE instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSCALE  <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fscale_sve(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fscale, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FSQRT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSQRT   <Zd>.<Ts>, <Pg>/M, <Zn>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fsqrt_sve(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_fsqrt, Zd, Pg, Zn)

/**
 * Creates a FADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.5 or 1.0.
 */
#define INSTR_CREATE_fadd_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fadd, Zdn, Pg, Zdn, imm)

/**
 * Creates a FADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADD    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fadd_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fadd, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FADD    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fadd_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_fadd, Zd, Zn, Zm)

/**
 * Creates a FSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSUB    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.5 or 1.0.
 */
#define INSTR_CREATE_fsub_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fsub, Zdn, Pg, Zdn, imm)

/**
 * Creates a FSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSUB    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fsub_sve_pred(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fsub, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FSUB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSUB    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fsub_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_fsub, Zd, Zn, Zm)

/**
 * Creates a FSUBR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSUBR   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.5 or 1.0.
 */
#define INSTR_CREATE_fsubr_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fsubr, Zdn, Pg, Zdn, imm)

/**
 * Creates a FSUBR instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FSUBR   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fsubr_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fsubr, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAX    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.0 or 1.0.
 */
#define INSTR_CREATE_fmax_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fmax, Zdn, Pg, Zdn, imm)

/**
 * Creates a FMAX instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAX    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmax_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fmax, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMAXNM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXNM  <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.0 or 1.0.
 */
#define INSTR_CREATE_fmaxnm_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fmaxnm, Zdn, Pg, Zdn, imm)

/**
 * Creates a FMAXNM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMAXNM  <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmaxnm_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fmaxnm, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMIN    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.0 or 1.0.
 */
#define INSTR_CREATE_fmin_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fmin, Zdn, Pg, Zdn, imm)

/**
 * Creates a FMIN instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMIN    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmin_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fmin, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMINNM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNM  <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <const>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.0 or 1.0.
 */
#define INSTR_CREATE_fminnm_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fminnm, Zdn, Pg, Zdn, imm)

/**
 * Creates a FMINNM instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMINNM  <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fminnm_sve_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fminnm, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLA    <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The third source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmla_sve_vector(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_fmla, Zda, Zda, Pg, Zn, Zm)

/**
 * Creates a FMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLA    <Zda>.H, <Zn>.H, <Zm>.H[<index>]
 *    FMLA    <Zda>.S, <Zn>.S, <Zm>.S[<index>]
 *    FMLA    <Zda>.D, <Zn>.D, <Zm>.D[<index>]
 * \endverbatim
 * \param dc     The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda    The third source and destination vector register, Z (Scalable).
 * \param Zn     The first source vector register, Z (Scalable).
 * \param Zm     The second source vector register, Z (Scalable).
 * \param index  Second source index constant.
 */
#define INSTR_CREATE_fmla_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_fmla, Zda, Zda, Zn, Zm, index)

/**
 * Creates a FMLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLS    <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The third source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmls_sve_vector(dc, Zda, Pg, Zn, Zm) \
    instr_create_1dst_4src(dc, OP_fmls, Zda, Zda, Pg, Zn, Zm)

/**
 * Creates a FMLS instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMLS    <Zda>.H, <Zn>.H, <Zm>.H[<index>]
 *    FMLS    <Zda>.S, <Zn>.S, <Zm>.S[<index>]
 *    FMLS    <Zda>.D, <Zn>.D, <Zm>.D[<index>]
 * \endverbatim
 * \param dc     The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda    The third source and destination vector register, Z (Scalable).
 * \param Zn     The first source vector register, Z (Scalable).
 * \param Zm     The second source vector register, Z (Scalable).
 * \param index  Second source index constant.
 */
#define INSTR_CREATE_fmls_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_fmls, Zda, Zda, Zn, Zm, index)

/**
 * Creates a FMSB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMSB    <Zdn>.<Ts>, <Pg>/M, <Zm>.<Ts>, <Za>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 * \param Za   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmsb_sve(dc, Zdn, Pg, Zm, Za) \
    instr_create_1dst_4src(dc, OP_fmsb, Zdn, Zdn, Pg, Zm, Za)

/**
 * Creates a FMUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMUL    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param imm  Floating point constant, either 0.5 or 2.0.
 */
#define INSTR_CREATE_fmul_sve(dc, Zdn, Pg, imm) \
    instr_create_1dst_3src(dc, OP_fmul, Zdn, Pg, Zdn, imm)

/**
 * Creates a FMUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMUL    <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmul_sve_pred_vector(dc, Zdn, Pg, Zm) \
    instr_create_1dst_3src(dc, OP_fmul, Zdn, Pg, Zdn, Zm)

/**
 * Creates a FMUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMUL    <Zd>.<Ts>, <Zn>.<Ts>, <Zm>.<Ts>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Zn   The first source vector register, Z (Scalable).
 * \param Zm   The second source vector register, Z (Scalable).
 */
#define INSTR_CREATE_fmul_sve_vector(dc, Zd, Zn, Zm) \
    instr_create_1dst_2src(dc, OP_fmul, Zd, Zn, Zm)

/**
 * Creates a FMUL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FMUL    <Zd>.H, <Zn>.H, <Zm>.H[<index>]
 *    FMUL    <Zd>.S, <Zn>.S, <Zm>.S[<index>]
 *    FMUL    <Zd>.D, <Zn>.D, <Zm>.D[<index>]
 * \endverbatim
 * \param dc     The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd     The destination vector register, Z (Scalable).
 * \param Zn     The first source vector register, Z (Scalable).
 * \param Zm     The second source vector register, Z (Scalable).
 * \param index  Second source index constant.
 */
#define INSTR_CREATE_fmul_sve_idx(dc, Zd, Zn, Zm, index) \
    instr_create_1dst_3src(dc, OP_fmul, Zd, Zn, Zm, index)

/**
 * Creates an ADDPL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ADDPL   <Xd|SP>, <Xn|SP>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination  register, X (Extended, 64 bits).
 * \param Rn   The first source  register, X (Extended, 64 bits).
 * \param simm   The signed immediate imm.
 */
#define INSTR_CREATE_addpl(dc, Rd, Rn, simm) \
    instr_create_1dst_2src(dc, OP_addpl, Rd, Rn, simm)

/**
 * Creates an ADDVL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ADDVL   <Xd|SP>, <Xn|SP>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination  register, X (Extended, 64 bits).
 * \param Rn   The first source  register, X (Extended, 64 bits).
 * \param simm   The signed immediate imm.
 */
#define INSTR_CREATE_addvl(dc, Rd, Rn, simm) \
    instr_create_1dst_2src(dc, OP_addvl, Rd, Rn, simm)

/**
 * Creates a RDVL instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    RDVL    <Xd>, #<imm>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Rd   The destination  register, X (Extended, 64 bits).
 * \param simm   The signed immediate imm.
 */
#define INSTR_CREATE_rdvl(dc, Rd, simm) instr_create_1dst_1src(dc, OP_rdvl, Rd, simm)

/**
 * Creates a LDFF1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1B  { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 *    LDFF1B  { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 *    LDFF1B  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 *    LDFF1B  { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ldff1b_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1b, Zt, Rn, Pg)

/**
 * Creates a LDFF1D instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1D  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #3}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_shift_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 1, 0, 0, OPSZ_32, 3)
 */
#define INSTR_CREATE_ldff1d_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1d, Zt, Rn, Pg)

/**
 * Creates a LDFF1H instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1H  { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
 *    LDFF1H  { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
 *    LDFF1H  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_shift_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 1, 0, 0, OPSZ_32, 1)
 */
#define INSTR_CREATE_ldff1h_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1h, Zt, Rn, Pg)

/**
 * Creates a LDFF1SB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 *    LDFF1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 *    LDFF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, false, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ldff1sb_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1sb, Zt, Rn, Pg)

/**
 * Creates a LDFF1SH instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
 *    LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_shift_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 1, 0, 0, OPSZ_16, 1)
 */
#define INSTR_CREATE_ldff1sh_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1sh, Zt, Rn, Pg)

/**
 * Creates a LDFF1SW instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_shift_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 1, 0, 0, OPSZ_16, 2)
 */
#define INSTR_CREATE_ldff1sw_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1sw, Zt, Rn, Pg)

/**
 * Creates a LDFF1W instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDFF1W  { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
 *    LDFF1W  { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_shift_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 1, 0, 0, OPSZ_32, 2)
 */
#define INSTR_CREATE_ldff1w_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldff1w, Zt, Rn, Pg)

/**

 * Creates a FCADD instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCADD   <Zdn>.<Ts>, <Pg>/M, <Zdn>.<Ts>, <Zm>.<Ts>, <rot>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zdn  The first source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zm   The second source vector register, Z (Scalable).
 * \param rot  The immediate rot, must be 90 or 270.
 */
#define INSTR_CREATE_fcadd_sve_pred(dc, Zdn, Pg, Zm, rot) \
    instr_create_1dst_4src(dc, OP_fcadd, Zdn, Pg, Zdn, Zm, rot)

/**
 * Creates a FCMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLA   <Zda>.<Ts>, <Pg>/M, <Zn>.<Ts>, <Zm>.<Ts>, <rot>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The source and destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 * \param rot  The immediate rot, must be 0, 90, 180, or 270.
 */
#define INSTR_CREATE_fcmla_sve_vector(dc, Zda, Pg, Zn, Zm, rot) \
    instr_create_1dst_5src(dc, OP_fcmla, Zda, Zda, Pg, Zn, Zm, rot)

/**
 * Creates a FCMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    FCMLA   <Zda>.H, <Zn>.H, <Zm>.H[<imm>], <rot>
 *    FCMLA   <Zda>.S, <Zn>.S, <Zm>.S[<imm>], <rot>
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 * \param imm  The immediate imm representing index of a Real and Imaginary pair
 * \param rot  The immediate rot, must be 0, 90, 180, or 270.
 */
#define INSTR_CREATE_fcmla_sve_idx(dc, Zda, Zn, Zm, imm, rot) \
    instr_create_1dst_5src(dc, OP_fcmla, Zda, Zda, Zn, Zm, imm, rot)

/*
 * Creates a LD1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1B    { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
 *    LD1B    { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]
 *    LD1B    { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]
 *    LD1B    { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ld1b_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1b, Zt, Rn, Pg)

/**
 * Creates a LD1ROB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1ROB  { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ld1rob_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rob, Zt, Rn, Pg)

/**
 * Creates a LD1RQB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1RQB  { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ld1rqb_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1rqb, Zt, Rn, Pg)

/**
 * Creates a LD1SB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LD1SB   { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
 *    LD1SB   { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]
 *    LD1SB   { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ld1sb_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ld1sb, Zt, Rn, Pg)

/**
 * Creates a LDNT1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    LDNT1B  { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The first source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_ldnt1b_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_ldnt1b, Zt, Rn, Pg)

/**
 * Creates a ST1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    ST1B    { <Zt>.<Ts> }, <Pg>, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The first source vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The second source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_st1b_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_st1b, Rn, Zt, Pg)

/**
 * Creates a STNT1B instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    STNT1B  { <Zt>.B }, <Pg>, [<Xn|SP>, <Xm>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zt   The first source vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Rn   The second source base register with a register offset,
 *             constructed with the function:
 *             opnd_create_base_disp_aarch64(Rn, Rm,
 *             DR_EXTEND_UXTX, 0, 0, 0, OPSZ_1)
 */
#define INSTR_CREATE_stnt1b_sve_pred(dc, Zt, Pg, Rn) \
    instr_create_1dst_2src(dc, OP_stnt1b, Rn, Zt, Pg)

/**
 * Creates a BFCVT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFCVT   <Zd>.H, <Pg>/M, <Zn>.S
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zd   The destination vector register, Z (Scalable).
 * \param Pg   The governing predicate register, P (Predicate).
 * \param Zn   The source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bfcvt_sve_pred(dc, Zd, Pg, Zn) \
    instr_create_1dst_2src(dc, OP_bfcvt, Zd, Pg, Zn)

/**
 * Creates a BFDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFDOT   <Zda>.S, <Zn>.H, <Zm>.H
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bfdot_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_bfdot, Zda, Zda, Zn, Zm)

/**
 * Creates a BFDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFDOT   <Zda>.S, <Zn>.H, <Zm>.H[<index>]
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The source and destination vector register, Z (Scalable).
 * \param Zn    The second source vector register, Z (Scalable).
 * \param Zm    The third source vector register, Z (Scalable).
 * \param index The immediate index
 */
#define INSTR_CREATE_bfdot_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_bfdot, Zda, Zda, Zn, Zm, index)

/**
 * Creates a BFMLALB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFMLALB <Zda>.S, <Zn>.H, <Zm>.H
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bfmlalb_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_bfmlalb, Zda, Zda, Zn, Zm)

/**
 * Creates a BFMLALB instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFMLALB <Zda>.S, <Zn>.H, <Zm>.H[<index>]
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The source and destination vector register, Z (Scalable).
 * \param Zn    The second source vector register, Z (Scalable).
 * \param Zm    The third source vector register, Z (Scalable).
 * \param index The immediate index
 */
#define INSTR_CREATE_bfmlalb_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_bfmlalb, Zda, Zda, Zn, Zm, index)

/**
 * Creates a BFMLALT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFMLALT <Zda>.S, <Zn>.H, <Zm>.H
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bfmlalt_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_bfmlalt, Zda, Zda, Zn, Zm)

/**
 * Creates a BFMLALT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFMLALT <Zda>.S, <Zn>.H, <Zm>.H[<index>]
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The source and destination vector register, Z (Scalable).
 * \param Zn    The second source vector register, Z (Scalable).
 * \param Zm    The third source vector register, Z (Scalable).
 * \param index The immediate index
 */
#define INSTR_CREATE_bfmlalt_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_bfmlalt, Zda, Zda, Zn, Zm, index)

/**
 * Creates a BFMMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    BFMMLA  <Zda>.S, <Zn>.H, <Zm>.H
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_bfmmla_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_bfmmla, Zda, Zda, Zn, Zm)

/**
 * Creates a SMMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SMMLA   <Zda>.S, <Zn>.B, <Zm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_smmla_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_smmla, Zda, Zda, Zn, Zm)

/**
 * Creates a SUDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    SUDOT   <Zda>.S, <Zn>.B, <Zm>.B[<index>]
 * \endverbatim
 * \param dc    The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda   The source and destination vector register, Z (Scalable).
 * \param Zn    The second source vector register, Z (Scalable).
 * \param Zm    The third source vector register, Z (Scalable).
 * \param index The immediate index
 */
#define INSTR_CREATE_sudot_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_sudot, Zda, Zda, Zn, Zm, index)

/**
 * Creates an UMMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    UMMLA   <Zda>.S, <Zn>.B, <Zm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_ummla_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_ummla, Zda, Zda, Zn, Zm)

/**
 * Creates an USDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    USDOT   <Zda>.S, <Zn>.B, <Zm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_usdot_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_usdot, Zda, Zda, Zn, Zm)

/**
 * Creates an USDOT instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    USDOT   <Zda>.S, <Zn>.B, <Zm>.B[<index>]
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 * \param index The immediate index
 */
#define INSTR_CREATE_usdot_sve_idx(dc, Zda, Zn, Zm, index) \
    instr_create_1dst_4src(dc, OP_usdot, Zda, Zda, Zn, Zm, index)

/**
 * Creates an USMMLA instruction.
 *
 * This macro is used to encode the forms:
 * \verbatim
 *    USMMLA  <Zda>.S, <Zn>.B, <Zm>.B
 * \endverbatim
 * \param dc   The void * dcontext used to allocate memory for the #instr_t.
 * \param Zda  The source and destination vector register, Z (Scalable).
 * \param Zn   The second source vector register, Z (Scalable).
 * \param Zm   The third source vector register, Z (Scalable).
 */
#define INSTR_CREATE_usmmla_sve(dc, Zda, Zn, Zm) \
    instr_create_1dst_3src(dc, OP_usmmla, Zda, Zda, Zn, Zm)

#endif /* DR_IR_MACROS_AARCH64_H */
