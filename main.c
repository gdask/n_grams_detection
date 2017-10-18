#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
            //FILE* fp;
            //fp=fopen("commands.txt","r");
            /*if(fp==NULL){
                fprintf(stderr,"Fopen failed::main\n");
                exit(-1);
            }*/
            line_manager lm;
            line_manager_init(&lm, stdin);
            
            line_manager_fin(&lm);
        //fclose(fp);
}