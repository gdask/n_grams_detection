#ifndef HASHTABLE_H
#define HASHTABLE_H
#define KEYS 500

#include "control_panel.h"
#include "trie_node.h"

#define HASH_MASK HASH_BUCKETS_INIT-1
#define OV_HASH_MASK (2*HASH_BUCKETS_INIT) -1

typedef struct hashtable{
    children_arr* ca_bucket;
    int round;
    int size; //m buckets 
    int init_size;
    int p; //pointer p 
    int old_p;
    int primary_size;
    //worth a shot?
    int mask;
    int ov_mask;
}hashtable;


//k: keys and m: number of children array size
void hashtable_init(hashtable* ht, int buckets, int ca_init);
void hashtable_fin(hashtable* obj);

/*hash function for my string*/
int hash_function(hashtable* obj, char* input_str);
int hash_function_overflow(hashtable* obj, char* input_str);

/*search in the table for <value>, returns pointer to carray*/
loc_res hash_lookup(hashtable* obj, char* input);
loc_res hashtable_search(hashtable* obj, char* word, int* bucket);
int hash_get_bucket(hashtable* obj, char* word);
bool split(hashtable* obj, char* input);

/*Insert*/
trie_node* hashtable_insert(hashtable* obj, char* word);
trie_node* hash_append(hashtable* obj, char* word, bool* overflow);
bool ca_bucket_append(children_arr* obj, char* input_word, int goal_index);

/*handles if overflow should happend*/
void hashtable_overflow(hashtable* obj);
void update_round(hashtable* obj);

/*George complete copy*/
int hash_redistribute(hashtable* obj, char* word);

void hash_print(hashtable* obj,FILE* dump);
#endif