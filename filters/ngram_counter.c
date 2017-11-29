#include "ngram_counter.h"
#include <stdio.h>
#include <stdlib.h>

void cr_init(cr* obj){
    obj->Ngram_Vector = malloc(NV_INIT*sizeof(char));
    obj->Ngram_Counts = malloc(NC_INIT*sizeof(int));
    obj->Vector_Offsets = malloc(NC_INIT*sizeof(int));
    if(obj->Ngram_Counts == NULL || obj->Ngram_Vector==NULL || obj->Vector_Offsets==NULL){
        fprintf(stderr,"malloc failed in cr_init\n");
        exit(-1);
    }
    obj->NV_Size = NV_INIT;
    obj->NC_Size = NC_INIT;
    obj->NV_First_Available_Slot=0;
    obj->NC_First_Available_Slot=0;
}

void cr_fin(cr* obj){
    free(obj->Ngram_Counts);
    free(obj->Ngram_Vector);
    free(obj->Vector_Offsets);
}

void cr_reuse(cr* obj){
    obj->NC_First_Available_Slot = 0;
    obj->NV_First_Available_Slot = 0;
}

void cr_force_append(cr* obj,int target_index,char* ngram){
    if(obj->NC_First_Available_Slot == obj->NC_Size){
        obj->Ngram_Counts = realloc(obj->Ngram_Counts,obj->NC_Size*2);
        if(obj->Ngram_Counts==NULL){
            fprintf(stderr,"realloc failed in cr_force append\n");
            exit(-1);
        }
        obj->NC_Size = obj->NC_Size*2;
    }
    size_t input_len = strlen(ngram)+1;
    if(obj->NV_First_Available_Slot + input_len >= obj->NV_Size){
        obj->Ngram_Vector = realloc(obj->Ngram_Vector,obj->NV_Size*2);
        if(obj->Ngram_Vector==NULL){
            fprintf(stderr,"realloc failed in cr_force append\n");
            exit(-1);
        }
        obj->NV_Size = obj->NV_Size*2;
    }
    size_t movable;
}