#include "task.h"
#include "x86.h"
#include "kmalloc.h"

struct task {
    uint32_t    tid;
    entry_t     entry;
    tregs_t     ctx;

    struct task *next;
    struct task *prev;
};

uint32_t next_tid = 0;

task_t *create_task(entry_t entry) {
    task_t *tsk = kmalloc(sizeof(tsk));
    tsk->tid = next_tid++;
    tsk->entry = entry;

    return tsk;
}
