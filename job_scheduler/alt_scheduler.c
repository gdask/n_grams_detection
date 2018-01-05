#include "alt_scheduler.h"
#include <stdio.h>
#include <stdlib.h>

pthread_t* alt_scheduler_init(alt_scheduler* obj,int num_of_threads){
    obj->threads = num_of_threads;
    obj->tids = malloc(obj->threads*sizeof(pthread_t));
    obj->tasks = malloc(obj->threads*sizeof(Job*));
    if(obj->tids==NULL || obj->tasks==NULL){
        fprintf(stderr,"Job Scheduler malloc failed\n");
        exit(-1);
    }
    int i;
    for(i=0;i<obj->threads;i++){
        obj->tasks[i] = malloc(INIT_ARR_SIZE*sizeof(Job));
        if(obj->tasks[i]==NULL){
            fprintf(stderr,"Job Scheduler malloc failed\n");
            exit(-1);
        }
    }
    obj->num_of_jobs = malloc(obj->threads*sizeof(int));
    obj->job_arr_size = malloc(obj->threads*sizeof(int));
    if(obj->num_of_jobs==NULL || obj->job_arr_size==NULL){
        fprintf(stderr,"Job Scheduler malloc failed\n");
        exit(-1);
    }
    for(i=0;i<obj->threads;i++){
        obj->num_of_jobs[i]=0;
        obj->job_arr_size[i]=INIT_ARR_SIZE;
    }

    obj->run = true;
    obj->total_tasks=0;
    obj->inactive_workers=0;
    pthread_mutex_init(&obj->mtx,NULL);
    pthread_cond_init(&obj->job_pending,NULL);
    pthread_cond_init(&obj->job_done,NULL);
    for(i=0;i<obj->threads;i++) pthread_create(&obj->tids[i],0,alt_worker,obj);
    /*pthread_mutex_lock(&obj->mtx);
    while(obj->inactive_workers != obj->threads){
        pthread_cond_wait(&obj->job_done,&obj->mtx);
    }
    pthread_mutex_unlock(&obj->mtx);*/
    return obj->tids;
}

void alt_scheduler_fin(alt_scheduler* obj){
    obj->run = false;
    pthread_cond_broadcast(&obj->job_pending);
    int i;
    for(i=0;i<obj->threads;i++) pthread_join(obj->tids[i],NULL);

    pthread_mutex_destroy(&obj->mtx);
    pthread_cond_destroy(&obj->job_pending);
    pthread_cond_destroy(&obj->job_done);
    for(i=0;i<obj->threads;i++) free(obj->tasks[i]);
    free(obj->tasks);
    free(obj->num_of_jobs);
    free(obj->job_arr_size);
    free(obj->tids);
}

//Round robin submition
void alt_submit_job(alt_scheduler* obj,Job task){
    int t_index = obj->total_tasks%obj->threads;

    if(obj->num_of_jobs[t_index]==obj->job_arr_size[t_index]){
        obj->tasks[t_index] = realloc(obj->tasks[t_index],2*obj->job_arr_size[t_index]*sizeof(Job));
        if(obj->tasks[t_index]==NULL){
            fprintf(stderr,"Job Scheduler malloc failed\n");
            exit(-1);
        }
        obj->job_arr_size[t_index] *=2;
    }
    obj->tasks[t_index][obj->num_of_jobs[t_index]]=task;
    obj->num_of_jobs[t_index]++;
    obj->total_tasks++;
}

void alt_execute_jobs(alt_scheduler* obj){
    //tell workers to start
    obj->inactive_workers = 0;
    pthread_cond_broadcast(&obj->job_pending);
    //wait until all jobs are done
    pthread_mutex_lock(&obj->mtx);
    while(obj->inactive_workers != obj->threads){
        pthread_cond_wait(&obj->job_done,&obj->mtx);
    }
    pthread_mutex_unlock(&obj->mtx);

    int i;
    for(i=0;i<obj->threads;i++) obj->num_of_jobs[i]=0;
    obj->total_tasks=0;
    return;
}

void * alt_worker(void* input){
    alt_scheduler* obj = (alt_scheduler*)input;
    pthread_t id = pthread_self();
    //Every thread works on different row on task array
    int i,index=-1;
    for(i=0;i<obj->threads;i++){
        if(id == obj->tids[i]) index = i;
    }
    if(index==-1){
        fprintf(stderr,"Worker:%d not in tids",(int)id);
        exit(-1);
    }
    bool keep_running;
    while(1){
        pthread_mutex_lock(&obj->mtx);
        obj->inactive_workers++;
        pthread_cond_signal(&obj->job_done);
        pthread_cond_wait(&obj->job_pending,&obj->mtx);
        keep_running = obj->run;
        pthread_mutex_unlock(&obj->mtx);
        if(keep_running==false) pthread_exit(0);
        //process data
        for(i=0;i<obj->num_of_jobs[index];i++) execute_job(&obj->tasks[index][i]);
    }
}