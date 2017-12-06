#include "hash_pointer_set.h"
#include "murmur3.h"
#include <stdio.h>
#include <stdlib.h>


uint64_t hash_64(uint64_t key, uint64_t mask)
{
	key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
	key = key ^ key >> 24;
	key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
	key = key ^ key >> 14;
	key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
	key = key ^ key >> 28;
	key = (key + (key << 31)) & mask;
	return key;
}

uint32_t hashint_1( uint64_t a){
    a = (a^0xdeadbeef) + (a<<4);
    a = a ^ (a>>10);
    a = a + (a<<7);
    a = a ^ (a>>13);
    return a;
}

uint32_t half_avalanche( uint64_t a){
    a = (a+0x479ab41d) + (a<<8);
    a = (a^0xe4aa10ce) ^ (a>>5);
    a = (a+0x9942f0a6) - (a<<14);
    a = (a^0x5aedd67d) ^ (a>>3);
    a = (a+0x17bea992) + (a<<7);
    return a;
}

uint32_t hashint_2( uint64_t a){
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

void hash_pset_init(hash_pset* obj,int init_size){
    //Actual size must be 2^k
    int i;
    obj->size = 0x01;

    while(obj->size < init_size) obj->size = obj->size << 1;
    obj->mask = obj->size - 1;
    //fprintf(stderr,"Init_Size:%d, Actual Size:%d, Mask:%d\n",init_size,obj->size,obj->mask);

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
    //Distribution check
    /*int max=0,sum=0;
    for(i=0;i<obj->size;i++){
        sum +=obj->pset_Arr[i].First_Available_Slot;
        if(obj->pset_Arr[i].First_Available_Slot>max) max=obj->pset_Arr[i].First_Available_Slot;
    }
    fprintf(stderr,"Bigest bucket:%d Dist:  ",max);
    int counts[max+1],collisions=0;
    for(i=0;i<max+1;i++) counts[i]=0;
    for(i=0;i<obj->size;i++){
        //if(obj->pset_Arr[i].First_Available_Slot!=0)
        //fprintf(stderr," %d:%d:,",i,obj->pset_Arr[i].First_Available_Slot);
        counts[obj->pset_Arr[i].First_Available_Slot]++;
    }
    for(i=2;i<max+1;i++){
        //fprintf(stderr,"%d:%f%%  ",i,(double)100*counts[i]/obj->size);
        //fprintf(stderr,"%d:%d  ",i,counts[i]);
        collisions += counts[i]*i;
    }
    fprintf(stderr,"Collisions:%f%%",(double)100*collisions/sum);
    fprintf(stderr,"\n");*/

    for(i=0;i<obj->size;i++) ps_reuse(&obj->pset_Arr[i]);
}

bool hash_pset_append(hash_pset* obj,void* input){
    //Hash input
    //uint64_t res = hash_64((size_t)input, obj->mask); //terrible
    //uint64_t res = (((size_t)input>>3)+(~(size_t)input<<6)) & obj->mask; //not so terrible but still..
    //uint64_t res = (size_t)input >> 8 & obj->mask; //suprisingly not so bad!
    //uint32_t res = hashint_1((size_t)input) & obj->mask; //better
    //uint32_t res = hashint_2((size_t)input) & obj->mask; //better
    uint32_t res = half_avalanche((size_t)input) & obj->mask; //best so far..
    //use res as index
    return ps_append(&obj->pset_Arr[res],input);
}