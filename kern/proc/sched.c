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
    // relocate the kernels stack to start below the kernel
    relocate_stack((uint32_t *) (0xC0000000 - 0x4000), 0x4000);

    ready = kmalloc(sizeof(task_list_t));
    ready->head = NULL;

    task_t *init = create_task(&kidle);
    init->mem = kern_dir;
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

