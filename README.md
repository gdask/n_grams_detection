# n_grams_detection
Software Development Project 2017 @ NKUA (informatics &amp; telcommunications)
Implemented on C.

Trie_node.h/.c :   Implements the 'Trie_node' data stracture.
    Trie_nodes exists in three states:
    Leaf,is a trie node with a stored word,but with unitialized children array.
    Normal,is a trie node with a stored word and initialized children array.
    Head, no word stored,but initialized children array.

    Each state is initialized by functions tn_leaf,tn_normal,tn_head.
    We could change a node state from leaf to normal and vica versa with functions tn_leaf_to_normal and tn_normal_to_leaf.
    Every dynamicly allocated memory from a trie_node is freed after tn_fin.

    tn_lookup(trie_node*,input_word) 'asks' if a trie_node has any child with stored_word==input_word.
    If the child exists a pointer to that child trie_node is returned,otherwise NULL is returned.

    tn_insert(trie_node* current,input_word) adds a new leaf at current trie node,with Word=input_word if that doesnt exists,and returns a pointer at trie_node with Word==input_word.

string_utils.h/.c: 
    Result Manager:
        This structure keeps a buffer of char that will be sent to output_file and an array of pointers to words that maybe will be included to my final result.

        Each time a search is asked, result mananger works as below: 
        -rm_start: checks the size of char** buffer and handles any expansion and initialise variables that are related to output_buffer. (Call once for each request Q).
        -rm_new_ngram: initialise the buffer that keeps words. If rm_new_ngram is called, it means that a new ngram is examined. For example, in order to have an output like this : this is|this is a cat DO NOT CALL new_ngram, if output should be: this is|is a cat call new_ngram.
        -rm_append_word: add new words to temporary ngram
        -rm_detected: the words of char** buffer are a ngram so transfer them to output_buffer
        -rm_undetected: discard everything in char** buffer
        -rm_complete: give output_buffer to output file. If it is empty write -1.
    