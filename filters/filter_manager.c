#include "filter_manager.h"
#include <stdio.h>

void filter_manager_init(filter_manager* obj,int threads,pthread_t* thread_ids){
    obj->number_of_threads=threads;
    obj->t_ids=thread_ids;
    #if WHICH_FILTER == 2
    obj->detected_nodes = malloc(threads*sizeof(filter));
    #elif WHICH_FILTER == 1
    obj->detected_nodes = malloc(threads*sizeof(pointer_set));
    #else
    obj->detected_nodes = malloc(threads*sizeof(hash_pset));
    #endif
    if(obj->detected_nodes==NULL){
        fprintf(stderr,"Filter manager init,malloc failed\n");
        exit(-1);
    }
    int i;
    #if WHICH_FILTER == 2
    obj->ngram_unique = (void*)&f_append;
    obj->reuse_filter = (void*)&f_reuse;
    for(i=0;i<threads;i++) filter_init(&obj->detected_nodes[i],FILTER_INIT_SIZE);
    #elif WHICH_FILTER == 1
    obj->ngram_unique = (void*)&ps_append;
    obj->reuse_filter = (void*)&ps_reuse;
    for(i=0;i<threads;i++)  pointer_set_init(&obj->detected_nodes[i],FILTER_INIT_SIZE);
    #else
    obj->ngram_unique = (void*)&hash_pset_append;
    obj->reuse_filter = (void*)&hash_pset_reuse;
    for(i=0;i<threads;i++) hash_pset_init(&obj->detected_nodes[i],FILTER_INIT_SIZE);
    #endif


}

void filter_manager_fin(filter_manager* obj){
    int i;
    #if WHICH_FILTER == 2
    for(i=0;i<obj->number_of_threads;i++) filter_fin(&obj->detected_nodes[i]);
    #elif WHICH_FILTER == 1
    for(i=0;i<obj->number_of_threads;i++) pointer_set_fin(&obj->detected_nodes[i]);
    #else
    for(i=0;i<obj->number_of_threads;i++) hash_pset_fin(&obj->detected_nodes[i]);
    #endif
    free(&obj->detected_nodes);
}

abstract_filter* get_filter(filter_manager* obj,void** unique_func_ptr){
    *unique_func_ptr = obj->ngram_unique;
    int thread_id; //Init with the proper function
    int i;
    for(i=0;i<obj->number_of_threads;i++){ //finds the right filter
        if(pthread_equal(thread_id,obj->t_ids[i])!=0){
            obj->reuse_filter(&obj->detected_nodes[i]);
            return &obj->detected_nodes[i];
        }
    }
    fprintf(stderr,"Thread:%d,not in filter manager list\n",thread_id);
    exit(-1);
    return NULL;
}