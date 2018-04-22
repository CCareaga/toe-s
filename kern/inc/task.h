#ifndef _TASK_H
#define _TASK_H

#include "types.h"

typedef struct task task_t;
typedef void (*entry_t)();

task_t *create_task(entry_t entry);

#endif
