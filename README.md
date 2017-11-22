# n_grams_detection
Software Development Project 2017 @ NKUA (informatics &amp; telcommunications)
Implemented on C.

#### Trie_node:   Implements the 'Trie_node' data stracture.
Trie_nodes exists in three states:
* Leaf,is a trie node with a stored word,but with unitialized children array.
* Normal,is a trie node with a stored word and initialized children array.
* Head, no word stored,but initialized children array.
    Each state is initialized by functions tn_leaf,tn_normal,tn_head.
    We could change a node state from leaf to normal and vica versa with functions tn_leaf_to_normal and tn_normal_to_leaf.
    Every dynamicly allocated memory from a trie_node is freed after tn_fin.
* tn_lookup(trie_node*,input_word) 'asks' if a trie_node has any child with stored_word==input_word.
    If the child exists a pointer to that child trie_node is returned,otherwise NULL is returned.
* tn_insert(trie_node* current,input_word) adds a new leaf at current trie node,with Word=input_word ifthat doesnt exists,and returns a pointer at trie_node with Word==input_word.

#### String_Utils: 
Line Manager:
This structure handles input, init and work files. For our implementation we used 2 line managers(each for init and work files).
We keep a buffer for each line read, which has size at first INIT_SIZE and in case of bigger lines, we handle it with realloc.
* lm_fetch_line(): reads a line from input file, keep it to input_buffer. In this fuction, spaces are erased and made \0 in order to seperate the words and we keep the upper limit of how many words are keep.
* lm_fetch_ngram(): Keeps the start of ngram in line.
* lm_fetch_word(): Returns words of current ngram that we work on.
* lm_is_query/insert/delete(): Boolean return value for id of line detection.

Result Manager:
    This structure keeps a buffer of char that will be sent to output_file and an array of pointers to words that maybe will be included to my final result.
    Each time a search is asked, result mananger works as below: 
* rm_start: checks the size of char** buffer and handles any expansion and initialise variables that are related to output_buffer. (Call once for each request Q).
* rm_new_ngram: initialise the buffer that keeps words. If rm_new_ngram is called, it means that a new ngram is examined. For example, in order to have an output like this : this is|this is a cat DO NOT CALL new_ngram, if output should be: this is|is a cat call new_ngram.
* rm_append_word: add new words to temporary ngram
* rm_detected: the words of char** buffer are a ngram so transfer them to output_buffer
* rm_undetected: discard everything in char** buffer
* rm_complete: give output_buffer to output file. If it is empty write -1.

#### Filters
Bloom Filter: 
    It comes with a test @ /filter/bloom_testing that checks how many false positives we have for 1000 inserted elements and 10,000,000 checks.
    In bloom_filter.h you can choose among 3 configurations for <b>k,m,n</b> and an option for the Mitzenmacher optimization that calls only two times the hash functions.
    The best result comes from the third configuration with the optimization disabled.
    If we have to insert more entities than <b>n</b> ,then we use more than in one bloom filter without a significant increase on false positive ratio.



#### i7 @ 1,8 ghz & hdd:
* Elapsed time without optimization: 3.913862 sec
* Elapsed time with O3 optimization: 2.378355 sec

#### i5 @ 2,4 ghz & ssd (on virtual machine):
* Elapsed time without optimization: 1.913981 sec
* Elapsed time with O3 optimization: 1.291364 sec

#### linux29.di.uoa.gr:
* Elapsed time without optimization: 1.501372 sec
* Elapsed time with O3 optimization: 1.045464 sec

    
