#include "bloom_filter.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "murmur3.h"

#define bit_get(bit_vector,target_bit) ((*(target_bit/8+bit_vector)>>(target_bit%8))&1)
#define bit_set(bit_vector,target_bit) (*(target_bit/8+bit_vector)|=1<<(target_bit%8))

/*
void bit_set(char* bit_vector,int target_bit){
    char* target = target_bit/8 + bit_vector;
    int swift = target_bit%8;
    *target |= 1 << swift;
}
bool bit_get(char* bit_vector,int target_bit){
    char* target = target_bit/8 + bit_vector;
    int swift = target_bit%8;
    return (*target >> swift) & 1;
}*/

static inline void bf_init(b_filter* obj){
    obj->inserted = 0;
    memset(obj->bit_vector,0,V_SIZE);
}

static inline void bf_insert(b_filter* obj,uint32_t* hashes,void* input){
    int i;
    for(i=0;i<OPT_K;i++){
        bit_set(obj->bit_vector,hashes[i]);
    }
    obj->inserted++;
}

static inline bool bf_lookup(b_filter* obj,uint32_t *hashes,void* input){
    int i;

    for(i=0;i<OPT_K;i++){
        if(bit_get(obj->bit_vector,hashes[i])==false){
            return false;
        }
    }
    return true;
}

static inline bool bf_full(b_filter* obj){
    if(obj->inserted < CAPACITY){
        return false;
    }
    return true;
}

void bf_print(b_filter* obj){
    int i;
    for(i=0;i<BITS;i++){
        printf("%d",(int)bit_get(obj->bit_vector,i));
    }
    printf("\n");
}

void filter_init(filter* obj){
    obj->size = 4;
    obj->in_use = 0;
    obj->arr = malloc(obj->size*sizeof(b_filter));
    if(obj->arr==NULL){
        fprintf(stderr,"filter_init malloc failed\n");
        exit(-1);
    }
    int i;
    for(i=0;i<obj->size;i++) bf_init(&obj->arr[i]);
}

void f_reuse(filter* obj){
    int i;
    for(i=0;i<=obj->in_use;i++) bf_init(&obj->arr[i]);
    obj->in_use=0;
}

void filter_fin(filter* obj){
    free(obj->arr);
}

/*RETURNS FALSE IF ENTRY ALREADY IN*/
/*RETURNS TRUE IF ENTRY WASNT ALREADY IN*/
bool f_append(filter* obj,void* input){
    uint32_t i;
    f_hash(obj,input);  
    for(i=0;i<=obj->in_use;i++){
        if(bf_lookup(&obj->arr[i],obj->hashes,input)==true) return false;
    }
    //IN CASE THAT INPUT IS NOT IN INSERT
    if(bf_full(&obj->arr[obj->in_use])==true){
        obj->in_use++;
        if(obj->in_use == obj->size){
            obj->arr = realloc(obj->arr,2*obj->size*sizeof(b_filter));
            if(obj->arr==NULL){
                fprintf(stderr,"f_append realloc failed\n");
                exit(-1);
            }
            for(i=obj->size;i<obj->size*2;i++) bf_init(&obj->arr[i]);
            obj->size = 2*obj->size;
        }
    }
    bf_insert(&obj->arr[obj->in_use],obj->hashes,input);
    return true;
}

bool f_lookup(filter* obj,void* input){
    f_hash(obj,input);
    int i;
    for(i=0;i<=obj->in_use;i++){
        if(bf_lookup(&obj->arr[i],obj->hashes,input)==true) return true;
    }
    return false;
}

#if OPT == 0 //NON OPTIMIZED
void f_hash(filter* obj,void* input){
    uint32_t i;
    for(i=0;i<OPT_K;i++){
        MurmurHash3_x86_32(&input,sizeof(void*),i,&obj->hashes[i]);
        obj->hashes[i] >>=SWIFT;
    }
}
#elif OPT == 1 //MITZENMACHER OPTIMIZATION
void f_hash(filter* obj,void* input){
    uint32_t i,h1,h2;
    MurmurHash3_x86_32(&input,sizeof(void*),0,&h1);
    MurmurHash3_x86_32(&input,sizeof(void*),h1,&h2);

    for(i=0;i<OPT_K;i++){
        //obj->hashes[i] = (h1 + (h2>>(i))) &MASK;
        obj->hashes[i] = (h1 + (h2*(i))) >>SWIFT;
    }
}
#endif