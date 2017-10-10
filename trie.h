#ifndef TRIE_H
#define	TRIE_H

#include <string>

using namespace std;

class trie_node;

//children has an array that contain pointers at the children of the node.
//Children of that node are alphabeticly order in the array.
//Array size starts with Init_Size and doubles everytime the array is full.
//Hint: Use malloc & realloc!
class children{
	int Size;
	int First_Available_Slot;
	trie_node **Array;

	void move_data(int index);
	void double_storage();
	bool storage_available(){if(Size==First_Available_Slot){return false;}return true;};
public:
	children(int Init_Size);
	~children();
	void insert_child(trie_node* new_child);
	trie_node* search_child(string keyword);
}

class trie_node {
	string word;
	bool final_node;
	children tmp_name;
}






#endif
