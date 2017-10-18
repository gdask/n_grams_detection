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
            line_manager lm;
            line_manager_init(&lm, fp);
            bool result1=true;
            result1=lm_fetch_line(&lm);
            while(result1!=false){
                bool result2=lm_fetch_ngram(&lm);
                while(result2!=false){
                    char* word=lm_fetch_word(&lm);
                    while(word!=NULL){
                        printf("word %s\n", word);
                        word=lm_fetch_word(&lm);
                    }
                    result2=lm_fetch_ngram(&lm);
                }
                result1=lm_fetch_line(&lm);
            }    
        line_manager_fin(&lm);
        fclose(fp);
}