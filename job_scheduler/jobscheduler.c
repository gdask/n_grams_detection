#include "jobscheduler.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

pthread_t* job_scheduler_init(job_scheduler* obj,int num_of_threads){
    obj->threads = num_of_threads;
    obj->tids = malloc(num_of_threads*sizeof(pthread_t));
    obj->job_arr_size = INIT_ARR_SIZE;
    obj->tasks = malloc(INIT_ARR_SIZE*sizeof(Job));
    if(obj->tids==NULL || obj->tasks==NULL){
        fprintf(stderr,"Job Scheduler malloc failed\n");
        exit(-1);
    }
    obj->next_job = 0;
    obj->num_of_jobs = 0;
    obj->thread_in_cs = 0;
    obj->run = true;
    pthread_mutex_init(&obj->mtx,NULL);
    pthread_cond_init(&obj->job_pending,NULL);
    pthread_cond_init(&obj->job_done,NULL);
    int i;
    for(i=0;i<obj->threads;i++) pthread_create(&obj->tids[i],0,worker,obj);
    return obj->tids;
}

void job_scheduler_fin(job_scheduler* obj){
    obj->run = false;
    obj->next_job = 0;
    obj->num_of_jobs = obj->threads;  
    pthread_cond_broadcast(&obj->job_pending);
    int i;
    for(i=0;i<obj->threads;i++) pthread_join(obj->tids[i],NULL);
    pthread_mutex_destroy(&obj->mtx);
    pthread_cond_destroy(&obj->job_pending);
    pthread_cond_destroy(&obj->job_done);
    free(obj->tids);
    free(obj->tasks);
}

void js_submit_job(job_scheduler* obj,Job task){
    if(obj->job_arr_size==obj->num_of_jobs){
        obj->tasks = realloc(obj->tasks,2*obj->job_arr_size*sizeof(Job));
        obj->job_arr_size *=2;
        if(obj->tasks==NULL){
            fprintf(stderr,"js_submit_job realloc failed\n");
            exit(-1);
        }
    }
    obj->tasks[obj->num_of_jobs] = task;
    obj->num_of_jobs++;
}

void js_execute_jobs(job_scheduler* obj){
    //tell workers to start
    pthread_cond_broadcast(&obj->job_pending);
    //wait until all jobs are done
    pthread_mutex_lock(&obj->mtx);
    while(obj->thread_in_cs - obj->threads != obj->num_of_jobs){
        pthread_cond_wait(&obj->job_done,&obj->mtx);
    }
    pthread_mutex_unlock(&obj->mtx);
    obj->num_of_jobs = 0;
    obj->next_job = 0;
    obj->thread_in_cs = obj->threads;
    return;
}

void * worker(void* input){
    //fprintf(stderr,"Thread created\n");
    job_scheduler* obj = (job_scheduler*)input;
    bool keep_running;
    Job *task;
    while(1){
        pthread_mutex_lock(&obj->mtx);
        obj->thread_in_cs++;
        pthread_cond_signal(&obj->job_done);
        //check if there is a job to be done
        while(obj->next_job == obj->num_of_jobs){
            pthread_cond_wait(&obj->job_pending,&obj->mtx);
        }
        keep_running = obj->run;
        task = &obj->tasks[obj->next_job];
        obj->next_job++;
        pthread_mutex_unlock(&obj->mtx);
        if(keep_running==false) pthread_exit(0);
        //process data
        execute_job(task);
    }
}

/*Job module doesnt know what function to call,therefor we use pointer to the proper
search function,which is set by trie*/
void execute_job(Job* obj){
    #if DEBUG_SCHEDULER != 1 //calls the proper search function for trie
    obj->arg1->trie_search(obj->arg1,obj->arg2,obj->arg3,obj->version);
    #else // just for debugging reasons
    obj->version++;
    #endif
}