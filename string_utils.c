#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>

/*Initialise structure query manager
Buffer starts with length INIT_SIZE_BUF, if a line cannot fit to a buffer with this size, i will handle it propely in qm_fetch_line*/
void line_manager_init(line_manager* obj,FILE *fp){
    obj->input=fp;
    obj->buffer= malloc(INIT_SIZE_BUF*sizeof(char)); 
    if(obj->buffer==NULL){
        fprintf(stderr,"Malloc failed in line_manager_init\n");
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


/*check if string has a newline char*/
bool complete_phrase(char* line){
    int i=0;
    while(line){
        if(line[i]=='\n')
            return true;
        i++;
    }
    return false;
}

/*  -Gets a line from file
    -Keep key of action (A,Q,D)
    -Replace \n and _ with Null
*/

bool lm_fetch_line(line_manager* obj){
    if(fgets(obj->buffer, obj->bufsize, obj->input)){
        /*if the space was not enough, i should allocate more memory(double my size) in order to fit eventually all the line*/
        while(complete_phrase(obj->buffer)==false){
            int oldsize;
            char* temp = realloc(obj->buffer, 2*obj->bufsize*sizeof(char));
            if(temp==NULL){
                fprintf(stderr,"Realloc Failed :: qm_fetch_line\n");
                exit(-1);
            }
            obj->buffer=temp;
            oldsize=obj->bufsize;
            obj->bufsize=2*obj->bufsize;
            /* George Note: i think the bug is here.
            You ask fgets to fetch 'oldsize*2' bytes,but you have only 'oldsize' space available
            in your buffer because [0-oldsize-1] stores data from previous fgets.
            */
            fgets(&obj->buffer[oldsize-1], obj->bufsize, obj->input); //fgets writes after oldsize-1 because before this position i have the previous line
        }

        printf("Line:%s", obj->buffer); 
    }  
    //George: I think in case of F you just ignore this line and get the next one.
    if(obj->buffer[0]=='F'){
        //no more tasks to do
        return false;
    }

    /*keep line status*/
    obj->line_status=obj->buffer[0];
    if(obj->line_status != 'A' && obj->line_status != 'D' && obj->line_status != 'Q'){
        fprintf(stderr,"Not a valid task :: qm_fetch_line\n");
        //George: In that case maybe we should exit execution.
        //If we dont exit execution at least we have to return false
        //Because if we return true, fetch_ngram and fetch_word will follow up.
        return true;
    }

    /*Replace \n and _ with NULL*/
    int i=0;
    while(obj->buffer[i]!='\n'){
        if(obj->buffer[i]==' '){
            obj->buffer[i]='\0';
        }
        i++;
    }
    obj->buffer[i+1]='\0'; // new line -> NULL
    obj->buffer[obj->bufsize-1]='\0';

    //Initialise n_gram position
    obj->n_gram_position= 0;
    return true;
}

/*each time that fetch_ngram is called, i modify my obj->Buffer with one word less.
Initialise word_position, word_start*/
bool lm_fetch_ngram(line_manager* obj){
    int i;
    int flag=0;
    i=obj->n_gram_position;
    while(obj->buffer[i]!='\0'){
        i++;
    }
    if(i>=obj->line_end){
        return false;
    }
    obj->n_gram_position=i+1; //position of n_gram
    obj->word_start=NULL;
    return true; //no more n grams
}

/*Return pointer a n gram word, each time i call lm_fetch_word, next word should be returned
word_position=-1 in first call of fetch_word*/
char* lm_fetch_word(line_manager* obj){
    int i;
    i=obj->n_gram_position; // i care only for words of current n_gram
    while(obj->buffer[i]!='\0'){
        if(obj->word_start == NULL){ //first word
            obj->word_position=i;
            obj->word_start= &obj->buffer[i];
            break;
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
    }
    return obj->word_start;
}



