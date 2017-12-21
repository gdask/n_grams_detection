#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

#define DEBUG_SCHEDULER 0

#include "../string_utils.h"
#include "../trie.h"
#include <pthread.h>
#include <stdbool.h>

#define INIT_ARR_SIZE 10

typedef struct job{
    trie* arg1;
    line* arg2;
    result* arg3;
    int version;
}Job;
void execute_job(Job* obj);

typedef struct job_scheduler{
    pthread_t* tids;
    int threads;
    Job* tasks;
    int next_job;
    int num_of_jobs;
    int thread_in_cs;
    int job_arr_size;
    pthread_mutex_t mtx;
    pthread_cond_t job_pending;
    pthread_cond_t job_done;
    bool run;
}job_scheduler;

pthread_t* job_scheduler_init(job_scheduler* obj,int num_of_threads);
void job_scheduler_fin(job_scheduler* obj);
void js_submit_job(job_scheduler* obj,Job task);
//Signals all the workers to process jobs. Returns when all jobs are done
void  js_execute_jobs(job_scheduler* obj);
//void js_wait_all_jobs_to_finish(job_scheduler* obj);
void * worker(void* input);

#endif