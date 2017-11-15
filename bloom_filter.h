#ifndef BLOOM_FILTER_H
#define	BLOOM_FILTER_H

#include <stdbool.h>
#define OPT 0 // MITZENMACHER OPTIMIZATION => NOT USEFULL IN OUR CASE
#define CONF 3 //SELECT CONFIGURATION
#if CONF == 3 //FASTEST BUT..
#define V_SIZE 2048
#define OPT_K 7
#define CAPACITY 500
#define SWIFT 18 // 14 BITS FOR INDEX
#define MASK 0x00002fff
#elif CONF == 2
#define V_SIZE 1024
#define OPT_K 10
#define CAPACITY 250
#define SWIFT 19 // 13 BITS FOR INDEX
#define MASK 0x00001fff
#elif CONF == 1 //SAFEST BUT SLOWEST
#define V_SIZE 512
#define OPT_K 16
#define CAPACITY 130
#define SWIFT 20 // 12 BITS FOR INDEX
#define MASK 0x00000fff
#endif
//1 CHAR == 8 BITS
#define BITS V_SIZE*8

struct b_filter{
    char bit_vector[V_SIZE];
    int inserted;
};
typedef struct b_filter b_filter;

void bf_init(b_filter* obj);
void bf_insert(b_filter* obj,void* input);
bool bf_lookup(b_filter* obj,void* input);
bool bf_full(b_filter* obj);

void bf_print(b_filter* obj);

struct filter{
    b_filter* arr;
    int size;
    int in_use;
};
typedef struct filter filter;

void filter_init(filter* obj);
void filter_fin(filter* obj);
void f_reuse(filter* obj);
bool f_append(filter* obj,void* input);
bool f_lookup(filter* obj,void* input);

#endif