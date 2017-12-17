#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "heap.h"

//initialise heap
void heap_init(maxHeap* obj, int size){
    na_node* temp = malloc(sizeof(na_node)*size);
    if(temp==NULL){
        fprintf(stderr, "Error in malloc :: initMaxHeap\n");
        exit(-1);
    }
    obj->elem = temp;
    obj->size = size;
    obj->FirstAvailableSlot=0;
}

void insertNode(maxHeap *obj, int data, char* ngram) {
    // check if new element feets in array
    if(obj->size==obj->FirstAvailableSlot){
        obj->elem = realloc(obj->elem, (obj->size * 2) * sizeof(na_node)) ;
        obj->size =obj->size * 2;
    }
    // initializing the node with value
    na_node nd ;
    nd.rank = data ;
    nd.ngram=malloc(sizeof(char)*(strlen(ngram)+1));
    nd.visited=false;
    strcpy(nd.ngram, ngram);

    // Positioning the node at the right position in the max heap
    int i = (obj->FirstAvailableSlot)++;
    while(i && (nd.rank > obj->elem[PARENT(i)].rank ||(strcmp(nd.ngram, obj->elem[PARENT(i)].ngram)<=0 && nd.rank > obj->elem[PARENT(i)].rank))){
        obj->elem[i] = obj->elem[PARENT(i)] ;
        i = PARENT(i) ;
    }
    obj->elem[i] = nd ;
}

//start from root-start=0 and print k top ngrams
void topK(maxHeap *obj, int start){
    int i=start;
    int rem;
    if(obj->k!=0 && i<obj->FirstAvailableSlot){
        if(obj->k==1){
            printf("%s\n", obj->elem[i].ngram);
        }
        else{
            printf("%s|\n", obj->elem[i].ngram);
        }
        obj->elem[i].visited=true;
        obj->k--;
        if(LCHILD(i)<obj->FirstAvailableSlot && obj->elem[LCHILD(i)].visited!=true){
            //printf("left %d\n", LCHILD(i));
            topK(obj, LCHILD(i));
        }
        if(RCHILD(i)<obj->FirstAvailableSlot && obj->elem[RCHILD(i)].visited!=true){
            //printf("right %d\n", RCHILD(i));
            topK(obj, RCHILD(i));
        }
    }
    return;
    
}

void buildMaxHeap(maxHeap *obj, na_node* arr, int size) {
    int i ;

    // Insertion into the heap without violating the shape property
    for(i=0;i<size; i++)
        insertNode(obj, arr[i].rank, arr[i].ngram);

    // Making sure that heap property is also satisfied
    for(i = (obj->FirstAvailableSlot - 1) / 2; i >= 0; i--) {
        heapify(obj, i) ;
    }
}

void heap_fin(maxHeap* obj){
    int i;
    if(obj->elem!=NULL){
        for(i=0; i<obj->FirstAvailableSlot; i++){
            if(obj->elem[i].ngram!=NULL){
                free(obj->elem[i].ngram);
            }
        }
        free(obj->elem);
    }
}

void heap_reuse(maxHeap *obj){
    int i;
    for(i=0; i<obj->FirstAvailableSlot; i++){
        if(obj->elem[i].ngram!=NULL){
                free(obj->elem[i].ngram);
        }
    }
}

void heap_display(maxHeap *obj){
    int i=0;
    for(i=0; i<obj->FirstAvailableSlot; i++){
        fprintf(stderr, "Item: pos:%d %d %s\n", i, obj->elem[i].rank, obj->elem[i].ngram);
        fprintf(stderr, "right is in position: %d | ",RCHILD(i));
        fprintf(stderr, "left is in position: %d \n",LCHILD(i));
    }
}


void swap(na_node *n1, na_node *n2) {
    na_node temp = *n1 ;
    *n1 = *n2 ;
    *n2 = temp ;
}


