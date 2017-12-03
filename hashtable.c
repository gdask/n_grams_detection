#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "./filters/murmur3.h"
#include <stdint.h>

/*Same as ca_force append in trie_node but it returns a boolean if double(overflow) happend*/
bool ca_bucket_append(children_arr* obj, char* input_word, int goal_index){
    bool doubled=false;
    if(goal_index == obj->Size || obj->First_Available_Slot == obj->Size){
        ca_double(obj);
        doubled= true; 
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
    tn_leaf(&obj->Array[goal_index], input_word);
    return doubled;
}

/*djb2, to turn a string to int*/
unsigned long str_to_int(char *str){
    unsigned long hash = 5381;
    //int c;
    char* input=str;

    //while (c = *str++){
    //    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    //}
    while(*input){
        hash = ((hash << 5) + hash) + *input;
        input++;
    }
    return hash;
}

int hash_function(hashtable* obj, char* str){
    //uint32_t code;
    //MurmurHash3_x86_32(str,strlen(str),0,&code);
    unsigned long code;
    code=str_to_int(str);
    //return code % (obj->init_size); //f(k)% size
    return code & obj->mask;
}

int hash_function_overflow(hashtable* obj, char*str){
    //uint32_t code;
    //MurmurHash3_x86_32(str,strlen(str),0,&code);
    unsigned long code;
    code=str_to_int(str);
    //return code % (2*obj->init_size); //f(k)% 2*size
    //return code % (obj->init_size << 1); //f(k)% 2*size
    return code & obj->ov_mask;
}

int hash_get_bucket(hashtable* obj, char* word){
    int ov_key = hash_function_overflow(obj,word);
    //int und_key = ov_key %obj->init_size;
    int und_key = ov_key - obj->init_size;
    if(und_key >= obj->p) return und_key;
    return ov_key;

    /*
    if(ov_key < obj->p){
        return ov_key;
    }
    else{
        int und_key = oov_key - obj->init_size;
        if(und_key >= obj->p) return und_key;
        return ov_key;
    }*/

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
        //fprintf(stderr, "Initialized bucket %d\n", i);
        ca_init(&obj->ca_bucket[i], bucket_size);
    }
    obj->round=0;
    obj->size= ca_bucket_size;
    obj->p=0;
    obj->primary_size= bucket_size;
    obj->init_size= ca_bucket_size;
    //works only for power of 2 init ca_bucket_size
    obj->mask = HASH_MASK;
    obj->ov_mask = OV_HASH_MASK;
}

void hashtable_fin(hashtable* obj){
    int i;
    for(i=0; i<obj->size; i++){
        ca_fin(&obj->ca_bucket[i]);
    }
    free(obj->ca_bucket);
}

trie_node* hashtable_insert(hashtable* obj, char* word){
    /*find if word is in or not, return also right position*/
    int pos=0;
    loc_res result = hashtable_search(obj, word, &pos); 
    bool overflow=false;
    //bool changed=false;
    if(result.node_ptr==NULL){
        overflow = ca_bucket_append(&obj->ca_bucket[pos], word, result.index);
    }
    if(overflow==true){ //overflow occures
        /*Expand table for one bucket*/
        children_arr* temp = (children_arr*) realloc(obj->ca_bucket, (obj->size+1)*sizeof(children_arr));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed\n");
            exit(-1);
        }
        obj->ca_bucket= temp;
        update_round(obj);
        split(obj, word);
        obj->size++;
        if(obj->p==0){
            obj->init_size=obj->size;
            obj->mask = (obj->mask << 1) + 0x01;
            obj->ov_mask = (obj->mask << 1) + 0x01;
        }
        result = hashtable_search(obj, word, &pos);
    }
    else{
        result = hashtable_search(obj, word, &pos);
    }
    return result.node_ptr;
    //in case key==p maybe it changed pos

}

void ca_bucket_delete(children_arr* obj, int goal_index){
    if(goal_index == obj->First_Available_Slot-1){
        //tn_fin(&obj->Array[goal_index]);
        obj->First_Available_Slot--;
        return;
    }

    size_t movable =(obj->First_Available_Slot-goal_index-1)*sizeof(trie_node);
    memmove(&obj->Array[goal_index],&obj->Array[goal_index+1],movable);
    obj->First_Available_Slot--;
}

/*By saving init_size, I save my program from computing 2^round*/
void update_round(hashtable* obj){
    /*Checks if new_size is divined by init_size pointer p ++*/
    if(obj->p==obj->init_size-1){
        //fprintf(stderr, "Round change \n");
        obj->old_p=obj->p;
        obj->p=0;
        obj->round++;
        //obj->init_size=obj->size;
    }
    else{
        obj->p++;
    }
}

//Spit happend between bucket obj->p and new bucket(size)
bool split(hashtable* obj, char *input){
    //empty page is added to the overflow
    ca_init(&obj->ca_bucket[obj->size], obj->primary_size);
    //the search values mapped into obj->p (using hash_function) now distributed between bucket obj->p 
    //and size using hash_function overflow
    int p=obj->p;
    int i=0, movable=0;
    bool changed=false;
    if(p==0){
        //round is updated, so you need old p
        p=obj->init_size-1;
    }
    else{
        p--;
    }
    while(i < obj->ca_bucket[p].First_Available_Slot){
        char* word = obj->ca_bucket[p].Array[i].Word;
        int key_res= hash_function_overflow(obj, word);        
        if(key_res != p){
            if(strcmp(word, input)==0){
                changed= true; 
            }
            ca_bucket_append(&obj->ca_bucket[obj->size],"tmp",movable);
            free(obj->ca_bucket[obj->size].Array[movable].Word);
            obj->ca_bucket[obj->size].Array[movable]=obj->ca_bucket[p].Array[i];
            movable++;
            ca_bucket_delete(&obj->ca_bucket[p],i);
            continue;
        }
        i++;
    }
    obj->ca_bucket[obj->size].First_Available_Slot=movable;
    return changed;
}

/*return trie_node result
There are 2 options, word used hash_function or word used hash_function_overflow */
loc_res hash_lookup(hashtable* obj, char* word){
    int pos=0;
    loc_res result=hashtable_search(obj, word, &pos);
    return result;
}

loc_res hashtable_search(hashtable* obj, char* word, int* bucket){
    int key=hash_function(obj, word);
    loc_res result;
    int p=obj->p;

    if(key >= p){
        //choose bucket h(word) since bucket has not been split yet in current round 
        result=ca_locate_bin(&obj->ca_bucket[key], word);
    }
    else if(key<p){
        //choose bucket from h_overflow(word)
        key=hash_function_overflow(obj, word);
        result=ca_locate_bin(&obj->ca_bucket[key], word);
    }
    *bucket=key;
    return result;
}


void hash_print(hashtable* obj){
    fprintf(stderr,"Hashtable on %p has %d buckets:\n",obj,obj->size);
    int i;
    for(i=0;i<obj->size;i++){
        fprintf(stderr,"Bucket on %d: has %d entries\n",i,obj->ca_bucket[i].First_Available_Slot);
        //int j;
        //for(j=0;j<obj->ca_bucket[i].First_Available_Slot;j++){
            //fprintf(stderr, "%s|", obj->ca_bucket[i].Array[j].Word);
            //tn_print_subtree(&obj->ca_bucket[i].Array[j]);
        //}
        //fprintf(stderr, "\n");
    }
}