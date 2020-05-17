
.globl main
.globl _start
.globl CLR_IF
.globl SET_IF

_start:	b      main

     .macro MOV_PC_LR
       @bx lr
       mov pc,lr
     .endm

@void CLR_IF(void)@
CLR_IF:
    @This function works only if the processor is in previliged mode.
   mrs r0,cpsr
   bic r0,r0,#NOINT
   msr cpsr_cxsf,r0		
   MOV_PC_LR

@int SET_IF(void)@
@The return value is current CPSR.
SET_IF:
    @This function works only if the processor is in previliged mode.
   mrs r0,cpsr
   mov r1,r0
   orr r1,r1,#NOINT
   msr cpsr_cxsf,r1		
   MOV_PC_LR
