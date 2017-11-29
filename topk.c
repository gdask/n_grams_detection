#include "topk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void na_init(ngram_array *obj){
    obj->Array= malloc(sizeof(na_node)*INIT_SIZE_QUEUE);
    
    if(obj->Array==NULL){
        fprintf(stderr,"Malloc failed :: na init\n");
        exit(-1);
    }
    obj->bufsize=INIT_SIZE_QUEUE;
    int i;
    for(i=0; i<obj->bufsize; i++){
        (obj->Array[i]).ngram = NULL;
        obj->Array[i].bufsize=0;
        obj->Array[i].rank=0;
    }
    obj->first_available_slot=0;

    obj->heap=malloc(sizeof(maxHeap));
    if(obj->heap==NULL){
        fprintf(stderr,"Malloc failed :: na init\n");
        exit(-1);
    }
    heap_init(obj->heap, INIT_SIZE_HEAP);
}

void na_fin(ngram_array *obj){
    int i;
    if(obj->Array != NULL){
        for(i=0;i<obj->first_available_slot; i++){
            if(obj->Array[i].ngram!=NULL){
                free(obj->Array[i].ngram);
            }
        }
        free(obj->Array);
    }
    heap_fin(obj->heap);
    free(obj->heap);
}

/*something like delete for ngrams only*/
void na_reuse(ngram_array *obj){
    int i;
    if(obj->Array != NULL){
        for(i=0;i<obj->first_available_slot; i++){
            if(obj->Array[i].ngram!=NULL){
                free(obj->Array[i].ngram);
                obj->Array[i].ngram=NULL;
                obj->Array[i].bufsize=0;
                obj->Array[i].rank=0;
            }
        }
        obj->first_available_slot=0;
        heap_reuse(obj->heap);
    }
}

/*checks if input_ngram is in na, if yes rank is augmented and return true else false*/
void na_lookup(ngram_array *obj, char* input_ngram, int len_ngram){
    int lower_bound = 0;
    int upper_bound = obj->first_available_slot-1;
    int middle = (lower_bound+upper_bound)/2;
    while(lower_bound <= upper_bound){
            int cmp_res = 0;
            cmp_res=strcmp(obj->Array[middle].ngram, input_ngram);
            if(cmp_res < 0){
                lower_bound = middle + 1;
            }
            else if(cmp_res > 0){
                upper_bound = middle - 1;
            }
            else{
                //Array[middle] == input_ngram
                //immidiatilly update rank
                obj->Array[middle].rank ++;
                return; 
            }
            middle = (lower_bound+upper_bound)/2;
        
    }
    //this is the right position for insert the new ngram
    na_insert(obj, input_ngram, lower_bound, len_ngram);
}

//lookup one after one (not used)
bool na_lookup_serial(ngram_array *obj, char* input_ngram){
    int i=0;
    for(i=0;i<obj->first_available_slot;i++){
        if(obj->Array[i].ngram!=NULL){
            if(strcmp(obj->Array[i].ngram, input_ngram)==0){
                obj->Array[i].rank++;
                return true;
            }
        }
    }
    return false;
}

void na_insert_at_the_end(ngram_array *obj, char* input_ngram, int len_ngram){
    /*double array*/
    if(obj->first_available_slot==obj->bufsize){
        na_node* temp = realloc(obj->Array, 2*(obj->bufsize)*sizeof(na_node));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: na_insert\n");
            exit(-1);
        }
        obj->Array=temp;
        obj->bufsize= 2*obj->bufsize;
    }

    int goal_index= obj->first_available_slot;
    bool found=na_lookup_serial(obj, input_ngram);

    if(found==false){
    //find the right pos in table
        na_append(&obj->Array[goal_index], input_ngram, len_ngram);
    }   
    obj->first_available_slot++;
}

/*insert new ngram at the end, at first available slot*/
void na_insert(ngram_array *obj, char* input_ngram, int goal_index, int len_ngram){  
    /*double array*/
    if(obj->bufsize==goal_index || obj->first_available_slot==obj->bufsize){
        na_node* temp = realloc(obj->Array, 2*(obj->bufsize)*sizeof(na_node));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: na_insert\n");
            exit(-1);
        }
        obj->Array=temp;
        obj->bufsize= 2*obj->bufsize;
    }
    else if(goal_index > obj->bufsize-1 || goal_index > obj->first_available_slot){
        fprintf(stderr,"na_append :: out of bounds goal_index\n");
        exit(-1);
    }

    if(goal_index != obj->first_available_slot){
        size_t movable= (obj->first_available_slot - goal_index)* sizeof(na_node);
        memmove(&obj->Array[goal_index+1], &obj->Array[goal_index], movable);
    }

    //find the right pos in table
    na_append(&obj->Array[goal_index], input_ngram, len_ngram);
   
    obj->first_available_slot++;
    return;
}


