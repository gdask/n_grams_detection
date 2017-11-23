#include <stdio.h>
#include "hashtable.h"

/*Same as ca_force append in trie_node but it returns a boolean if double(overflow) happend*/
bool ca_bucket_append(children_arr* obj, char* input_word, int goal_index){
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        ca_double(obj); 
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

int hash_function_overflow(hashtable* obj, char*str){
    unsigned long code;
    code=str_to_int(str);
    return code % ((obj->size)*2); //f(k)% size
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
    obj->size= ca_bucket_size;
    obj->p=0;
    obj->primary_size= bucket_size;
    obj->init_size= ca_bucket_size;
}


void hashtable_insert(hashtable* obj, char* word){
    bool overflow;
    overflow=hash_append(obj, word);
    if(overflow==false){ //overflow should happend
        hashtable_overflow(obj);
    }
    update_round(obj);
}

void hashtable_fin(hashtable* obj){
    int i;
    for(i=0; i<obj->size; i++){
        ca_fin(&obj->ca_bucket[i]);
    }
    free(obj->ca_bucket);
}

bool hash_append(hashtable* obj, char* word){
    int key=hash_function(obj, word);
    /*If key is less than p then use hash function overflow(h2)*/
    if(key<obj->p){
        key=hash_function_overflow(obj, word);
    }

    loc_res res = ca_locate_bin(&obj->ca_bucket[key], word);
    bool overflow;
    if(res.found==false){
        overflow=ca_bucket_append(&obj->ca_bucket[key], word, res.index);
    }
    return overflow;
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
    ca_init(&obj->ca_bucket[new], obj->primary_size);
    /*Redistribute trie_nodes between the new bucket and the bucket number p*/
    hash_redistribute(obj);
    
}

/*By saving init_size, I save my program from computing 2^round*/
void update_round(hashtable* obj){
    /*Checks if new_size is divined by init_size pointer p ++*/
    if(obj->p==obj->init_size){
        obj->p=0;
        obj->round++;
        obj->init_size=obj->size;
    }
    else{
        obj->p++;
    }
}

void hash_redistribute(hashtable* obj){
    /*take all trie nodes from children array and check if it can be move to new bucket*/
    int key=obj->p;
    int i=0, end;
    end=obj->ca_bucket[key].First_Available_Slot;

    while(obj->ca_bucket[key].First_Available_Slot < i){
        char* word=  obj->ca_bucket[key].Array[i].Word;
        int key_res=hash_function_overflow(obj, word);
        if(key_res!=obj->p){
            //GEORGE, make the move between obj->ca_bucket[key].Array[i] to obj->ca_bucket[obj->size-1]
            obj->ca_bucket[obj->size-1].Array[i]=obj->ca_bucket[key].Array[i];
            
        }
    }
}

/*return trie_node result
There are 2 options, word used hash_function or word used hash_function_overflow */
loc_res hash_lookup(hashtable* obj, char* word){
    int key=hash_function(obj, word);
    loc_res result=ca_locate_bin(&obj->ca_bucket[key], word);
    
    /*if not found check other hashfunction*/
    if(result.found==false){
        key=hash_function_overflow(obj, word);
        result=ca_locate_bin(&obj->ca_bucket[key], word);
    }

    return result;
}

