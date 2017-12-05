#ifndef HASH_POINTER_SET_H
#define HASH_POINTER_SET_H

#include "pointer_set.h"
#include <stdint.h>

#define PSET_INIT 4
#define PTR_SIZE sizeof(void*)

//Array of pointer sets
typedef struct hash_pset{
    pointer_set* pset_Arr;
    int size;
    int mask;
    bool used;
}hash_pset;

void hash_pset_init(hash_pset* obj,int init_size);
void hash_pset_fin(hash_pset* obj);
void hash_pset_reuse(hash_pset* obj);
bool hash_pset_append(hash_pset* obj,void* input);

#endif