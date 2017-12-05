#include "trie.h"
#include <stdlib.h>
#include <string.h>

void trie_init(trie* obj,int init_child_arr_size){
    if(init_child_arr_size < 1){
        fprintf(stderr,"Trie init called with wrong init child arr size\n");
        exit(-1);
    }
    trie_node* pt;
    //fprintf(stderr,"obj on %p, ca on %p\n",pt,&pt->next);
    obj->offset = (size_t)&pt->next -(size_t)pt;
    //fprintf(stderr,"obj on %p, ca on %p\n",pt,(void*)pt+obj->offset);

    obj->ca_init_size = init_child_arr_size;
    obj->max_height = 0;
    obj->dynamic = true;
    hashtable_init(&obj->zero_level, HASH_BUCKETS_INIT, init_child_arr_size);
    #if WHICH_FILTER == 2
    filter_init(&obj->detected_nodes,FILTER_INIT_SIZE);
    obj->reuse_filter = (void*)&f_reuse;
    obj->ngram_unique = (void*)&f_append;
    #elif WHICH_FILTER == 1
    pointer_set_init(&obj->detected_nodes,FILTER_INIT_SIZE);
    obj->reuse_filter = (void*)&ps_reuse;
    obj->ngram_unique = (void*)&ps_append;
    #else
    hash_pset_init(&obj->detected_nodes,FILTER_INIT_SIZE);
    obj->reuse_filter = (void*)&hash_pset_reuse;
    obj->ngram_unique = (void*)&hash_pset_append;
    #endif
}

void trie_fin(trie* obj){
    hashtable_fin(&obj->zero_level);
    #if WHICH_FILTER == 2
    filter_fin(&obj->detected_nodes);
    #elif WHICH_FILTER == 1
    pointer_set_fin(&obj->detected_nodes);
    #else
    hash_pset_fin(&obj->detected_nodes);
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
        if(current.node_ptr == NULL){
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

clock_t trie_search(trie* obj,line_manager* lm,result_manager* rm, TopK* top){
//clock_t trie_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na){
    clock_t start = clock();
    rm_start(rm,obj->max_height);
    obj->reuse_filter(&obj->detected_nodes);
    char* eof = &lm->buffer[lm->line_end];

    while(lm_fetch_ngram(lm)){
        rm_new_ngram(rm);
        char* current_word = lm_fetch_word(lm);
        loc_res current_node;

        //Search first word in hash table
        if(current_word==NULL) continue;
        int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
        //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
        current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;

        while(current_word<eof){
            //current_node.node_ptr = tn_lookup(current_node.node_ptr,current_word);
            current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
            if(current_node.node_ptr==NULL) break;
            rm_append_word(rm,current_word);
            //NEW CHANGE
            if(current_node.node_ptr->final==true){
                if(obj->ngram_unique(&obj->detected_nodes,current_node.node_ptr)==true){
                    rm_ngram_detected(rm, top);
                }
            }
            //get next word
            current_word += current_node.string_length+1;
            while(*current_word=='\0') current_word++;
        }
    }
    rm_completed(rm);
    return clock() - start;
}

clock_t trie_static_search(trie* obj,line_manager* lm,result_manager* rm, TopK* top){
//clock_t trie_static_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na){
    clock_t start = clock();
    rm_start(rm,obj->max_height);
    obj->reuse_filter(&obj->detected_nodes);
    char* eof = &lm->buffer[lm->line_end];

    while(lm_fetch_ngram(lm)){
        rm_new_ngram(rm);
        char* current_word = lm_fetch_word(lm);
        loc_res current_node;

        //Search first word in hash table
        if(current_word==NULL) continue;
        int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
        //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
        current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;

        while(current_word < eof){
            current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
            if(current_node.node_ptr==NULL) break;
            rm_append_word(rm,current_word);
            if(current_node.node_ptr->final==true){
                if(obj->ngram_unique(&obj->detected_nodes,current_node.node_ptr)==true){
                    rm_ngram_detected(rm, top);
                }
            }
            current_word += current_node.string_length+1;
            while(*current_word=='\0') current_word++;

            if(current_node.node_ptr->mode=='s'){ //hyper node
                //set buf at the second hyper node word
                char* buf1 = ((hyper_node*)current_node.node_ptr)->Word_Vector +current_node.string_length+2;
                //set input buffer at next word
                char* buf2 = current_word;
                //compare buffers
                while(current_word < eof){
                    //compare char by char
                    while(*buf1 !='\0' && *buf1==*buf2){
                        buf1++;
                        buf2++;
                    }
                    if(*buf1 !=*buf2) break;
                    //Words are the same
                    rm_append_word(rm,current_word);
                    //get final info bit
                    buf1++;
                    if(*buf1==true){
                        if(obj->ngram_unique(&obj->detected_nodes,buf1)){
                            rm_ngram_detected(rm, top);
                        }
                    }
                    //set buffers at next word
                    buf1++;
                    buf2++;
                    while(*buf2=='\0') buf2++;
                    current_word = buf2;
                }
                break;
            }
        }
    }
    rm_completed(rm);
    return clock() - start;
}

//It doesnt work with the last hyper node version
/*void trie_hyper_search(trie* obj,line_manager* lm,result_manager* rm, ngram_array* na,hyper_node* current){
    char* current_word = lm_fetch_word(lm);
    char* hyper_word = current->Word_Vector + strlen(current->Word_Vector)+1;
    short* metadata = current->Word_Info + 1;
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
            //rm_ngram_undetected(rm);
            return;
        }
        rm_append_word(rm,current_word);
        if(final==true){
            if(obj->ngram_unique(&obj->detected_nodes,hyper_word)==true){
                rm_ngram_detected(rm, na);
            }
        }
        metadata++;
        hyper_word += bytes;
        current_word = lm_fetch_word(lm);
    }
    return;
}*/


void trie_compress(trie* obj){
    if(obj->dynamic==false){
        fprintf(stderr,"Trie object is already compressed\n");
        return ;
    }
    if(sizeof(hyper_node) >= sizeof(trie_node)){
        fprintf(stderr,"Size of trie node must be greater or equal to Size of hyper node due to compability issues\n");
        fprintf(stderr,"trie_node: %d bytes, hyper_node: %d bytes\n",(int)sizeof(trie_node),(int)sizeof(hyper_node));
        fprintf(stderr,"Trie cannod be compressed\n");
        return;
    }
    obj->dynamic = false;
    //ca_compress every hashtable bucket
    int i;
    for(i=0;i<obj->zero_level.size;i++){
        ca_compress(&obj->zero_level.ca_bucket[i],0);
    }
}