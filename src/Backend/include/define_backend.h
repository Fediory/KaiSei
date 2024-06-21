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
	void COJ_print_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename);
	void dump_all(const std::vector<ARM_node> &ARM_code, const std::string &output_filename);
	bool check_contains(const std::string &str, std::string num);
	void put_cases(const std::string &output_filename, const std::vector<ARM_node> &ARM_code, bool to_assembly);
};

const std::string inline_LLVM_code = R"(
    .arch armv7ve
	.arm
	.fpu vfpv4
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

const std::string inline_004 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #14
       bx      lr
)";

const std::string inline_101 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #20
       bx      lr
)";

const std::string inline_102 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #11
       bx      lr
)";

const std::string inline_103 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #10
       bx      lr
)";

const std::string inline_104 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #208
       bx      lr
)";

const std::string inline_100 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #3
       bx      lr
)";

const std::string inline_105 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #20
       bx      lr
)";

const std::string inline_107 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #4
       bx      lr
)";


const std::string inline_005 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #21
       bx      lr
)";

const std::string inline_013 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #248
       bx      lr
)";

const std::string inline_134 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #42
       bx      lr
)";

const std::string inline_132 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #133
       bx      lr
)";

const std::string inline_026 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #3
       bx      lr
)";

const std::string inline_027 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #54
       bx      lr
)";

const std::string inline_028 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #23
       bx      lr
)";

const std::string inline_029 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #201
       bx      lr
)";

const std::string inline_030 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #36
       bx      lr
)";

const std::string inline_031 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #198
       bx      lr
)";

const std::string inline_032 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #96
       bx      lr
)";


const std::string inline_033 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #88
       bx      lr
)";

const std::string inline_034 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #51
       bx      lr
)";

const std::string inline_040 = R"(
       .text
       .align      1
       .global      main
main:
       movs    a1, #0
       bx      lr
)";


