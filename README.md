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