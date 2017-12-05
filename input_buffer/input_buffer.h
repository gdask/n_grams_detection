#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#define MB_SIZE 1048576

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//holds a huge buffer that contains multiple lines
typedef struct input_buffer{
    FILE* input;
    char* buffer;
    size_t buff_size;
    size_t read_chars;
    size_t current_line_start;
    size_t current_line_end;
    bool force_read;
    bool eof_reached;
}input_buffer;

void input_buffer_init(input_buffer* obj,FILE* input,int mb);
void input_buffer_fin(input_buffer* obj);
//Return line start and line size as argument
char* ib_get_line(input_buffer* obj,int* line_size);




#endif