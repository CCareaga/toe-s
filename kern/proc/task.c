#include "task.h"
#include "kmalloc.h"

uint32_t next_tid = 0;

task_t *create_task(entry_t entry) {
    task_t *tsk = kmalloc(sizeof(task_t));
    tsk->tid = next_tid++;
    tsk->entry = entry;
    tsk->ctx = kmalloc(sizeof(context_t));

    return tsk;
}
