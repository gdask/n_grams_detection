CC = gcc
OUT = n_gram
CFLAGS = -Wall -g -O3

OBJECTS = trie.o trie_node.o main.o string_utils.o topk.o hashtable.o ./filters/hash_pointer_set.o ./filters/bloom_filter.o ./filters/murmur3.o ./filters/pointer_set.o ./topk_hash/topk_hash.o
SOURCES = trie.c trie_node.c main.c string_utils.c topk.c hashtable.c ./filters/hash_pointer_set.c ./filters/bloom_filter.c ./filters/murmur3.c ./filters/pointer_set.c ./topk_hash/topk_hash.c
HEADERS = trie.h trie_node.h string_utils.h topk.h hashtable.h ./filters/hash_pointer_set.h ./filters/bloom_filter.h ./filters/murmur3.h ./filters/pointer_set.h ./topk_hash/topk_hash.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
