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
	int Current;
	trie_node **array;

	void move_data();
public:
	children(int Init_Size);
	~children();
	void Insert_Child(trie_node* new_child);
	trie_node* Search_Child(string keyword);
}

class trie_node {
	string word;
	bool final_node;
	children tmp_name;
}






#endif
