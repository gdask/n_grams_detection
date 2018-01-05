#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "string_utils.h"
#include "trie.h"
#include "./job_scheduler/jobscheduler.h"
#include "./job_scheduler/alt_scheduler.h"
#include <pthread.h>
#include "control_panel.h"

int main(int argc,char* argv[]){
    //Argument checking
    if(argc!=5){
        fprintf(stderr,"Essential arguments -i 'init filename' -q 'query filename'\n");
        exit(-1);
    }
    FILE *query_file,*init_file;
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
    start = clock();

    #if ALT_SCHEDULER == 0
    pthread_t* (*scheduler_init)(job_scheduler*,int) = &job_scheduler_init;
    void (*scheduler_fin)(job_scheduler*) = &job_scheduler_fin;
    void (*submit_job)(job_scheduler*,Job) = &js_submit_job;
    void (*execute_jobs)(job_scheduler*) = &js_execute_jobs;
    job_scheduler js;
    #else
    pthread_t* (*scheduler_init)(alt_scheduler*,int) = &alt_scheduler_init;
    void (*scheduler_fin)(alt_scheduler*) = &alt_scheduler_fin;
    void (*submit_job)(alt_scheduler*,Job)= &alt_submit_job;
    void (*execute_jobs)(alt_scheduler*) = &alt_execute_jobs;
    alt_scheduler js;
    #endif
    //FIRST CREATE JOB SCHEDULER
    int threads = THREADS;
    pthread_t *thread_ids=scheduler_init(&js,threads);
    //THEN CREATE TRIE
    trie db;
    trie_init(&db,threads,thread_ids,INIT_SIZE);

    //Init trie from init file
    line_manager lm_init;
    line_manager_init(&lm_init,init_file,'I');
    line* current_line = lm_fetch_sequence_line(&lm_init);
    while(current_line!=NULL){
        trie_insert(&db,current_line,0);
        current_line = lm_fetch_sequence_line(&lm_init);
    }
    //return 0;
    char trie_status = lm_get_file_status(&lm_init);
    //compress trie if needed
    if(trie_status=='S')trie_compress(&db);
    line_manager_fin(&lm_init);
    fclose(init_file);

    //Execute batches
    line_manager queries;
    result_manager results;
    line_manager_init(&queries,query_file,'Q');
    rm_init(&results, stdout);
    Job task;
    task.arg1 = &db;
    task.version = 1;
    current_line = lm_fetch_sequence_line(&queries);
    while(current_line!=NULL){
        if(line_is_query(current_line)){
            task.arg2 = current_line;
            task.arg3 = rm_get_result(&results);
            submit_job(&js,task);
            current_line = lm_fetch_independent_line(&queries);
        }
        else if(line_is_insert(current_line)){
            task.version++;
            trie_insert(&db,current_line,task.version);
            current_line = lm_fetch_sequence_line(&queries);
        }
        else if(line_is_delete(current_line)){
            task.version++;
            trie_mark_deleted(&db,current_line,task.version);
            current_line = lm_fetch_independent_line(&queries);
        }
        else if(line_is_F(current_line)){
            rm_use_topk(&results,current_line->k);
            //fprintf(stdout, "F\n");
            execute_jobs(&js);
            rm_display_result(&results);
            //now we have to make actual deletions in trie
            //make sure that every resource is ready for the next batch
            //now delete nodes from trie
            int i;
            for(i=0;i<queries.first_available_slot;i++){
                if(line_is_delete(queries.line[i])){
                    trie_delete(&db,queries.line[i]);
                }
            }
            queries.first_available_slot=0;
            current_line = lm_fetch_sequence_line(&queries);
        }
    }

    line_manager_fin(&queries);
    rm_fin(&results);
    fclose(query_file);
    trie_fin(&db);
    scheduler_fin(&js);

    end=clock();
    fprintf(stderr,"%s Elapsed time:%f\n",argv[4],((float)end-start)/CLOCKS_PER_SEC);

    return 0;
}