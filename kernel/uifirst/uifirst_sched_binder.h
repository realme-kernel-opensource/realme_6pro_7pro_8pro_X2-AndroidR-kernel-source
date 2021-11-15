/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2020 Oplus. All rights reserved.
 */


#ifndef _OPPO_CFS_BINDER_H_
#define _OPPO_CFS_BINDER_H_
#include "uifirst_sched_common.h"
static inline void binder_set_inherit_ux(struct task_struct *thread_task, struct task_struct *from_task)
{
	if (from_task && test_set_dynamic_ux(from_task) && !test_task_ux(thread_task)) {
		dynamic_ux_enqueue(thread_task, DYNAMIC_UX_BINDER, from_task->ux_depth);
	}
#ifdef CONFIG_CAMERA_OPT
	else if (from_task && from_task->camera_opt) {
		thread_task->camera_opt = 1;
	}
#endif
}

static inline void binder_unset_inherit_ux(struct task_struct *thread_task)
{
	if (test_dynamic_ux(thread_task, DYNAMIC_UX_BINDER)) {
		dynamic_ux_dequeue(thread_task, DYNAMIC_UX_BINDER);
	}
#ifdef CONFIG_CAMERA_OPT
	else if (thread_task->camera_opt) {
		thread_task->camera_opt = 0;
	}
#endif
}
#endif
