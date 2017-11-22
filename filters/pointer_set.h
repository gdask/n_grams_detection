#ifndef POINTER_SET_H
#define	POINTER_SET_H

#include <stdbool.h>

struct pointer_set{
    int Size;
    int First_Available_Slot;
    void** Array;
};
typedef struct pointer_set pointer_set;

void pointer_set_init(pointer_set* obj,int init_size);
void pointer_set_fin(pointer_set* obj);
void ps_reuse(pointer_set* obj);
bool ps_append(pointer_set* obj,void* input);

#endif