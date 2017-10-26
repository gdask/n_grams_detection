#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
    trie db;
    trie_init(&db,5);
    FILE *fp,*re,*in;
    in=fopen("test.init","r");
    fp=fopen("test.work","r");
    re=fopen("query_res.txt","w");
    //INIT FILE
    line_manager lmin;
    line_manager_init(&lmin,in);
    bool has_line = lm_fetch_line(&lmin,'I');
    while(has_line==true){
        lm_fetch_ngram(&lmin);
        trie_insert(&db,&lmin);
        has_line= lm_fetch_line(&lmin,'I');
    }

    //tn_print_subtree(db.head);

    line_manager lm;
    line_manager_init(&lm,fp);
    result_manager rm;
    result_manager_init(&rm,re);
    //QUERY FILE
    has_line=lm_fetch_line(&lm,'Q');
    while(has_line==true){
        if(lm_is_insert(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_insert(&db,&lm);
        }
        else if(lm_is_delete(&lm)==true){
            lm_fetch_ngram(&lm);
            trie_delete(&db,&lm);
        }
        else if(lm_is_query(&lm)==true){
            trie_search(&db,&lm,&rm);
        }
        else{
            fprintf(stderr,"Corrupted line\n");
        }
        //tn_print_subtree(db.head);
        has_line=lm_fetch_line(&lm,'Q');
    }

    line_manager_fin(&lm);
    line_manager_fin(&lmin);
    result_manager_fin(&rm);
    fclose(fp);
    fclose(re);
    fclose(in);
    trie_fin(&db);
    return 0;
}