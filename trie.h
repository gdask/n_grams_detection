#ifndef TRIE_H
#define	TRIE_H

#include <stdbool.h>

#define INIT_SIZE 2

struct trie_node;
struct children_arr;
typedef struct trie_node trie_node;
typedef struct children_arr children_arr;
//children has an array that contain trie nodes in alphabetical order.
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

//locate index returns where the input word should be placed or where is currently placed.
int ca_locate_index(children_arr* obj,char* input_word);
//After locate index we should check if the word already exists in goal_index
bool ca_word_exists(children_arr* obj,char* input_word,int goal_index);
//If not,we need to place it with force_append. if goal_index =Size we should double the size.
void ca_force_append(children_arr* obj,char* input_word,int goal_index);

struct trie_node {
	bool Initialized;
	bool Leaf;
	char *Word;
	children_arr Children;
};

void tn_init(trie_node* obj);
void tn_fin(trie_node* obj);
int  tn_compare(trie_node* obj,char* input_word);



#endif
