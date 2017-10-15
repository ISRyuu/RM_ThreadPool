/*  Kevin
 *  2017-10-12 */

#ifndef _IS_PRIORITY_QEUEUE_H
#define _IS_PRIORITY_QEUEUE_H

#define MAX_INC 4096
#define INIT_C  100

struct is_node;
struct is_queue;
  
typedef struct is_node* IS_node;
typedef struct is_queue* IS_queue;

int is_heap_parent(int i);
int is_heap_left(int i);
int is_heap_right(int i);

IS_queue is_create_queue();
//int is_build_max_heap();
void** is_heap_maximum(IS_queue heap);
void   is_heap_extract_max(IS_queue heap);
void   is_free_heap(IS_queue* heap);
int    is_max_heap_insert(IS_queue heap, int priority, void* usr_data);

void printh(IS_queue q);

#endif
