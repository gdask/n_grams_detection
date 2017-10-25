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

void trie_delete(trie* obj,line_manager* lm){
    char* current_word = lm_fetch_word(lm);
    if(current_word==NULL){
        return;
    }

    trie_node* current_node = obj->head;
    int current_index=-1;
    trie_node* last_fork = obj->head;
    int critical_index= tn_lookup_index(current_node,current_word);

    while(current_word!=NULL){
        current_index = tn_lookup_index(current_node,current_word);
        if(current_index < 0){
            //N_gram didnt found,nothing changes in trie
            return;
        }
        if(tn_has_fork(current_node)==true || (current_node->final==true && tn_has_child(current_node)==true)){
            last_fork = current_node;
            critical_index = current_index;
        }
        current_node = ca_get_pointer(&current_node->next,current_index);
        current_word = lm_fetch_word(lm);
    }

    if(tn_has_child(current_node)==true){
        tn_unset_final(current_node);
    }
    else{
        //deletes entire path from last_fork to leaf
        ca_force_delete(&last_fork->next,critical_index);
        //if(last_fork!=obj->head && tn_has_fork(last_fork)==false)tn_normal_to_leaf(last_fork);
    }

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