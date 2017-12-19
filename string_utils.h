#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#define INIT_SIZE_BUF 512
#define NUMBER_OF_LINES 28
#include <stdbool.h>
#include <stdio.h>
//#include "./topk/topk.h"
#include "./topk/topk_hash.h"

typedef struct line{
    char* buffer; 
    int bufsize;
    char line_status; //buffer[0]:A,Q,D,F
    char* word_start; 
    char* n_gram_start;
    int n_gram_position;
    int word_position;
    int line_end; //where \n found
    int k; //for case F_3
}line;

typedef struct line_manager{
    FILE *input;
    char file_status;// static(S) or dynamic(D)
    char lm_status; //lm_status is for init file or query
    int first_available_slot;
    int number_of_lines;
    line* line; //lines of batch
}line_manager;

typedef struct result{
    FILE *output;
    char* output_buffer;
    int output_bufsize;
    int first_available_slot; //first available slot to put words of word_buffer
    int buffer_start; //output buffer of print result of queries
}result;

typedef struct result_manager{
    result* result;
    int size;
    int first_available_slot;
    TopK *topk;
    int k; //if k!=0, keeps how many ngrams printed from topk, if k==-1, no need for create topk
}result_manager;

/*Line Functions*/
void line_init(line* obj);
void line_fin(line* obj);
//prepare line for other functions
void line_parse(line* obj);
//check ig line fetch ngram can return char* at first word of ngram
char* line_fetch_ngram(line* obj);
char* line_fetch_word(line* obj);

bool line_is_query(line* obj);
bool line_is_insert(line* obj);
bool line_is_delete(line* obj);
bool line_is_F(line* obj);

//Input manager functions
void line_manager_init(line_manager* obj,FILE *fp, char file_status); //Initilize struct
void line_manager_fin(line_manager* obj); //Deallocates any malloced memory
/*return file_status*/
char lm_get_file_status(line_manager *obj);
//Fetch line gets the next line from file,returns FALSE if there is no next line
//bool lm_fetch_line(line_manager* obj, ngram_array *na);
bool lm_fetch_line(line_manager* obj);

//stores line at previous line buffer(first_available_slot-1)
line* lm_fetch_sequence_line(line_manager* obj);
//stores line at next line buffer(first_available_slot)
line* lm_fetch_independent_line(line_manager* obj);

///////////////////////////////////////////////////////////////////////////

//Result of each query functions
void result_init(result* obj,FILE *fp); //Initilize struct
void result_fin(result* obj); //Deallocates any malloced memory
// "|hello world [new words]" is appended at the result
void result_ngram_detected(result* obj, line_manager *lm, int word_count);
//writes the result, handles no result
void result_completed(result* obj);

//Result Manager 
void rm_init(result_manager* obj);
void rm_fin(result_manager* obj);
//Topk structure should be used
void rm_use_topk(result_manager* obj, int k);
//display result of each query
void rm_display_result(result* obj);
//return space where result of each query can be written
result* get_result(result_manager* obj);

#endif