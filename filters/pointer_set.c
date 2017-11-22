#include "pointer_set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void pointer_set_init(pointer_set* obj,int init_size){
    obj->Size = init_size;
    obj->First_Available_Slot=0;
    if(init_size < 1){
        fprintf(stderr,"Pointer set init called with < 1 init size\n");
        exit(-1);
    }
    obj->Array = (void**)malloc(init_size*sizeof(void*));
    if(obj->Array==NULL){
        fprintf(stderr,"Pointer set init, malloc failed\n");
        exit(-1);
    }
}

void pointer_set_fin(pointer_set* obj){
    free(obj->Array);
}

void ps_reuse(pointer_set* obj){
    obj->First_Available_Slot=0;
}

//binary implementation
bool ps_append(pointer_set* obj,void* ptr){
    int lower_bound = 0;
    int upper_bound = obj->First_Available_Slot-1;
    int middle = (lower_bound + upper_bound)/2;
    while(lower_bound <= upper_bound){
        if(obj->Array[middle] < ptr){
            lower_bound = middle +1;
        }
        else if(obj->Array[middle] > ptr){
            upper_bound = middle -1;
        }
        else{ // INPUT ALREADY IN
            return false;
        }
        middle = (lower_bound+upper_bound)/2;
    }
    //INSERT INPUT AT LOWER BOUND
    if(obj->Size == obj->First_Available_Slot){
        obj->Array = (void**)realloc(obj->Array,obj->Size*2*sizeof(void*));
        if(obj->Array==NULL){
            fprintf(stderr,"ps resize realloc failed\n");
            exit(-1);
        }
        obj->Size = obj->Size*2;
    }
    size_t movable =(obj->First_Available_Slot - lower_bound)*sizeof(void*);
    memmove(&obj->Array[lower_bound+1],&obj->Array[lower_bound],movable);
    obj->Array[lower_bound] = ptr;
    obj->First_Available_Slot++;
    return true;
}