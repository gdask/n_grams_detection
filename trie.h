#ifndef TRIE_H
#define	TRIE_H

#include "hashtable.h"
#include "trie_node.h"
#include "string_utils.h"
#include "filters/filter_manager.h"
#include <time.h>
#include <pthread.h>

struct trie;
typedef struct trie trie;

struct trie{
    //unsigned int version;
    bool dynamic;
    int ca_init_size;
    hashtable zero_level;
    size_t offset;
    filter_manager fm;
    //pointer to proper search function,either dynamic search or static search
    void (*trie_search)(trie* obj,line* l,result *r,unsigned int version);
};

//Those functions are not thread safe.
void trie_init(trie* obj,int threads,pthread_t *tids,int init_child_arr_size);
void trie_fin(trie* obj);
void trie_insert(trie* obj,line* l,unsigned int version);
//changes the timestamp of the proper nodes on the trie. Use for burst preparation
bool trie_mark_deleted(trie* obj,line* l,unsigned int version);
//makes the actual deletion from the trie. Use at the end of burst
bool trie_delete(trie* obj,line* l);
//Version ++
//void trie_update_version(trie* obj);
//Transform dynamic trie to static for faster queries.
void trie_compress(trie* obj);
//Those functions are thread safe
void trie_search_dynamic(trie* obj,line* l,result *r,unsigned int version);
void trie_search_static (trie* obj,line* l,result* r,unsigned int version);
//static search doesnt use version,but takes it as argument for abstraction purposes

//debug print entrie struct
void trie_print(trie* obj);

#endif