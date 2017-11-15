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
        obj->Array[i].rank=-1;
    }
    obj->first_available_slot=0;
}

void na_fin(ngram_array *obj){
    int i;
    printf("buffsize:%d\n", obj->bufsize);
    if(obj->Array != NULL){
        for(i=0;i<obj->first_available_slot; i++){
            if(obj->Array[i].ngram!=NULL){
                free(obj->Array[i].ngram);
            }
        }
        free(obj->Array);
    }
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
                obj->Array[i].rank=-1;
            }
        }
        obj->first_available_slot=0;
    }
}

/*checks if input_ngram is in na, if yes rank is augmented and return true else false*/
int na_lookup(ngram_array *obj, char* input_ngram, int len_ngram){
    int lower_bound = 0;
    int upper_bound = obj->first_available_slot-1;
    int middle = (lower_bound+upper_bound)/2;
    while(lower_bound <= upper_bound){
            int cmp_res = 0;
            cmp_res=strncmp(obj->Array[middle].ngram, input_ngram, len_ngram-1);
            if(cmp_res < 0){
                lower_bound = middle + 1;
            }
            else if(cmp_res > 0){
                upper_bound = middle - 1;
            }
            else{
                //Array[middle] == input_ngram
                //immidiatilly update rank
                obj->Array[middle].rank = (obj->Array[middle]).rank +1;
                return -1;
            }
            middle = (lower_bound+upper_bound)/2;
        
    }
    //this is the right position for insert the new ngram
    return lower_bound;
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
    memmove(obj->ngram, input_ngram, len_ngram);
    obj->rank=0;
}

void na_topk(ngram_array *obj, int k){
    int max;
    max=max_rank(obj);
    int newk;
    while(k>0 && max>=0){
        /*change for file with "|"*/
        newk=na_ngram(obj, max, k);
        k=newk;
        max--;
    }
}

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

int na_ngram(ngram_array *obj, int obj_rank, int k){
    int i=0;
    while(k!=0 && i<obj->first_available_slot){        
        if(obj->Array[i].rank==obj_rank && k-1==0){
            printf("%s", obj->Array[i].ngram);
            k--;
        }
        else if(obj->Array[i].rank==obj_rank){
            printf("%s|", obj->Array[i].ngram);
            k--;
        }
        i++;
    }
    return k;
}

void swap(ngram_array * obj, int a, int b){
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
    int pivot = obj->Array[high].rank;    // pivot
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (obj->Array[j].rank >= pivot)
        {
            i++;    // increment index of smaller element
            swap(obj, i, j);
        }
    }
    swap(obj, i + 1, high);
    return (i + 1);
}
 
/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
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

void na_topk_sort(ngram_array* obj, int k){
    int i=0;
    quickSort(obj, 0, obj->first_available_slot-1);
    while(i<obj->first_available_slot && k!=0){
            if(obj->Array[i].ngram!=NULL){
                printf("%s|", obj->Array[i].ngram);
                k--;
            }
        i++;
    }
}