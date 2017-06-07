#include "process.h"
#include "page_table.h"
#include "kmalloc.h"
#include "vga.h"
#include "types.h"
#define NULL 0

//TODO: move these to interrupt.c
typedef void (*syscall_func) ();
syscall_func sys_impl[30];

struct Process *mainProcPtr = NULL;

struct Process *headProcPtr = NULL;
struct Process *tailProcPtr = NULL;

struct Process *scheduleHeadPtr = NULL;
struct Process *scheduleTailPtr = NULL;

struct Process *curr_proc; 
struct Process *next_proc;

//counter of proc_id
int id = 1;

void set_sys_handler(int num, syscall_func isr) {
    sys_impl[num] = isr;

}
void init_syscall_handler_table() {

    set_sys_handler(3, yield_isr);
    set_sys_handler(4, exit_isr);

    curr_proc = NULL;
    next_proc = NULL;
}

//add process to the main process linked list
void add_proc(Process *new) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    if (headProcPtr == NULL) {
        headProcPtr = new;
        tailProcPtr = new;
    }   
    else {
        tailProcPtr->next = new;
        tailProcPtr = new;
    }

    if (interrupts) {
        sti();
    }
}

//should only be called on a proc in the linked list, error check for this?
void remove_proc(Process *cur) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

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

    if (interrupts) {
        sti();
    }
}

//add process to scheduler linked list
void add_proc_to_scheduler(Process *new) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    //if the scheduleHeadPtr is pointing to the main process, overwrite
    //its just a placeholder for when there are no processes
    if (scheduleHeadPtr == mainProcPtr || !scheduleHeadPtr) {
        scheduleHeadPtr = new;
        scheduleTailPtr = new;
    }
    else {
        scheduleTailPtr->nextSched = new;
        new->prevSched = scheduleTailPtr;

        scheduleTailPtr = new;
    }

    if (interrupts) {
        sti();
    }
}

void remove_head_from_scheduler() {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }
    
    Process *temp = scheduleHeadPtr;

    if (scheduleHeadPtr != mainProcPtr) {
        
        scheduleHeadPtr = scheduleHeadPtr->nextSched;
        
        temp->prevSched = NULL;
        temp->nextSched = NULL;

        if (!scheduleHeadPtr) {
            //scheduleHeadPtr should never be null
            scheduleHeadPtr = mainProcPtr;
            //scheduleTailPtr doesn't matter because 
            //next time a proc is added with scheduleTailPtr
            //it'll see that scheduleHeadPtr is null and overwrite
        }
        else {
            scheduleHeadPtr->prevSched = NULL;
        }
    }
    else {
        //debug
        printk("scheduleHeadPtr is mainProcPtr in remove_head, shouldn't be happening\n");
    }

    if (interrupts) {
        sti();
    }
    
    
}

//creates a thread and adds it to proc & scheduler linked lists
struct Process* PROC_create_kthread(kproc_t entry_pt, void *arg) {

    
    Process *new = malloc(sizeof(struct Process));
    memset(new, 0, sizeof(struct Process));

    new->rip = entry_pt;
    new->cs = 0x08;
    new->rsp = alloc_stack_vpage(0); //rename alloc_new_stack() to make more sense?
    new->ss = 0;
    new->flags = 512;//flag with interrupts enabled;
    
    new->rsp = new->rsp - 8;
    *((uint64_t *) new->rsp) = kexit;

    new->rdi = arg;
    /*
    if (arg) {
        new->rdi = *(uint64_t *) arg;
    }*/
    new->pid = id++;

    //check all fields are filled
    new->nextSched = NULL;
    new->prevSched = NULL;
    new->nextBlock = NULL;
    new->next = NULL;
    new->blocked = FALSE;

    // add to main proc linked list
    add_proc(new);
    // add to scheduler linked list
    add_proc_to_scheduler(new);

    return new;
}

// starts everything running
// kthreads probably schedule prior to PROC_run being called
void PROC_run(void) {

    struct Process mainProc;

    mainProc.next = NULL;
    mainProc.nextSched = NULL;
    mainProc.prevSched = NULL;
    mainProc.pid = 1234;

    mainProcPtr = &mainProc;
    curr_proc = &mainProc;

    if (!scheduleHeadPtr) {
        scheduleHeadPtr = mainProcPtr;
    }

    yield();

    
}


