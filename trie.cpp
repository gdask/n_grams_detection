#include <stdlib.h>
#include <string.h>
#include "trie.h"

using namespace std;

/*The only way to double an array capacity with legacy methods on c++
is by using realloc from stdlib. Realloc works only with malloc and free.
Realloc cannot operate with memory allocated from new operator.
*/
children::children(int Init_Size):size(Init_Size),First_Available_Slot(0){
    Array = (trie_node*)malloc(size*sizeof(trie_node*));
    if(Array==NULL){
        throw "Malloc Failed, Children Array not initialized properly";
    }
}

children::~children(){
    int i;
    for(i=0;i<First_Available_Slot;i++){
        delete array[i];
    }

    free(Array);
}

void children::double_storage(){
    trie_node **temp = realloc(Array,2*Size*sizeof(trie_node*));
    if(temp==NULL){
        throw "Realloc Failed, Children Array didnt doubled"
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
    if(storage_available()==false) return;
    if(goal_index>=First_Available_Slot) return;

    memmove(Array[goal_index],Array[goal_index+1],(First_Available_Slot-goal_index)*sizeof(trie_node*));
    First_Available_Slot++;
}