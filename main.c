#include "rm_threadpool.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void
task(void *para) {
    printf("thread %ld: %s\n", (unsigned long)pthread_self(), (char*)para);
    sleep(1);
}

#define JOBS 100

int main(int argc, char const *argv[]) {
  RMThreadPool tp = RM_create_tpool(5);
  RMTask tasks[JOBS];
  char buff[100];

  for (size_t i = 0; i < JOBS; i++ ) {
    sprintf(buff, "hello %zu", i);
    tasks[i] = RM_task_enqueue(tp, task, buff, strlen(buff) + 1);
  }

  sleep(1);

  for (size_t i = 0; i < JOBS; i++) {
    if( RM_task_done(tasks[i]) )
      printf("task %zu done\n", i);
    RM_free_task(tasks + i);
  }

  RM_free_tpool(&tp);
  return 0;
}
