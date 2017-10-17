#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#define INIT_SIZE_BUF 512
#include <stdbool.h>

struct line_manager;
typedef struct line_manager;

struct line_manager{
    FILE *input;
    char* buffer; //each line from file
    int bufsize;
    char line_status; //buffer[0]:A,Q,D
    char* word_start; 
    char* n_gram_start;
    int word_position;
    int n_gram_position;
    int line_end; //where \n found
};

//Input manager functions
void line_manager_init(line_manager* obj,FILE *fp); //Initilize struct
void line_manager_fin(line_manager* obj); //Deallocates any malloced memory

//Fetch line gets the next line from file,returns FALSE if there is no next line
bool lm_fetch_line(line_manager* obj);
//ex: "hello world re" >> "world re" >> "re" >> RETURN FALSE
bool lm_fetch_ngram(line_manager* obj);
//Returns the next word from ngram, ex: "hello" >> "world" >> "re" >> RETURNS NUll
char* lm_fetch_word(line_manager* obj);

bool lm_is_query(line_manager* obj);
bool lm_is_insert(line_manager* obj);
bool lm_is_delete(line_manager* obj);


//struct result_manager;
//typedef struct result_manager;

struct result_manager{
    FILE *output;
};
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