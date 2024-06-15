//
// Created by Simon on 2022/6/2.
//

#pragma once

#include "define.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <ostream>

//-------------------------basic define----------------------------//

struct BackExpression
{
    static std::set<BackExpression> AllExpressions;
    std::string opera;
    std::string org_1, org_2;
    int index{};

    explicit BackExpression(const IR_node &node)
    {
        opera = node.opera;
        org_1 = node.org_1.name;
        org_2 = node.org_2.name;
        index = node.index;
    }

    BackExpression() = default;

    friend bool operator==(const BackExpression &first, const BackExpression &second)
    {
        return (first.opera == second.opera && first.org_1 == second.org_1 && first.org_2 == second.org_2);
    }

    friend bool operator<(const BackExpression &first, const BackExpression &second)
    {
        if (first.opera + first.org_1 + first.org_2 < second.opera + second.org_1 + second.org_2)
            return true;
        return false;
    }

    BackExpression &operator=(const BackExpression &target) = default;

    friend std::ostream &operator<<(std::ostream &os, const BackExpression &expression)
    {
        os << "opera: " << expression.opera << " org_1: " << expression.org_1 << " org_2: " << expression.org_2
           << " index: " << expression.index;
        return os;
    }
};

#define EXP_PTR std::shared_ptr<BackExpression>

//-------------------------CFG----------------------------//

struct CFG_node
{
    int index = -1;
    std::string name;

    std::vector<IR_PTR> content;

    // For ActivityAnalysis
    std::set<std::string> in_variables;
    std::set<std::string> out_variables;
    std::set<std::string> used_variables;
    std::set<std::string> defined_variables;

    // For ExpressionAnalysis
    std::set<BackExpression> in_expressions;
    std::set<BackExpression> out_expressions;
    std::set<BackExpression> generated_expressions;
    std::set<BackExpression> killed_expressions;

    // save linear index of other basic block in the CFG
    std::vector<std::string> str_predecessor;
    std::vector<std::string> str_successor;
    std::vector<std::shared_ptr<CFG_node>> predecessor;
    std::vector<std::shared_ptr<CFG_node>> successor;

    virtual void print() const;
};

#define CFG_PTR std::shared_ptr<CFG_node>

namespace CFG_list
{
    void print_all(const std::vector<CFG_PTR> &CFG_blocks_chain);
    void print_all(const std::vector<std::vector<CFG_PTR>> &static_chain);
    void print_all(const std::map<std::string, std::vector<CFG_PTR>> &function_chain);
}

namespace CFG_safe
{
    void raise_error(const std::string &error_code);
}

//-------------------------LVA----------------------------//

namespace LVA_safe
{
    void raise_error(const std::string &error_code);
}

//-------------------------RA----------------------------//

enum register_name
{
    a1,
    a2,
    a3,
    a4,
    v1,
    v2,
    v3,
    v4,
    v5,
    v6,
    v7,
    v8,
    ip,
    sp,
    lr,
    pc,
    spill,
    no_name
};

struct register_id
{
    register_name type = no_name;
    int spill_len = 0;
};

const std::string register_name_str[] = {
    "a1", "a2", "a3", "a4",
    "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8",
    "ip", "sp", "lr", "pc",
    "spill",
    "no_name"};

struct IR_node_pro : public IR_node
{
    register_id tar;
    register_id src1;
    register_id src2;

    void print() const override;
};

#define IR_pro_PTR std::shared_ptr<IR_node_pro>

struct CFG_pro_node : public CFG_node
{
    std::vector<IR_pro_PTR> content_pro;
    void print() const override;
};

#define CFG_pro_PTR std::shared_ptr<CFG_pro_node>

namespace CFG_pro_list
{
    void print_all(const std::vector<CFG_pro_PTR> &CFG_pro_blocks_chain);
    void print_all(const std::vector<std::vector<CFG_pro_PTR>> &static_chain);
    void print_all(const std::map<std::string, std::vector<CFG_pro_PTR>> &function_pro_chain);
}

//-------------------------IA&ARM----------------------------//

enum arm_type
{
    arm_global_label,
    arm_func_label,
    arm_block_label,
    arm_ins,
    arm_section
};

struct ARM_node
{
    int index = -1;
    arm_type type = arm_ins;
    std::string instruction;
    std::string comment;
};

namespace ARM
{
    void print_normal_chain(std::vector<IR_pro_PTR> normal_chain);
    void print_static_chain(std::vector<IR_PTR> static_chain);
    void print_all(const std::vector<ARM_node> &ARM_code);
    void dump_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename);
};



