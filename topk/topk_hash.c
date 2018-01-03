#include "topk_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUCKET_SIZE 9
#define HASH_SIZE 99
#define ARRAY_NODE_SIZE 5
#include "../filters/murmur3.h"

/*djb2, to turn a string to int*/
unsigned long transposeSTR(char* str){
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

int hashfuction(char* input){
    unsigned long code;
    //djb2
    code=transposeSTR(input);
    //int len=strlen(input);
    //MurmurHash3_x86_32(input, len, 0, &code);
    return code % HASH_SIZE; //f(k)% size
}

void Hash_init(Hash* obj){
    obj->Bucket=malloc(sizeof(bucket)*HASH_SIZE);
    if(obj->Bucket==NULL){
        fprintf(stderr, "Error in malloc::Hash_init");
        exit(-1);
    }
    int i;
    for(i=0; i<HASH_SIZE; i++){
        obj->Bucket[i].Node=malloc(sizeof(node)*BUCKET_SIZE);
        if(obj->Bucket[i].Node==NULL){
            fprintf(stderr, "Error in malloc::Hash_init");
            exit(-1);
        }
        obj->Bucket[i].max_freq=0;
        obj->Bucket[i].size=BUCKET_SIZE;
        obj->Bucket[i].first_available_slot=0;
    }
    obj->max_freq=0;
}

void Hash_fin(Hash* obj){
    int i;
    if(obj->Bucket!=NULL){
        for(i=0;i<HASH_SIZE; i++){
            if(obj->Bucket[i].Node!=NULL){
                int j;
                for(j=0; j<obj->Bucket[i].first_available_slot; j++){
                    if(obj->Bucket[i].Node[j].word!=NULL){
                        free(obj->Bucket[i].Node[j].word);
                    }
                }
                free(obj->Bucket[i].Node);
            }
        }
        free(obj->Bucket);
    }
    //printf("Freed words:%d nodes: %d buckets%d\n", w,n,b);
}

//reuse structure
void Hash_reuse(Hash* obj){
    int i;
    if(obj->Bucket!=NULL){
        for(i=0;i<HASH_SIZE; i++){
            if(obj->Bucket[i].Node!=NULL){
                int j;
                for(j=0; j<obj->Bucket[i].first_available_slot;j++){
                    if(obj->Bucket[i].Node[j].word!=NULL){
                        //fprintf(stderr, "%s\n",obj->Bucket[i].Node[j].word);
                        free(obj->Bucket[i].Node[j].word);
                        obj->Bucket[i].Node[j].freq=0;
                    }

                }
                obj->Bucket[i].first_available_slot=0;
                obj->Bucket[i].max_freq=0;
            }
        }
    }
    obj->max_freq=0;
} 

void bucket_insert(bucket* obj, char* word, int goal_index, int word_len){
    //check if should realloc
    if(obj->size==goal_index ||obj->first_available_slot==obj->size){
        node* temp= realloc(obj->Node, 2*obj->size*sizeof(node));
        if(temp==NULL){
            fprintf(stderr, "Error in realloc::bucket_insert\n");
            exit(-1);
        }
        obj->Node=temp;
        obj->size=2*obj->size;
    }
    else if(goal_index > obj->size-1 || goal_index > obj->first_available_slot){
        fprintf(stderr,"na_append :: out of bounds goal_index\n");
        exit(-1);
    }

    //insertion is done by alphabetical order
     if(goal_index != obj->first_available_slot){
        size_t movable= (obj->first_available_slot - goal_index)* sizeof(node);
        memmove(&obj->Node[goal_index+1], &obj->Node[goal_index], movable);
    }
    //create node
    obj->Node[goal_index].freq=1;
    obj->Node[goal_index].word=malloc(word_len);
    if(obj->Node[goal_index].word==NULL){
        fprintf(stderr, "Error in malloc::bucket_insert\n");
         exit(-1);
    }
    strncpy(obj->Node[goal_index].word, word, word_len);
    obj->first_available_slot++;
}

void Hash_insert(Hash* obj, char* word, int word_len){
    int key=hashfuction(word);
    //search in bucket[key] if word already exists 
    //if exists add +1 to freq of this node and return
    //else insert word
    int lower_bound = 0;
    int upper_bound = obj->Bucket[key].first_available_slot-1;
    int middle = (lower_bound+upper_bound)/2;

    while(lower_bound <= upper_bound){
            
            int cmp_res = 0;
           /*char* com1 = obj->Bucket[key].Node[middle].word;
            char* com2 = word;
            while(*com1!='\0' && *com1==*com2){
                com1++;
                com2++;
            }
            cmp_res = *com1 - *com2;*/
            cmp_res=strcmp(obj->Bucket[key].Node[middle].word, word);
            if(cmp_res < 0){
                lower_bound = middle + 1;
            }
            else if(cmp_res > 0){
                upper_bound = middle - 1;
            }
            else{
                //Array[middle] == input_ngram
                //immidiatilly update rank and keep the largest freq of node array
                obj->Bucket[key].Node[middle].freq ++;
                if(obj->max_freq<obj->Bucket[key].Node[middle].freq){
                    obj->max_freq=obj->Bucket[key].Node[middle].freq;
                }
                return; 
            }
            middle = (lower_bound+upper_bound)/2;
        
    }
    //this is the right position for insert the new ngram
    bucket_insert(&obj->Bucket[key], word, lower_bound, word_len);

}

//return maxfreq, in order to create an array size maxfreq+1
int Hash_maxfreq(Hash *obj){
    int i;
    int max=0;
    for(i=0; i<HASH_SIZE; i++){
        if(max<obj->Bucket[i].max_freq){
            max=obj->Bucket[i].max_freq;
        }
    }
    return obj->max_freq;
}

void display_Hash(Hash* obj){
    int i;
    for(i=0; i<HASH_SIZE; i++){
        fprintf(stderr, "Bucket:%d\t", i);
        fprintf(stderr, "Max_freq:%d\n", obj->Bucket[i].max_freq);
        int j;
        for(j=0; j<obj->Bucket[i].first_available_slot; j++){
            fprintf(stderr, "Word:%s\t", obj->Bucket[i].Node[j].word);
            fprintf(stderr, "Freq:%d\n", obj->Bucket[i].Node[j].freq);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Array_init(top_array* obj, int size){
    obj->nodes=malloc(sizeof(arr_node)*(size));
    if(obj->nodes==NULL){
        fprintf(stderr, "Error in malloc::Array_init\n");
        exit(-1);
    }
    obj->size=size;
    int i;
    for(i=0; i<obj->size; i++){
        obj->nodes[i].arr_node=malloc(sizeof(char*)*ARRAY_NODE_SIZE);
        if(obj->nodes[i].arr_node==NULL){
            fprintf(stderr, "Error in malloc::Array_init\n");
            exit(-1);
        }
        obj->nodes[i].size=ARRAY_NODE_SIZE;
        obj->nodes[i].first_available_slot=0;
    }
}

void Array_fin(top_array* obj){
    int i;
    if(obj->nodes==NULL)
        return;
    for(i=0; i<obj->size; i++){
        if(obj->nodes[i].arr_node!=NULL){
            int j=0;
            for(j=0; j<obj->nodes[i].first_available_slot; j++){
                if(obj->nodes[i].arr_node[j]!=NULL){
                    free(obj->nodes[i].arr_node[j]);
                }
            }
            free(obj->nodes[i].arr_node);
        }
    }
    free(obj->nodes);
}

void Array_insert(top_array* obj, char* word, int freq){
    //fprintf(stderr,"Freq %d\n", freq-1);
    //find the right position to enter new element. Binary search is used
    int pos = arr_node_lookup_bin(&obj->nodes[freq-1], word);
    if(pos==-1){
        return; //word already in structure
    }
    //insert element to right pos
    arr_node_insert(&obj->nodes[freq-1], word, pos);

}

int arr_node_lookup_bin(arr_node* obj, char* word){
    int lower_bound = 0;
    int upper_bound = obj->first_available_slot-1;
    int middle = (lower_bound+upper_bound)/2;

    while(lower_bound <= upper_bound){
            //fprintf(stderr, "FAS: %d, Middle:%d\n", upper_bound, middle);
            int cmp_res = 0;
            if(obj->arr_node[middle]==NULL){
                fprintf(stderr, "Null in arr_node_lookup_bin\n");
                exit(-1);
            }
            cmp_res=strcmp(obj->arr_node[middle], word);
            if(cmp_res < 0){
                lower_bound = middle + 1;
            }
            else if(cmp_res > 0){
                upper_bound = middle - 1;
            }
            else{
                //Array[middle] == input_ngram
                //immidiatilly update rank and keep the largest freq of node array
                return -1; 
            }
            middle = (lower_bound+upper_bound)/2;
        
    }
    //this is the right position for insert the new ngram
    return lower_bound;
}

void arr_node_insert(arr_node* obj, char*word, int goal_index){
    //check if should realloc
    if(obj->size==goal_index ||obj->first_available_slot==obj->size){
        //fprintf(stderr, "Realloc\n");
        char** temp= realloc(obj->arr_node, 2*obj->size*sizeof(char*));
        if(temp==NULL){
            fprintf(stderr, "Error in realloc::bucket_insert\n");
            exit(-1);
        }
        obj->arr_node=temp; 
        obj->size=2*obj->size;
    }
    else if(goal_index > obj->size-1 || goal_index > obj->first_available_slot){
        fprintf(stderr,"na_append :: out of bounds goal_index\n");
        exit(-1);
    }

    //insertion is done by alphabetical order
     if(goal_index != obj->first_available_slot){
        size_t movable= (obj->first_available_slot - goal_index)* sizeof(char*);
        memmove(&obj->arr_node[goal_index+1], &obj->arr_node[goal_index], movable);
    }

    //create node
    //fprintf(stderr,"Initialized %d\n", goal_index);
    obj->arr_node[goal_index]=malloc(sizeof(char)*(strlen(word)+1));
    if(obj->arr_node[goal_index]==NULL){
        fprintf(stderr, "Error in malloc::arr_node_insert\n");
        exit(-1);
    }
    strcpy(obj->arr_node[goal_index], word);
    obj->first_available_slot++;   
}

void display_topk(top_array* obj, int k){
    int start=obj->size-1; //start from end
    int i=start;
    int flag=0; //no result
    while(k!=0 && i>=0){
        int j;
        if(obj->nodes!=NULL){
            for(j=0; j<obj->nodes[i].first_available_slot; j++){
                if(flag==0){
                    printf("Top: ");
                    flag=1;
                }
                if(k==1){
                    printf("%s\n", obj->nodes[i].arr_node[j]);
                    
                }
                else{
                    printf("%s|", obj->nodes[i].arr_node[j]);
                }
                k--;
                if(k==0){
                    break;
                }
            }
        }
        i--;
    }
}

void topk(Hash* obj, int k){
    top_array array;
    int max_freq= Hash_maxfreq(obj);
    Array_init(&array, max_freq);
    //insert to top k array
    int i;
    for(i=0; i<HASH_SIZE; i++){
        if(obj->Bucket!=NULL){
            int j;
            for(j=0; j<obj->Bucket[i].first_available_slot;j++){
                Array_insert(&array, obj->Bucket[i].Node[j].word, obj->Bucket[i].Node[j].freq);
                //printf("insert done\n");
            }
        }
    }
    //display topK
    display_topk(&array, k);
                   
    Array_fin(&array);
}