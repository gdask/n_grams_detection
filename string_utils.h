#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#define INIT_SIZE_BUF 512
#include <stdbool.h>
#include <stdio.h>
//#include "topk.h"
#include "./topk_hash/topk_hash.h"

struct line_manager{
    FILE *input;
    char file_status; // static(S) or dynamic(D)
    char lm_status; //lm_status is for init file or query
    char* buffer; //each line from file
    int bufsize;
    char line_status; //buffer[0]:A,Q,D
    char* word_start; 
    char* n_gram_start;
    int word_position;
    int n_gram_position;
    int n_gram_counter;
    int line_end; //where \n found
};

typedef struct result_manager{
    FILE *output;
    char** word_buffer;
    char* output_buffer;
    int bufsize;
    int output_bufsize;
    int first_available_slot; //first available slot to put words of word_buffer
    char* current_ngram; //last ngram of output_buffer
    int current_ngram_index; //keep where current ngram is starting
    int current_word_index; //keep where current word is starting
    int buffer_start;
}result_manager;

typedef struct line_manager line_manager;
//Input manager functions
void line_manager_init(line_manager* obj,FILE *fp, char file_status); //Initilize struct
void line_manager_fin(line_manager* obj); //Deallocates any malloced memory

//Fetch line gets the next line from file,returns FALSE if there is no next line
//bool lm_fetch_line(line_manager* obj, ngram_array *na);
bool lm_fetch_line(line_manager* obj, TopK* top, result_manager* rm);
//bool lm_fetch_line(line_manager* obj, TopK* top);
//ex: "hello world re" >> "world re" >> "re" >> RETURN FALSE
bool lm_fetch_ngram(line_manager* obj);
//Returns the next word from ngram, ex: "hello" >> "world" >> "re" >> RETURNS NUll
char* lm_fetch_word(line_manager* obj);

bool lm_is_query(line_manager* obj);
bool lm_is_insert(line_manager* obj);
bool lm_is_delete(line_manager* obj);
int lm_n_gram_counter(line_manager* obj);

/*return file_status*/
char lm_get_file_status(line_manager *obj);

///////////////////////////////////////////////////////////////////////////

//Output manager functions
void result_manager_init(result_manager* obj,FILE *fp); //Initilize struct
void result_manager_fin(result_manager* obj); //Deallocates any malloced memory

//Output functions
void rm_start(result_manager *obj,int max_words);
// ex: previous detected ngram "hello world" next ngram "|hello world [new words]"
void rm_new_ngram(result_manager *obj);
// appended at [new words]
void rm_append_word(result_manager* obj,char* word);
// "|hello world [new words]" is appended at the result
void rm_ngram_detected(result_manager* obj, TopK* top);
//void rm_ngram_detected(result_manager* obj, ngram_array* na);
// "|hello world [new words]" will not be a part of the result
void rm_ngram_undetected(result_manager* obj);
// writes the result 
void rm_completed(result_manager* obj);
//display result
void rm_display_result(result_manager* obj);

#endif