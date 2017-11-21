#ifndef TRIE_H
#define	TRIE_H

#include "trie_node.h"
#include "string_utils.h"
#include "bloom_filter.h"

struct pointer_set{
    int Size;
    int First_Available_Slot;
    void** Array;
};
typedef struct pointer_set pointer_set;

void pointer_set_init(pointer_set* obj,int init_size);
void pointer_set_fin(pointer_set* obj);
void ps_reuse(pointer_set* obj);
bool ps_append(pointer_set* obj,void* ptr);

struct trie{
    bool dynamic;
    trie_node *head;
    int max_height;
    int ca_init_size;
    pointer_set detected_nodes;
    //filter detected_nodes;
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