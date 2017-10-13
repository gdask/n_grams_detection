#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

using namespace std;

/*The only way to double an array capacity with legacy methods on c++
is by using realloc from stdlib. Realloc works only with malloc and free.
Realloc cannot operate with memory allocated from new operator.
*/
children::children(){
    Size = INIT_SIZE;
    First_Available_Slot = 0;
    Array = (trie_node**)malloc(Size*sizeof(trie_node*));
    if(Array==NULL){
        cerr << "Malloc Failed, Children Array not initialized properly" << endl;
        exit(-1);
    }
}

children::~children(){
    int i;
    for(i=0;i<First_Available_Slot;i++){
        delete Array[i];
        //cout << "Deleted child from arr" << endl;
    }
    free(Array);
    //cout << "Deleted children arr" << endl;
}

void children::double_storage(){
    trie_node **temp = (trie_node**)realloc(Array,2*Size*sizeof(trie_node*));
    if(temp==NULL){
        cerr << "Realloc Failed, Children Array didnt doubled" << endl;
        exit(-1);
    }
    else{
        Size=Size*2;
        Array = temp;
    }
    return;
}

/* After this command every entry from [0-goal,index-1] stays exactly as before
Every entry from [goal_index, Occupied_Storage-1] shifts +1 => [goal_index, Occupied_Storage]
Array[goal_index] is available to store the new entry.
*/
void children::move_data(int goal_index){
    //if(storage_available()==false) return;
    if(goal_index>=First_Available_Slot) return;

    memmove(&Array[goal_index],&Array[goal_index+1],(First_Available_Slot-goal_index)*sizeof(trie_node*));
    First_Available_Slot++;
}

trie_node* children::raw_insert(string input_word,int goal_index){
    if(storage_available()==false){ //We need more space!
        double_storage();
    }
    move_data(goal_index);
    Array[goal_index] = new trie_node(input_word);
    First_Available_Slot++;
    //cout << "Created trie_node " << endl;
    return Array[goal_index];
}

bool children::child_exists(string input_word,int goal_index){
    if(goal_index >= First_Available_Slot){ //No object in that location
        return false;
    }
    if(Array[goal_index]->word_compare(input_word)==0){
        return true;
    }
    return false;
}