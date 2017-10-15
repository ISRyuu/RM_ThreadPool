/*  Kevin
 *  2017-10-12 */

#include "is_priority_queue.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

int is_max_heapity(IS_queue heap, int i);
int is_increase_priority(IS_queue heap, int i, int priority);

struct is_node {
  /* priority must be positive 
     zero is reserved for internal use. */
  unsigned int priority;
  unsigned long _index;
  void* usr_data;
};

struct is_queue {
  IS_node *nodes;
  unsigned long counter;
  unsigned int capacity;
  unsigned int heap_size;
};
  
#define is_heap_parent(i) (((i)+1)/2 - 1)

#define is_heap_left(i)   (((i)+1)*2 - 1)

#define is_heap_right(i)  (((i)+1)*2)

void
swap(IS_node *node1, IS_node *node2)
{
  IS_node nodet = *node1;
  *node1 = *node2;
  *node2 = nodet;
}

IS_queue
is_create_queue()
{
  IS_queue heap = (IS_queue)malloc(sizeof(struct is_queue));
  if (heap == NULL)
    return NULL;
  
  heap->nodes = (IS_node*)malloc(sizeof(IS_node) * INIT_C);
  if (heap->nodes == NULL) {
    free(heap);
    return NULL;
  }

  heap->heap_size = 0;
  heap->capacity  = INIT_C;
  heap->counter = 0;
  return heap;
}

void**
is_heap_maximum(IS_queue heap)
{
  if ( !heap || heap->heap_size <= 0 )
    return NULL;
  return &heap->nodes[0]->usr_data;
}

int
is_max_heapify(IS_queue heap, int i)
{
  if ( !heap ) return -1;
  int left, right;    
  int largest    = i;
  IS_node *nodes = heap->nodes;
  
  while ( 1 ) {
    
    left  = is_heap_left(i);
    right = is_heap_right(i);
    
    if ( left < heap->heap_size &&
	 nodes[largest]->priority <= nodes[left]->priority ) {
      /* FIFO for the same priorities. */
      if ( (nodes[largest]->priority < nodes[left]->priority) ||
	  (nodes[largest]->_index > nodes[left]->_index) )
	largest = left;
    }
    
    if ( right < heap->heap_size &&
	 nodes[largest]->priority <= nodes[right]->priority ) {
      if ( (nodes[largest]->priority < nodes[right]->priority) ||
      	   (nodes[largest]->_index > nodes[right]->_index) )
	largest = right;
    }

    if ( largest == i )
      return 0;

    swap(&nodes[largest], &nodes[i]);
    i = largest;
  }  
}

int
is_increase_priority(IS_queue heap, int i, int priority)
{
  if ( !heap || i >= heap->heap_size ) return -1;
  IS_node node   = heap->nodes[i];
  IS_node *nodes = heap->nodes;
  // new priority must greater than current priority.
  if ( node->priority > priority ) return -1;
  nodes[i]->priority = priority;
  while ( i > 0 && priority >= nodes[is_heap_parent(i)]->priority ) {
    if ( (priority > nodes[is_heap_parent(i)]->priority) ||
	 (nodes[i]->_index < nodes[is_heap_parent(i)]->_index)) {
      swap(&nodes[i], &nodes[is_heap_parent(i)]);
      i = is_heap_parent(i);
    } else break;
  }
  return 0;
}

int 
is_max_heap_insert(IS_queue heap, int priority, void* usr_data)
{
  if (!heap) return -1;
  if ( heap->heap_size >= heap->capacity ) {
    /* reallocate */
    IS_node *p = heap->nodes;
    int inc = heap->heap_size > MAX_INC ? MAX_INC : heap->heap_size;
    int newsize = (heap->heap_size + inc) * sizeof(IS_node);
    heap->nodes = (IS_node*)realloc(heap->nodes, newsize);
    if ( heap->nodes == NULL ) {
      /* if reallocation failed, restore original nodes. */
      heap->nodes = p;
      return errno;
    }
    heap->capacity += inc;
  }
  
  IS_node new = (IS_node)malloc(sizeof(struct is_node));
  if (new == NULL) return errno;
  new->priority = 0;
  new->usr_data = usr_data;
  new->_index = heap->counter;
  heap->nodes[heap->heap_size] = new;
  heap->heap_size++;
  heap->counter++;
  
  is_increase_priority(heap, heap->heap_size-1, priority);
  
  return 0;
}

void
is_heap_extract_max(IS_queue heap)
{
  void** res = NULL;
  if (!heap || (heap->heap_size == 0)) return;
  IS_node *nodes = heap->nodes;
  nodes[0]->usr_data = NULL;
  free(nodes[0]);
  nodes[0] = nodes[heap->heap_size-1];
  nodes[heap->heap_size-1] = NULL;
  heap->heap_size--;
  is_max_heapify(heap, 0);
}

void
is_free_heap(IS_queue *heap)
{
  if (heap && *heap) {
    for (int i = 0; i < (*heap)->heap_size; i++)
      free((*heap)->nodes[i]);
    free((*heap)->nodes);
    free(*heap);
    *heap = NULL;
  }
}

void
printh(IS_queue q)
{
  for (int i = 0; i < q->heap_size; i++)
    printf("%d ",q->nodes[i]->priority);
  printf("\n");
}
