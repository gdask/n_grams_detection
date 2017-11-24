#ifndef TRIE_H
#define	TRIE_H

#include "hashtable.h"
#include "trie_node.h"
#include "string_utils.h"
#include "filters/bloom_filter.h"
#include "filters/pointer_set.h"

#define HASH_BUCKETS_INIT 8
#define USE_BLOOM 0 //1 for bloom , 0 for pointer set
#define FILTER_INIT_SIZE 500

struct trie{
    bool dynamic;
    trie_node *head;
    int max_height;
    int ca_init_size;
    hashtable zero_level;
    #if USE_BLOOM==1
    filter detected_nodes;
    #else
    pointer_set detected_nodes;
    #endif
    //pointers to filter(bloom or pointer_set) functions,because function overloading is not an option in c!
    void (*reuse_filter)(void* obj);
    bool (*ngram_inserted)(void* obj,void* input);

};
typedef struct trie trie;

void trie_init(trie* obj,int init_child_arr_size);
void trie_fin(trie* obj);
void trie_insert(trie* obj,line_manager* lm);
void trie_search(trie* obj,line_manager* lm,result_manager *rm, ngram_array* na);
void trie_hyper_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na,hyper_node* current);
bool trie_delete(trie* obj,line_manager* lm);
void trie_compress(trie* obj);



#endif