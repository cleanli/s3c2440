
.globl main
.globl _start
.globl disable_arm_interrupt
.globl enable_arm_interrupt
.globl get_psr
.globl w_psr
.globl mass_adc_get

_start:	b      main
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef

.globl _stack_top
_stack_top:
	.word _start + 0x40000
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word _start + 0x50000


     .macro MOV_PC_LR
       @bx lr
       mov pc,lr
     .endm

@Interrupt, IRQ en/disable
#define NOINT 0x80 @ 1000 0000

@void enable_arm_interrupt(void)@
enable_arm_interrupt:
    @This function works only if the processor is in previliged mode.
   mrs r0,cpsr
   bic r0,r0,#NOINT
   msr cpsr_cxsf,r0		
   MOV_PC_LR

@void disable_arm_interrupt(void)@
disable_arm_interrupt:
    @This function works only if the processor is in previliged mode.
   mrs r0,cpsr
   mov r1,r0
   orr r1,r1,#NOINT
   msr cpsr_cxsf,r1		
   MOV_PC_LR

get_psr:
	mrs 	r0, cpsr
	mov	pc, lr

w_psr:
	msr 	cpsr, r0
	mov	pc, lr

mass_adc_get:
    push {r2,r3}

    mov r3, #0x58000000
    //while(!(rADCCON & 0x8000));
wait_adc_done:
    ldr r2, [r3]
    and r2, r2, #0x8000
    cmp r2, #0
    beq wait_adc_done

    //adc_data1[tct++] = rADCDAT0
    ldr r2, [r3, #0x0c]
    str r2, [r0], #4

    //rADCCON|=0x1
trigger_adc:
    ldr r2, [r3]
    orr r2, r2, #1
    str r2, [r3]

    sub r1, r1, #1
    cmp r1, #0
    bne wait_adc_done

    pop {r2,r3}
    mov pc, lr
/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */

@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC	0x13
#define I_BIT		0x80
/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

#define CONFIG_STACKSIZE (128*1024)
#define CONFIG_STACKSIZE_IRQ    (4*1024)
#define CONFIG_ENV_SIZE     0x20000
#define CONFIG_SYS_MALLOC_LEN       (CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE    128
	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	ldr	r2, _stack_top
	sub	r2, r2, #(CONFIG_STACKSIZE)
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	/* set base 2 words into abort stack */
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE+8)
	ldmia	r2, {r2 - r3}			@ get pc, cpsr
	add	r0, sp, #S_FRAME_SIZE		@ restore sp_SVC

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r7, sp, #S_PC
	stmdb	r7, {sp, lr}^			@ Calling SP, LR
	str	lr, [r7, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r7, #4]			@ Save CPSR
	str	r0, [r7, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	/* return & move spsr_svc into cpsr */
	subs	pc, lr, #4
	.endm

	.macro get_bad_stack
	ldr	r13, _stack_top  @ setup our mode stack
	sub	r13, r13, #(CONFIG_STACKSIZE)
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)
	/* reserve a couple spots in abort stack */
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE+8)

	str	lr, [r13]			@ save caller lr / spsr
	mrs	lr, spsr
	str	lr, [r13, #4]

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13
	mov	lr, pc
	movs	pc, lr
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	@ldr	sp, FIQ_STACK_START
	.endm
/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	@bl	do_undefined_instruction
    b .

	.align	5
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	@bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	@bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	@bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	@bl	do_not_used

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
/*
	ldr	r0, =0x50000020
	mov	r1, #0x35
	str	r1, [r0]
*/
	bl	do_irq
	irq_restore_user_regs

fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	@bl	do_fiq
	@irq_restore_user_regs