void PROC_reschedule(void) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }
    
    //scheduleHeadPtr should only be NULL prior to any thread creation
    //if scheduleHeadPtr == mainProcPtr, there are no processes left
    if (scheduleHeadPtr == mainProcPtr) {
        if (!curr_proc || curr_proc->blocked) { 
            //if the current process is null or blocked, return to main context
            next_proc = scheduleHeadPtr;
            
        }
        else {
            //if it can still run, just run it again
            next_proc = curr_proc; //only necessary bc next_proc starts out null
        }
    }
    else {
        //if there are one or more processes left

        //set it to next proc and remove it from the scheduler
        next_proc = scheduleHeadPtr;
        remove_head_from_scheduler();

        if (!curr_proc || curr_proc->blocked || curr_proc == mainProcPtr) {
            //if curr_proc is blocked or exited, don't add it back into scheduler
        }
        else {
            //if the current process isn't blocked or didn't exit and isn't the mainProcPtr
            //essentially if it can still run, schedule it again

            add_proc_to_scheduler(curr_proc);
        }
    }

    if (interrupts) {
        sti();
    }
}

// yield is syscall 3
void yield(void) {
    syscall(0,0,3); 
}

void yield_isr() {
    PROC_reschedule();
}

//exit is syscall 4
void kexit(void) {
    syscall(0,0,4); 
}

void exit_isr() {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    //remove the process from the main linked list of procs TODO: check if these funcs are right
    remove_proc(curr_proc);
    //don't need to remove it from the scheduler because curr_proc is already not in the scheduler bc its running, unless its the last one and we just rescheduled it

    if (scheduleHeadPtr==curr_proc) {
        remove_head_from_scheduler();
    }

    //free curr_proc and everything in it
    //TODO: free stack at some point
    free(curr_proc);

    //IMPORTANT: curr_proc is only set to NULL upon exit
    curr_proc = NULL;
    

    //TODO: check assembly code

    // reschedule

    PROC_reschedule();

    if (interrupts) {
        sti();
    }

    //printk("exited with success!\n");

}




void syscall(int unused1, int unused2, int sys_num) {
    asm volatile("int $123");
}



void syscall_handler(int irq_num, int err, void *sys_num) {
    int num = *((int *) sys_num);

    //actually calls the system call
    sys_impl[num]();
}   


/* PROC MANAGEMENT CODE */

void PROC_init_queue(ProcessQueue *pq) {
    pq->head = NULL;
    pq->read = &pq->buffer[0];
    pq->write = &pq->buffer[0];
}

void PROC_block_on(ProcessQueue *pq, int enable_ints) {
    
    add_curr_to_block_queue(pq);
    //do not need to remove from scheduler because curr_proc is not in scheduler

    if (enable_ints) {
        sti();
    }

    yield();
}

void PROC_unblock_head(ProcessQueue *pq) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    add_proc_to_scheduler(pq->head);
    remove_blockq_head(pq);

    if (interrupts) {
        sti();
    }
    
}

//remove the head of the block queue and mark it was unblocked
void remove_blockq_head(ProcessQueue *pq) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    Process *temp = pq->head;
    pq->head = pq->head->nextBlock;

    temp->nextBlock = NULL;

    temp->blocked = FALSE;

    if (interrupts) {
        sti();
    }
}

//add curr_proc to the block queue and mark it as blocked
void add_curr_to_block_queue(ProcessQueue *pq) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    Process *iter;
    
    if (!pq->head) {
        pq->head = curr_proc;
    }
    else {
        iter = pq->head;
        while (iter->nextBlock) {
            iter = iter->nextBlock;
        }
        iter->nextBlock = curr_proc;
    }
    curr_proc->blocked = TRUE;

    if (interrupts) {
        sti();
    }
}




//TODO: this has never been tested
void PROC_unblock_all(ProcessQueue *pq) {
    while(pq->head) {
        PROC_unblock_head(pq);
    }
}

