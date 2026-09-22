.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.type Reset_Handler, %function
.extern SystemInit
.extern __libc_init_array
.extern main
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss
.extern USART1_IRQHandler

.section .isr_vector, "a", %progbits
.word _estack
.word Reset_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.rept 37
.word Default_Handler
.endr
.word USART1_IRQHandler
.rept 22
.word Default_Handler
.endr

.section .text.Reset_Handler, "ax", %progbits
.thumb_func
Reset_Handler:
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
0:
    cmp r1, r2
    bcc 2f
    b 3f
2:
    ldr r3, [r0], #4
    str r3, [r1], #4
    b 0b
3:
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0
4:
    cmp r1, r2
    bcc 5f
    b 6f
5:
    str r3, [r1], #4
    b 4b
6:
    bl SystemInit
    bl __libc_init_array
    bl main
1:
    b 1b
.size Reset_Handler, .-Reset_Handler

.section .text.Default_Handler, "ax", %progbits
.thumb_func
Default_Handler:
1:
    b 1b
.size Default_Handler, .-Default_Handler
