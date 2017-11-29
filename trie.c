#include "trie.h"
#include <stdlib.h>
#include <string.h>

void trie_init(trie* obj,int init_child_arr_size){
    if(init_child_arr_size < 1){
        fprintf(stderr,"Trie init called with wrong init child arr size\n");
        exit(-1);
    }
    obj->ca_init_size = init_child_arr_size;
    obj->max_height = 0;
    obj->dynamic = true;
    hashtable_init(&obj->zero_level, HASH_BUCKETS_INIT, init_child_arr_size);
    #if USE_BLOOM == 1
    filter_init(&obj->detected_nodes,FILTER_INIT_SIZE);
    obj->reuse_filter = (void*)&f_reuse;
    obj->ngram_inserted = (void*)&f_append;
    #else
    pointer_set_init(&obj->detected_nodes,FILTER_INIT_SIZE);
    obj->reuse_filter = (void*)&ps_reuse;
    obj->ngram_inserted = (void*)&ps_append;
    #endif
}

void trie_fin(trie* obj){
    hashtable_fin(&obj->zero_level);
    #if USE_BLOOM == 1
    filter_fin(&obj->detected_nodes);
    #else
    pointer_set_fin(&obj->detected_nodes);
    #endif
}


void trie_insert(trie* obj,line_manager* lm){
    if(obj->dynamic==false){
        fprintf(stderr,"Insert on static trie is not available");
        return;
    }
    char* current_word = lm_fetch_word(lm);
    if(current_word==NULL) return;
    trie_node* current_node = hashtable_insert(&obj->zero_level,current_word);
    if(current_node==NULL){
        fprintf(stderr,"NULL CURRENT NODE EXCEPTION\n");
        exit(-1);
    }
    int height=1;
    current_word = lm_fetch_word(lm);
    while(current_word!=NULL){
        current_node = tn_insert(current_node,obj->ca_init_size,current_word);
        height++;
        current_word = lm_fetch_word(lm);
    }
    tn_set_final(current_node);
    if(height > obj->max_height){
        obj->max_height = height;
    }
}

bool trie_delete(trie* obj,line_manager* lm){
    if(obj->dynamic==false){
        fprintf(stderr,"Delete on static trie is not available");
        return false;
    }
    char* current_word = lm_fetch_word(lm);
    if(current_word==NULL){
        return false;
    }

    int hash_bucket = 0;
    loc_res current;
    children_arr* last_fork;
    int critical_index = -1;
    bool get_critical_index = true;
    bool hash_search = true;
    
    while(current_word!=NULL){
        if(hash_search==true){
            hash_search= false;
            current = hashtable_search(&obj->zero_level,current_word,&hash_bucket);
            last_fork = &obj->zero_level.ca_bucket[hash_bucket];
        }
        else{
            current = ca_locate_bin(&current.node_ptr->next,current_word);
        }
        if(current.found == false){
            //N_gram didnt found,nothing changes in trie
            return false;
        }
        if(get_critical_index==true){
            critical_index = current.index;
            get_critical_index=false;
        }
        if(tn_has_fork(current.node_ptr)==true){
            last_fork = &current.node_ptr->next;
            get_critical_index=true;
        }
        current_word = lm_fetch_word(lm);
    }

    if(tn_has_child(current.node_ptr)==true){
        tn_unset_final(current.node_ptr);
    }
    else{
        //deletes entire path from last_fork to leaf
        ca_force_delete(last_fork,critical_index);
    }
    return true;
}

clock_t trie_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na){
    clock_t start = clock();
    bool valid_ngram = lm_fetch_ngram(lm);
    rm_start(rm,obj->max_height);
    obj->reuse_filter(&obj->detected_nodes);

    while(valid_ngram==true){
        rm_new_ngram(rm);
        char* current_word = lm_fetch_word(lm);
        bool hash_search = true;
        loc_res current_node;
        while(current_word!=NULL){
            if(hash_search==true){
                current_node = hash_lookup(&obj->zero_level,current_word);
                hash_search = false;
            }
            else{
                current_node.node_ptr = tn_lookup(current_node.node_ptr,current_word);
            }
            if(current_node.node_ptr==NULL){
                rm_ngram_undetected(rm);
                break;
            }
            rm_append_word(rm,current_word);
            if(current_node.node_ptr->mode=='s'){ //hyper node
                hyper_node* tmp =(hyper_node*)current_node.node_ptr;
                if(tmp->Word_Info[0]>0){ //Final n_gram case
                    if(obj->ngram_inserted(&obj->detected_nodes,tmp->Word_Vector)==true){
                        rm_ngram_detected(rm, na);
                    }
                    trie_hyper_search(obj,lm,rm,na,tmp);
                }
                break;
            }
            if(current_node.node_ptr->final==true){
                if(obj->ngram_inserted(&obj->detected_nodes,current_node.node_ptr)==true){
                    rm_ngram_detected(rm, na);
                }
            }
            current_word = lm_fetch_word(lm);
        }
        valid_ngram=lm_fetch_ngram(lm);
    }
    rm_completed(rm);
    return clock() - start;
}

void trie_hyper_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na,hyper_node* current){
    char* current_word = lm_fetch_word(lm);
    char* hyper_word = current->Word_Vector + current->Word_Info[0];
    short* metadata = current->Word_Info +1;
    size_t bytes;
    bool final;
    while(current_word!=NULL){
        if(*metadata<0){
            bytes = -*metadata;
            final = false;
        }
        else if(*metadata>0){
            bytes = *metadata;
            final = true;
        }
        else{
            //HYPER NODE DOESNT HAVE ANY OTHER WORD
            return;
        }
        if(strcmp(current_word,hyper_word)!=0){
            rm_ngram_undetected(rm);
            return;
        }
        rm_append_word(rm,current_word);
        if(final==true){
            if(obj->ngram_inserted(&obj->detected_nodes,hyper_word)==true){
                rm_ngram_detected(rm, na);
            }
        }
        metadata++;
        hyper_word += bytes;
        current_word = lm_fetch_word(lm);
    }
    return;
}

void trie_compress(trie* obj){
    if(obj->dynamic==false){
        fprintf(stderr,"Trie object is already compressed\n");
        return ;
    }
    if(sizeof(hyper_node) > sizeof(trie_node)){
        fprintf(stderr,"Size of trie node must be greater or equal to Size of hyper node due to compability issues\n");
        fprintf(stderr,"trie_node: %d bytes, hyper_node: %d bytes\n",(int)sizeof(trie_node),(int)sizeof(hyper_node));
        fprintf(stderr,"Trie cannod be compressed\n");
        return;
    }
    obj->dynamic = false;
    //tn_compress(obj->head);
    //tn_compress every trie node on hashtable
    int i;
    for(i=0;i<obj->zero_level.size;i++){
        int j;
        for(j=0;j<obj->zero_level.ca_bucket->First_Available_Slot;j++){
            tn_compress(&obj->zero_level.ca_bucket->Array[j]);
        }
    }
}