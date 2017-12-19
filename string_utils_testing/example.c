#include <stdio.h>
#include "../string_utils.h"
int main(){
    line_manager lm;
    FILE* in;
    line* has_line;
    in=fopen("../small_static/small_static.work", "r");
    if(in==NULL){
        return -1;
    }
    line_manager_init(&lm, in, 'Q');
    has_line=lm_fetch_sequence_line(&lm);
    while(has_line!=NULL){
        line_parse(has_line);
        char* first_word_of_ngram;
        first_word_of_ngram=line_fetch_ngram(has_line);
        if(first_word_of_ngram!=NULL)
            printf("%s\n", first_word_of_ngram);
        has_line=lm_fetch_sequence_line(&lm);
    }
    line_manager_fin(&lm);
    fclose(in);
}