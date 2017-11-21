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
    tn_head(obj->head,obj->ca_init_size);
    obj->max_height = 0;
    obj->dynamic = true;
    pointer_set_init(&obj->detected_nodes,obj->ca_init_size*5);
    //filter_init(&obj->detected_nodes);
}

void trie_fin(trie* obj){
    tn_fin(obj->head);
    free(obj->head);
    pointer_set_fin(&obj->detected_nodes);
    //filter_fin(&obj->detected_nodes);
}


void trie_insert(trie* obj,line_manager* lm){
    if(obj->dynamic==false){
        fprintf(stderr,"Insert on static trie is not available");
        return;
    }
    char* current_word = lm_fetch_word(lm);
    trie_node* current_node = obj->head;
    int height=0;
    while(current_word!=NULL){
        //fprintf(stderr,"WORD:%s\n",current_word);
        current_node = tn_insert(current_node,obj->ca_init_size,current_word);
        height++;
        current_word = lm_fetch_word(lm);
    }
    if(current_node!=obj->head){
        tn_set_final(current_node);
    }
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
    ps_reuse(&obj->detected_nodes);
    //f_reuse(&obj->detected_nodes);

    while(valid_ngram==true){
        rm_new_ngram(rm);
        trie_node* current_node = obj->head;
        char* current_word = lm_fetch_word(lm);
        while(current_word!=NULL){
            current_node = tn_lookup(current_node,current_word);
            if(current_node==NULL){
                rm_ngram_undetected(rm);
                break;
            }
            rm_append_word(rm,current_word);
            if(current_node->mode=='s'){ //hyper node
                hyper_node* tmp =(hyper_node*)current_node;
                if(tmp->Word_Info[0]>0){ //Final n_gram case
                    if(ps_append(&obj->detected_nodes,tmp->Word_Vector)==true){
                        rm_ngram_detected(rm, na);
                    }
                }
                trie_hyper_search(obj,lm,rm,na,tmp);
                break;
            }
            if(current_node->final==true){
                if(ps_append(&obj->detected_nodes,current_node)==true){
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
            if(ps_append(&obj->detected_nodes,hyper_word)==true){
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

void pointer_set_init(pointer_set* obj,int init_size){
    obj->Size = init_size;
    obj->First_Available_Slot=0;
    if(init_size < 1){
        fprintf(stderr,"Pointer set init called with < 1 init size\n");
        exit(-1);
    }
    obj->Array = (void**)malloc(init_size*sizeof(void*));
    if(obj->Array==NULL){
        fprintf(stderr,"Pointer set init, malloc failed\n");
        exit(-1);
    }
}

void pointer_set_fin(pointer_set* obj){
    free(obj->Array);
}

void ps_reuse(pointer_set* obj){
    obj->First_Available_Slot=0;
}

//binary implementation
bool ps_append(pointer_set* obj,void* ptr){
    int lower_bound = 0;
    int upper_bound = obj->First_Available_Slot-1;
    int middle = (lower_bound + upper_bound)/2;
    while(lower_bound <= upper_bound){
        if(obj->Array[middle] < ptr){
            lower_bound = middle +1;
        }
        else if(obj->Array[middle] > ptr){
            upper_bound = middle -1;
        }
        else{ // INPUT ALREADY IN
            return false;
        }
        middle = (lower_bound+upper_bound)/2;
    }
    //INSERT INPUT AT LOWER BOUND
    if(obj->Size == obj->First_Available_Slot){
        obj->Array = (void**)realloc(obj->Array,obj->Size*2*sizeof(void*));
        if(obj->Array==NULL){
            fprintf(stderr,"ps resize realloc failed\n");
            exit(-1);
        }
        obj->Size = obj->Size*2;
    }
    size_t movable =(obj->First_Available_Slot - lower_bound)*sizeof(void*);
    memmove(&obj->Array[lower_bound+1],&obj->Array[lower_bound],movable);
    obj->Array[lower_bound] = ptr;
    obj->First_Available_Slot++;
    return true;
}