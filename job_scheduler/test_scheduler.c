#include <stdio.h>
//#include "jobscheduler.h"
#include "alt_scheduler.h"

//SET DEBUG SCHEDULER = 1

int main(){
    /*job_scheduler test;
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
    return 0;*/
    Job task;
    alt_scheduler ajs;
    alt_scheduler_init(&ajs,4);
    fprintf(stderr,"Success\n");
    int i;
    for(i=0;i<5;i++)alt_submit_job(&ajs,task);
    alt_execute_jobs(&ajs);
    alt_scheduler_fin(&ajs);

}