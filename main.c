#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "is_threadpool.h"

void
task(void *para) {
    sleep(1);
    printf("thread %ld: %s\n", (unsigned long)pthread_self(), (char*)para);
}

#define JOBS 100

int main(int argc, char const *argv[]) {

  RMThreadPool tp = RM_create_tpool(5);
  RMTask tasks[JOBS];
  char buff[100];

  for (size_t i = 0; i < JOBS; i++ ) {
    sprintf(buff, "hello %zu", i);
    tasks[i] = RM_task_enqueue(tp, task, buff, strlen(buff) + 1, i);
  }

  sleep(5);

  RM_free_tpool(&tp);
    
  for (size_t i = 0; i < JOBS; i++) {
    if( RM_task_done(tasks[i]) )
      printf("task %zu done\n", i);
    RM_free_task(&tasks[i]);
  }

  return 0;
}
