#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
    /* //testing line manager
    line_manager lm;
    line_manager_init(&lm,stdin);

    lm_fetch_line(&lm);
    lm_fetch_ngram(&lm);

    printf("first word: %s \n",lm_fetch_word(&lm));
    printf("Second word: %s\n",lm_fetch_word(&lm));

    line_manager_fin(&lm);*/
    
    //Testing trie_nodes
    trie_node head;
    tn_head(&head,2);

    //n_gram insertion
    trie_node* n1 = tn_insert(&head,"this");
    trie_node* n2 = tn_insert(n1,"is");
    trie_node* n3 = tn_insert(n2,"a");
    trie_node* n4 = tn_insert(n3,"ngram");
    tn_set_final(n4);

    //second n_gram insertion
    trie_node* ng1 = tn_insert(&head,"this");
    trie_node* ng2 = tn_insert(ng1,"is");
    trie_node* ng3 = tn_insert(ng2,"sparta");
    tn_set_final(ng3);

    tn_print_subtree(&head);

    ca_force_delete(&ng2->next,1);
    tn_print_subtree(&head);

    tn_fin(&head);
}