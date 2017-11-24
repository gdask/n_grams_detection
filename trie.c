#include "trie.h"
#include <stdlib.h>
#include <string.h>

void trie_init(trie* obj,int init_child_arr_size){
    if(init_child_arr_size < 1){
        fprintf(stderr,"Trie init called with wrong init child arr size\n");
        exit(-1);
    }
    obj->ca_init_size = init_child_arr_size;
    obj->head = (trie_node*)malloc(sizeof(trie_node));
    if(obj->head==NULL){
        fprintf(stderr,"Malloc failed in trie init\n");
        exit(-1);
    }
    //tn_head(obj->head,obj->ca_init_size);
    obj->max_height = 0;
    obj->dynamic = true;
    hashtable_init(&obj->zero_level,HASH_BUCKETS_INIT,init_child_arr_size);
    #if USE_BLOOM == 1
    //filter_init(&obj->detected_nodes,1000);
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
    //tn_fin(obj->head);
    free(obj->head);
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
    fprintf(stderr,"%s\n", current_word);
    trie_node* current_node = hashtable_insert(&obj->zero_level,current_word);
    if(current_node==NULL){
        fprintf(stderr,"NULL CURRENT NODE EXCEPTION\n");
        exit(-1);
    }
    int height=1;
    current_word = lm_fetch_word(lm);
    while(current_word!=NULL){
        //fprintf(stderr,"WORD:%s\n",current_word);
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

    loc_res current;
    current.node_ptr = obj->head;

    trie_node* last_fork = obj->head;
    int critical_index = -1;
    bool get_critical_index = true;

    while(current_word!=NULL){
        current = ca_locate_bin(&current.node_ptr->next,current_word);
        if(current.found == false){
            //N_gram didnt found,nothing changes in trie
            return false;
        }
        if(get_critical_index==true){
            critical_index = current.index;
            get_critical_index=false;
        }
        if(tn_has_fork(current.node_ptr)==true){
            last_fork = current.node_ptr;
            get_critical_index=true;
        }
        current_word = lm_fetch_word(lm);
    }

    if(tn_has_child(current.node_ptr)==true){
        tn_unset_final(current.node_ptr);
    }
    else{
        //deletes entire path from last_fork to leaf
        ca_force_delete(&last_fork->next,critical_index);
    }
    return true;
}

void trie_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na){
    bool valid_ngram = lm_fetch_ngram(lm);
    rm_start(rm,obj->max_height);
    obj->reuse_filter(&obj->detected_nodes);

    while(valid_ngram==true){
        rm_new_ngram(rm);
        char* current_word = lm_fetch_word(lm);
        if(current_word==NULL){
            //valid_ngram = lm_fetch_ngram(lm);
            //continue;
            return;
        }
        loc_res current_node = hash_lookup(&obj->zero_level,current_word);
        if(current_node.found==false){
            valid_ngram= lm_fetch_ngram(lm);
            continue;
        }
        current_word = lm_fetch_word(lm);
        while(current_word!=NULL){
            current_node.node_ptr = tn_lookup(current_node.node_ptr,current_word);
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
                }
                trie_hyper_search(obj,lm,rm,na,tmp);
                break;
            }
            if(current_node.node_ptr->final==true){
                if(obj->ngram_inserted(&obj->detected_nodes,current_node.node_ptr)==true){
                    rm_ngram_detected(rm, na);
                }
            }
            //CHECK CASE OF HYPER NODE HERE
            current_word = lm_fetch_word(lm);
        }
        valid_ngram=lm_fetch_ngram(lm);
    }
    rm_completed(rm);
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
    tn_compress(obj->head);
}