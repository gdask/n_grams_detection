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

int lm_n_gram_counter(line_manager* obj){
    return obj->n_gram_counter;
}

/*Initialise structure query manager
Buffer starts with length INIT_SIZE_BUF, if a line cannot fit to a buffer with this size, i will handle it propely in qm_fetch_line*/
void line_manager_init(line_manager* obj,FILE *fp, char file_status){
    obj->input=fp;
    obj->file_status=file_status;
    obj->buffer= malloc(INIT_SIZE_BUF*sizeof(char)); 
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

/*  
    lm_fetch_line need to know the status of file(init file='I', query file= 'Q')
    -Gets a line from file, handles bigger lines than expected
    -Keep key of action (A,Q,D)
    -Replace \n and _ with Null
    -Init n_gram
*/
bool lm_fetch_line(line_manager* obj){
    /*clear obj->buffer just in case*/
    //memset(obj->buffer, 0, obj->buffer);

    if(obj->file_status=='Q'){
        if(fgets(obj->buffer, obj->bufsize, obj->input)){
            /*if the space was not enough, i should allocate more memory(double my size) in order to fit eventually all the line*/
            while(complete_phrase(obj->buffer)==false){
                //printf("Ready to make a realloc\n");
                int oldsize;
                char* temp = realloc(obj->buffer, 2*obj->bufsize*sizeof(char));
                if(temp==NULL){
                    fprintf(stderr,"Realloc Failed :: qm_fetch_line\n");
                    exit(-1);
                }
                obj->buffer=temp;
                oldsize=obj->bufsize;
                obj->bufsize=2*obj->bufsize;
                fgets(&obj->buffer[oldsize-1], oldsize+1, obj->input); //fgets writes after oldsize-1 because before this position i have the previous line
            }
            printf("Line read:%s", obj->buffer);
        }
        else{ // EOF is found, my work here is done
            return false;
        }  
    
        /*Valid Task Recognition*/
        if((obj->buffer[0]=='F')||((obj->buffer[0]=='A' || obj->buffer[0]=='D' ||obj->buffer[0]=='Q' ) && obj->buffer[1]==' ')){
            //In case of F you just ignore this line and get the next one.
            if(obj->buffer[0]=='F'){
                //no more tasks to do, IGNORE for now
                return lm_fetch_line(obj);  
            }
            /*keep line status, check if ID is valid*/
            obj->line_status=obj->buffer[0];
    
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
    else if(obj->file_status=='I'){
        //every line is a ngram
        obj->buffer[0]='A';
        obj->buffer[1]=' ';
        if(fgets(&obj->buffer[2],  obj->bufsize-2, obj->input)){
            /*if the space was not enough, i should allocate more memory(double my size) in order to fit eventually all the line*/
            while(complete_phrase(obj->buffer)==false){
                //printf("Ready to make a realloc\n");
                int oldsize;
                char* temp = realloc(obj->buffer, 2*obj->bufsize*sizeof(char));
                if(temp==NULL){
                    fprintf(stderr,"Realloc Failed :: qm_fetch_line\n");
                    exit(-1);
                }
                obj->buffer=temp;
                oldsize=obj->bufsize;
                obj->bufsize=2*obj->bufsize;
                fgets(&obj->buffer[oldsize-1], oldsize+1, obj->input); //fgets writes after oldsize-1 because before this position i have the previous line
            }
            printf("Line read:%s", obj->buffer);
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
    //return false;   
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
    printf("Ngram first word is%s\n", &obj->buffer[obj->n_gram_position]);
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
    if(i>=obj->line_end){
        obj->word_start=NULL;
        obj->word_position=0;
    }
    else{
        obj->word_start= &obj-> buffer[i+1]; 
        obj->word_position=i+1;
        /*if(obj->buffer[i+1]=='\0'){
            return lm_fetch_word(obj);
        }*/
    }
    return obj->word_start;
}

/*Result Manager*/

/*Initialise and deallocate Memory*/

void result_manager_init(result_manager* obj,FILE *fp){
    obj->output=fp;
    obj->output_buffer=malloc(sizeof(char)*INIT_SIZE_BUF);
    if(obj->output_buffer==NULL){
        fprintf(stderr,"Malloc failed :: result_manager_init\n");
        exit(-1);
    }
    obj->output_bufsize=INIT_SIZE_BUF;
    obj->word_buffer=malloc(sizeof(char*)*INIT_SIZE_BUF);
    if(obj->word_buffer==NULL){
        fprintf(stderr,"Malloc failed :: result_manager_init\n");
        exit(-1);
    }
    obj->bufsize=INIT_SIZE_BUF;
}

void result_manager_fin(result_manager* obj){
    if(obj->output_buffer!=NULL){
        free(obj->output_buffer);
    }
    if(obj->word_buffer!=NULL){
        free(obj->word_buffer);
    }
    return;
}

/*Checks if word_size is bigger than max_words and empty-init word_buffer*/
void rm_start(result_manager *obj,int max_words){
    if(obj->bufsize<max_words){ //you need to allocate more memory
        char** temp = realloc(obj->word_buffer, (obj->bufsize+(max_words-obj->bufsize))*sizeof(char*));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: qm_fetch_line\n");
            exit(-1);
        }
        obj->word_buffer=temp;
    }
    /*a new new ngram starts*/
    obj->first_available_slot=0;
    return;
}

/*Now new ngram is entering to word buffer, so i want it clean and i dont mind of old ngram*/
void rm_new_ngram(result_manager *obj){
    obj->current_ngram=NULL;
    obj->current_word_index=-1;
    memset(obj->word_buffer, 0, obj->bufsize);
}

/*add in word_buffer a new pointer to word after the first available*/
void rm_append_word(result_manager* obj,char* word){
    int i;
    i=obj->current_word_index;
    i++;
    /*check if a new word fits, maybe it doesnt need, because rm_start handles it but just in case*/
    if(i>=obj->bufsize){
        char** temp = realloc(obj->word_buffer, 2*obj->bufsize*sizeof(char*));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: rm_append_word\n");
            exit(-1);
        }
        obj->word_buffer=temp;
        obj->bufsize= 2*obj->bufsize*sizeof(char*);
    }
    obj->word_buffer[i]= word;
    obj->current_word_index=i;
    return;
}

/*This word is not a ngram, so i "clean" word_buffer*/
void rm_ngram_undetected(result_manager* obj){
    memset(obj->word_buffer, 0, obj->bufsize);
    /*make all buffer \0*/
    obj->current_word_index=-1;
    return;
}

/*Αdd all the words of word_buffer to output buffer plus one |
if current_ngram==NULL that means that i dont need to strcpy last ngram of output buffer, else make a copy of that*/
void rm_ngram_detected(result_manager* obj){
    int word_len;
    int i=0;
    if(obj->first_available_slot>=obj->output_bufsize){
        /*first available slot is out of boundries so double output buffer */
        char* temp = realloc(obj->output_buffer, 2*obj->output_bufsize*sizeof(char));
        if(temp==NULL){
            fprintf(stderr,"Realloc Failed :: rm_ngram_detected\n");
            exit(-1);
        }
        obj->output_buffer=temp;
        obj->output_bufsize= 2*obj->output_bufsize;
    }
    /*check if current_ngram is null, so you can or not use last thing that was written in output_buffer*/
    if(obj->current_ngram!=NULL){
        /*check if it fits*/
        word_len=strlen(obj->current_ngram); //i dont need |
        strcpy(&obj->output_buffer[obj->first_available_slot+1], obj->current_ngram);
        obj->output_buffer[obj->first_available_slot+word_len]=' '; // if it dont counts /0 else i will test word_len-1
        //obj->current_ngram=&obj->output_buffer[first_available_slot];
        obj->first_available_slot=obj->first_available_slot+1+word_len;
    }
    else{
        obj->current_ngram=&obj->output_buffer[obj->first_available_slot];
    }
    while(obj->word_buffer[i]!='\0'){
        printf("%s\n",obj->word_buffer[i]);
        /*find and copy to first available slot in output_buffer*/
        strcpy(&obj->output_buffer[obj->first_available_slot], obj->word_buffer[i]);
        strcat(&obj->output_buffer[obj->first_available_slot], " ");
        word_len=strlen(obj->word_buffer[i])+1;
        obj->first_available_slot=obj->first_available_slot+word_len;
        i++;
    }
    obj->output_buffer[obj->first_available_slot-1]='|'; //last thing shouldn't be space but |
    printf("Current ngram:%s\n",obj->current_ngram);
    /*clean word buffer*/
    memset(obj->word_buffer, 0, obj->bufsize);
    obj->current_word_index=-1;
}

/*Print everything in buffer, buffer_end is the end of output and is probably | so i dont print it*/
void rm_completed(result_manager* obj){
    int i=0;
    if(obj->first_available_slot==0){ //no ngram detected
        fprintf(obj->output,"-1\n");
    }    
    else{
        while(i<obj->first_available_slot-1){ 
            while(obj->output_buffer[i]!='\0'){
                if(i!=obj->first_available_slot-1){
                    fprintf(obj->output,"%c",obj->output_buffer[i]);
                }
                else{
                    break;
                }
                i++;
            }
            i++;
        }
        fprintf(obj->output,"\n");
        /*clean output_buffer and word_buffer*/
        memset(obj->output_buffer,0, obj->output_bufsize);
        memset(obj->word_buffer,0, obj->bufsize);
    }
    obj->first_available_slot=0;
    obj->current_ngram=NULL;
}