//static and simple hashtable, only buckets overflow
#ifndef TOPK_HASH_H
#define TOPK_HASH_H
#define HASH_SIZE 99
#define INIT_WORDLEN 128
#define TOPK_MUTEX 0

//In this part we use init_wordlen in order not to use malloc and free for each word, 
//maybe later this side should be changed.
typedef struct node{
    char* word;
    int freq;
}node;

typedef struct bucket{
    node* Node; //array of nodes
    int max_freq;
    int size;
    int first_available_slot;
}bucket;

#if TOPK_MUTEX == 1
#include <pthread.h>
typedef struct Hash{
    int max_freq;
    bucket* Bucket;
    pthread_mutex_t mtx;
}Hash;
#else
typedef struct Hash{
    int max_freq;
    bucket* Bucket;
}Hash;
#endif

void Hash_init(Hash*);
void Hash_fin(Hash*);
void Hash_reuse(Hash* obj);

void Hash_insert(Hash* obj, char* word, int word_len);
void display_Hash(Hash* obj);

void topk(Hash* obj, int k);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*For topK I need also an array of array of strings, each index is a frequency number
and array has size of max frequancy of hash table*/

typedef struct arr_node{
    char** arr_node;
    int size;
    int first_available_slot;
}arr_node;

typedef struct array{
    arr_node* nodes;
    int size;
}top_array;

void Array_init(top_array* obj, int size);
void Array_fin(top_array* obj);

//insert to top_array word. Insertion should be use alphabetical order
void Array_insert(top_array* obj, char* word, int goal_index);

//look up in arr_node to find the right position
int arr_node_lookup_bin(arr_node* obj, char* word);
//insert in right position
void arr_node_insert(arr_node* obj, char*word, int goal_index);

//print top k results. Start from the end
void display_topk(top_array* obj, int k);


typedef struct TopK{
    Hash Hash[HASH_SIZE];
}TopK;

#endif