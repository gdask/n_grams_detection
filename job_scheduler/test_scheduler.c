#include <stdio.h>
#include "jobscheduler.h"

//SET DEBUG SCHEDULER = 1

int main(){
    job_scheduler test;
    job_scheduler_init(&test,4);
    Job task;
    int index,i;
    index=5;
    for(i=0;i<index;i++){
        task.version=i;
        js_submit_job(&test,task);
    }
    js_execute_jobs(&test);
    for(i=0;i<index;i++) fprintf(stderr,"Res: %d\n",test.tasks[i].version);

    for(i=1;i<=index;i++){
        task.version=-1;
        js_submit_job(&test,task);
    }
    js_execute_jobs(&test);
    for(i=0;i<index;i++) fprintf(stderr,"Res: %d\n",test.tasks[i].version);

    job_scheduler_fin(&test);
    return 0;
}