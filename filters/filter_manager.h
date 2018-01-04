#ifndef FILTER_MANAGER_H
#define FILTER_MANAGER_H

#include <stdbool.h>
#include <pthread.h>
#include "bloom_filter.h"
#include "pointer_set.h"
#include "hash_pointer_set.h"
#include "../control_panel.h"

#if WHICH_FILTER==2
typedef filter abstract_filter;
#elif WHICH_FILTER==1
typedef pointer_set abstract_filter;
#else
typedef hash_pset abstract_filter;
#endif

//Filter Manager holds an array of selected type filter
//Every thread is associated with a different filter.
typedef struct filter_manager{
    int number_of_threads;
    pthread_t* t_ids;
    abstract_filter* detected_nodes;
    void (*reuse_filter)(void* obj);
    bool (*ngram_unique)(void* obj,void* input);
} filter_manager;
void filter_manager_init(filter_manager* obj,int threads,pthread_t* thread_ids);
void filter_manager_fin(filter_manager* obj);
//Get filter returns the appropriate filter(by reference)for the thread which called the function.
//Get filter also calls the reuse filter function and returns the proper function for that filter
//Of course its threadsafe,becase it doesnt write on any shared structure
abstract_filter* get_filter(filter_manager* obj,void** unique_func_ptr);

#endif