const std::string inline_LLVM_code = R"(
    .arch armv7-a
	.fpu vfpv3-d16
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"std.c"
	.text
	.section	.rodata
	.align	2
.LC0:
	.ascii	"%d\000"
	.text
	.align	1
	.global	getint
	.syntax unified
	.thumb
	.thumb_func
	.type	getint, %function
getint:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	ldr	r2, .L4
.LPIC1:
	add	r2, pc
	ldr	r3, .L4+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #4]
	mov	r3, #0
	mov	r3, r7
	mov	r1, r3
	ldr	r3, .L4+8
.LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r3, [r7]
	ldr	r1, .L4+12
.LPIC2:
	add	r1, pc
	ldr	r2, .L4+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #4]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L3
	bl	__stack_chk_fail(PLT)
.L3:
	mov	r0, r3
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L5:
	.align	2
.L4:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.size	getint, .-getint
	.section	.rodata
	.align	2
.LC1:
	.ascii	"%c\000"
	.text
	.align	1
	.global	getch
	.syntax unified
	.thumb
	.thumb_func
	.type	getch, %function
getch:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	ldr	r2, .L9
.LPIC4:
	add	r2, pc
	ldr	r3, .L9+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #4]
	mov	r3, #0
	adds	r3, r7, #3
	mov	r1, r3
	ldr	r3, .L9+8
.LPIC3:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldrb	r3, [r7, #3]	@ zero_extendqisi2
	ldr	r1, .L9+12
.LPIC5:
	add	r1, pc
	ldr	r2, .L9+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #4]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L8
	bl	__stack_chk_fail(PLT)
.L8:
	mov	r0, r3
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L10:
	.align	2
.L9:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC4+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC1-(.LPIC3+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC5+4)
	.size	getch, .-getch
	.align	1
	.global	getarray
	.syntax unified
	.thumb
	.thumb_func
	.type	getarray, %function
getarray:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #24
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r2, .L16
.LPIC8:
	add	r2, pc
	ldr	r3, .L16+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #20]
	mov	r3, #0
	add	r3, r7, #12
	mov	r1, r3
	ldr	r3, .L16+8
.LPIC6:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L12
.L13:
	ldr	r3, [r7, #16]
	lsls	r3, r3, #2
	ldr	r2, [r7, #4]
	add	r3, r3, r2
	mov	r1, r3
	ldr	r3, .L16+12
.LPIC7:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L12:
	ldr	r3, [r7, #12]
	ldr	r2, [r7, #16]
	cmp	r2, r3
	blt	.L13
	ldr	r3, [r7, #12]
	ldr	r1, .L16+16
.LPIC9:
	add	r1, pc
	ldr	r2, .L16+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #20]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L15
	bl	__stack_chk_fail(PLT)
.L15:
	mov	r0, r3
	adds	r7, r7, #24
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L17:
	.align	2
.L16:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC8+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC6+4)
	.word	.LC0-(.LPIC7+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC9+4)
	.size	getarray, .-getarray
	.align	1
	.global	putint
	.syntax unified
	.thumb
	.thumb_func
	.type	putint, %function
putint:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r1, [r7, #4]
	ldr	r3, .L19
.LPIC10:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	nop
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L20:
	.align	2
.L19:
	.word	.LC0-(.LPIC10+4)
	.size	putint, .-putint
	.align	1
	.global	putch
	.syntax unified
	.thumb
	.thumb_func
	.type	putch, %function
putch:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	uxtb	r3, r3
	mov	r0, r3
	bl	putchar(PLT)
	nop
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	putch, .-putch
	.section	.rodata
	.align	2
.LC2:
	.ascii	"%d:\000"
	.align	2
.LC3:
	.ascii	" %d\000"
	.text
	.align	1
	.global	putarray
	.syntax unified
	.thumb
	.thumb_func
	.type	putarray, %function
putarray:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #16
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r1, [r7, #4]
	ldr	r3, .L25
.LPIC11:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	movs	r3, #0
	str	r3, [r7, #12]
	b	.L23
.L24:
	ldr	r3, [r7, #12]
	lsls	r3, r3, #2
	ldr	r2, [r7]
	add	r3, r3, r2
	ldr	r3, [r3]
	mov	r1, r3
	ldr	r3, .L25+4
.LPIC12:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
.L23:
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	cmp	r2, r3
	blt	.L24
	movs	r0, #10
	bl	putchar(PLT)
	nop
	adds	r7, r7, #16
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L26:
	.align	2
.L25:
	.word	.LC2-(.LPIC11+4)
	.word	.LC3-(.LPIC12+4)
	.size	putarray, .-putarray)";

