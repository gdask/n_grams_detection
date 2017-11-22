#include <stdio.h>
#include <string.h>
#include "hashtable.h"

/*djb2, to turn a string to int*/
unsigned long str_to_int(char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = (*str)++)){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int hash_function(hashtable* obj, char* str){
    unsigned long code;
    code=str_to_int(str);
    return code % (obj->size); //f(k)% size
}

void hashtable_init(hashtable* obj, int ca_bucket_size, int bucket_size){
    obj->ca_bucket = malloc(sizeof(children_arr) * ca_bucket_size);
    if(obj->ca_bucket==NULL){
        fprintf(stderr, "Error in malloc :: hashtable init");
        exit(-1);
    }
    //for each ca_bucket dont allocate now
    int i;
    for(i=0; i < ca_bucket_size; i++){
        //allocate ca_bucket
        ca_init(&obj->ca_bucket[i], bucket_size);
    }
    obj->round=0;
    obj->size=ca_bucket_size;
    obj->p=0;
    obj->primary_pages=bucket_size;
    obj->init_size=ca_bucket_size;
}

bool ca_bucket_append(children_arr* obj, char* input_word, int goal_index){
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        return false; 
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
    tn_leaf(&obj->Array[goal_index],input_word);
    return true;
}

bool hash_append(hashtable* obj, char* word){
    int key=hash_function(obj, word);
    
    loc_res res = ca_locate_bin(&obj->ca_bucket[key], word);
    bool overflow=false;
    if(res.found==false){
        overflow=ca_bucket_append(&obj->ca_bucket[key], word, res.index);
    }
    return overflow;
}

void ca_bucket_delete(children_arr* obj,int goal_index){
    if(goal_index == obj->First_Available_Slot-1){
        //tn_fin(&obj->Array[goal_index]);
        obj->First_Available_Slot--;
        return;
    }

    size_t movable =(obj->First_Available_Slot-goal_index-1)*sizeof(trie_node);
    memmove(&obj->Array[goal_index],&obj->Array[goal_index+1],movable);
    obj->First_Available_Slot--;
}

void hashtable_overflow(hashtable* obj){
    /*Expand table for one bucket*/
    children_arr* temp = (children_arr*) realloc(obj->ca_bucket, (obj->size+1)*sizeof(children_arr));
    if(temp==NULL){
        fprintf(stderr,"Realloc Failed\n");
        exit(-1);
    }
    obj->ca_bucket= temp;
    obj->size = obj->size+1;

    int new=obj->size-1;
    /*init new bucket*/
    ca_init(&obj->ca_bucket[new], obj->primary_pages);
    /*double children array for bucket number=p*/
    ca_double(&obj->ca_bucket[obj->p]);
    /*Redistribute trie_nodes between the new bucket and the bucket number p*/
    
}

void update_round(hashtable* obj){
    /*Checks if new_size is divined by init_size pointer p ++*/
    if(obj->size%obj->init_size==0){
        obj->p=0;
        obj->round++;
    }
    else{
        obj->p++;
    }
}

void hash_redistribute(hashtable* obj){
    /*take all trie nodes from children array and check if it can be move to new bucket*/
    int key=obj->p;
    int i=0,movable=0;

    while(obj->ca_bucket[key].First_Available_Slot < i){
        char* word=  obj->ca_bucket[key].Array[i].Word;
        int key_res=hash_function(obj, word);
        if(key_res!=obj->p){
            //GEORGE, make the move between obj->ca_bucket[key].Array[i] to obj->ca_bucket[obj->size-1]
            obj->ca_bucket[obj->size-1].Array[movable]=obj->ca_bucket[key].Array[i];
            movable++;
            ca_bucket_delete(&obj->ca_bucket[key],i);
        }
        i++;
    }
    obj->ca_bucket[obj->size-1].First_Available_Slot=movable;
}

void hashtable_insert(hashtable* obj, char* word){
    bool overflow;
    overflow=hash_append(obj, word);
    if(overflow==false){ //overflow should happend

    }
    update_round(obj);
}