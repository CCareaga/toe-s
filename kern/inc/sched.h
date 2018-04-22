#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

void switch_to(task_t *tsk);
void tasking_init();
void schedule();
void add_task(task_t *tsk);

#endif
