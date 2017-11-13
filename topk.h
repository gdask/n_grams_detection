#ifndef TOP_K_H
#define	TOP_K_H
#define INIT_SIZE_QUEUE 512
#define INIT_SIZE_NGRAM 128
#include <stdbool.h>
#include <stdio.h>

typedef struct queue_node{
    int rank;
    char* ngram; //initialized to initbufsize
    int bufsize;
}queue_node;

typedef struct myqueue{
        queue_node* Array; 
        int bufsize;
        int first_available_slot;
}myqueue;

void queue_init(myqueue *obj);
void queue_fin(myqueue *obj);

/*if ngram is not in my queue, insert at the end*/
void queue_insert(myqueue *q, char* ngram);
void queue_topk(myqueue *obj, int k);

/*this function is called by result manager each time a ngram is detected*/
bool queque_lookup(myqueue *obj, char* input_ngram);

#endif