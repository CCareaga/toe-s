#include "sched.h"
#include "task.h"
#include "init.h"
#include "kmalloc.h"

typedef struct tlist {
    task_t *head;
} task_list_t;
    
task_t *current;

task_list_t *ready;

void tasking_init() {
    ready = kmalloc(sizeof(task_list_t));
    ready->head = NULL;

    task_t *init = create_task(&init_entry);
    ready->head = init;
}

void schedule() {
     
}

void add_task(task_t *tsk) {

}

void switch_to(task_t *tsk) {

}
