#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
    /*
    FILE* fp;
    fp=fopen("commands.txt","r");
    if(fp==NULL){
        fprintf(stderr,"Fopen failed::main\n");
        exit(-1);
    }
    trie tree;
    trie_init(&tree,4);

    line_manager lm;
    line_manager_init(&lm, fp); 

    while(lm_fetch_line(&lm)){
        
    }

   // tn_print_subtree(tree.head);

    line_manager_fin(&lm);
    trie_fin(&tree);
    fclose(fp);*/
    
    FILE *fp;
    fp=fopen("commands.txt","rw");
    line_manager lm;
    line_manager_init(&lm, fp);
    bool result=lm_fetch_line(&lm, 'I');
    char* word;
    while(result==true){
        bool result_=lm_fetch_ngram(&lm);
        while(result_==true){
            word=lm_fetch_word(&lm);
            while(word!=NULL){
                printf("word return: %s\n",word);
                word=lm_fetch_word(&lm);
            }
            result_=lm_fetch_ngram(&lm);
        }
        result=lm_fetch_line(&lm, 'I');
    }
    line_manager_fin(&lm);
    fclose(fp);
    return 0;
}