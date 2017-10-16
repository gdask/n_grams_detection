#include "string_utils.h"
#include <stdio.h>

#define INIT_SIZE_BUF 512

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
    if(fgets(obj->buffer, obj->bufsize, input)){
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
            fgets(&obj->buffer[oldsize-1], obj->bufsize, input); //fgets writes after oldsize-1 because before this position i have the previous line
        }

        printf("Line:%s", obj->buffer); 

    }  
    if(obj->buffer[0]=="F"){
        //no more tasks to do
        return false;
    }  

    /*keep line status*/
    obj->line_status=obj->buffer[0];
    if(obj->line_status != 'A' || obj->line_status != 'D' || obj->line_status != 'Q'){
        fprintf(stderr,"Not a valid task :: qm_fetch_line\n");
        return true;
    }

    /*Replace \n and _ with NULL*/
    int i=0;
    while(obj->buffer[i]!='\n'){
        if(obj->buffer[i]==' '){
            obj->buffer[i]=NULL;
        }
        i++;
    }
    obj->buffer[bufsize-1]='\0';
    return true;
}