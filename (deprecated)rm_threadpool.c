//
//  rm_threadpool.c
//  rm_threadpool
//
//  Created by Kevin on 2017/3/29.
//  Copyright © 2017年 Kevin. All rights reserved.
//

#include "rm_threadpool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static RMTask PopTask(RMTaskQueue);
static void* thread_template(void *arg);

struct RMTask {
  Runner       runner;
  void         *argument;
  sig_atomic_t done;
  /* next pointer is ok since struct is invisible to users */
  struct RMTask *next;
};

struct RMThread {
  pthread_t t_id;
  struct    RMThread* next;
};

struct RMTaskQueue {
  /* FIFO */
  RMTask head;
  RMTask end;
  /* mutex for modifying queue and cond */
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
};

struct RMThreadPool {
  RMThread    threads;
  RMTaskQueue tqueue;
  /* exit flag */
  volatile sig_atomic_t run;
};

#define ERR_RETURN(x, y) \
do { perror((x)); return (y); } while (0)

void
clean(void *arg)
{
    printf("thread clean: %ld\n", (unsigned long)arg);
}

void*
thread_template(void *arg)
{
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pthread_cleanup_push(clean, pthread_self());
  RMThreadPool tpool  = (RMThreadPool)arg;
  RMTaskQueue  tqueue = tpool->tqueue;
  while ( 1 ) {
    pthread_mutex_lock(&tqueue->mutex);
    RMTask task = NULL;
    /* if signal comes but no task can be fetched, resume waiting.*/
    while ( !(task = PopTask(tqueue)) && tpool->run )
      pthread_cond_wait(&tqueue->cond, &tqueue->mutex);
    pthread_mutex_unlock(&tqueue->mutex);
    /* should exit */
    if (!tpool->run)
      pthread_exit((void*)0);
    Runner run = task->runner;
    void *arg  = task->argument;
    task->done = 0;
    run(arg);
    /* task done */
    task->done = 1;
  }

  pthread_exit((void*)0);
  pthread_cleanup_pop(0);
}

RMTask
PopTask(RMTaskQueue tqueue)
{
  /* PopTask itself doesnt lock any mutex before modify TaskQueue,
   * this should provided by user(e.g: thread_template()) */
  RMTask task = NULL;
  if ( tqueue->head ) {
    task = tqueue->head;
    tqueue->head = tqueue->head->next;
    /* if the last task is poped(now queue is empty),
     * should set end to null too */
    if (!tqueue->head)
      tqueue->end = NULL;
  }
  return task;
}

RMThreadPool
RM_create_tpool(size_t count)
{
  RMThreadPool tpool = (RMThreadPool)malloc(sizeof(struct RMThreadPool));
  if (tpool == NULL)
    /* doesn't allocate any thing, directly return no clean job is needed. */
    ERR_RETURN("cannot malloc", NULL);
  tpool->threads = NULL;
  tpool->run     = 1;
  if ( (tpool->tqueue = (RMTaskQueue)malloc(sizeof(struct RMTaskQueue))) == NULL ) {
    perror("cannot malloc");
    goto clean;
  }

  RMTaskQueue queue = tpool->tqueue;
  queue->head = queue->end = NULL;
  pthread_mutex_init(&queue->mutex, NULL);
  pthread_cond_init(&queue->cond, NULL);

  RMThread *thread = &tpool->threads;

  for (size_t i = 0; i < count; i++) {
    RMThread new = (RMThread)malloc(sizeof(struct RMThread));
    if ( new == NULL ) {
      perror("cannot malloc");
      goto clean;
    }
    /* null represents end of threads, also, set 'next' to null and append to threads
     * (i.e: *thread = new)as soon as allocated the memory guarantees the clean process
     * correctly frees all memory when pthread_create fail.
     */
    new->next = NULL;
    *thread   = new;
    thread    = &new->next;

    if( pthread_create(&new->t_id, NULL, thread_template, (void*)tpool) != 0 ) {
      perror("cannot create thread");
      goto clean;
    }
  }

  return tpool;

  clean:
    RM_free_tpool(&tpool);
    return NULL;
}

RMTask
RM_task_enqueue(RMThreadPool tpool, Runner runner, void* arg, size_t arg_size)
{
  RMTask new = (RMTask)malloc(sizeof(struct RMTask));

  if (new == NULL)
    ERR_RETURN("cannot malloc", NULL);

  new->runner = runner;
  new->done   = 0;
  new->next   = NULL;
  if ( (new->argument = malloc(arg_size)) == NULL ) {
    free(new);
    ERR_RETURN("cannot malloc", NULL);
  }
  memcpy((char*)new->argument, (char*)arg, arg_size);
  pthread_mutex_t *mutex = &tpool->tqueue->mutex;
  pthread_cond_t  *cond  = &tpool->tqueue->cond;

  pthread_mutex_lock(mutex);

  RMTask *end  = &tpool->tqueue->end;
  RMTask *head = &tpool->tqueue->head;

  if (*end)
    (*end)->next = new;
  else
    /* if queue is empty, we need initialize header too */
    *head = new;

  /* set end to new whether queue is empty or not */
  *end = new;
  RMTask t = *head;

  /* new task, notify an idle(if any) thread */
  pthread_cond_signal(cond);
  pthread_mutex_unlock(mutex);

  return new;
}

void
RM_free_tpool(RMThreadPool *tpool)
{
  RMThread t = NULL;
  RMThreadPool tp = *tpool;

  /* change run flag and notify all idle(blocked by cond) thread to exit */
  pthread_mutex_lock(&tp->tqueue->mutex);
  tp->run = 0;
  pthread_cond_broadcast(&tp->tqueue->cond);
  pthread_mutex_unlock(&tp->tqueue->mutex);

  /* waiting for all thread exit */
  while(tp->threads) {
    t = tp->threads;
    pthread_join(tp->threads->t_id, NULL);
    tp->threads = tp->threads->next;
    free(t);
  }

  if (tp->tqueue) {
    pthread_mutex_destroy(&tp->tqueue->mutex);
    pthread_cond_destroy(&tp->tqueue->cond);
    free(tp->tqueue);
  }

  free(tp);
  *tpool = NULL;
}

void
RM_free_task(RMTask *task)
{
  if (*task) {
    if ((*task)->argument)
      free((*task)->argument);
    free(*task);
  }
  *task = NULL;
}

sig_atomic_t
RM_task_done(RMTask task)
{
  return task->done;
}
