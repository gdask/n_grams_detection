#include "hash_pointer_set.h"
#include <stdio.h>
#include <stdlib.h>

#define bit_get(bit_vector,target_bit) ((*(target_bit/8+bit_vector)>>(target_bit%8))&1)
#define bit_set(bit_vector,target_bit) (*(target_bit/8+bit_vector)|=1<<(target_bit%8))

void hash_pset_init(hash_pset* obj,int init_size){
    //check if input size if power of 2
    int i;
    obj->mask = 0x01;
    obj->size = 0x01;

    while(obj->size < init_size){
        obj->size = obj->size << 1;
        obj->mask = (obj->mask << 1) | obj->mask;
    }
    fprintf(stderr,"Init_Size:%d, Actual Size:%d, Mask:%d\n",init_size,obj->size,obj->mask);
    //exit(-1);

    obj->pset_Arr = malloc(obj->size*sizeof(pointer_set));
    if(obj->pset_Arr==NULL){
        fprintf(stderr,"Hash pointer set,malloc failed\n");
        exit(-1);
    }
    //init pointer sets
    for(i=0;i<obj->size;i++) pointer_set_init(&obj->pset_Arr[i],PSET_INIT);
}

void hash_pset_fin(hash_pset* obj){
    int i;
    for(i=0;i<obj->size;i++) pointer_set_fin(&obj->pset_Arr[i]);
}

void hash_pset_reuse(hash_pset* obj){
    int i;
    for(i=0;i<obj->size;i++) ps_reuse(&obj->pset_Arr[i]);
}

bool hash_pset_append(hash_pset* obj,void* input){
    //Hash input

}