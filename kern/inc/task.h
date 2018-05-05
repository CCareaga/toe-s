#ifndef _TASK_H
#define _TASK_H

#include "types.h"
#include "x86.h"

typedef void (*entry_t)();

typedef struct task {
    uint32_t    tid;
    entry_t     entry;
    context_t   *ctx;
    pg_dir_t    *mem;

    struct task *next;
    struct task *prev;
} task_t;

task_t *create_task(entry_t entry);

#endif
