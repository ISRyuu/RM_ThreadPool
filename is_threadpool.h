//
//  rm_threadpool.h
//  rm_threadpool
//
//  Created by Kevin on 2017/3/29.
//  Copyright © 2017年 Kevin. All rights reserved.
//

#ifndef _RM_THREAD_POOL_H
#define _RM_THREAD_POOL_H

#include <signal.h>

typedef void (*Runner)(void* para);

struct _RMThread;
struct _RMTask;
struct _RMTaskQueue;
struct _RMThreadPool;

typedef struct _RMTask   *RMTask;
typedef struct _RMThread *RMThread;
typedef struct _RMTaskQueue  *RMTaskQueue;
typedef struct _RMThreadPool *RMThreadPool;

RMThreadPool  RM_create_tpool(size_t threads);
RMTask        RM_task_enqueue(RMThreadPool, Runner runner, void* arg, size_t arg_size, unsigned int priority);
sig_atomic_t  RM_task_done(RMTask task);
void          RM_free_task(RMTask *task);
void          RM_free_tpool(RMThreadPool *tpool);
void          RM_bs_insert(RMThreadPool tpool, RMTask task);
#endif
