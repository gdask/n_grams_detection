#include "hash_pointer_set.h"
#include "murmur3.h"
#include <stdio.h>
#include <stdlib.h>


unsigned long ptr_to_int(void* str){
    return 2654435761*(size_t)str;
}

void hash_pset_init(hash_pset* obj,int init_size){
    //check if input size if power of 2
    int i;
    obj->mask = 0x01;
    obj->size = 0x01;

    while(obj->size < init_size){
        obj->size = obj->size << 1;
        obj->mask = (obj->mask << 1) | obj->mask;
    }
    obj->mask = obj->mask >> 1;
    //fprintf(stderr,"Init_Size:%d, Actual Size:%d, Mask:%d\n",init_size,obj->size,obj->mask);
    //exit(-1);

    obj->pset_Arr = malloc(obj->size*sizeof(pointer_set));
    if(obj->pset_Arr==NULL){
        fprintf(stderr,"Hash pointer set,malloc failed\n");
        exit(-1);
    }
    //init pointer sets
    for(i=0;i<obj->size;i++) pointer_set_init(&obj->pset_Arr[i],PSET_INIT);
    obj->used=false;
}

void hash_pset_fin(hash_pset* obj){
    int i;
    for(i=0;i<obj->size;i++) pointer_set_fin(&obj->pset_Arr[i]);
}

void hash_pset_reuse(hash_pset* obj){
    if(obj->used==false) return;
    int i;
    /*for(i=0;i<obj->size;i++){
        if(obj->pset_Arr[i].First_Available_Slot!=0)
        fprintf(stderr,"Index: %d Contains:%d\n",i,obj->pset_Arr[i].First_Available_Slot);
    }*/
    for(i=0;i<obj->size;i++) ps_reuse(&obj->pset_Arr[i]);
}

bool hash_pset_append(hash_pset* obj,void* input){
    //Hash input
    obj->used=true;
    unsigned long  res = ((size_t)input>>3) & obj->mask;
    //use res as index
    return ps_append(&obj->pset_Arr[res],input);
}