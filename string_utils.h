#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#include <stdbool.h>

//struct query_manager;
//typedef struct query_manager query_manager;

struct query_manager{
char* buffer;git 
FILE *input;
} query_manager;
//Input manager functions
void query_manager_init(query_manager* obj,FILE *fp); //Initilize struct
void query_manager_fin(query_manager* obj); //Deallocates any malloced memory

//Fetch query gets the next line from file,returns FALSE if there is no next line
bool qm_fetch_line(query_manager* obj);
//ex: "hello world re" >> "world re" >> "re" >> RETURN FALSE
bool qm_fetch_ngram(query_manager* obj);
//Returns the next word from ngram, ex: "hello" >> "world" >> "re" >> RETURNS FALSE
char* qm_fetch_word(query_manager* obj);

bool qm_is_query(query_manager* obj);
bool qm_is_insert(query_manager* obj);
bool qm_is_delete(query_manager* obj);


//struct result_manager;
//typedef struct result_manager;

struct result_manager{
FILE *output;
} result_manager;
//Input manager functions
void result_manager_init(result_manager* obj,FILE *fp); //Initilize struct
void result_manager_fin(result_manager* obj); //Deallocates any malloced memory

//Output functions
void rm_start(result_manager *obj,int max_words); not sure if neccessary
// ex: previous detected ngram "hello world" next ngram "|hello world [new words]"
void rm_new_ngram(result_managet *obj);
// appended at [new words]
void rm_append_word(result_manager* obj,char* word);
// "|hello world [new words]" is appended at the result
void rm_ngram_detected(result_manager* obj);
// "|hello world [new words]" will not be a part of the result
void rm_ngram_undetected(result_manager* obj);
// writes the result 
void rm_completed(result_manager* obj);

#endif