#ifndef TRIE_H
#define	TRIE_H

#include <string>
#define INIT_SIZE 2

using namespace std;

class trie_node;

//children has an array that contain alphabeticly ordered pointers at the children of the node
class children{
	int Size;
	int First_Available_Slot;
	trie_node **Array;

	//Move data shifts data from [index-First_Avail] => [index+1-First_Avail+1]
	void move_data(int index);
	void double_storage();
	bool storage_available(){if(Size==First_Available_Slot){return false;}return true;};
	//Locate returns the index where the child should be.
	int locate_child(string input_word);
	//Checks if the child is in goal_index
	bool child_exists(string input_word,int goal_index);
	//Creates a new trie node on goal_index with word = input_word.
	//trie_node* raw_insert(string input_word,int goal_index);
public:
	trie_node* raw_insert(string input_word,int goal_index);
	children();
	~children();
	//void insert_child(trie_node* new_child);
	//trie_node* search_child(string input_word);
};

class trie_node {
	string word;
	bool final_node;
	children next_gen;
public:
	trie_node(string input_word):word(input_word),final_node(true){};
	int word_compare(string input_word){return word.compare(input_word);};
};



#endif
