#include "process.h"
#include "page_table.h"
#include "kmalloc.h"
#include "vga.h"
#define NULL 0

//TODO: move these to interrupt.c
typedef void (*syscall_func) ();
syscall_func sys_impl[30];

struct Process *mainProcPtr = NULL;

struct Process *headProcPtr = NULL;
struct Process *tailProcPtr = NULL;

struct Process *scheduleHeadPtr = NULL;
struct Process *scheduleTailPtr = NULL;

struct Process testProc1;
struct Process testProc2;



struct Process *curr_proc; 
struct Process *next_proc;


int id = 0;

void set_sys_handler(int num, syscall_func isr) {
    sys_impl[num] = isr;

}
void init_syscall_handler_table() {

    set_sys_handler(3, yield_isr);
    set_sys_handler(4, exit_isr);

    curr_proc = NULL;
    next_proc = NULL;

    //test for debugging

    //testProc1.r15 = 7;
    //testProc2.r15 = 12;

    //curr_proc = &testProc1;
    //next_proc = &testProc2;
    
}

//add process to the main process linked list
void add_proc(Process *new) {

    if (headProcPtr == NULL) {
        headProcPtr = new;
        tailProcPtr = new;
    }   
    else {
        tailProcPtr->next = new;
        tailProcPtr = new;
    }
}

//add process to scheduler linked list
void add_proc_to_scheduler(Process *new) {
    if (scheduleHeadPtr == NULL) {
        scheduleHeadPtr = new;
        scheduleTailPtr = new;
    }
    else {
        scheduleTailPtr->nextSched = new;
        new->prevSched = scheduleTailPtr;

        scheduleTailPtr = new;
    }
}

//creates a thread and adds it to the end of the scheduler list
struct Process* PROC_create_kthread(kproc_t entry_pt, void *arg) {

    //printk("PROC_create_kthread is called\n");

    //malloc a new process "new"
    

    
    Process *new = malloc(sizeof(struct Process));

    memset(new, 0, sizeof(struct Process));
    
    new->rip = entry_pt;
    new->cs = 0x08;
    new->rsp = alloc_stack_vpage(0); //rename alloc_new_stack() to make more sense?
    new->ss = 0;
    new->flags = 0;//get flags somehow with interrupts enabled;
    new->rdi = arg;
    new->rsp = new->rsp - 8;
    *((uint64_t *) new->rsp) = kexit;
    //

    /*
    int loop = 1;
    while(loop) {

    }*/
    new->next = NULL;

    new->pid = id++;

    //check all fields are filled

    new->nextSched = NULL;
    new->prevSched = NULL;

    // add to main proc linked list
    add_proc(new);

    /*
    int loop = 1;
    while (loop) {

    }*/
    // add to scheduler linked list
    add_proc_to_scheduler(new);

    /*
    int loop = 1;
    while (loop) {  
    }*/

    return new;
}

//starts everything
void PROC_run(void) {

    struct Process mainProc;

    mainProc.next = NULL;
    mainProc.nextSched = NULL;
    mainProc.prevSched = NULL;
    mainProc.pid = 1234;

    mainProcPtr = &mainProc;
    curr_proc = &mainProc;

    yield();

    printk("back to main\n");
}


//every time someone calls reschedule its because yield has been called
//"just sets the next_proc ptr"

//sets next_proc and removes it from the scheduler
//adds curr_proc to the scheduler
void PROC_reschedule(void) {

    //TODO: dont forget about edge cases of just 1 or 2 procs (??)
    

    //if no more threads to run
    if (!scheduleHeadPtr) {

        next_proc = mainProcPtr;
    }
    else {
        
        
        next_proc = scheduleHeadPtr;
        scheduleHeadPtr = scheduleHeadPtr->next;
        scheduleHeadPtr->prevSched = NULL;

        //this probs isn't necessary
        next_proc->prevSched = NULL;
        next_proc->nextSched = NULL;

        if (!scheduleHeadPtr) {
            //if that was the last process in the scheduler, set both schedule ptrs to the curr_proc, which will now be the only proc in the list

            // if curr_proc is null that means it just exited
            if (curr_proc) {

                scheduleHeadPtr = curr_proc;
                scheduleTailPtr = curr_proc;

                curr_proc->prevSched = NULL;
                curr_proc->nextSched = NULL;
            }
        

        }
        else {


            if (curr_proc && curr_proc != mainProcPtr) {

                /*
                int loop = 1;
                while (loop) {  
                }*/
        
                curr_proc->prevSched = scheduleTailPtr;
                curr_proc->nextSched = NULL;//just doing this in case

                scheduleTailPtr->next = curr_proc;
                scheduleTailPtr = curr_proc;

            }
        }
    }
}

// yield is syscall 3
void yield(void) {
    syscall(0,0,3); 
}

void yield_isr() {
   // printk("yield isr called\n");
    PROC_reschedule();
}

//exit is syscall 4
void kexit(void) {
    syscall(0,0,4); 
}

void exit_isr() {
    //TODO: set this up on TSS
    //printk("exit isr called\n");

    /*
    int loop = 1;
    while(loop) {
    }*/

    remove_proc(curr_proc); //removes proc from linked list
    
    if (scheduleHeadPtr == curr_proc) {
        scheduleHeadPtr = NULL;
        scheduleTailPtr = NULL;
    }
    free(curr_proc);
    //free it and everything in it
    curr_proc = NULL;

    

    // reschedule
    PROC_reschedule();
    
    //returns inside general asm handler
    /*
    int loop = 1;
    while(loop) {
    }*/
}

//should only be called on a proc in the linked list, error check for this?
void remove_proc(Process *cur) {

    /*int loop = 1;
    while (loop) {  
    }*/
    Process *procIter = headProcPtr;
    Process *prevProc = headProcPtr;

    while (procIter->pid != cur->pid) {
        prevProc = procIter;
        procIter = procIter->next;
    }

    if (procIter == prevProc) {// is the first one in the list
        headProcPtr = headProcPtr->next;
    }
    else {
        prevProc->next = procIter->next;
    }

}

void syscall(int unused1, int unused2, int sys_num) {
    asm volatile("int $123");
}



void syscall_handler(int irq_num, int err, void *sys_num) {

    int num = *((int *) sys_num);
    //printk("system call %d\n", num);

 

    //actually calls the system call
    sys_impl[num]();

    /*
    int loop = 1;
    while (loop) {

    }*/

}   


