#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "string_utils.h"
#include "trie.h"
#include "hashtable.h"

int main(int argc,char* argv[]){
    if(argc!=5){
        fprintf(stderr,"Essential arguments -i 'init filename' -q 'query filename'\n");
        exit(-1);
    }

    FILE *query_file,*result_file,*init_file;
    result_file = stdout;
    if(strcmp(argv[1],"-i")==0 && strcmp(argv[3],"-q")==0){
        init_file = fopen(argv[2],"r");
        query_file = fopen(argv[4],"r");
    }
    else if(strcmp(argv[3],"-i")==0 && strcmp(argv[1],"-q")==0){
        init_file = fopen(argv[4],"r");
        query_file = fopen(argv[2],"r");
    }
    else{
        fprintf(stderr,"Essential arguments -i 'init filename' -q 'query filename'\n");
        exit(-1);
    }

    if(init_file==NULL){
        perror("Init File: ");
        exit(-1);
    }
    if(query_file==NULL){
        perror("Query File: ");
        exit(-1);
    }


    clock_t start,end;
    clock_t queries=0;
    start = clock();

    trie db;
    trie_init(&db,4);

    //Ngrams array
    //ngram_array na;
    //na_init(&na);

    //Hash and ngram array
    TopK t;
    Hash_init(t.Hash);

    bool has_line;
    //INIT FILE
    line_manager lm_init;
    line_manager_init(&lm_init,init_file, 'I');
    has_line = lm_fetch_line(&lm_init, &t); 
    //has_line = lm_fetch_line(&lm_init, &na);    

    while(has_line==true){
        lm_fetch_ngram(&lm_init);
        trie_insert(&db,&lm_init);
        has_line= lm_fetch_line(&lm_init, &t);
        //has_line= lm_fetch_line(&lm_init, &na);
    }

    if(lm_init.file_status=='S'){ //Static files only
        fprintf(stderr,"Static\n");
        trie_compress(&db);
    }
    //hash_print(&db.zero_level);
    //return 0;
    char status = lm_init.file_status;
    line_manager_fin(&lm_init);
    end = clock();
    //fprintf(stderr,"Init & compress time:%f\n",((float)end-start)/CLOCKS_PER_SEC);


    line_manager lm;
    line_manager_init(&lm,query_file,'Q');
    result_manager rm;
    result_manager_init(&rm,result_file);

    //QUERY FILE
    if(status=='S'){
        has_line=lm_fetch_line(&lm, &t);
        //has_line=lm_fetch_line(&lm, &na);
        while(has_line==true){
            // /queries+=trie_static_search(&db,&lm,&rm, &na);
            queries+=trie_static_search(&db,&lm,&rm, &t);
            has_line=lm_fetch_line(&lm, &t);
            //has_line=lm_fetch_line(&lm, &na);
        }
    }
    else{
        has_line=lm_fetch_line(&lm, &t);
        //has_line=lm_fetch_line(&lm, &na);
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
                //queries+=trie_search(&db,&lm,&rm, &na);
                queries+=trie_search(&db,&lm,&rm, &t);
            }
            else{
                fprintf(stderr,"Corrupted line\n");
            }
            has_line=lm_fetch_line(&lm, &t);
            //has_line=lm_fetch_line(&lm, &na);
        }
    }

    fprintf(stderr,"Queries time:%f\n",((float)queries)/CLOCKS_PER_SEC);

    line_manager_fin(&lm);
    result_manager_fin(&rm);
    Hash_fin(t.Hash);
    //na_fin(&na);
    fclose(query_file);
    //fclose(result_file);
    fclose(init_file);
    trie_fin(&db);

    end=clock();
    fprintf(stderr,"Elapsed time:%f\n",((float)end-start)/CLOCKS_PER_SEC);

    return 0;
}