/*
void PROC_unblock_head(ProcessQueue *pq) {

    if (pq->head) {
        Process *unblocked = pq->head;
        pq->head = pq->head->nextBlock;
        
        unblocked->blocked = FALSE;
        add_proc_to_scheduler(unblocked);
        //PROC_reschedule();
    }
} */


/*
void PROC_block_on(ProcessQueue *pq, int enable_ints) {
    //remove curr_proc from the scheduler (it already isn't in the scheduler)
    //add it to the PQ of blocked processes
    if (!pq->head) {
        pq->head = curr_proc;       
    }
    else {
        Process *iter = pq->head;  
       while (iter->nextBlock) {
            iter = iter->nextBlock;
        }        
        iter->nextBlock = curr_proc;
        this line is a problem 
        curr_proc->nextBlock = NULL;
    }    
    if (curr_proc == scheduleHeadPtr && curr_proc == scheduleTailPtr) {
        //EDGE CASE: this condition is true when the current process is the last one to run
        //(??) what are we supposed to do when the last process to run blocks? return to the main intialization thread or block on it?       
        //option 1: this would cause it to return to the main context
        scheduleHeadPtr = NULL;    
    }

    //curr_proc = NULL;
    curr_proc->blocked = TRUE;

    if (enable_ints) {
        sti();
    }

    yield();
    
}
*/


/*
void exit_isr() {
    remove_proc(curr_proc); //removes proc from linked list
    // EDGE CASE: if the next scheduled process IS the current process,
    // set schedule ptrs to NULL
    // This is the special case where the current process is kept in the scheduler if its the last one that can run
    if (scheduleHeadPtr == curr_proc) {
        scheduleHeadPtr = NULL;
        scheduleTailPtr = NULL;
    }
    //free it and everything in it
    //should probably free stack at some point (the physical pages)
    free(curr_proc);
    //set curr_proc to NULL
    curr_proc = NULL;
    // reschedule
    PROC_reschedule();   
    //returns inside general asm handler
}
*/

/*
//every time someone calls reschedule its because yield has been called
//"just sets the next_proc ptr"
//sets next_proc and removes it from the scheduler
//adds curr_proc to the scheduler
void PROC_reschedule(void) {

    // If there are no more threads to run, set to main process pointer to "return"
    if (!scheduleHeadPtr) {
        next_proc = mainProcPtr;
    }
    else {
        //else, set next_proc to the next process in the scheduler
        next_proc = scheduleHeadPtr;
    
        //set scheduler pointers accordingly. this removes next proc from the scheduler
        //ad this point both curr_proc and next_proc are out of the scheduler
        scheduleHeadPtr = scheduleHeadPtr->nextSched;
        scheduleHeadPtr->prevSched = NULL;
        next_proc->prevSched = NULL; // just for clarity, not necessary
        next_proc->nextSched = NULL; // clarity


        if (!scheduleHeadPtr) {
            //if that was the last process in the scheduler
            //set both schedule ptrs to the curr_proc, which will now be the only process in the scheduler
            //EDGE CASE: curr_proc is still in the scheduler if and only if it is the last process that can run

            // if curr_proc is null, it just exited
            if (curr_proc && !curr_proc->blocked) {

                //if it is not null, add it back into the scheduler
                scheduleHeadPtr = curr_proc;
                scheduleTailPtr = curr_proc;

                curr_proc->prevSched = NULL;
                curr_proc->nextSched = NULL;
            }
            else if (curr_proc && curr_proc->blocked) {
                curr_proc->prevSched = NULL;
                curr_proc->nextSched = NULL;    

                //doing these just in case;
               // scheduleHeadPtr = NULL;
              //  scheduleTailPtr = NULL;           
            }
        }
        else {
            //if there are still processes to run other than the current process (curr_proc),
            //and if the process that called yield (curr_proc) isn't the main initializing process:
            //add the current process to the scheduler
            if (curr_proc && curr_proc != mainProcPtr && !curr_proc->blocked) {

                curr_proc->prevSched = scheduleTailPtr;
                curr_proc->nextSched = NULL;//just doing this in case
                scheduleTailPtr->nextSched = curr_proc;
                scheduleTailPtr = curr_proc;

            }
            
        }
    }
}
*/



