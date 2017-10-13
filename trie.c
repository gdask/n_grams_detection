#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

void tn_leaf(trie_node* obj,char* input_word){
    //obj->Initialized = true;
    obj->Leaf = true;
    obj->Head = false;
    if(strlen(input_word)>=MAX_WORD_SIZE-1){
        fprintf(stderr,"Input word too long\n");
        exit(-1);
    }
    strncpy(obj->Word,input_word,MAX_WORD_SIZE);
    obj->next.Initialized = false;
    fprintf(stderr,"Tn_leaf init\n");
}

void tn_normal(trie_node* obj,int init_child_size,char* input_word){
    tn_leaf(obj,input_word);
    obj->Leaf=false;
    ca_init(&obj->next,init_child_size);
}

void tn_head(trie_node* obj,int init_child_size){
    //obj->Initialized=true;
    obj->Head=true;
    obj->Leaf=false;
    ca_init(&obj->next,init_child_size);
}

void tn_leaf_to_normal(trie_node* obj,int init_child_size){
    if(tn_is_leaf(obj)!=true){
        fprintf(stderr,"tn_leaf_to_normal called on a non leaf object\n");
        exit(-1);
    }
    ca_init(&obj->next,init_child_size);
    obj->Leaf=false;
}

void tn_normal_to_leaf(trie_node* obj){
    if(tn_is_normal(obj)!=true){
        fprintf(stderr,"tn_normal_to_leaf called on a non tn_normal object\n");
        exit(-1);
    }
    ca_fin(&obj->next);
    obj->Leaf=true;
}

bool tn_is_leaf(trie_node* obj){
    if(obj->Leaf==true && obj->Head==false && obj->next.Initialized==false){
        return true;
    }
    return false;
}

bool tn_is_normal(trie_node* obj){
    if(obj->Leaf==false && obj->Head==false && obj->next.Initialized==true){
        return true;
    }
    return false;
}

bool tn_is_head(trie_node* obj){
    if(obj->Leaf==false && obj->Head==true && obj->next.Initialized==true){
        return true;
    }
    return false;
}

void tn_fin(trie_node* obj){
    fprintf(stderr,"Trie_Node fin\n");
    //char[MAX] is a static variable,doesnt need deallocation.
    if(obj->next.Initialized==true){
        ca_fin(&obj->next);
    }
}

int tn_compare(trie_node* obj,char* input_word){
    if(tn_is_leaf(obj)==false && tn_is_normal(obj)==false){
        fprintf(stderr,"tn_compare called on false object\n");
        exit(-1);
    }
    return strcmp(obj->Word,input_word);
}

void ca_init(children_arr* obj,int init_size){
    if(init_size==0){
        fprintf(stderr,"children_arr_size called with 0 init_size\n");
        exit(-1);
    }
    if(obj->Initialized==true){
        fprintf(stderr,"children_arr called on an already initialized object\n");
        exit(-1);
    }
    obj->Array = malloc(init_size*sizeof(trie_node*));
    if(obj->Array==NULL){
        fprintf(stderr,"Malloc Failed\n");
        exit(-1);
    }
    obj->Initialized=true;
    obj->Size = init_size;
    obj->First_Available_Slot = 0;
}

void ca_fin(children_arr* obj){
    if(obj->Initialized == true){
        int i;
        for(i=0;i<obj->First_Available_Slot;i++){
            tn_fin(obj->Array[i]);
            free(obj->Array[i]);   
        }
        free(obj->Array);
        fprintf(stderr,"Children_arr fin\n");
    }
    obj->Initialized=false;
}

void ca_double(children_arr* obj){
    if(obj->Initialized==false){
        fprintf(stderr,"children_arr_double called for a unitialized object\n");
        exit(-1);
    }
    trie_node** temp = realloc(obj->Array,2*obj->Size*sizeof(trie_node*));
    if(temp==NULL){
        fprintf(stderr,"Realloc Failed\n");
        exit(-1);
    }
    obj->Array= temp;
    obj->Size = obj->Size*2;
}

//ca_locate must implement a binary search later
int ca_locate(children_arr *obj,char *input_word){
    if(obj->Initialized==false){
        fprintf(stderr,"ca_locate called on a unitialized object\n");
        exit(-1);
    }
    int i,goal_index=0;
    for(i=0;i<obj->First_Available_Slot;i++){
        if(tn_compare(obj->Array[i],input_word)>=0){
            break;
        }
        goal_index++;
    }
    return goal_index;
}

bool ca_word_exists(children_arr* obj,char* input_word,int goal_index){
    if(obj->Initialized==false){
        fprintf(stderr,"ca_word_exists called on a unitialized object\n");
        exit(-1);
    }
    if(goal_index>=obj->First_Available_Slot){
        return false;
    }
    if(tn_compare(obj->Array[goal_index],input_word)==0){
        return true;
    }
    return false;
}

void ca_force_append(children_arr* obj,char* input_word,int goal_index){
    if(obj->Initialized==false){
        fprintf(stderr,"ca_force_append called on a unitialized object\n");
        exit(-1);
    }
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        ca_double(obj); 
    }
    else if(goal_index > obj->Size-1 || goal_index > obj->First_Available_Slot){
        fprintf(stderr,"ca_force_append called with out of bounds goal_index\n");
        exit(-1);
    }
    if(goal_index != obj->First_Available_Slot){
        size_t movable= (obj->First_Available_Slot - goal_index)*sizeof(trie_node*);
        memmove(&obj->Array[goal_index+1],&obj->Array[goal_index],movable);
    }
    obj->First_Available_Slot++;
    obj->Array[goal_index] = malloc(sizeof(trie_node*));
    if(obj->Array[goal_index]==NULL){
        fprintf(stderr,"Malloc failed in ca_force_append\n");
        exit(-1);
    }
    tn_leaf(obj->Array[goal_index],input_word);
}