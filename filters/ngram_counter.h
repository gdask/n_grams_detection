#ifndef NGRAM_COUNTER_H
#define	NGRAM_COUNTER_H

#define NV_INIT 128
#define NC_INIT 20
#define COUNTER_INIT 20

struct counter_row{
    char* Ngram_Vector;
    int* Vector_Offsets;
    int* Ngram_Counts;
    int NV_Size;
    int NV_First_Available_Slot;
    int NC_Size;
    int NC_First_Available_Slot;
};
typedef struct counter_row cr;

void cr_init(cr* obj);
void cr_fin(cr* obj);
void cr_reuse(cr* obj);
void cr_force_append(cr* obj,int target_index,char* ngram);
void cr_force_delete(cr* obj,int target_index);

struct ngram_counter{
    cr* rows;
    int size;
    int First_Available_Row;
};
typedef struct ngram_counter ngram_counter;

void ngram_counter_init(ngram_counter* obj);
void ngram_counter_fin(ngram_counter* obj);
void nc_reuse(ngram_counter* obj);

void nc_append(ngram_counter* obj,char* ngram);
void nc_topk(ngram_counter* obj,int k);



#endif