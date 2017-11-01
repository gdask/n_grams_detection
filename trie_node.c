#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie_node.h"

#define btoa(x) ((x)?"true":"false")

void tn_leaf(trie_node* obj,char* input_word){
    //obj->Initialized = true;
    obj->Leaf = true;
    obj->Head = false;
    obj->final = false;
    obj->Word = (char*)malloc(strlen(input_word)+1);
    if(obj->Word==NULL){
        fprintf(stderr,"Malloc failed in tn_leaf\n");
        exit(-1);
    }
    strcpy(obj->Word,input_word);
    obj->next.Initialized = false;
}

void tn_normal(trie_node* obj,int init_child_size,char* input_word){
    tn_leaf(obj,input_word);
    obj->Leaf=false;
    ca_init(&obj->next,init_child_size);
}

void tn_head(trie_node* obj,int init_child_size){
    //obj->Initialized=true;
    obj->next.Initialized=false;
    obj->Word=NULL;
    obj->Head=true;
    obj->Leaf=false;
    ca_init(&obj->next,init_child_size);
}

void tn_leaf_to_normal(trie_node* obj,int init_child_size){
    #if DEBUG != 0
    if(tn_is_leaf(obj)!=true){
        fprintf(stderr,"tn_leaf_to_normal called on a non leaf object\n");
        exit(-1);
    }
    #endif
    ca_init(&obj->next,init_child_size);
    obj->Leaf=false;
}

void tn_normal_to_leaf(trie_node* obj){
    #if DEBUG != 0
    if(tn_is_normal(obj)!=true){
        fprintf(stderr,"tn_normal_to_leaf called on a non tn_normal object\n");
        exit(-1);
    }
    #endif
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
    if(obj->Word!=NULL){
        free(obj->Word);
    }
    if(obj->next.Initialized==true){
        ca_fin(&obj->next);
    }
}


trie_node* tn_lookup(trie_node* obj,char* input_word){
    #if DEBUG != 0
    if(tn_is_leaf(obj)!=true && tn_is_head(obj)!=true && tn_is_normal(obj)!=true){
        fprintf(stderr,"tn lookup called on a non valid trie_node object");
        exit(-1);
    }
    #endif
    if(tn_is_leaf(obj)==true){
        //Leaf has no childern
        return NULL;
    }
    loc_res result = ca_locate_bin(&obj->next,input_word);
    return result.node_ptr;
}

trie_node* tn_insert(trie_node* obj,int init_child_size,char* input_word){
    #if DEBUG != 0
    if(tn_is_leaf(obj)!=true && tn_is_head(obj)!=true && tn_is_normal(obj)!=true){
        fprintf(stderr,"tn insert called on a non valid trie_node object");
        exit(-1);
    }
    #endif
    if(tn_is_leaf(obj)==true){
        tn_leaf_to_normal(obj,init_child_size);
        ca_force_append(&obj->next,input_word,0);
        return &obj->next.Array[0];
    }
    //Search in children
    loc_res result = ca_locate_bin(&obj->next,input_word);
    if(result.found==true){
        return result.node_ptr;
    }
    else{
        ca_force_append(&obj->next,input_word,result.index);
        return &obj->next.Array[result.index];
    }
}

void tn_set_final(trie_node* obj){
    #if DEBUG != 0
    if(tn_is_head(obj)==true){
        fprintf(stderr,"tn_is_head called on a head trie node\n");
        exit(-1);
    }
    #endif
    obj->final=true;
}

void tn_unset_final(trie_node* obj){
    #if DEBUG != 0
    if(tn_is_head(obj)==true){
        fprintf(stderr,"tn_is_head called on a head trie node\n");
        exit(-1);
    }
    #endif
    obj->final=false;
}
/* A node has fork when has 2 or more entries, or if it has at least one child & is a final n_gram
*/
bool tn_has_fork(trie_node* obj){
    if(tn_is_leaf(obj)==true){
        return false;
    }
    if(obj->next.First_Available_Slot > 1 || (obj->next.First_Available_Slot==1 && obj->final==true)){
        return true;
    }
    return false;
}

bool tn_has_child(trie_node* obj){
    if(tn_is_leaf(obj)==true){
        return false;
    }
    if(obj->next.First_Available_Slot < 1){
        return false;
    }
    return true;
}

void tn_print_subtree(trie_node* obj){
    if(tn_is_leaf(obj)==true){
        fprintf(stderr,"Leaf node on %p ,Word= %s ,Final = %s \n\n",obj,obj->Word,btoa(obj->final));
        return;
    }
    else if(tn_is_normal(obj)==true){
        fprintf(stderr,"Normal node on %p ,Word= %s ,Final = %s \n",obj,obj->Word,btoa(obj->final));
    }
    else if(tn_is_head(obj)==true){
        fprintf(stderr,"\nHead node on %p \n",obj);
    }
    else{
        fprintf(stderr,"tn_print_subtree called on an invalid object %p\n",obj);
        exit(-1);
    }
    fprintf(stderr,"Size of children_array = %d ,Children ptr = ",obj->next.Size);
    int i;
    for(i=0;i<obj->next.First_Available_Slot;i++) fprintf(stderr," %p ",&obj->next.Array[i]);
    fprintf(stderr,"\n\n");

    for(i=0;i<obj->next.First_Available_Slot;i++) tn_print_subtree(&obj->next.Array[i]);
}


