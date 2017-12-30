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

void print_line(line* obj){
    int i;
    fprintf(stderr,"LINE STATUS:%c\n",obj->line_status);
    for(i=0;i<obj->line_end;i++){
        if(obj->buffer[i]=='\0') fprintf(stderr," ");
        fprintf(stderr,"%c",obj->buffer[i]);
    }
    fprintf(stderr,"\n");
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
    if(obj->buffer==NULL) exit(-1);
    int i=0;
    int words=0;
    while(obj->buffer[i]!='\n'){
        //fprintf(stderr,"%c",obj->buffer[i]);
        if(obj->buffer[i]==' '){
            words++;
            obj->buffer[i]='\0';
        }
        i++;
    }
    //fprintf(stderr,"\n");
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
    obj->line=(line**) malloc(sizeof(line*)*NUMBER_OF_LINES);
    if(obj->line==NULL){
        fprintf(stderr, "Malloc failed :: line_manager_init\n");
        exit(-1);
    }
    //for each line, initialize each structure
    int i;
    for(i=0; i<obj->number_of_lines; i++){
        obj->line[i] = malloc(sizeof(line));
        if(obj->line[i]==NULL){
            fprintf(stderr, "Malloc failed :: line_manager_init\n");
            exit(-1);
        }
        line_init(obj->line[i]);
    }
    obj->first_available_slot=0;
    return;
}

