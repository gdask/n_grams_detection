#ifndef TRIE_H
#define	TRIE_H

#include "hashtable.h"
#include "trie_node.h"
#include "string_utils.h"
#include "filters/filter_manager.h"
#include <time.h>
#include <pthread.h>

struct trie{
    unsigned int version;
    bool dynamic;
    int ca_init_size;
    hashtable zero_level;
    size_t offset;
    filter_manager fm;
};
typedef struct trie trie;

//Those functions are not thread safe.
void trie_init(trie* obj,int threads,pthread_t *tids,int init_child_arr_size);
void trie_fin(trie* obj);
void trie_insert(trie* obj,line* l,int version);
//changes the timestamp of the proper nodes on the trie. Use for burst preparation
bool trie_mark_deleted(trie* obj,line* l,int version);
//makes the actual deletion from the trie. Use at the end of burst
bool trie_delete(trie* obj,line* l);
//Version ++
void trie_update_version(trie* obj);
//Transform dynamic trie to static for faster queries.
void trie_compress(trie* obj);
//Those functions are thread safe
clock_t trie_search(trie* obj,line* l,result *r, TopK* top,int version);
clock_t trie_static_search(trie* obj,line* l,result* r, TopK* top);



#endif