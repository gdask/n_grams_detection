#include "trie.h"
#include <stdlib.h>
#include <string.h>

void trie_init(trie* obj,int threads,pthread_t *tids,int init_child_arr_size){
    if(init_child_arr_size < 1){
        fprintf(stderr,"Trie init called with wrong init child arr size\n");
        exit(-1);
    }
    //offset for virtual pointer
    trie_node* pt;
    obj->offset = (size_t)&pt->next -(size_t)pt;
    obj->ca_init_size = init_child_arr_size;
    obj->dynamic = true;
    obj->trie_search = &trie_search_dynamic;
    //obj->version=0;
    hashtable_init(&obj->zero_level, HASH_BUCKETS_INIT, init_child_arr_size);
    filter_manager_init(&obj->fm,threads,tids);
}

void trie_fin(trie* obj){
    hashtable_fin(&obj->zero_level);
    filter_manager_fin(&obj->fm);
}
/*Trie insert updates version info instead of trie_node insert
That could cause false results in case we insert the same ngram twice*/
void trie_insert(trie* obj,line* lm,unsigned int version){
    line_fetch_ngram(lm);
    if(obj->dynamic==false){
        fprintf(stderr,"Insert on static trie is not available");
        return;
    }
    char* current_word = line_fetch_word(lm);
    if(current_word==NULL) return;
    trie_node* current_node = hashtable_insert(&obj->zero_level,current_word);
    /*if(current_node==NULL){
        fprintf(stderr,"NULL CURRENT NODE EXCEPTION\n");
        exit(-1);
    }*/
    current_node->version.added = version;
    current_node->version.deleted = -1;
    current_word = line_fetch_word(lm);
    while(current_word!=NULL){
        current_node = tn_insert(current_node,obj->ca_init_size,current_word);
        current_word = line_fetch_word(lm);
        current_node->version.added = version;
        current_node->version.deleted = -1;
    }
    tn_set_final(current_node);
}

bool trie_mark_deleted(trie* obj,line* l,unsigned int version){
    if(obj->dynamic==false){
        fprintf(stderr,"Delete on static trie is not available");
        return false;
    }
    line_fetch_ngram(l);
    loc_res current_node;
    //current_node.found=false;
    char* current_word = line_fetch_word(l);
    if(current_word==NULL) return false;
    int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
    //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
    current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;
    
    while(current_word!=NULL){
        current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
        if(current_node.node_ptr==NULL) return false;
        current_word = line_fetch_word(l);
    }
    current_node.node_ptr->version.deleted = version;
    return true;
}

bool trie_delete(trie* obj,line* lm){
    if(obj->dynamic==false){
        fprintf(stderr,"Delete on static trie is not available");
        return false;
    }
    line_fetch_ngram(lm);
    char* current_word = line_fetch_word(lm);
    if(current_word==NULL){
        return false;
    }

    int hash_bucket = 0;
    loc_res current;
    children_arr* last_fork;
    int critical_index = -1;
    bool get_critical_index = true;
    bool hash_search = true;
    
    while(current_word!=NULL){
        if(hash_search==true){
            hash_search= false;
            current = hashtable_search(&obj->zero_level,current_word,&hash_bucket);
            last_fork = &obj->zero_level.ca_bucket[hash_bucket];
        }
        else{
            current = ca_locate_bin(&current.node_ptr->next,current_word);
        }
        if(current.node_ptr == NULL){
            //N_gram didnt found,nothing changes in trie
            return false;
        }
        if(get_critical_index==true){
            critical_index = current.index;
            get_critical_index=false;
        }
        if(tn_has_fork(current.node_ptr)==true){
            last_fork = &current.node_ptr->next;
            get_critical_index=true;
        }
        current_word = line_fetch_word(lm);
    }

    if(tn_has_child(current.node_ptr)==true){
        tn_unset_final(current.node_ptr);
    }
    else{
        //deletes entire path from last_fork to leaf
        ca_force_delete(last_fork,critical_index);
    }
    return true;
}

