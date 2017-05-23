#include <stdint.h>
typedef struct Process {

    //start stack vars
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rsi;
    uint64_t rdi;
    //already on stack ^^
    uint64_t cs;
    uint64_t ss;
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;

    uint64_t rsp;
    uint64_t rip;
    uint64_t flags;
    //uint64_t cr3; //aka page table
    //other control registers?
    
    uint64_t proc_id;
    struct Process *next;
    struct Process *nextSched;
    struct Process *prevSched;
} Process;



typedef void (*kproc_t)(void *);
extern void PROC_create_kthread(kproc_t entry_pt, void *arg);

extern void PROC_run(void);
extern void PROC_reschedule(void);
extern void yield(void);
extern void kexit(void);

void yield_isr();
void exit_isr();

void syscall_handler(int irq_num, int err, void *sys_num);

void syscall(int unused1, int unused2, int sys_num);

void remove_proc(Process *cur);

extern void init_syscall_handler_table();

