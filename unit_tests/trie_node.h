#ifndef TRIE_NODE_H
#define	TRIE_NODE_H

#include <stdbool.h>

#define INIT_SIZE 2

struct trie_node;
struct children_arr;
typedef struct trie_node trie_node;
typedef struct children_arr children_arr;

struct locate_result{
	int index;
	bool found;
	trie_node* node_ptr;
};
typedef struct locate_result loc_res;

//children has an array that contain pointers to trie nodes in alphabetical order.
//Array size starts with Init_Size and doubles everytime the array is full.
struct children_arr{
	bool Initialized;
	int Size;
	int First_Available_Slot;
	trie_node *Array;
};

void ca_init(children_arr* obj,int init_size);
void ca_fin(children_arr* obj);
void ca_double(children_arr* obj);


//Places at goal index a new leaf with input word. Shifts right existing entries if needed
void ca_force_append(children_arr* obj,char* input_word,int goal_index);
//Removes Array[goal_index] from goal index. Shifts left existing entries if needed.
void ca_force_delete(children_arr* obj,int goal_index);
//Binary search on childern array,returns a locate_result struct.
loc_res ca_locate_bin(children_arr* obj,char* input_word);

struct trie_node {
	//bool Initialized;
	bool Leaf;
	bool Head;
	char* Word;
	bool final;
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
trie_node* tn_insert(trie_node* obj,int init_child_size,char* input_word);
void tn_set_final(trie_node* obj);
void tn_unset_final(trie_node* obj);
bool tn_has_fork(trie_node* obj);
bool tn_has_child(trie_node* obj);

//Internal functions
//Transform leaf node to normal,initiliazes the children array struct.
void tn_leaf_to_normal(trie_node* obj,int init_child_size);
void tn_normal_to_leaf(trie_node* obj);
//Check functions
bool tn_is_leaf(trie_node* obj);
bool tn_is_normal(trie_node* obj);
bool tn_is_head(trie_node* obj);
//Debug purpose printing function
void tn_print_subtree(trie_node* obj);

#endif
