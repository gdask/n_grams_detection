#ifndef HASHTABLE_H
#define HASHTABLE_H
#define KEYS 500

#include "trie_node.h"

typedef struct hashtable{
    children_arr* ca_bucket;
    int round;
    int size; //m buckets 
    int init_size;
    int p; //pointer p 
    int primary_size;
}hashtable;


//k: keys and m: number of children array size
void hashtable_init(hashtable* ht, int k, int m);
void hashtable_fin(hashtable* obj);

/*hash function for my string*/
int hash_function(hashtable* obj, char* input_str);
int hash_function_overflow(hashtable* obj, char* input_str);

/*search in the table for <value>, returns pointer to carray*/
loc_res hash_lookup(hashtable* obj, char* input);

/*Insert*/
void hashtable_insert(hashtable* obj, char* word);
bool hash_append(hashtable* obj, char* word);
bool ca_bucket_append(children_arr* obj, char* input_word, int goal_index);

/*handles if overflow should happend*/
void hashtable_overflow(hashtable* obj);
void update_round(hashtable* obj);

/*George complete copy*/
void hash_redistribute(hashtable* obj);
#endif