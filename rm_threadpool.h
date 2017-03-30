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

struct RMThread;
struct RMTask;
struct RMTaskQueue;
struct RMThreadPool;

typedef struct RMTask   *RMTask;
typedef struct RMThread *RMThread;
typedef struct RMTaskQueue  *RMTaskQueue;
typedef struct RMThreadPool *RMThreadPool;

RMThreadPool  RM_create_tpool(size_t count);
RMTask        RM_task_enqueue(RMThreadPool, Runner runner, void* arg, size_t arg_size);
sig_atomic_t  RM_task_done(RMTask task);
void          RM_free_task(RMTask *task);
void          RM_free_tpool(RMThreadPool *tpool);

#endif
