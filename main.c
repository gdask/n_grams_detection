#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "trie_node.h"
#include "string_utils.h"
#include "trie.h"

int main(){
    clock_t start,end;
    start = clock();

    trie db;
    trie_init(&db,5);
    FILE *fp,*re,*in;
    in=fopen("small.init","r");
    fp=fopen("small.work","r");
    re=fopen("query_res.txt","w");

    //setvbuf (in,NULL,_IONBF,0);
    //setvbuf (fp,NULL,_IONBF,0);
    //setvbuf (re,NULL,_IONBF,0);

    bool has_line;
    //INIT FILE
    line_manager lmin;
    line_manager_init(&lmin,in, 'I');
    has_line = lm_fetch_line(&lmin);    
    while(has_line==true){
        lm_fetch_ngram(&lmin);
        trie_insert(&db,&lmin);
        has_line= lm_fetch_line(&lmin);
    }

    line_manager lm;
    line_manager_init(&lm,fp,'Q');
    result_manager rm;
    result_manager_init(&rm,re);
    //QUERY FILE
    has_line=lm_fetch_line(&lm);
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
        has_line=lm_fetch_line(&lm);
    }

    //tn_print_subtree(db.head);

    line_manager_fin(&lm);
    line_manager_fin(&lmin);
    result_manager_fin(&rm);
    fclose(fp);
    fclose(re);
    fclose(in);
    trie_fin(&db);

    end=clock();
    printf("Elapsed time:%f\n",((float)end-start)/CLOCKS_PER_SEC);

    return 0;
}