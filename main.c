#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"
//#include "string_utils.h"
//#include "trie.h"

int main(){
    trie_node head;
    tn_head(&head,2);
    trie_node* r1 = tn_insert(&head,"this");
    trie_node* r2 = tn_insert(r1,"is");
    trie_node* r3 = tn_insert(r2,"it");
    tn_set_final(r3);

    tn_print_subtree(&head);

    tn_fin(&head);
}