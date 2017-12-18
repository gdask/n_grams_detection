#ifndef HEAP_H
#define	HEAP_H
#define LCHILD(x) 2 * x + 1 // left child
#define RCHILD(x) 2 * x + 2 // right child
#define PARENT(x) (x - 1) / 2 // parent

#define INIT_SIZE_HEAP 4

#include <stdbool.h>
#include "./topk/topk.h"

typedef struct maxHeap{
    int size ;
    int k; //k is needed for topK
    int FirstAvailableSlot;
    na_node *elem ;
}maxHeap ;


void heap_init(maxHeap* obj, int size);
void heap_reuse(maxHeap *obj);
void heap_fin(maxHeap* obj);

void insertNode(maxHeap *obj, int data, char* ngram);
void swap(na_node *n1, na_node *n2);
void heapify(maxHeap *obj, int i);
void buildMaxHeap(maxHeap *obj, na_node *arr, int size);

void inorderTraversal(maxHeap *obj, int i);

void heap_display(maxHeap *obj);
void topK(maxHeap *obj, int start);


#endif
