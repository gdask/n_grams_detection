#include "trie.h"
#include <stdlib.h>

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
    pointer_set_init(&obj->detected_nodes,obj->ca_init_size*5);
}

void trie_fin(trie* obj){
    tn_fin(obj->head);
    free(obj->head);
    pointer_set_fin(&obj->detected_nodes);
}


void trie_insert(trie* obj,line_manager* lm){
    char* current_word = lm_fetch_word(lm);
    trie_node* current_node = obj->head;
    int height=0;
    while(current_word!=NULL){
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

void trie_search(trie* obj,line_manager* lm,result_manager* rm){
    bool valid_ngram = lm_fetch_ngram(lm);
    rm_start(rm,obj->max_height);
    ps_reuse(&obj->detected_nodes,lm_n_gram_counter(lm));

    while(valid_ngram==true){
        rm_new_ngram(rm);
        trie_node* current_node = obj->head;
        char* current_word = lm_fetch_word(lm);
        while(current_word!=NULL){
            printf("Trie search : %s\n", current_word);
            current_node = tn_lookup(current_node,current_word);
            if(current_node==NULL){
                rm_ngram_undetected(rm);
                break;
            }
            rm_append_word(rm,current_word);
            if(current_node->final==true && ps_append(&obj->detected_nodes,current_node)==true){
                rm_ngram_detected(rm);
            }
            current_word = lm_fetch_word(lm);
        }
        valid_ngram=lm_fetch_ngram(lm);
    }
    rm_completed(rm);
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

void ps_reuse(pointer_set* obj, int new_size){
    if(obj->Size < new_size){
        obj->Array = (void**)realloc(obj->Array,new_size*2*sizeof(void*));
        if(obj->Array==NULL){
            fprintf(stderr,"ps resize realloc failed\n");
            exit(-1);
        }
        obj->Size = obj->Size*2;
    }
    obj->First_Available_Slot=0;
}

bool ps_append(pointer_set* obj,void* ptr){
    int i;
    for(i=0;i<obj->First_Available_Slot;i++){
        if(obj->Array[i]==ptr) return false;
    }
    obj->Array[obj->First_Available_Slot]= ptr;
    obj->First_Available_Slot++;
    return true;
}