/******************************************************************************
 * @file startup.s
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Startup Code for VPTemplate Application Project
 *
 *
 *****************************************************************************/
 
 .syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

/**
 * @brief  This is the code that gets called when the processor first
 * starts execution following a reset event. Only the absolutely
 * necessary set is performed, after which the application
 * supplied main() routine is called.
 *
 * @param  None
 *
 * @retval : None
 */

/* SCB VTOR register address (Cortex-M4) */
.equ SCB_VTOR, 0xE000ED08

.section .text.StartApp_Handler
.type StartApp_Handler, %function
.global StartApp_Handler
StartApp_Handler:
    /* Relocate Vector Table because .signature is before .isr_vector */
    ldr r0, =_app_vector_table      /* computed constant below */
    ldr r1, =SCB_VTOR
    str r0, [r1]

    dsb
    isb

    /* Copy the data segment initializers from flash to SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sloaddata
    movs r3, #0
    b .loopCopyData

MAGIC_STACK_VALUE:
	.word 0xDEADBEEF

.copyData:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

.loopCopyData:
    adds r4, r0, r3
    cmp r4, r1
    bcc .copyData

    /* Zero fill the bss segment. */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b .loopFillZerobss

.fillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

.loopFillZerobss:
    cmp r2, r4
    bcc .fillZerobss

   ldr r0, = _bottom_of_stack
   ldr r1, = _top_of_stack
   ldr r2, = MAGIC_STACK_VALUE
   ldr r2, [r2]
   b .loopFillStack

.fillStack:
   str  r2, [r0]
   adds r0, r0, #4

.loopFillStack:
   cmp r0, r1
   bne .fillStack    
   bcc .fillStack    

    /* Initialize the Stack-Pointer */
    /* Load address of initial_stack_pointer into R0 for. Symbol defined in Linker Script */
    ldr r0, =_initial_stack_pointer
    /* Set stack pointer */
    mov   sp, r0

    /* Call the clock system intitialization function.*/
    bl  SystemInit

    /* Call the application's entry point.*/
    bl main
    bx lr
.size StartApp_Handler, .-StartApp_Handler


/* ---- Vector table address constant ----
   This MUST match your linker layout:
   FLASH origin = 0x08010000
   .signature   = 4 bytes
   => .isr_vector begins at 0x08010004

   If signature size changes, update this or (better) export a linker symbol for it.
*/
_app_vector_table = 0x08010200