#ifndef TRIE_H
#define	TRIE_H

#include "trie_node.h"
#include "string_utils.h"

struct trie;
typedef struct trie trie;

stuct trie{
    trie_node *head;
    int max_height;
}

void trie_insert(trie* obj,line_manager* qm);
void trie_search(trie* obj,line_manager* qm,result_manager *rm);
void trie_delete(trie* obj,line_manager* qm);





#endif