void trie_search_dynamic(trie* obj,line* lm,result *rm,unsigned int version){
    if(line_is_query(lm)!=true){
        fprintf(stderr,"WRONG INPUT\n");
        print_line(lm);
        return;
    }
    //line_parse(lm);
    bool (*ngram_unique)(void* obj,void* input);
    abstract_filter* detected_nodes = get_filter(&obj->fm,(void**)&ngram_unique);
    char* eof = &lm->buffer[lm->line_end];
    char* current_word;

    while((current_word=line_fetch_ngram(lm))!=NULL){
        //char* current_word = line_fetch_word(lm);
        loc_res current_node;
        int words_found=0;
        //Search first word in hash table
        if(current_word==NULL) continue;
        int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
        //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
        current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;

        while(current_word<eof){
            current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
            if(current_node.node_ptr==NULL) break;
            words_found++;
            //NEW CHANGE
            if(current_node.node_ptr->final){
                //Version check
                if(current_node.node_ptr->version.added > version) break;
                if(current_node.node_ptr->version.deleted <= version) break;
                if(ngram_unique(detected_nodes,current_node.node_ptr)==true){
                    result_ngram_detected(rm,lm,words_found);
                }
            }
            //get next word
            current_word += current_node.string_length+1;
            while(*current_word=='\0' && current_word< eof) current_word++;
        }
    }
    result_completed(rm);
    return;
}
#if HYPER_NODE_OPT == 1
void trie_search_static (trie* obj,line* lm,result* rm,unsigned int version){
    if(line_is_query(lm)!=true){
        fprintf(stderr,"FALSE INPUT\n");
        exit(-1);
    }
    //line_parse(lm);
    bool (*ngram_unique)(void* obj,void* input);
    abstract_filter* detected_nodes = get_filter(&obj->fm,(void**)&ngram_unique);
    char* eof = &lm->buffer[lm->line_end];
    char* current_word;

    while((current_word=line_fetch_ngram(lm))!=NULL){
        //char* current_word = line_fetch_word(lm);
        loc_res current_node;
        int words_found=0;

        //Search first word in hash table
        if(current_word==NULL) continue;
        int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
        //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
        current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;

        while(current_word < eof){
            current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
            if(current_node.node_ptr==NULL) break;
            words_found++;
            if(current_node.node_ptr->final==true){
                if(ngram_unique(detected_nodes,current_node.node_ptr)==true){
                    result_ngram_detected(rm, lm, words_found);
                }
            }
            current_word += current_node.string_length+1;
            while(*current_word=='\0' && current_word<eof) current_word++;

            if(current_node.node_ptr->mode=='s'){ //hyper node
                //set buf at the second hyper node word
                char* buf1 = ((hyper_node*)current_node.node_ptr)->Word_Vector +current_node.string_length+2;
                //set input buffer at next word
                char* buf2 = current_word;
                //compare buffers
                while(current_word < eof){
                    //compare char by char
                    while(*buf1 !='\0' && *buf1==*buf2){
                        buf1++;
                        buf2++;
                    }
                    if(*buf1 !=*buf2) break;
                    //Words are the same
                    words_found++;
                    //get final info bit
                    buf1++;
                    if(*buf1==true){
                        if(ngram_unique(detected_nodes,buf1)){
                            result_ngram_detected(rm, lm, words_found);
                        }
                    }
                    //set buffers at next word
                    buf1++;
                    buf2++;
                    while(*buf2=='\0' && buf2 < eof) buf2++;
                    current_word = buf2;
                }
                break;
            }
        }
    }
    result_completed(rm);
    return;
}
#else
void trie_search_static (trie* obj,line* lm,result* rm,unsigned int version){
    if(line_is_query(lm)!=true){
        fprintf(stderr,"FALSE INPUT\n");
        exit(-1);
    }
    //line_parse(lm);
    clock_t start = clock();
    bool (*ngram_unique)(void* obj,void* input);
    abstract_filter* detected_nodes = get_filter(&obj->fm,(void**)&ngram_unique);
    char* eof = &lm->buffer[lm->line_end];
    char* current_word;

    while((current_word=line_fetch_ngram(lm))!=NULL){
        char* current_word = line_fetch_word(lm);
        loc_res current_node;
        int words_found=0;

        //Search first word in hash table
        if(current_word==NULL) continue;
        int target_bucket = hash_get_bucket(&obj->zero_level,current_word);
        //Pointer at 'virtual' trie node that has same children as target_bucket on hashtable.
        current_node.node_ptr = (void*)&obj->zero_level.ca_bucket[target_bucket] - obj->offset;

        while(current_word < eof){
            current_node = ca_locate_bin(&current_node.node_ptr->next,current_word);
            if(current_node.node_ptr==NULL) break;
            words_found++;
            if(current_node.node_ptr->final==true){
                if(ngram_unique(detected_nodes,current_node.node_ptr)==true){
                    result_ngram_detected(rm, lm, words_found);
                }
            }
            current_word += current_node.string_length+1;
            while(*current_word=='\0' && current_word<eof) current_word++;

            if(current_node.node_ptr->mode=='s'){ //hyper node
                //set word info at second word
                short* info = ((hyper_node*)current_node.node_ptr)->Word_Info + 1;
                //set buf at the second hyper node word
                char* buf1 = ((hyper_node*)current_node.node_ptr)->Word_Vector +current_node.string_length;
                //set input buffer at next word
                char* buf2 = current_word;
                //compare buffers
                while(current_word < eof){
                    //get word info
                    int len = *info;
                    bool final = true;
                    if(len==0) break;
                    if(len<0){
                        len *=-1;
                        final=false;
                    }
                    //compare buffers char by char
                    while(len > 0 && *buf1==*buf2 && *buf2!='\0'){
                        buf1++;
                        buf2++;
                        len--;
                    }
                    if(len!=0 || (len==0 && *buf2!='\0'))break;

                    //Words are the same
                    words_found++;
                    if(final==true){
                        if(ngram_unique(detected_nodes,buf1)){
                            result_ngram_detected(rm, lm, words_found);
                        }
                    }
                    //set buffers at next word
                    info++;
                    while(*buf2=='\0' && buf2<eof) buf2++;
                    current_word = buf2;
                }
                break;
            }
        }
    }
    result_completed(rm);
    return;
}
#endif

void trie_compress(trie* obj){
    if(obj->dynamic==false){
        fprintf(stderr,"Trie object is already compressed\n");
        return ;
    }
    if(sizeof(hyper_node) >= sizeof(trie_node)){
        fprintf(stderr,"Size of trie node must be greater or equal to Size of hyper node due to compability issues\n");
        fprintf(stderr,"trie_node: %d bytes, hyper_node: %d bytes\n",(int)sizeof(trie_node),(int)sizeof(hyper_node));
        fprintf(stderr,"Trie cannod be compressed\n");
        return;
    }
    obj->dynamic = false;
    //ca_compress every hashtable bucket
    int i;
    for(i=0;i<obj->zero_level.size;i++){
        ca_compress(&obj->zero_level.ca_bucket[i],0);
    }
    obj->trie_search = &trie_search_static;
}