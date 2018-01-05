#ifndef ALT_SCHEDULER_H
#define ALT_SCHEDULER_H
#include "jobscheduler.h" //for job declaration
#include <pthread.h>
#include <stdbool.h>
#define INIT_ARR_SIZE 10

typedef struct alt_scheduler{
    pthread_t* tids;
    int threads;
    Job** tasks;
    int* num_of_jobs;
    int* job_arr_size;
    int total_tasks;
    int inactive_workers;
    pthread_mutex_t mtx;
    pthread_cond_t job_pending;
    pthread_cond_t job_done;
    bool run;
}alt_scheduler;


pthread_t* alt_scheduler_init(alt_scheduler* obj,int num_of_threads);
void alt_scheduler_fin(alt_scheduler* obj);
void alt_submit_job(alt_scheduler* obj,Job task);
//Signals all the workers to process jobs. Returns when all jobs are done
void  alt_execute_jobs(alt_scheduler* obj);
void * alt_worker(void* input);

#endif