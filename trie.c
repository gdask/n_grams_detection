#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

void tn_init(trie_node* obj){
    obj->Initialized = true;
    obj->Leaf = true;
    obj->Word = NULL;
    obj->Children.Initialized = false;
    fprintf(stderr,"Trie_node init\n");
}

void tn_fin(trie_node* obj){
    fprintf(stderr,"Trie_Node fin\n");
    if(obj->Word!=NULL){
        free(obj->Word);
    }
    if(obj->Children.Initialized){
        ca_fin(&obj->Children);
    }
}

int tn_compare(trie_node* obj,char* input_word){
    if(obj->Word!=NULL){
        return strcmp(obj->Word,input_word);
    }
    return 0; //WHAT NOW?
}

void ca_init(children_arr* obj,int init_size){
    if(init_size==0){
        fprintf(stderr,"children_arr_size called with 0 init_size");
        exit(-1);
    }
    if(obj->Initialized==true){
        fprintf(stderr,"children_arr already initialized\n");
        exit(-1);
    }
    obj->Array = malloc(init_size*sizeof(trie_node));
    if(obj->Array==NULL){
        fprintf(stderr,"Malloc Failed\n");
        exit(-1);
    }
    obj->Initialized=true;
    obj->Size = init_size;
    obj->First_Available_Slot = 0;
}

void ca_fin(children_arr* obj){
    if(obj->Initialized){
        int i;
        for(i=0;i<obj->First_Available_Slot;i++) tn_fin(&obj->Array[i]);
        free(obj->Array);
    }
    fprintf(stderr,"Children_arr fin\n");
}

void ca_double(children_arr* obj){
    if(obj->Initialized==false){
        fprintf(stderr,"children_arr_double called for a unitialized object\n");
        exit(-1);
    }
    trie_node* temp = realloc(obj->Array,2*obj->Size*sizeof(trie_node));
    if(temp==NULL){
        fprintf(stderr,"Realloc Failed\n");
        exit(-1);
    }
    obj->Array= temp;
    obj->Size = obj->Size*1;
}

int ca_locate(children_arr *obj,char *input_word){
    if(obj->Initialized==false){
        fprintf(stderr,"ca_locate called on a unitialized object\n");
        exit(-1);
    }
    int i,goal_index=0;
    for(i=0;i<obj->First_Available_Slot;i++){
        if(tn_compare(&obj->Array[i],input_word)>=0){
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
    if(tn_compare(&(obj->Array[goal_index]),input_word)==0){
        return true;
    }
    return false;
}

void ca_force_append(children_arr* obj,char* input_word,int goal_index){
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        //DOUBLE SIZE 
    }
    else if(goal_index > obj->Size-1 || goal_index > obj->First_Available_Slot){
        //ERROR ON GOAL INDEX OUT OF BOUNDS
    }

    if(goal_index != obj->First_Available_Slot){
        //MEMMOVE DATA
    }
    //ARR[INDEX] INIT
    //ARR[INDEX] SET STRING?
}