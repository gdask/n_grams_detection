#include <stdio.h>
#include "stdlib.h"
#include "trie_node.h"

int main(){
    char *buf;
    buf = malloc(5);
    char *result;
    result=fgets(buf,5,stdin);
    printf("Buf* = %p,Buf = %s ,Res=%s\n",buf,buf,result);
    buf = realloc(buf,10);
    result=fgets(&(buf[4]),5,stdin);
    printf("Buf* = %p,Buf = %s ,Res=%s\n",buf,buf,result);
    return 0;
}