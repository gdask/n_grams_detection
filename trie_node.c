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
    obj->Word = malloc(strlen(input_word)+1);
    if(obj->Word==NULL){
        fprintf(stderr,"Malloc failed in tn_leaf\n");
        exit(-1);
    }
    strcpy(obj->Word,input_word);
    obj->next.Initialized = false;
    //fprintf(stderr,"Tn_leaf init\n");
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
        //tn_print_subtree(obj);
        exit(-1);
    }
    ca_fin(&obj->next);
    obj->Leaf=true;
}

bool tn_is_leaf(trie_node* obj){
    //if(obj->Leaf==true) return true;
    if(obj->Leaf==true && obj->Head==false && obj->next.Initialized==false){
        return true;
    }
    return false;
}

bool tn_is_normal(trie_node* obj){
    //if(obj->Leaf==false && obj->Head==false) return true;
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
    //fprintf(stderr,"Trie_Node fin\n");
    if(obj->Word!=NULL){
        free(obj->Word);
    }
    if(obj->next.Initialized==true){
        ca_fin(&obj->next);
    }
}

int tn_compare(trie_node* obj,char* input_word){
    if(tn_is_leaf(obj)==false && tn_is_normal(obj)==false){
        fprintf(stderr,"tn_compare called on false object\n");
        tn_print_subtree(obj);
        exit(-1);
    }
    return strcmp(obj->Word,input_word);
}

int tn_lookup_index(trie_node* obj,char* input_word){
    if(tn_is_leaf(obj)!=true && tn_is_head(obj)!=true && tn_is_normal(obj)!=true){
        fprintf(stderr,"tn lookup called on a non valid trie_node object");
        exit(-1);
    }
    if(tn_is_leaf(obj)==true){
        //Leaf has no childern
        return -1;
    }
    int possible_index = ca_locate_index(&obj->next,input_word);
    if(ca_word_exists(&obj->next,input_word,possible_index)==true){
        //Using ca_ function protect us from possible illegal mem access.
        return possible_index;
    }
    return -1;
}

trie_node* tn_lookup(trie_node* obj,char* input_word){
    if(tn_is_leaf(obj)!=true && tn_is_head(obj)!=true && tn_is_normal(obj)!=true){
        fprintf(stderr,"tn lookup called on a non valid trie_node object");
        exit(-1);
    }
    if(tn_is_leaf(obj)==true){
        //Leaf has no childern
        return NULL;
    }
    return ca_get_pointer(&obj->next,tn_lookup_index(obj,input_word));
}

trie_node* tn_insert(trie_node* obj,int init_child_size,char* input_word){
    if(tn_is_leaf(obj)!=true && tn_is_head(obj)!=true && tn_is_normal(obj)!=true){
        fprintf(stderr,"tn insert called on a non valid trie_node object");
        exit(-1);
    }
    if(tn_is_leaf(obj)==true){
        tn_leaf_to_normal(obj,init_child_size);
        ca_force_append(&obj->next,input_word,0);
        return ca_get_pointer(&obj->next,0);
    }
    //Search in children
    int possible_index = ca_locate_index(&obj->next,input_word);
    if(ca_word_exists(&obj->next,input_word,possible_index)==true){
        return ca_get_pointer(&obj->next,possible_index);
    }
    else{
        ca_force_append(&obj->next,input_word,possible_index);
        return ca_get_pointer(&obj->next,possible_index);
    }
}

void tn_set_final(trie_node* obj){
    if(tn_is_head(obj)==true){
        fprintf(stderr,"tn_is_head called on a head trie node\n");
        exit(-1);
    }
    obj->final=true;
}

void tn_unset_final(trie_node* obj){
    if(tn_is_head(obj)==true){
        fprintf(stderr,"tn_is_head called on a head trie node\n");
        exit(-1);
    }
    obj->final=false;
}

bool tn_has_fork(trie_node* obj){
    if(tn_is_leaf(obj)==true){
        return false;
    }
    if(obj->next.First_Available_Slot < 2){
        return false;
    }
    return true;
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
    if(init_size==0){
        fprintf(stderr,"children_arr_size called with 0 init_size\n");
        exit(-1);
    }
    if(obj->Initialized==true){
        fprintf(stderr,"children_arr called on an already initialized object\n");
        exit(-1);
    }
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
        //fprintf(stderr,"Children_arr fin\n");
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
    obj->Size = obj->Size*2;
}

//ca_locate must implement a binary search later
int ca_locate_index(children_arr *obj,char *input_word){
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
    if(tn_compare(&obj->Array[goal_index],input_word)==0){
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

trie_node* ca_get_pointer(children_arr* obj,int goal_index){
    if(obj->Initialized==false){
        //return NULL;
        fprintf(stderr,"ca_get_pointer called on an uninitialized object\n");
        exit(-1);
    }
    if(goal_index < 0){
        return NULL;
    }
    if(goal_index < obj->First_Available_Slot){
        return &obj->Array[goal_index];
    }
    return NULL;
}

void ca_force_delete(children_arr* obj,int goal_index){
    if(obj->Initialized==false){
        fprintf(stderr,"ca_force_delete called on an uninitialized object at %p\n",obj);
        exit(-1);
    }
    if(goal_index >= obj->First_Available_Slot || goal_index < 0){
        fprintf(stderr,"ca_force_delete called with out of bounds goal_index ,FAS: %d ,%d\n",goal_index,obj->First_Available_Slot);
        exit(-1);
    }
    if(goal_index == obj->First_Available_Slot-1){
        tn_fin(&obj->Array[goal_index]);
        obj->First_Available_Slot--;
        return;
    }
    trie_node* temp = &obj->Array[goal_index];
    //fprintf(stderr,"subtree to be fin: %p\n",temp);
    //tn_print_subtree(temp);
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
    result.node=NULL;

    int lower_bound = 0;
    int upper_bound = obj->First_Available_Slot-1;
    int middle = (lower_bound+upper_bound)/2;

    while(lower_bound < upper_bound){
        int cmp_res = strcmp(obj->Array[middle].Word,input_word);
        if(cmp_res < 0){
            lower_bound = middle + 1;
        }
        else if(cmp_res > 0){
            upper_bound = middle - 1;
        }
        else{
            //Array[middle] == input_word
            break;
        }
        middle = (lower_bound+upper_bound)/2;
    }
    if(lower_bound > upper_bound){
        //Input word not in array,should be placed in lower bound,IRENE CHECK
        result.index=lower_bound;
    }
    else{
        result.index = middle;
        result.found = true;
        result.node = &Array[middle];
    }
    return result;
}