void na_append(na_node *obj, char* input_ngram, int len_ngram){
    obj->ngram= (char*) malloc(len_ngram);
    if(obj->ngram==NULL){
        fprintf(stderr,"na append :: Malloc failed\n");
        exit(-1);
    }
    strncpy(obj->ngram, input_ngram, len_ngram);
    obj->rank=1;
}

/*Topk using heap*/
void topk_heap(ngram_array *obj, int k){
    obj->heap->k=k;
    //Build maxHeap
    buildMaxHeap(obj->heap, obj->Array, obj->bufsize);
    topK(obj->heap, 0);
}

/*Topk k using our algorithm(description on ReadMe)*/
void na_topk(ngram_array *obj, int k){
    int max;
    max=max_rank(obj);
    int newk;
    printf("Top: ");
    while(k>0 || max>=0){
        newk=na_ngram(obj, max, k);
        k=newk;
        max--;
    }
    printf("\n");
}

/*Find maximun on array*/
int max_rank(ngram_array *obj){
    int i;
    int max=0;
    for(i=0; i<obj->first_available_slot;i++){
        if(obj->Array[i].rank>max){
            max=obj->Array[i].rank;
        }
    }
    return max;
}

/*Print ngrams of rank==obj_rank*/
int na_ngram(ngram_array *obj, int obj_rank, int k){
    int i=0;
    while(i<obj->first_available_slot){        
        if(obj->Array[i].rank==obj_rank && k==1){
            printf("%s %d", obj->Array[i].ngram, obj->Array[i].rank);
            k--;
        }
        else if(obj->Array[i].rank==obj_rank){
            printf("%s %d|", obj->Array[i].ngram, obj->Array[i].rank);
            k--;
        }
        i++;
    }
    return k;
}

void swap_quicksort(ngram_array * obj, int a, int b){
    size_t movable= sizeof(na_node);
    na_node temp;
    temp=obj->Array[a];
    memmove(&obj->Array[a], &obj->Array[b], movable);
    memmove(&obj->Array[b], &temp, movable);
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (ngram_array *obj, int low, int high){
    int pivot1 = obj->Array[high].rank;    // pivot
    char* pivot2=malloc(sizeof(char)*strlen(obj->Array[high].ngram)+1);
    strcpy(pivot2, obj->Array[high].ngram);
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++){
        // If current element is smaller than or
        // equal to pivot
        if (obj->Array[j].rank > pivot1)
        {
            i++;    // increment index of smaller element
            swap_quicksort(obj, i, j);
        }
        else if(obj->Array[j].rank == pivot1){
            if(strcmp(obj->Array[j].ngram,pivot2)<0){
                i++;    // increment index of smaller element
                swap_quicksort(obj, i, j);
            }
        }
    }
    swap_quicksort(obj, i + 1, high);
    free(pivot2);
    return (i + 1);
}
 
/* The main function that implements QuickSort, use pivot*/
void quickSort(ngram_array* obj, int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(obj, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(obj, low, pi - 1);
        quickSort(obj, pi + 1, high);
    }
}

/*Print sort table*/
void na_topk_sort(ngram_array* obj, int k){
    int i=0;
    int flag=1; //nothing printed
    quickSort(obj, 0, obj->first_available_slot-1);
    while(i<obj->first_available_slot && k!=0){
            if(obj->Array[i].ngram!=NULL){
                if(flag==1){
                    printf("Top: ");
                    flag=0;
                }
                 if(i==obj->first_available_slot-2 || k-1==0){
                    printf("%s", obj->Array[i].ngram);
                    k--;
                }
                else{
                    printf("%s|", obj->Array[i].ngram);
                    k--;
                }
            }
        i++;
    }
    if(flag==0)
        printf("\n");
}

//print all line with rank(only for testing)
void na_topk_sort1(ngram_array* obj, int k){
    int i=0;
    quickSort(obj, 0, obj->first_available_slot-1);
    while(i<obj->first_available_slot){
            if(obj->Array[i].ngram!=NULL){
                 if(i==obj->first_available_slot-2 || k-1==0){
                    printf("%d %s", obj->Array[i].rank, obj->Array[i].ngram);
                    k--;
                }
                else{
                    printf("%d %s|", obj->Array[i].rank, obj->Array[i].ngram);
                    k--;
                }
            }
        i++;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ιnitialise heap
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