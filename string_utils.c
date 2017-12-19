#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Line Functions*/
/*Initialise structure line
Buffer starts with length INIT_SIZE_BUF, if a line cannot fit to a buffer with this size, i will handle it propely in qm_fetch_line*/
void line_init(line* obj){
    obj->buffer= (char*) malloc(INIT_SIZE_BUF*sizeof(char)); 
    if(obj->buffer==NULL){
        fprintf(stderr,"Malloc failed :: line_init\n");
        exit(-1);
    }
    obj->bufsize = INIT_SIZE_BUF;
    return;
}
void line_fin(line* obj){
    if(obj->buffer==NULL) return;
    free(obj->buffer);
}

/*boolean functions that concern line_status*/
bool line_is_query(line* obj){
    if(obj->line_status=='Q')
        return true;
    return false;
}

bool line_is_insert(line* obj){
    if(obj->line_status=='A')
        return true;
    return false;
}

bool line_is_delete(line* obj){
    if(obj->line_status=='D')
        return true;
    return false;
}

bool line_is_F(line* obj){
    if(obj->line_status=='F'){
        return true;
    }
    return false;
}

/*each time that fetch_ngram is called, i modify my obj->Buffer with one word less.
Initialise word_position, word_start*/
char* line_fetch_ngram(line* obj){
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
        return line_fetch_ngram(obj);
    }
    obj->word_start=NULL;
    obj->word_position=obj->n_gram_position;
    //printf("Ngram first word is%s\n", &obj->buffer[obj->n_gram_position]);
    return &obj->buffer[obj->n_gram_position]; //no more n grams
}

/*Return pointer a n gram word, each time i call lm_fetch_word, next word should be returned
word_position=-1 in first call of fetch_word*/
char* line_fetch_word(line* obj){
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

/*Parse ngram and replace \n with \0, also keeps the end of line*/
void line_parse(line* obj){
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
    obj->buffer[i]='\0'; // new line -> NULL
    obj->line_end=i;
    obj->buffer[obj->bufsize-1]='\0';
    //Initialise n_gram position
    obj->n_gram_position= 0;
}

/*Line Manager*/

void line_manager_init(line_manager* obj, FILE *fp, char lm_status){
    obj->input=fp;
    obj->lm_status=lm_status;
    obj->number_of_lines=NUMBER_OF_LINES;
    obj->line=(line*) malloc(sizeof(line)*NUMBER_OF_LINES);
    if(obj->line==NULL){
        fprintf(stderr, "Malloc failed :: line_manager_init\n");
        exit(-1);
    }
    //for each line, initialize each structure
    int i;
    for(i=0; i<obj->number_of_lines; i++){
        line_init(&obj->line[i]);
    }
    obj->first_available_slot=0;
    return;
}

void line_manager_fin(line_manager* obj){
    if(obj->line==NULL) return;
    int i=0;
    for(i=0; i<obj->number_of_lines; i++){
        line_fin(&obj->line[i]);
    }
    free(obj->line);

}

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

/*Get a line from file fp and save the status of this line.
Error handling for corrapted lines
Normally returns 0, -1 when eof or error, 1 when F is found and keep k(if exist)*/
int Qline_fetch(line* obj, FILE* fp){
    /*Read a line from input file*/
    if(fgets(obj->buffer, obj->bufsize, fp)){
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
            retbuf=fgets(&obj->buffer[oldsize-1], oldsize+1, fp); //fgets writes after oldsize-1 because before this position i have the previous line
            if(retbuf==NULL){ //something went wrong(EOF)
                return -1;
            }
        }
        //printf("Line read:%s", obj->buffer);
    }
    else{ // EOF is found, my work here is done
        //printf("EOF\n");
        return -1;
    } 
    
    /*Valid Task Recognition*/
    if((obj->buffer[0]=='F')||((obj->buffer[0]=='A' || obj->buffer[0]=='D' ||obj->buffer[0]=='Q' ) && obj->buffer[1]==' ')){
    /*keep line status, check if ID is valid*/
        obj->line_status=obj->buffer[0];

        //In case of F you just ignore this line.
        if(obj->buffer[0]=='F'){
            if(obj->buffer[1]==' '){
                char *ptr;
                long ret;
                int i=0;
                while(obj->buffer[i]!='\n'){
                    i++;
                }
                obj->buffer[i]='\0';
                ret = strtol(&obj->buffer[2], &ptr, 10);
                obj->k=ret;
                obj->buffer[i]='\n'; //because of line_parse()
            }
            return 1;
        }
    } 
    else{
        fprintf(stderr,"Invalid task :: line_fetch\n");
        return -1;
    }
    return 0;
}

