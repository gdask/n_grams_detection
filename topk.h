#ifndef TOP_K_H
#define	TOP_K_H
#define INIT_SIZE_QUEUE 512
#define INIT_SIZE_NGRAM 128
#define LCHILD(x) 2 * x + 1 // left child
#define RCHILD(x) 2 * x + 2 // right child
#define PARENT(x) (x - 1) / 2 // parent

#define INIT_SIZE_HEAP 4
#include <stdbool.h>
#include <stdio.h>

typedef struct na_node{
    int rank;
    char* ngram; //initialized to initbufsize
    bool visited;
    int bufsize;
}na_node;

typedef struct maxHeap{
    int size ;
    int k; //k is needed for topK
    int FirstAvailableSlot;
    na_node *elem ;
}maxHeap ;

typedef struct ngram_array{
        na_node* Array; 
        int bufsize;
        int first_available_slot;
        maxHeap* heap;
}ngram_array;


void na_init(ngram_array *obj);
void na_fin(ngram_array *obj);
void na_reuse(ngram_array *obj);

/*if ngram is not in my na, insert at the end*/
void na_insert(ngram_array *q, char* ngram, int goal_index, int len);
void na_insert_at_the_end(ngram_array *obj, char* input_ngram, int len_ngram);
void na_append(na_node *obj, char* input_ngram, int len);
void na_topk(ngram_array *obj, int k);

/*for heuristic topk*/
int na_ngram(ngram_array *obj, int obj_rank, int k);
int max_rank(ngram_array *obj);

/*this function returns -1 if ngram already in array or else the right position*/
void na_lookup(ngram_array *obj, char* input_ngram, int len);

//Quicksort
void quickSort(ngram_array obj[], int low, int high);
int partition (ngram_array *obj, int low, int high);
void na_topk_sort(ngram_array* obj, int k);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Heap Functions

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