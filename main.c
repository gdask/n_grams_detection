#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"

int main(){
    trie_node head;
    tn_head(&head,2);
    
    trie_node *d11=tn_insert(&head,"lalala");
    tn_insert(d11,"lolo");
    tn_insert(&head,"bb");
    tn_insert(&head,"ba");
    tn_insert(&head,"bb");
    tn_print_subtree(&head);

    tn_fin(&head);
    return 0;
}