void ca_init(children_arr* obj,int init_size){
    #if DEBUG != 0
    if(init_size==0){
        fprintf(stderr,"children_arr_size called with 0 init_size\n");
        exit(-1);
    }
    if(obj->Initialized==true){
        fprintf(stderr,"children_arr called on an already initialized object\n");
        exit(-1);
    }
    #endif
    obj->Array = (trie_node*)malloc(init_size*sizeof(trie_node));
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
            tn_fin(&obj->Array[i]);
            //free(obj->Array[i]);   
        }
        free(obj->Array);
    }
    obj->Initialized=false;
}

void ca_double(children_arr* obj){
    #if DEBUG != 0
    if(obj->Initialized==false){
        fprintf(stderr,"children_arr_double called for a unitialized object\n");
        exit(-1);
    }
    #endif
    trie_node* temp = (trie_node*)realloc(obj->Array,2*obj->Size*sizeof(trie_node));
    if(temp==NULL){
        fprintf(stderr,"Realloc Failed\n");
        exit(-1);
    }
    obj->Array= temp;
    obj->Size = obj->Size*2;
}

void ca_force_append(children_arr* obj,char* input_word,int goal_index){
    #if DEBUG != 0
    if(obj->Initialized==false){
        fprintf(stderr,"ca_force_append called on a unitialized object\n");
        exit(-1);
    }
    #endif
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        ca_double(obj); 
    }
    else if(goal_index > obj->Size-1 || goal_index > obj->First_Available_Slot){
        fprintf(stderr,"ca_force_append called with out of bounds goal_index\n");
        exit(-1);
    }
    if(goal_index != obj->First_Available_Slot){
        size_t movable= (obj->First_Available_Slot - goal_index)*sizeof(trie_node);
        memmove(&obj->Array[goal_index+1],&obj->Array[goal_index],movable);
    }
    obj->First_Available_Slot++;
    //obj->Array[goal_index] = malloc(sizeof(trie_node));
    //if(obj->Array[goal_index]==NULL){
    //    fprintf(stderr,"Malloc failed in ca_force_append\n");
    //    exit(-1);
    //}
    tn_leaf(&obj->Array[goal_index],input_word);
}

void ca_force_delete(children_arr* obj,int goal_index){
    #if DEBUG != 0
    if(obj->Initialized==false){
        fprintf(stderr,"ca_force_delete called on an uninitialized object at %p\n",obj);
        exit(-1);
        //return;
    }
    if(goal_index >= obj->First_Available_Slot || goal_index < 0){
        fprintf(stderr,"ca_force_delete called with out of bounds goal_index ,FAS: %d ,%d\n",goal_index,obj->First_Available_Slot);
        exit(-1);
        //return;
    }
    #endif
    if(goal_index == obj->First_Available_Slot-1){
        tn_fin(&obj->Array[goal_index]);
        obj->First_Available_Slot--;
        return;
    }
    trie_node* temp = &obj->Array[goal_index];
    tn_fin(temp);
    //free(temp);

    size_t movable =(obj->First_Available_Slot-goal_index-1)*sizeof(trie_node);
    memmove(&obj->Array[goal_index],&obj->Array[goal_index+1],movable);
    obj->First_Available_Slot--;
}

//That function assumes that is called on an initiliazed object
//And every trie_node from [0-First_Available_Slot-1] is normal or leaf
loc_res ca_locate_bin(children_arr* obj,char* input_word){
    loc_res result;
    result.index=0;
    result.found=false;
    result.node_ptr=NULL;

    if(obj->Initialized==false){
        //fprintf(stderr,"ca_locate_bin called on unitialized object");
        return result;
    }

    int lower_bound = 0;
    int upper_bound = obj->First_Available_Slot-1;
    int middle = (lower_bound+upper_bound)/2;

    while(lower_bound <= upper_bound){
        int cmp_res = strcmp(obj->Array[middle].Word,input_word);
        if(cmp_res < 0){
            lower_bound = middle + 1;
        }
        else if(cmp_res > 0){
            upper_bound = middle - 1;
        }
        else{
            //Array[middle] == input_word
            result.index = middle;
            result.found = true;
            result.node_ptr = &obj->Array[middle];
            return result;
        }
        middle = (lower_bound+upper_bound)/2;
    }
    //Input word not in array,should be placed in lower bound,IRENE CHECK
    result.index=lower_bound;
    return result;
}