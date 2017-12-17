#ifndef TRIE_H
#define	TRIE_H

#include "hashtable.h"
#include "trie_node.h"
#include "string_utils.h"
#include "filters/bloom_filter.h"
#include "filters/pointer_set.h"
#include "filters/hash_pointer_set.h"
#include <time.h>


#define WHICH_FILTER 0 // 2 for bloom, 1 for pointer set, 0 for hash pointer set 
#define FILTER_INIT_SIZE 500

struct trie{
    unsigned int version;
    bool dynamic;
    int ca_init_size;
    hashtable zero_level;
    size_t offset;
    #if WHICH_FILTER==2
    filter detected_nodes;
    #elif WHICH_FILTER==1
    pointer_set detected_nodes;
    #else
    hash_pset detected_nodes;
    #endif
    //pointers to filter(bloom or pointer_set) functions,because function overloading is not an option in c!
    void (*reuse_filter)(void* obj);
    bool (*ngram_unique)(void* obj,void* input);
};
typedef struct trie trie;

void trie_init(trie* obj,int init_child_arr_size);
void trie_fin(trie* obj);
void trie_insert(trie* obj,line_manager* lm);
//clock_t trie_search(trie* obj,line_manager* lm,result_manager *rm, ngram_array* na);
//clock_t trie_static_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na);
clock_t trie_search(trie* obj,int version,line_manager* lm,result_manager *rm, TopK* top);
clock_t trie_static_search(trie* obj,line_manager* lm,result_manager* rm, TopK* top);
//void trie_hyper_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na,hyper_node* current);
bool trie_delete(trie* obj,line_manager* lm);
//Deleted node pointers stays waiting this function until are permantly removed from trie.
void trie_remove_deleted_nodes(trie* obj);
//Version ++
void trie_update_version(trie* obj);
void trie_compress(trie* obj);



#endif