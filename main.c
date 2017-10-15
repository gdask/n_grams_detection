#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"

int main(){
    trie_node head;
    tn_head(&head,2);
    char str1[] = "ab";
    char str2[] = "bc";
    char str3[] = "cd";
    
    trie_node *d11=tn_insert(&head,str1);
    tn_insert(d11,str1);
    tn_insert(&head,str3);
    tn_insert(&head,str2);
    tn_insert(&head,str1);
    tn_print_subtree(&head);

    trie_node *rh=tn_lookup(&head,str3);
    fprintf(stderr,"pointer at head str3: %p \n",rh);
    rh=tn_lookup(d11,str1);
    fprintf(stderr,"pointer at d11 str1: %p \n",rh);
    

    tn_fin(&head);
    return 0;
}