void line_manager_fin(line_manager* obj){
    if(obj->line==NULL) return;
    int i=0;
    for(i=0; i<obj->number_of_lines; i++){
        line_fin(obj->line[i]);
        free(obj->line[i]);
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
        //fprintf(stderr,"Line read:%s", obj->buffer);
    }
    else{ // EOF is found, my work here is done
        //printf("EOF\n");
        return -1;
    } 
    
    /*Valid Task Recognition*/
    if((obj->buffer[0]=='F')||((obj->buffer[0]=='A' || obj->buffer[0]=='D' ||obj->buffer[0]=='Q' ) && obj->buffer[1]==' ')){
    /*keep line status, check if ID is valid*/
        obj->line_status=obj->buffer[0];
        //fprintf(stderr, "Line status %c\n", obj->line_status);
        //In case of F you just ignore this line.
        if(obj->buffer[0]=='F'){
            obj->k=0;
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
            return 0;
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
        pos = 0;
    }
    else{
        pos = obj->first_available_slot-1;
    }
    //For files .work
    int retval=0;
    if(obj->lm_status=='Q'){
        retval=Qline_fetch(obj->line[pos], obj->input);
        if(obj->first_available_slot==0) obj->first_available_slot++;
        if(retval==0){
            //if(line_is_query(obj->line[pos])!=true){
                line_parse(obj->line[pos]);
            //}
            return obj->line[pos];
        }
        else return 0;
    }
    //For files .init
    else if(obj->lm_status=='I'){
        retval=Iline_fetch(obj->line[pos], obj->input);
        if(obj->first_available_slot==0) obj->first_available_slot++;
        if(retval==2){
            obj->file_status='D';
            retval=Iline_fetch(obj->line[pos], obj->input);
        }
        if(retval==3){
            obj->file_status='S';
            retval=Iline_fetch(obj->line[pos], obj->input);
        }
        if(retval==0){
            //obj->first_available_slot++;
            line_parse(obj->line[pos]);
            return obj->line[pos];
        }
        else return 0;
    }
    return 0;   
}

line* lm_fetch_independent_line(line_manager* obj){
    int pos=obj->first_available_slot;
    //Check if number of lines is enough
    if(obj->first_available_slot==obj->number_of_lines){
        line** temp = realloc(obj->line, 2*obj->number_of_lines*sizeof(line*));
        if(temp==NULL){
            fprintf(stderr, "Error in realloc :: lm_fetch_sequence_line\n");
            exit(-1);
        }
        obj->line=temp;
        obj->number_of_lines=2*obj->number_of_lines;
        /*Initialise new lines*/
        int i;
        for(i=obj->first_available_slot; i< obj->number_of_lines; i++){
            obj->line[i] = malloc(sizeof(line));
            if(obj->line[i]==NULL){
                fprintf(stderr, "Malloc failed :: line_manager_init\n");
                exit(-1);
            }
            line_init(obj->line[i]);
        }
    }
    //For files .work
    int retval=0;
    if(obj->lm_status=='Q'){
        retval=Qline_fetch(obj->line[pos], obj->input);
        if(retval==0){
            obj->first_available_slot++;
            //if(line_is_query(obj->line[pos])!=true){
                line_parse(obj->line[pos]);
            //}
            return obj->line[pos];
        }
        else return 0;
    }
    else if(obj->lm_status=='I'){
        retval=Iline_fetch(obj->line[pos], obj->input);
        obj->first_available_slot++;
        if(retval==2){
            obj->file_status='D';
            retval=Iline_fetch(obj->line[pos], obj->input);
        }
        if(retval==3){
            obj->file_status='S';
            retval=Iline_fetch(obj->line[pos], obj->input);
        }
        if(retval==0){    
            line_parse(obj->line[pos]);
            return obj->line[pos];
        }
        else return 0;
    }
    return 0;   
}

/*Result Manager*/

/*Initialise and deallocate Memory*/

void result_init(result* obj){
    obj->output_buffer=(char*)malloc(sizeof(char)*INIT_SIZE_BUF);
    if(obj->output_buffer==NULL){
        fprintf(stderr,"Malloc failed :: result_init\n");
        exit(-1);
    }
    obj->output_bufsize=INIT_SIZE_BUF;
    obj->first_available_slot=0;
    obj->start_ngram=0;
}

void result_fin(result* obj){
    if(obj->output_buffer!=NULL){
        free(obj->output_buffer);
    }
    return;
}

/*Αdd word_count of the words of line manager(current_ngram) to output buffer plus one |*/
void result_ngram_detected(result* obj, line *l, int word_count){
    return;
    char* p_ngram;
    p_ngram=&l->buffer[l->n_gram_position];
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
    obj->output_buffer[obj->first_available_slot-1]='\0'; //last thing shouldn't be space but |
}


/*Keep the result of Q in output buffer, buffer_start is keep in order to know where to start keeping new result*/
void result_completed(result* obj){
    return ;
    if(obj->first_available_slot==0){ //no ngram detected
        strcpy(&obj->output_buffer[obj->first_available_slot],"-1\n");
    }    
    else{
        obj->output_buffer[obj->first_available_slot-1]='\n';
    }
    obj->first_available_slot=0;
}

char* result_fetch_ngram(result* obj){
    int i;
    for(i=obj->start_ngram; i<obj->first_available_slot; i++){
        if(obj->output_buffer[i]=='\0'){
            char*ngram=&obj->output_buffer[obj->start_ngram];
            obj->start_ngram=i+1;
            return ngram;
        }
    }
    return 0;
}

/*Print result of batch*/
//-rm display result has to print all the results that has been used from result manager
//-also give to topk, if needed, the ngrams
void rm_display_result(result_manager* obj){
    return ;
    if(obj->k>0){
        rm_prepare_topk(obj);
    }
    int i;
    for(i=0; i<obj->size; i++){
        //Format result as needed
        int j;
        for(j=0; j<obj->first_available_slot; i++){
            if(obj->result[i]->output_buffer[j]=='\0'){
                obj->result[i]->output_buffer[i]='|';
            }
        }
        obj->result[i]->output_buffer[obj->first_available_slot]='\0';
        fprintf(obj->output,"%s",obj->result[i]->output_buffer);
    }
    if(obj->k>0){
        topk(obj->topk.Hash, obj->k);
        Hash_reuse(obj->topk.Hash);
    }
}

void rm_prepare_topk(result_manager* obj){
    int i;
    for(i=0; i<obj->first_available_slot; i++){
        char* ngram;
        ngram=result_fetch_ngram(obj->result[i]);
        while(ngram!=NULL){
            int length=strlen(ngram);
            Hash_insert(obj->topk.Hash, ngram, length);
            ngram=result_fetch_ngram(obj->result[i]);
        }
    }
}

void rm_init(result_manager* obj, FILE* fp){
    obj->output=fp;
    /*At first topk is not needed*/
    obj->k=-1;
    Hash_init(obj->topk.Hash);
    obj->result=(result**)malloc(sizeof(result*)*NUMBER_OF_LINES); 
    if(obj->result==NULL){
        fprintf(stderr,"Error in malloc :: rm_init\n");
        exit(-1);
    }
    obj->size=NUMBER_OF_LINES;
    obj->first_available_slot=0;
    /*Initialise each result*/
    int i;
    for(i=0;i<obj->size;i++){
        obj->result[i]=malloc(sizeof(result));
        if(obj->result[i]==NULL){
            fprintf(stderr, "Error in malloc :: rm_init\n");
            exit(-1);
        }
        result_init(obj->result[i]);
    }
}

void rm_fin(result_manager* obj){
    //if(obj->k>0){
        Hash_fin(obj->topk.Hash);
    //}
    if(obj->result==NULL) return;
    int i;
    for(i=0; i<obj->size; i++){
        result_fin(obj->result[i]);
        free(obj->result[i]);
    }
    free(obj->result);
}

/*This function is called when line mananager read a line F and a number.
Initialise topk*/
void rm_use_topk(result_manager* obj, int k){
    obj->k=k;
    //Hash_init(obj->topk.Hash);
}   

//Returns the result space of first available slot
result* rm_get_result(result_manager* obj){
    int pos=obj->first_available_slot;
    //Checks if array should be expanded
    if(obj->first_available_slot==obj->size){
        result** temp= realloc(obj->result, sizeof(result*)*2*obj->size);
        if(temp==NULL){
            fprintf(stderr, "Realloc failed :: rm_get_result\n");
            exit(-1);
        }
        obj->result=temp;
        obj->size=2*obj->size;
        //Initisialize result
        int i;
        for(i=obj->first_available_slot; i<obj->size; i++){
            obj->result[i]=malloc(sizeof(result));
            if(obj->result[i]==NULL){
                fprintf(stderr, "Error in malloc :: rm_init\n");
                exit(-1);
            }
            result_init(obj->result[i]);
        }
    }
    obj->first_available_slot++;
    return obj->result[pos];
}