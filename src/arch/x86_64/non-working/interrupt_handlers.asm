extern irq_c_handler

global irq_handler1


irq_handler1:
    ;about to use rdi
    push rdi
    mov rdi, 1

    jmp common_irq_handler    

irq_gpf_handler:
    push rsi
    mov rsi, [rsp + 8]
    mov [rsp + 8], [rsp]

    add rsp, 8
    
    push rdi
    move rdi, 11
    jmp common_irq_handler


common_irq_handler:

    pushad

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15


    call irq_c_handler
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    popad

    pop rsi
    pop rdi

    iretq
        