/*In heapify() function, given a node at index i, 
we shall compare all the three nodes (parent, left and right child),
and find the largest node of the three. If its not the parent node, 
then heap property is violated. Swap parent node with largest node, and call heapify() 
function until heap property is satisfied.*/
void heapify(maxHeap *obj, int i) {
    int largest;
    if(LCHILD(i) < obj->FirstAvailableSlot && obj->elem[LCHILD(i)].rank > obj->elem[i].rank){
            largest=LCHILD(i);
    }
    else{
        largest=i;
    }
    if(RCHILD(i) < obj->FirstAvailableSlot){
        if(obj->elem[RCHILD(i)].rank > obj->elem[largest].rank){
            //if right child has more apperances
            largest = RCHILD(i) ;
        }
        else if(obj->elem[RCHILD(i)].rank == obj->elem[largest].rank){
            //if the two nodes have the same number of apperances, check alphabetical order
            if(strcmp(obj->elem[RCHILD(i)].ngram, obj->elem[largest].ngram)<0){
                largest = RCHILD(i) ;   
            }
        }
    }
    if(largest != i) {
        swap(&(obj->elem[i]), &(obj->elem[largest])) ;
        heapify(obj, largest) ;
    }
}

/*delete root*/
bool heap_top(maxHeap *obj) {
    if(obj->FirstAvailableSlot!=0) {
        printf("Deleting root %d\n\n", obj->elem[0].rank) ;
        if(LCHILD(0)<obj->FirstAvailableSlot){
            //printf("left %d\n", LCHILD(i));
            obj->elem[0]= obj->elem[LCHILD(0)];
        }
        else if(RCHILD(0)<obj->FirstAvailableSlot){
            //printf("right %d\n", RCHILD(i));
            obj->elem[0]= obj->elem[LCHILD(0)];
        }
        else{
            obj->elem[0] = obj->elem[--(obj->FirstAvailableSlot)];
        }
        //obj->FirstAvailableSlot--;
        return false;
    }
    return true;
}

void findKtop(maxHeap *obj, int k){
    int i=k;
    bool heap_empty=false;
    while(k!=0 && heap_empty==false){
        printf("%d %s|", obj->elem[0].rank, obj->elem[0].ngram);
        heap_empty=heap_top(obj);
        k--;
    }
    printf("\n");
}

//start from root-start=0 and print k top ngrams
void topK(maxHeap *obj, int k, int start){
    int i=start;
    int rem;
    if(obj->k!=0 && i<obj->FirstAvailableSlot){
        if(obj->k==1){
            printf("%s\n", obj->elem[i].ngram);
        }
        else{
            printf("%s|\n", obj->elem[i].ngram);
        }
        obj->elem[i].visited=true;
        obj->k--;
        if(LCHILD(i)<obj->FirstAvailableSlot && obj->elem[LCHILD(i)].visited!=true){
            //printf("left %d\n", LCHILD(i));
            topK(obj, k, LCHILD(i));
        }
        if(RCHILD(i)<obj->FirstAvailableSlot && obj->elem[RCHILD(i)].visited!=true){
            //printf("right %d\n", RCHILD(i));
            topK(obj, k, RCHILD(i));
        }
    }
    return;
    
}

void buildMaxHeap(maxHeap *obj, na_node* arr, int size) {
    int i ;

    // Insertion into the heap without violating the shape property
    for(i=0;i<size; i++)
        insertNode(obj, arr[i].rank, arr[i].ngram);

    // Making sure that heap property is also satisfied
    for(i = (obj->FirstAvailableSlot - 1) / 2; i >= 0; i--) {
        heapify(obj, i) ;
    }
}

void heap_fin(maxHeap* obj){
    int i;
    if(obj->elem!=NULL){
        for(i=0; i<obj->FirstAvailableSlot; i++){
            if(obj->elem[i].ngram!=NULL){
                free(obj->elem[i].ngram);
            }
        }
        free(obj->elem);
    }
}


void heap_display(maxHeap *obj){
    int i=0;
    for(i=0; i<obj->FirstAvailableSlot; i++){
        fprintf(stderr, "Item: pos:%d %d %s\n", i, obj->elem[i].rank, obj->elem[i].ngram);
        fprintf(stderr, "right is in position: %d | ",RCHILD(i));
        fprintf(stderr, "left is in position: %d \n",LCHILD(i));
    }
}



