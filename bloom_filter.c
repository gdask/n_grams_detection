#include "bloom_filter.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "murmur3.h"

void bit_set(char* bit_vector,int target_bit){
    char* target = target_bit/8 + bit_vector;
    int swift = target_bit%8;
    *target |= 1 << swift;
}

bool bit_get(char* bit_vector,int target_bit){
    char* target = target_bit/8 + bit_vector;
    int swift = target_bit%8;
    return (*target >> swift) & 1;
}

void bf_init(b_filter* obj){
    obj->inserted = 0;
    memset(obj->bit_vector,0,V_SIZE);
}

void bf_insert(b_filter* obj,void* input){
    uint32_t seed;
    uint32_t result;

    for(seed=0;seed<OPT_K;seed++){
        MurmurHash3_x86_32(&input,sizeof(void*),seed,&result);
        bit_set(obj->bit_vector,result>>SWIFT);
    }
    obj->inserted++;
}

bool bf_lookup(b_filter* obj,void* input){
    uint32_t seed;
    uint32_t result;

    for(seed=0;seed<OPT_K;seed++){
        MurmurHash3_x86_32(&input,sizeof(void*),seed,&result);
        if(bit_get(obj->bit_vector,result>>SWIFT)==false){
            return false;
        }
    }
    return true;
}
/*Conditional insert,if input doesnt already exists,is inserted and returns true*/
/*Otherwise false is returned*/
bool bf_append(b_filter* obj,void* input){
    uint32_t seed;
    uint32_t result;

    bool found = true;
    for(seed=0;seed<OPT_K;seed++){
        MurmurHash3_x86_32(&input,sizeof(void*),seed,&result);
        if(bit_get(obj->bit_vector,result>>SWIFT)==false){
            found = false;
            bit_set(obj->bit_vector,result>>SWIFT);
        }
    }
    if(found==false) obj->inserted++;
    return !found;
}

bool bf_full(b_filter* obj){
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
    int i;
    for(i=0;i<=obj->in_use;i++){
        if(bf_lookup(&obj->arr[i],input)==true) return false;
    }
    //IN CASE THAT INPUT IS NOT IN
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
    bf_insert(&obj->arr[obj->in_use],input);
    return true;
}

bool f_lookup(filter* obj,void* input){
    int i=0;
    for(i=0;i<=obj->in_use;i++){
        if(bf_lookup(&obj->arr[i],input)==true) return true;
    }
    return false;
}