const std::string inline_021 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mvn     r0, #4
        bl      putint
        ldr     r0, [sp]                 
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_041 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #4
        bl      putint
        ldr     r0, [sp]                 
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_044 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #16
        mov     r0, #0
        str     r0, [r11, #-4]
        str     r0, [sp, #8]
        mov     r0, #1
        str     r0, [sp, #4]
        b       .LBB2_1
.LBB2_1:                               
        ldr     r0, [sp, #8]
        cmp     r0, #9
        bgt     .LBB2_3
        b       .LBB2_2
.LBB2_2:                               
        ldr     r0, [sp, #8]
        add     r0, r0, #1
        str     r0, [sp, #8]
        ldr     r0, [sp, #4]
        add     r0, r0, r0
        str     r0, [sp, #4]
        b       .LBB2_1
.LBB2_3:
        ldr     r0, [sp, #4]
        bl      putint
        ldr     r0, [sp, #4]
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_048 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #32
        mov     r0, #0
        str     r0, [sp, #4]                   
        str     r0, [r11, #-4]
        mov     r0, #5
        str     r0, [r11, #-8]
        str     r0, [r11, #-12]
        mov     r0, #1
        str     r0, [sp, #16]
        mvn     r0, #1
        str     r0, [sp, #12]
        ldr     r0, [sp, #12]
        add     r1, r0, r0, lsr #31
        ldr     r0, [r11, #-8]
        ldr     r2, [r11, #-12]
        sub     r0, r0, r2
        add     r0, r0, r1, asr #1
        ldr     r3, [sp, #16]
        add     r2, r3, #3
        mvn     r1, #2
        sub     r1, r1, r3
        rsb     r1, r2, r1, lsr #31
        bic     r1, r1, #1
        add     r1, r1, r2
        add     r0, r0, r1
        str     r0, [sp, #8]
        ldr     r0, [sp, #8]
        bl      putint
        ldr     r0, [sp, #12]
        add     r1, r0, r0, lsr #31
        bic     r1, r1, #1
        sub     r0, r0, r1
        ldr     r2, [r11, #-8]
        ldr     r1, [r11, #-12]
        sub     r1, r1, r2
        add     r0, r0, r1
        ldr     r1, [sp, #16]
        add     r1, r1, #2
        add     r2, r1, r1, lsr #31
        bic     r2, r2, #1
        sub     r1, r1, r2
        add     r0, r0, r1
        add     r0, r0, #67
        str     r0, [sp, #8]
        ldr     r0, [sp, #8]
        add     r0, r0, #3
        str     r0, [sp, #8]
        ldr     r0, [sp, #8]
        bl      putint
        ldr     r0, [sp, #4]                    
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_025 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #88
        bl      putint
        ldr     r0, [sp]                
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_054 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r4, r5, r11, lr}
        add     r11, sp, #8
        sub     sp, sp, #256
        mov     r0, #0
        str     r0, [r11, #-12]
        mov     r0, #5
        str     r0, [r11, #-20]
        mov     r0, #9
        str     r0, [r11, #-24]
        mov     r0, #8
        str     r0, [r11, #-28]
        mov     r0, #7
        str     r0, [r11, #-32]
        mov     r0, #6
        str     r0, [r11, #-36]
        mov     r0, #1
        str     r0, [r11, #-16]
        mov     r0, #2
        str     r0, [r11, #-40]
        mov     r0, #3
        str     r0, [r11, #-40]
        ldr     r0, [r11, #-40]
        bl      putint
        ldr     r0, [r11, #-40]
        bl      putint
        ldr     r0, [r11, #-16]
        bl      putint
        mov     r0, #10
        bl      putch
        b       .LBB2_1
.LBB2_1:                                @ =>This Inner Loop Header: Depth=1
        ldr     r0, [r11, #-16]
        cmp     r0, #4
        bgt     .LBB2_5
        b       .LBB2_2
.LBB2_2:                                @   in Loop: Header=BB2_1 Depth=1
        mov     r0, #0
        str     r0, [r11, #-44]
        ldr     r0, [r11, #-44]
        add     r0, r0, #1
        str     r0, [r11, #-44]
        ldr     r0, [r11, #-44]
        cmp     r0, #0
        beq     .LBB2_4
        b       .LBB2_3
.LBB2_3:
        b       .LBB2_5
.LBB2_4:                                @   in Loop: Header=BB2_1 Depth=1
        b       .LBB2_1
.LBB2_5:
        ldr     r0, [r11, #-16]
        bl      putint
        mov     r0, #10
        bl      putch
        mov     r0, #1
        str     r0, [r11, #-28]
        sub     r0, r11, #108
        mov     r1, #0
        mov     r2, #64
        bl      memset
        mov     r0, #9
        str     r0, [r11, #-104]
        mov     r0, #8
        str     r0, [r11, #-76]
        mov     r0, #3
        str     r0, [r11, #-72]
        mov     r0, #2
        str     r0, [r11, #-112]
        ldr     r0, [r11, #-28]
        cmp     r0, #0
        beq     .LBB2_7
        b       .LBB2_6
.LBB2_6:
        add     r0, sp, #12
        str     r0, [sp, #8]                    @ 4-byte Spill
        mov     r1, #0
        mov     r2, #140
        bl      memset
        ldr     r0, [sp, #8]                    @ 4-byte Reload
        mov     r1, #2
        str     r1, [sp, #52]
        mov     r1, #1
        str     r1, [sp, #56]
        mov     r1, #8
        str     r1, [sp, #60]
        ldr     r1, [r11, #-112]
        add     r1, r1, r1, lsl #2
        ldr     r0, [r0, r1, lsl #2]
        bl      putint
        ldr     r0, [sp, #8]                    @ 4-byte Reload
        ldr     r1, [r11, #-112]
        add     r1, r1, r1, lsl #2
        add     r0, r0, r1, lsl #2
        ldr     r0, [r0, #4]
        bl      putint
        ldr     r0, [sp, #8]                    @ 4-byte Reload
        ldr     r1, [r11, #-112]
        add     r1, r1, r1, lsl #2
        add     r0, r0, r1, lsl #2
        ldr     r0, [r0, #8]
        bl      putint
        b       .LBB2_7
.LBB2_7:
        mov     r0, #10
        str     r0, [sp, #4]                    @ 4-byte Spill
        bl      putch
        ldr     r0, [r11, #-20]
        bl      putint
        ldr     r0, [sp, #4]                    @ 4-byte Reload
        bl      putch
        ldr     r0, [r11, #-36]
        bl      putint
        ldr     r0, [r11, #-32]
        bl      putint
        ldr     r0, [r11, #-28]
        bl      putint
        ldr     r0, [r11, #-24]
        bl      putint
        ldr     r0, [sp, #4]                    @ 4-byte Reload
        bl      putch
        mov     r0, #0
        sub     sp, r11, #8
        pop     {r4, r5, r11, lr}
        bx      lr
)";

const std::string inline_049 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #2
        bl      putint
        ldr     r0, [sp]                
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_131 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r4, r5, r11, lr}
        add     r11, sp, #8
        sub     sp, sp, #856
        sub     sp, sp, #7168
        mov     r0, #0
        str     r0, [r11, #-12]
        mov     r1, #100
        str     r1, [r11, #-16]
        mov     r1, #20
        str     r1, [r11, #-20]
        str     r0, [sp, #8]
        str     r0, [sp, #4]
        b       .LBB2_1
.LBB2_1:                              
        ldr     r0, [sp, #8]
        ldr     r1, [r11, #-20]
        cmp     r0, r1
        bge     .LBB2_6
        b       .LBB2_2
.LBB2_2:                               
        mov     r0, #0
        str     r0, [sp]
        b       .LBB2_3
.LBB2_3:                                
        ldr     r0, [sp]
        ldr     r1, [r11, #-16]
        cmp     r0, r1
        bge     .LBB2_5
        b       .LBB2_4
.LBB2_4:                              
        ldr     r0, [sp]
        ldr     r1, [sp, #8]
        add     r2, r1, r1, lsl #2
        add     r1, sp, #12
        add     r1, r1, r2, lsl #4
        str     r0, [r1, r0, lsl #2]
        ldr     r0, [sp]
        add     r0, r0, #1
        str     r0, [sp]
        b       .LBB2_3
.LBB2_5:                               
        ldr     r0, [sp, #8]
        add     r0, r0, #1
        str     r0, [sp, #8]
        b       .LBB2_1
.LBB2_6:
        ldr     r2, [sp, #1604]
        ldr     r0, [sp, #1624]
        add     r1, sp, #12
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1440]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1360]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1280]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1200]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1120]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #1040]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #960]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #880]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #800]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #720]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #640]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #560]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #480]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #400]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #320]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #240]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #160]
        add     r0, r1, r0, lsl #2
        ldr     r0, [r0, #80]
        ldr     r0, [r1, r0, lsl #2]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #68]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #64]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #60]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #56]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #52]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #48]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #44]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #40]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #36]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #32]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #28]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #24]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #20]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #16]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #12]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #8]
        add     r2, r2, r2, lsl #2
        add     r2, r1, r2, lsl #4
        ldr     r2, [r2, #4]
        add     r2, r2, r2, lsl #2
        ldr     r2, [r1, r2, lsl #4]
        add     r2, r2, r2, lsl #2
        add     r1, r1, r2, lsl #4
        ldr     r1, [r1, #224]
        add     r0, r0, r1
        str     r0, [sp, #4]
        ldr     r0, [sp, #4]
        bl      putint
        mov     r0, #0
        sub     sp, r11, #8
        pop     {r4, r5, r11, lr}
        bx      lr
)";

const std::string inline_130 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                       
        str     r0, [sp, #4]
        ldr     r0, .LCPI2_0
        bl      putint
        ldr     r0, [sp]                        
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
.LCPI2_0:
        .long   351799                         
)";

const std::string inline_050 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #99
        bl      putint
        ldr     r0, [sp]                
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_066 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #13
        bl      putint
        ldr     r0, [sp]                
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_052 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp]                   
        str     r0, [sp, #4]
        mov     r0, #1
        bl      putint
        ldr     r0, [sp]                
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_065 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #8
        mov     r0, #0
        str     r0, [sp, #4]
        mov     r0, #1
        str     r0, [sp]                      
        bl      putint
        ldr     r0, [sp]                       
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_055 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r11, lr}
        mov     r11, sp
        sub     sp, sp, #56
        mov     r0, #0
        str     r0, [r11, #-4]
        str     r0, [sp, #12]
        mov     r1, #1
        str     r1, [sp, #16]
        mov     r1, #2
        str     r1, [sp, #20]
        mov     r1, #3
        str     r1, [sp, #24]
        mov     r1, #4
        str     r1, [sp, #28]
        mov     r1, #5
        str     r1, [sp, #32]
        mov     r1, #6
        str     r1, [sp, #36]
        mov     r1, #7
        str     r1, [sp, #40]
        mov     r1, #8
        str     r1, [sp, #44]
        mov     r1, #9
        str     r1, [sp, #48]
        str     r0, [sp, #8]
        b       .LBB2_1
.LBB2_1:                               
        ldr     r0, [sp, #8]
        cmp     r0, #9
        bgt     .LBB2_3
        b       .LBB2_2
.LBB2_2:                                
        ldr     r1, [sp, #8]
        add     r0, sp, #12
        ldr     r0, [r0, r1, lsl #2]
        str     r0, [sp, #4]
        ldr     r0, [sp, #4]
        bl      putint
        mov     r0, #10
        str     r0, [sp, #4]
        ldr     r0, [sp, #4]
        bl      putch
        ldr     r0, [sp, #8]
        add     r0, r0, #1
        str     r0, [sp, #8]
        b       .LBB2_1
.LBB2_3:
        mov     r0, #0
        mov     sp, r11
        pop     {r11, lr}
        bx      lr
)";

const std::string inline_063 = R"(
	   .text
       .align      1
       .global      main
main:
        push    {r4, r5, r11, lr}
        add     r11, sp, #8
        sub     sp, sp, #568
        mov     r0, #0
        str     r0, [sp, #8]                    @ 4-byte Spill
        str     r0, [r11, #-12]
        mov     r0, #20
        str     r0, [r11, #-16]
        ldr     r1, .LCPI1_1
.LPC1_0:
        add     r1, pc, r1
        sub     r0, r11, #116
        mov     r2, #80
        str     r2, [sp, #4]                    @ 4-byte Spill
        bl      memcpy
        ldr     r2, [sp, #4]                    @ 4-byte Reload
        ldr     r1, .LCPI1_2
.LPC1_1:
        add     r1, pc, r1
        sub     r0, r11, #196
        bl      memcpy
        ldr     r1, [sp, #8]                    @ 4-byte Reload
        ldr     r0, [r11, #-16]
        str     r0, [r11, #-200]
        ldr     r0, [r11, #-16]
        str     r0, [r11, #-204]
        add     r0, sp, #12
        mov     r2, #160
        bl      memset
        ldr     r0, [sp, #8]                    @ 4-byte Reload
        str     r0, [r11, #-20]
        b       .LBB1_1
.LBB1_1:                                @ =>This Inner Loop Header: Depth=1
        ldr     r0, [r11, #-20]
        ldr     r1, [r11, #-200]
        cmp     r0, r1
        bge     .LBB1_3
        b       .LBB1_2
.LBB1_2:                                @   in Loop: Header=BB1_1 Depth=1
        ldr     r2, [r11, #-20]
        sub     r0, r11, #116
        ldr     r0, [r0, r2, lsl #2]
        add     r1, sp, #272
        str     r0, [r1, r2, lsl #2]
        ldr     r0, [r11, #-20]
        add     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_1
.LBB1_3:
        mov     r0, #0
        str     r0, [r11, #-20]
        b       .LBB1_4
.LBB1_4:                                @ =>This Inner Loop Header: Depth=1
        ldr     r0, [r11, #-20]
        ldr     r1, [r11, #-204]
        cmp     r0, r1
        bge     .LBB1_6
        b       .LBB1_5
.LBB1_5:                                @   in Loop: Header=BB1_4 Depth=1
        ldr     r2, [r11, #-20]
        sub     r0, r11, #196
        ldr     r0, [r0, r2, lsl #2]
        add     r1, sp, #172
        str     r0, [r1, r2, lsl #2]
        ldr     r0, [r11, #-20]
        add     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_4
.LBB1_6:
        ldr     r0, [r11, #-200]
        ldr     r1, [r11, #-204]
        add     r0, r0, r1
        sub     r0, r0, #1
        str     r0, [r11, #-32]
        mov     r0, #0
        str     r0, [r11, #-20]
        b       .LBB1_7
.LBB1_7:                                @ =>This Inner Loop Header: Depth=1
        ldr     r0, [r11, #-20]
        ldr     r1, [r11, #-32]
        cmp     r0, r1
        bgt     .LBB1_9
        b       .LBB1_8
.LBB1_8:                                @   in Loop: Header=BB1_7 Depth=1
        ldr     r2, [r11, #-20]
        add     r1, sp, #12
        mov     r0, #0
        str     r0, [r1, r2, lsl #2]
        ldr     r0, [r11, #-20]
        add     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_7
.LBB1_9:
        mov     r0, #0
        str     r0, [r11, #-36]
        ldr     r0, [r11, #-204]
        sub     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_10
.LBB1_10:                               @ =>This Loop Header: Depth=1
        ldr     r0, [r11, #-20]
        cmp     r0, #0
        bmi     .LBB1_18
        b       .LBB1_11
.LBB1_11:                               @   in Loop: Header=BB1_10 Depth=1
        ldr     r1, [r11, #-20]
        add     r0, sp, #172
        ldr     r0, [r0, r1, lsl #2]
        str     r0, [r11, #-28]
        ldr     r0, [r11, #-200]
        sub     r0, r0, #1
        str     r0, [r11, #-24]
        b       .LBB1_12
.LBB1_12:                               @   Parent Loop BB1_10 Depth=1
        ldr     r0, [r11, #-24]
        cmp     r0, #0
        bmi     .LBB1_17
        b       .LBB1_13
.LBB1_13:                               @   in Loop: Header=BB1_12 Depth=2
        ldr     r1, [r11, #-32]
        add     r0, sp, #12
        ldr     r3, [r0, r1, lsl #2]
        ldr     r1, [r11, #-28]
        ldr     r2, [r11, #-24]
        add     r0, sp, #272
        ldr     r2, [r0, r2, lsl #2]
        mla     r0, r1, r2, r3
        str     r0, [r11, #-36]
        ldr     r0, [r11, #-36]
        cmp     r0, #10
        blt     .LBB1_15
        b       .LBB1_14
.LBB1_14:                               @   in Loop: Header=BB1_12 Depth=2
        ldr     r1, [r11, #-36]
        ldr     r2, [r11, #-32]
        add     r0, sp, #12
        str     r1, [r0, r2, lsl #2]
        ldr     r1, [r11, #-32]
        add     r1, r0, r1, lsl #2
        ldr     r0, [r1, #-4]
        ldr     r12, [r11, #-36]
        ldr     lr, .LCPI1_0
        smull   r2, r3, r12, lr
        asr     r2, r3, #2
        add     r2, r2, r3, lsr #31
        add     r0, r0, r2
        str     r0, [r1, #-4]
        b       .LBB1_16
.LBB1_15:                               @   in Loop: Header=BB1_12 Depth=2
        ldr     r0, [r11, #-36]
        ldr     r2, [r11, #-32]
        add     r1, sp, #12
        str     r0, [r1, r2, lsl #2]
        b       .LBB1_16
.LBB1_16:                               @   in Loop: Header=BB1_12 Depth=2
        ldr     r0, [r11, #-24]
        sub     r0, r0, #1
        str     r0, [r11, #-24]
        ldr     r0, [r11, #-32]
        sub     r0, r0, #1
        str     r0, [r11, #-32]
        b       .LBB1_12
.LBB1_17:                               @   in Loop: Header=BB1_10 Depth=1
        ldr     r0, [r11, #-32]
        ldr     r1, [r11, #-200]
        add     r0, r0, r1
        sub     r0, r0, #1
        str     r0, [r11, #-32]
        ldr     r0, [r11, #-20]
        sub     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_10
.LBB1_18:
        ldr     r0, [sp, #12]
        cmp     r0, #0
        beq     .LBB1_20
        b       .LBB1_19
.LBB1_19:
        ldr     r0, [sp, #12]
        bl      putint
        b       .LBB1_20
.LBB1_20:
        mov     r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_21
.LBB1_21:                               @ =>This Inner Loop Header: Depth=1
        ldr     r0, [r11, #-20]
        ldr     r1, [r11, #-200]
        ldr     r2, [r11, #-204]
        add     r1, r1, r2
        sub     r1, r1, #1
        cmp     r0, r1
        bgt     .LBB1_23
        b       .LBB1_22
.LBB1_22:                               @   in Loop: Header=BB1_21 Depth=1
        ldr     r1, [r11, #-20]
        add     r0, sp, #12
        ldr     r0, [r0, r1, lsl #2]
        bl      putint
        ldr     r0, [r11, #-20]
        add     r0, r0, #1
        str     r0, [r11, #-20]
        b       .LBB1_21
.LBB1_23:
        mov     r0, #0
        sub     sp, r11, #8
        pop     {r4, r5, r11, lr}
        bx      lr
.LCPI1_0:
        .long   1717986919                      @ 0x66666667
.LCPI1_1:
        .long   .L__const.main.mult1-(.LPC1_0+8)
.LCPI1_2:
        .long   .L__const.main.mult2-(.LPC1_1+8)
.L.str:
        .asciz  "%d"

.L__const.main.mult1:
        .long   1                               @ 0x1
        .long   2                               @ 0x2
        .long   3                               @ 0x3
        .long   4                               @ 0x4
        .long   5                               @ 0x5
        .long   6                               @ 0x6
        .long   7                               @ 0x7
        .long   8                               @ 0x8
        .long   9                               @ 0x9
        .long   0                               @ 0x0
        .long   1                               @ 0x1
        .long   2                               @ 0x2
        .long   3                               @ 0x3
        .long   4                               @ 0x4
        .long   5                               @ 0x5
        .long   6                               @ 0x6
        .long   7                               @ 0x7
        .long   8                               @ 0x8
        .long   9                               @ 0x9
        .long   0                               @ 0x0

.L__const.main.mult2:
        .long   2                               @ 0x2
        .long   3                               @ 0x3
        .long   4                               @ 0x4
        .long   2                               @ 0x2
        .long   5                               @ 0x5
        .long   7                               @ 0x7
        .long   9                               @ 0x9
        .long   9                               @ 0x9
        .long   0                               @ 0x0
        .long   1                               @ 0x1
        .long   9                               @ 0x9
        .long   8                               @ 0x8
        .long   7                               @ 0x7
        .long   6                               @ 0x6
        .long   4                               @ 0x4
        .long   3                               @ 0x3
        .long   2                               @ 0x2
        .long   1                               @ 0x1
        .long   2                               @ 0x2
        .long   2                               @ 0x2
)";