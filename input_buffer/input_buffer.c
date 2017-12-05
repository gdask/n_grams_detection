#include "input_buffer.h"
#include <stdio.h>


void input_buffer_init(input_buffer* obj,FILE* input,int mb){
    if(mb<1){
        fprintf(stderr,"MB should be at least one\n");
        exit(-1);
    }
    obj->buffer = malloc(MB_SIZE*mb*sizeof(char));
    if(obj->buffer==NULL){
        fprintf(stderr,"Input buffer init malloc failed\n");
        exit(-1);
    }
    obj->buff_size=mb*MB_SIZE*sizeof(char);
    obj->input = input;
    obj->current_line_end=0;
    obj->current_line_start=0;
    obj->read_chars=0;
    obj->force_read = true;
    obj->eof_reached = false;
}

void input_buffer_fin(input_buffer* obj){
    free(obj->buffer);
}

char* ib_get_line(input_buffer* obj,int* line_size){
    size_t succ_read;
    //first read case
    if(obj->force_read==true){
        obj->force_read=false;
        succ_read=fread(obj->buffer,1,obj->buff_size,obj->input);
        if(succ_read!=obj->buff_size){
            if(feof(obj->input)){
                obj->eof_reached = true;
            } 
            else{ //unsuccesfull read
                fprintf(stderr,"Read error on ib get line:%s\n",ferror(obj->input));
                exit(-1);
            }
        }
    }
    //find end of line
    size_t search_bytes = obj->buff_size - obj->current_line_end;
    char* start = &obj->buffer[obj->current_line_start];
    char* end = memchr(start,'\n',search_bytes);
    //if end of line is not in buffer
    if(end==NULL){
        //check for EOF
        if(obj->eof_reached==false){
            //Load from file,starting from the incomplete line's first character
            size_t look_back = obj->buff_size - obj->current_line_end;
            if(fseek(obj->input,-look_back,SEEK_CUR)!=0){
                fprintf(stderr,"Fseek error on ib get line:%s\n",ferror(obj->input));
                exit(-1);
            }
            succ_read=fread(obj->buffer,1,obj->buff_size,obj->input);
            if(succ_read!=obj->buff_size){
                if(feof(obj->input)){
                    obj->eof_reached = true;
                } 
                else{
                    fprintf(stderr,"(2)Read error on ib get line:%s\n",ferror(obj->input));
                    exit(-1);
                }
            }
            //Line starts at 0,search for the end of line
        }
        else{
            //File entirely read
        }
    }
}