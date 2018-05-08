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
    return ret;
}


void tasking_init() {

    // here we allocate a stack outside of the kernel addr range
    // this just maps in pages that will get copied to other pg dirs
    uint32_t bottom = (0xC0000000 - 0x4000);
    allocate_stack((uint32_t)bottom, 0x4000, kern_dir);

    ready = kmalloc(sizeof(task_list_t));
    ready->head = NULL;

    task_t *init = create_task(&kidle);

    // give this process a new page directory with the kernel mapped in
    init->mem = copy_pg_dir(kern_dir);
    switch_page_directory(init->mem);

    // move that stack to the space we allocated earlier.. just for testing
    relocate_stack((uint32_t *) bottom, 0x4000);

    asm volatile("mov %%esp, %0" : "=r"(init->ctx->esp));
    // init->ctx->eip = init->entry;
    current = init;

    ctx_swtch(init->ctx, init->ctx);
}

void schedule() {
    // don't do anything if we havent initialized tasking;

    if (!current)
        return;

    kprintf("schedulin' \n");
    task_t *new = pop_task();
    task_t *old = current;

    add_task(current);
    current = new;
     
    ctx_swtch(old->ctx, new->ctx);
}

void switch_to(task_t *tsk) {   

}

