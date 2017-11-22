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
    int primary_pages;
}hashtable;


//k: keys and m: number of children array size
void hashtable_init(hashtable* ht, int k, int m);
void hash_fin(hashtable* ht);

/*hash function for my string*/
int hash_function(hashtable* obj, char* input_str);

/*search in the table for <value>, returns pointer to carray*/
void hash_search(hashtable* obj, char* input);

/*Insert*/
void hashtable_insert(hashtable* obj, char* word);
bool hash_append(hashtable* obj, char* word);
void hash_expand(hashtable* obj);

//overflow occures
void hash_bucket_split(hashtable* obj);

void hash_redistribute(hashtable* obj);
#endif