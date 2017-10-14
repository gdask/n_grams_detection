#ifndef TRIE_H
#define	TRIE_H

#include <stdbool.h>

#define INIT_SIZE 2

struct trie_node;
struct children_arr;
typedef struct trie_node trie_node;
typedef struct children_arr children_arr;

//children has an array that contain pointers to trie nodes in alphabetical order.
//Array size starts with Init_Size and doubles everytime the array is full.
struct children_arr{
	bool Initialized;
	int Size;
	int First_Available_Slot;
	trie_node **Array;
};

void ca_init(children_arr* obj,int init_size);
void ca_fin(children_arr* obj);
void ca_double(children_arr* obj);

//forward: [goal_index,First_Avail]=>[goal_index+1,First_Avail+1]
//backward: [goal_index,First_Avail]=>[goal_index-1,First_Avail-1]
//void ca_shift_entries(children_arr* obj,int goal_index,bool forward);
//locate index returns where the input word should be placed or where is currently placed.
int ca_locate_index(children_arr* obj,char* input_word);
//After locate index we should check if the word already exists in goal_index
bool ca_word_exists(children_arr* obj,char* input_word,int goal_index);
//If not,we need to place it with force_append.
void ca_force_append(children_arr* obj,char* input_word,int goal_index);
//Returns trie_node* of goal_index or NULL
trie_node* ca_get_pointer(children_arr* obj,int goal_index);

struct trie_node {
	//bool Initialized;
	bool Leaf;
	bool Head;
	char* Word;
	children_arr next;
};
//External functions
//Initilalizers for different purposes
void tn_leaf(trie_node* obj,char* input_word);
void tn_normal(trie_node* obj,int init_child_size,char* input_word);
void tn_head(trie_node* obj,int init_child_size);
//Deallocates dynamicly allocated memory
void tn_fin(trie_node* obj);
//Queries functions
trie_node* tn_lookup(trie_node* obj,char* input_word);
trie_node* tn_insert(trie_node* obj,char* input_word);

//Internal functions
//Transform leaf node to normal,initiliazes the children array struct.
void tn_leaf_to_normal(trie_node* obj,int init_child_size);
void tn_normal_to_leaf(trie_node* obj);
//Check functions
bool tn_is_leaf(trie_node* obj);
bool tn_is_normal(trie_node* obj);
bool tn_is_head(trie_node* obj);
//Internal query functions
int	tn_compare(trie_node* obj,char* input_word);
int	tn_lookup_index(trie_node* obj,char* input_word);


#endif
