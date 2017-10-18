#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
    FILE* fp;
    fp=fopen("commands.txt","r");
    if(fp==NULL){
        fprintf(stderr,"Fopen failed::main\n");
        exit(-1);
    }
    trie tree;
    trie_init(&tree);

    line_manager lm;
    line_manager_init(&lm, fp); 

    while(lm_fetch_line(&lm)){
        
    }

   // tn_print_subtree(tree.head);

    line_manager_fin(&lm);
    trie_fin(&tree);
    fclose(fp);
    return 0;
}