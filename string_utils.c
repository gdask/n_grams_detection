#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*check if string has a newline char*/
bool complete_phrase(char* line){
    int i=0;
    while(line[i]!='\0'){
        if(line[i]=='\n')
            return true;
        i++;
    }
    return false;
}

char lm_get_file_status(line_manager *obj){
    return obj->file_status;
}

/*Initialise structure query manager
Buffer starts with length INIT_SIZE_BUF, if a line cannot fit to a buffer with this size, i will handle it propely in qm_fetch_line*/
void line_manager_init(line_manager* obj,FILE *fp, char lm_status){
    obj->input=fp;
    obj->lm_status=lm_status;
    obj->buffer= (char*) malloc(INIT_SIZE_BUF*sizeof(char)); 
    if(obj->buffer==NULL){
        fprintf(stderr,"Malloc failed :: line_manager_init\n");
        exit(-1);
    }
    obj->bufsize = INIT_SIZE_BUF;
    return;
}

void line_manager_fin(line_manager* obj){
    //Frees the dynamicly allocated memory
    if(obj->buffer==NULL) return;
    free(obj->buffer);
}

/*boolean functions that concern line_status*/
bool lm_is_query(line_manager* obj){
    if(obj->line_status=='Q')
        return true;
    return false;
}

bool lm_is_insert(line_manager* obj){
    if(obj->line_status=='A')
        return true;
    return false;
}

bool lm_is_delete(line_manager* obj){
    if(obj->line_status=='D')
        return true;
    return false;
}

bool lm_is_F(line_manager* obj){
    if(obj->line_status=='F'){
        return true;
    }
    return false;
}

/*  
    lm_fetch_line need to know the status of file(init file='I', query file= 'Q')
    -Reads a line from file, handles bigger lines than expected
    -Keep key of action (A,Q,D)
    -Replace \n and _ with Null
    -Init n_gram
*/
bool lm_fetch_line(line_manager* obj){
    if(obj->buffer==NULL){
        fprintf(stderr,"Object is not initisialised:: lm_fetch_line\n");
        exit(-1);
    }
    if(obj->lm_status=='Q'){
        obj->file_status='\0';
        if(fgets(obj->buffer, obj->bufsize, obj->input)){
            /*if the space was not enough, i should allocate more memory(double my size) in order to fit eventually all the line*/
            while(complete_phrase(obj->buffer)==false){
                int oldsize;
                char* temp = (char*)realloc(obj->buffer, 2*obj->bufsize*sizeof(char));
                if(temp==NULL){
                    fprintf(stderr,"Realloc Failed :: lm_fetch_line\n");
                    exit(-1);
                }
                obj->buffer=temp;
                oldsize=obj->bufsize;
                obj->bufsize=2*obj->bufsize;
                char* retbuf;
                retbuf=fgets(&obj->buffer[oldsize-1], oldsize+1, obj->input); //fgets writes after oldsize-1 because before this position i have the previous line
                if(retbuf==NULL){ //something went wrong(EOF)
                    return false;
                }
            }
            //printf("Line read:%s", obj->buffer);
        }
        else{ // EOF is found, my work here is done
            //printf("EOF\n");
            return false;
        }  
    
        /*Valid Task Recognition*/
        if((obj->buffer[0]=='F')||((obj->buffer[0]=='A' || obj->buffer[0]=='D' ||obj->buffer[0]=='Q' ) && obj->buffer[1]==' ')){
            /*keep line status, check if ID is valid*/
            obj->line_status=obj->buffer[0];

            //In case of F you just ignore this line.
            if(obj->buffer[0]=='F'){
                return true;
            }
    
            /*Replace \n and _ with NULL*/
            int i=0;
            int words=0;
            while(obj->buffer[i]!='\n'){
                if(obj->buffer[i]==' '){
                    words++;
                    obj->buffer[i]='\0';
                }
                i++;
            }
            words++; //dont forget the last word is followed by \n not space
            obj->n_gram_counter=(2*words)-1;
            obj->buffer[i]='\0'; // new line -> NULL
            obj->line_end=i;
            obj->buffer[obj->bufsize-1]='\0';
    
            //Initialise n_gram position
            obj->n_gram_position= 0;
            return true;
        }
        else{
            fprintf(stderr,"Invalid task :: qm_fetch_line\n");
            return false;
        }
    }
    else if(obj->lm_status=='I'){
        //every line is a ngram
        obj->buffer[0]='A';
        obj->buffer[1]=' ';
        if(fgets(&obj->buffer[2],  obj->bufsize-2, obj->input)){
            /*if the space was not enough, i should allocate more memory(double my size) in order to fit eventually all the line*/
            while(complete_phrase(obj->buffer)==false){
                //printf("Ready to make a realloc\n");
                int oldsize;
                char* temp = (char*)realloc(obj->buffer, 2*obj->bufsize*sizeof(char));
                if(temp==NULL){
                    fprintf(stderr,"Realloc Failed :: qm_fetch_line\n");
                    exit(-1);
                }
                obj->buffer=temp;
                oldsize=obj->bufsize;
                obj->bufsize=2*obj->bufsize;
                char* retbuf;
                retbuf=fgets(&obj->buffer[oldsize-1], oldsize+1, obj->input); //fgets writes after oldsize-1 because before this position i have the previous line
                if(retbuf==NULL){ //something went wrong(EOF)
                    return false;
                }
            }
            if(strcmp(&obj->buffer[2], "DYNAMIC\n")==0){
                obj->file_status='D';
                return lm_fetch_line(obj); 
            }
            else if(strcmp(&obj->buffer[2], "STATIC\n")==0){
                obj->file_status='S';
                return lm_fetch_line(obj);
            }
            //printf("line %s", obj->buffer);
        }
        else{ // EOF is found, my work here is done
            return false;
        }  

        /*keep line status, for this case is only an addition*/
        obj->line_status='A';
    
        /*Replace \n and _ with NULL*/
        int i=0;
        int words=0;
        while(obj->buffer[i]!='\n'){
            if(obj->buffer[i]==' '){
                words++;
                obj->buffer[i]='\0';
            }
            i++;
        }
        words++;
        obj->n_gram_counter=(2*words)-1;
        obj->buffer[i]='\0'; // new line -> NULL
        obj->line_end=i;
        obj->buffer[obj->bufsize-1]='\0';
    
        //Initialise n_gram position  
        obj->n_gram_position= 0;
        return true;
    }
    return false;   
}