/*Get a line from init file. Return -1 when something went wrong or if EOF*/
int Iline_fetch(line* obj, FILE* input){
    //every line is a ngram
    obj->buffer[0]='A';
    obj->buffer[1]=' ';
    if(fgets(&obj->buffer[2],  obj->bufsize-2, input)){
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
            retbuf=fgets(&obj->buffer[oldsize-1], oldsize+1, input); //fgets writes after oldsize-1 because before this position i have the previous line
            if(retbuf==NULL){ //something went wrong(EOF)
                return false;
            }
        }
        if(strcmp(&obj->buffer[2], "DYNAMIC\n")==0){
            return 2; 
        }
        else if(strcmp(&obj->buffer[2], "STATIC\n")==0){
            return 3;
        }
        //printf("line %s", obj->buffer);
    }
    else{ // EOF is found, my work here is done
        return -1;
    }  
    /*keep line status, for this case is only an addition*/
    obj->line_status='A';
    return 0;
}

line* lm_fetch_sequence_line(line_manager* obj){
    int pos;
    if(obj->first_available_slot==0){
        pos = obj->first_available_slot;
    }
    else{
        pos = obj->first_available_slot-1;
    }
    //Check if number of lines is enough
    if(obj->first_available_slot==obj->number_of_lines){
        line* temp = realloc(obj->line, 2*obj->number_of_lines*sizeof(line));
        if(temp==NULL){
            fprintf(stderr, "Error in realloc :: lm_fetch_sequence_line\n");
            exit(-1);
        }
        obj->line=temp;
        obj->number_of_lines=2*obj->number_of_lines;
        /*Initialise new lines*/
        int i;
        for(i=obj->first_available_slot; i< obj->number_of_lines; i++){
            line_init(&obj->line[i]);
        }
    }
    //For files .work
    int retval=0;
    if(obj->lm_status=='Q'){
        retval=Qline_fetch(&obj->line[pos], obj->input);
        if(retval==0){
            obj->first_available_slot++;
            return &obj->line[pos];
        }
        else if(retval==1) return &obj->line[pos]; //HOW DO I HANDLE F???
        else return 0;
    }
    //For files .init
    else if(obj->lm_status=='I'){
        retval=Iline_fetch(&obj->line[pos], obj->input);
        if(retval==2){
            obj->file_status='D';
            retval=Iline_fetch(&obj->line[pos], obj->input);
        }
        if(retval==3){
            obj->file_status='D';
            retval=Iline_fetch(&obj->line[pos], obj->input);
        }
        if(retval==0){
            obj->first_available_slot++;
            return &obj->line[pos];
        }
        else return 0;
    }
    return 0;   
}

line* lm_fetch_independent_line(line_manager* obj){
    int pos=obj->first_available_slot;
    //Check if number of lines is enough
    if(obj->first_available_slot==obj->number_of_lines){
        line* temp = realloc(obj->line, 2*obj->number_of_lines*sizeof(line));
        if(temp==NULL){
            fprintf(stderr, "Error in realloc :: lm_fetch_sequence_line\n");
            exit(-1);
        }
        obj->line=temp;
        obj->number_of_lines=2*obj->number_of_lines;
        /*Initialise new lines*/
        int i;
        for(i=obj->first_available_slot; i< obj->number_of_lines; i++){
            line_init(&obj->line[i]);
        }
    }
    //For files .work
    int retval=0;
    if(obj->lm_status=='Q'){
        retval=Qline_fetch(&obj->line[pos], obj->input);
        if(retval==0) return &obj->line[pos];
        else if(retval==1) return &obj->line[pos]; //HOW DO I HANDLE F???
        else return 0;
    }
    else if(obj->lm_status=='I'){
        retval=Iline_fetch(&obj->line[pos], obj->input);
        if(retval==2){
            obj->file_status='D';
            retval=Iline_fetch(&obj->line[pos], obj->input);
        }
        if(retval==3){
            obj->file_status='D';
            retval=Iline_fetch(&obj->line[pos], obj->input);
        }
        if(retval==0) return &obj->line[pos];
        else return 0;
    }
    return 0;   
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