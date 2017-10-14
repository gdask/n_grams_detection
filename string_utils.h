#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#include <stdbool.h>

//Input functions
bool fetch_line();
bool fetch_ngram();
char* fetch_word();
//Output functions
void ngram_detected(char*);

#endif