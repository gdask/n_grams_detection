#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

/*Control Panel contains all important defined sizes that could be changed in program*/

//MAIN
#define THREADS 1
#define ALT_SCHEDULER 0 // 1 for alternative scheduler, 0 for regular
#define INIT_SIZE 4 //CHILDREN ARRAY INIT SIZE,HAS IMPACT ON HASHTABLE TOO

//TRIE NODE
#define HYPER_NODE_OPT 1 //OPTIMIZED STATIC 1 , STANDART 0

//HASHTABLE
#define HASH_BUCKETS_INIT 8

//STRING_UTILS
#define INIT_SIZE_BUF 512
#define NUMBER_OF_LINES 28

//FILTER MANAGER
#define WHICH_FILTER 0 // 2 for bloom, 1 for pointer set, 0 for hash pointer set (best option)
#define FILTER_INIT_SIZE 500 //Expected n_grams per query

//TOPK HASH
#define HASH_SIZE 99

#endif