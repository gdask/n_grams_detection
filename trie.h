#ifndef TRIE_H
#define	TRIE_H

#include "trie_node.h"
#include "string_utils.h"

struct trie{
    trie_node *head;
    int max_height;
};
typedef struct trie trie;

void trie_init(trie* obj);
void trie_fin(trie* obj);
void trie_insert(trie* obj,line_manager* lm);
void trie_search(trie* obj,line_manager* lm,result_manager *rm);
void trie_delete(trie* obj,line_manager* lm);





#endif