#include "trie.h"
#include <stdlib.h>

void trie_init(trie* obj){
    obj->head = malloc(sizeof(trie_node));
    if(obj->head==NULL){
        fprintf(stderr,"Malloc failed in trie init\n");
        exit(-1);
    }
    //TN init size have to change
    tn_head(obj->head,2);
    obj->max_height = 0;
}

void trie_fin(trie* obj){
    tn_fin(obj->head);
    free(obj->head);
}


void trie_insert(trie* obj,line_manager* lm){
    char* current_word = lm_fetch_word(lm);
    trie_node* current_node = obj->head;
    int height=0;
    while(current_word!=NULL){
        current_node = tn_insert(current_node,current_word);
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
    int current_index;
    trie_node* last_fork = obj->head;
    int critical_index=0;

    while(current_word!=NULL){
        current_index = tn_lookup_index(current_node,current_word);
        if(current_index < 0){
            //N_gram didnt found,nothing changes in trie
            return;
        }
        current_word = lm_fetch_word(lm);

        current_node = ca_get_pointer(&current_node->next,current_index);
        if(tn_has_fork(current_node)==true || (current_node->final==true && current_word!=NULL)){
            last_fork = current_node;
            critical_index = current_index;
        }
    }

    if(tn_has_child(current_node)==true){
        tn_unset_final(current_node);
    }
    else{
        //deletes entire path from last_fork to leaf
        ca_force_delete(&last_fork->next,critical_index);
        //tn_normal_to_leaf(last_fork);
    }

}