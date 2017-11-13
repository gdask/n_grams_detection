#include "topk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void queue_init(myqueue *obj){
    obj->Array= malloc(sizeof(queue_node)*INIT_SIZE_QUEUE);
    if(obj->Array==NULL){
        fprintf(stderr,"Malloc failed :: queue init\n");
        exit(-1);
    }
    obj->bufsize=INIT_SIZE_QUEUE;
    int i;
    for(i=0; i<obj->bufsize; i++){
        (obj->Array[i]).ngram=malloc(sizeof(char)*INIT_SIZE_NGRAM);
        if((obj->Array[i]).ngram==NULL){
            fprintf(stderr,"Malloc failed :: queue init\n");
            exit(-1);
        }
        (obj->Array[i]).bufsize=INIT_SIZE_NGRAM;
    }
    obj->first_available_slot=0;
}

void queue_fin(myqueue *obj){
    int i;
    if(obj->Array != NULL){
        for(i=0;i<obj->bufsize; i++){
            if(obj->Array[i].ngram!=NULL){
                free(obj->Array[i].ngram);
            }
        }
        free(obj->Array);
    }
}

/*checks if input_ngram is in queue, if yes rank is augmented and return true else false*/
bool queque_lookup(myqueue *obj, char* input_ngram){
    int lower_bound = 0;
    int upper_bound = obj->first_available_slot-1;
    int middle = (lower_bound+upper_bound)/2;

    while(lower_bound <= upper_bound){
        int cmp_res = strcmp(obj->Array[middle].ngram, input_ngram);
        if(cmp_res < 0){
            lower_bound = middle + 1;
        }
        else if(cmp_res > 0){
            upper_bound = middle - 1;
        }
        else{
            //Array[middle] == input_word
            obj->Array[middle].rank = (obj->Array[middle]).rank +1;
            return true;
        }
        middle = (lower_bound+upper_bound)/2;
    }
    return false;
}

/*insert new ngram at the end, at first available slot*/
void queue_insert(myqueue *obj, char* input_ngram){
    int occupied_slots=obj->first_available_slot-1;
    if(occupied_slots==obj->bufsize){ //must expand array
        queue_node* temp = realloc(obj->Array, 2*obj->bufsize*sizeof(queue_node));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: queue_insert\n");
            exit(-1);
        }
        obj->Array=temp;
        obj->bufsize= 2*obj->bufsize;
    }
    int ngramlen;
    ngramlen=strlen(input_ngram);
    if(ngramlen>=obj->Array[obj->first_available_slot].bufsize){
        char* temp = realloc(obj->Array[obj->first_available_slot].ngram, 2*obj->Array[obj->first_available_slot].bufsize*sizeof(char));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: queue_insert\n");
            exit(-1);
        }
        obj->Array[obj->first_available_slot].ngram=temp;
        obj->Array[obj->first_available_slot].bufsize= 2*obj->Array[obj->first_available_slot].bufsize;
    }
    strcpy(obj->Array[obj->first_available_slot].ngram, input_ngram);
    obj->Array[obj->first_available_slot].rank=0;
    obj->first_available_slot=obj->first_available_slot+1;
    return;
}

void queue_topk(myqueue *obj, int k){
    int i=0;
    while(i<=k){
        if(i >= obj->first_available_slot){
            return;
        }
        /*change for file with "|"*/
        printf("Rank %d: ngram %s", obj->Array[i].rank, obj->Array[i].ngram);
        i++;
    }
}
