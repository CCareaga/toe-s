#include "sched.h"
#include "task.h"
#include "init.h"
#include "kmalloc.h"
#include "klib.h"
#include "vmm.h"
#include "timer.h"

extern uint32_t get_eip();
extern void ctx_swtch(context_t *old, context_t *new);
extern pg_dir_t *kern_dir;

typedef struct tlist {
    task_t *head;
} task_list_t;
    
task_list_t *ready;
task_t *current;

void add_task(task_t *tsk) {
    if (!ready->head) {
        ready->head = tsk;
        return;
    }
    
    task_t *tmp = ready->head;
    while (tmp->next) tmp = tmp->next;
    tmp->next = tsk;
}

task_t *pop_task() {
    if (!ready->head) return NULL;

    task_t *ret = ready->head;
    ready->head = ready->head->next;

    ret->next = NULL;
    ret->prev = NULL;
    return ret;
}

void tasking_init() {
    
    // here we create a new task that is basically the idle task
    // we allocate a stack for it and then copy the page directory
    // then we move the stack pointers
    uint32_t bottom = (0xC0000000 - 0x4000);
    allocate_stack((uint32_t)bottom, 0x4000, kern_dir);

    ready = kmalloc(sizeof(task_list_t));
    ready->head = NULL;

    task_t *init = create_task(&kidle);

    init->mem = copy_pg_dir(kern_dir);
    switch_page_directory(init->mem);

    relocate_stack((uint32_t *) bottom, 0x4000);

    asm volatile("mov %%esp, %0" : "=r" (init->ctx->esp));
    asm volatile("mov %%ebp, %0" : "=r" (init->ctx->ebp));

    current = init;
    add_task(init);
}

void schedule() {
    // don't do anything if we havent initialized tasking;

    if (!current)
        return;

    kprintf("schedulin' \n");
    task_t *new = pop_task();
    task_t *old = current;

    add_task(old);
    current = new;
     
    ctx_swtch(old->ctx, new->ctx);
    switch_page_directory(new->mem);
}

void switch_to(task_t *tsk) {   

}

// for now what fork does is create a new task that has
// a copy of the current tasks page directory, it is added
// to the task list to be scheduled
uint32_t fork() {

    task_t *child = create_task(NULL);
    child->parent = current;
    child->mem = copy_pg_dir(current->mem);
    asm volatile("mov %%esp, %0" : "=r" (child->ctx->esp));
    asm volatile("mov %%ebp, %0" : "=r" (child->ctx->ebp));
    
    // after this point either the parent or child
    // can be the current task
    add_task(child);

    if (current->tid == child->tid) {
        return 0;
    }
    else {
        return child->tid;
    }
}