/*each time that fetch_ngram is called, i modify my obj->Buffer with one word less.
Initialise word_position, word_start*/
bool lm_fetch_ngram(line_manager* obj){
    int i;
    i=obj->n_gram_position;
    //printf("i=%d end=%d", i, obj->line_end);
    /*looks after id code*/
    while(obj->buffer[i]!='\0'){
        i++;
    }
    if(i>=obj->line_end){
        obj->word_start=NULL; //no ngrams no words
        return false;
    }
    obj->n_gram_position=i+1; //position of n_gram
    if(obj->buffer[i+1]=='\0'){
        return lm_fetch_ngram(obj);
    }
    obj->word_start=NULL;
    obj->word_position=obj->n_gram_position;
    //printf("Ngram first word is%s\n", &obj->buffer[obj->n_gram_position]);
    return true; //no more n grams
}

/*Return pointer a n gram word, each time i call lm_fetch_word, next word should be returned
word_position=-1 in first call of fetch_word*/
char* lm_fetch_word(line_manager* obj){
    int i;
    i=obj->word_position; // i care only for words of current n_gram
    
    while(obj->buffer[i]!='\0'){
        if(obj->word_start == NULL){ //first word
            obj->word_start= &obj->buffer[obj->word_position];
            return obj->word_start;
        }
        i++;
    }

    while(obj->buffer[i]=='\0' && i<obj->line_end){
        i++;
    }

    if(i>=obj->line_end){
        obj->word_start=NULL;
        obj->word_position=0;
    }
    else{
        obj->word_start= &obj-> buffer[i]; 
        obj->word_position=i;
    }
    return obj->word_start;
}

/*Result Manager*/

/*Initialise and deallocate Memory*/

void result_init(result* obj,FILE *fp){
    obj->output=fp;
    obj->output_buffer=(char*)malloc(sizeof(char)*INIT_SIZE_BUF);
    if(obj->output_buffer==NULL){
        fprintf(stderr,"Malloc failed :: result_init\n");
        exit(-1);
    }
    obj->output_bufsize=INIT_SIZE_BUF;
    obj->first_available_slot=0;
    obj->buffer_start=0;
}

void result_fin(result* obj){
    if(obj->output_buffer!=NULL){
        free(obj->output_buffer);
    }
    return;
}

/*Αdd word_count of the words of line manager(current_ngram) to output buffer plus one |*/
//Use of hash and array
void rm_ngram_detected(result* obj, line *l, int word_count){
    int current_index=0;
    char* p_ngram;
    p_ngram=&l->buffer[l->n_gram_position];
    current_index=obj->first_available_slot;
    while(word_count>0){
        if(obj->first_available_slot==obj->output_bufsize){
            char* temp =(char*) realloc(obj->output_buffer, 2*obj->output_bufsize*sizeof(char));
            if(temp==NULL){
                fprintf(stderr,"Realloc Failed :: rm_ngram_detected\n");
                exit(-1);
            }
            obj->output_buffer=temp;
            obj->output_bufsize= 2*obj->output_bufsize;
        }
        if(*p_ngram=='\0'){
            while(*p_ngram=='\0'){
                p_ngram++;
            }
            word_count--;
            obj->output_buffer[obj->first_available_slot]=' ';
        }
        else{
            obj->output_buffer[obj->first_available_slot]=*p_ngram;
            p_ngram++;
        }
        obj->first_available_slot++;
    }
    obj->output_buffer[obj->first_available_slot-1]='|'; //last thing shouldn't be space but |
}

/*Keep the result of Q in output buffer, buffer_start is keep in order to know where to start keeping new result*/
void rm_completed(result* obj){
    if(obj->first_available_slot==0){ //no ngram detected
        strcpy(&obj->output_buffer[obj->first_available_slot],"-1\n");
        obj->first_available_slot=obj->first_available_slot+3;
    }    
    else{
        obj->output_buffer[obj->first_available_slot-1]='\n';
    }
    obj->first_available_slot=0;
}

/*Print result of gust*/
void rm_display_result(result* obj){
    obj->output_buffer[obj->first_available_slot]='\0';
    fprintf(obj->output,"%s",obj->output_buffer);
    obj->first_available_slot=0;
}