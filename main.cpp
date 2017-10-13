#include <iostream>
#include <string>
#include "trie.h"

using namespace std;

int main(){
    children *tmp = new children();
    string str1("lala");
    string str2("lolo");

    trie_node *tn0=tmp->raw_insert(str1,0);
    trie_node *tn1=tmp->raw_insert(str2,1);
    trie_node *tn2=tmp->raw_insert(str1,2);

    
    delete tmp;

    return 0;
}