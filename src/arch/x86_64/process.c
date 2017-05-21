#include "process.h"

//TODO: move these to interrupt.c
typedef void (*syscall_func) ();
syscall_func sys_impl[30];

struct Process *mainProcPtr = NULL;

void PROC_create_kthread(kproc_t entry_pt, void *arg) {
    //malloc a new process "new"

    new->rip = entry_pt;
    new->cs = 0x08;
    new->rsp = alloc_stack_vpage(); //rename alloc_new_stack() to make more sense?
    new->ss = 0;
    new->flags = //get flags somehow with interrupts enabled;
    new->rdi = arg;
    *new->rsp-- = kexit;
    
}

void PROC_run(void) {
    struct Process mainProc;
    mainProcPtr = &mainProc;
    curr_proc = &mainProc;
    yield();
}

void PROC_reschedule(void) {

}

void yield(void) {
    syscall(0,0,3); // yield is syscall 3
}

void yield_isr() {
    PROC_reschedule();
}

void kexit(void) {
    syscall(0,0,4); //exit is syscall 4
}

void exit_isr() {
    //remove cur proc
    //free it and everything in it
    // reschedule
    //returns inside general asm handler

}



void syscall(int unused1, int unused2, int sys_num) {
    asm volatile("int $123");
}

void syscall_handler(int irq_num, int err, int sys_num) {
    //need to alter common_c_handler to have 3 params, but others don't use the third
    
}


