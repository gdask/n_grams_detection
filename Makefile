CC = gcc
OUT = n_gram
CFLAGS = -Wall -g -O3

<<<<<<< HEAD
OBJECTS = trie.o trie_node.o main.o string_utils.o bloom_filter.o murmur3.o topk.o hashtable.o
SOURCES = trie.c trie_node.c main.c string_utils.c bloom_filter.c murmur3.c topk.c hashtable.c
HEADERS = trie.h trie_node.h string_utils.h bloom_filter.h murmur3.h topk.h hashtable.h
=======
OBJECTS = trie.o trie_node.o main.o string_utils.o topk.o ./filters/bloom_filter.o ./filters/murmur3.o ./filters/pointer_set.o
SOURCES = trie.c trie_node.c main.c string_utils.c topk.c ./filters/bloom_filter.c ./filters/murmur3.c ./filters/pointer_set.c
HEADERS = trie.h trie_node.h string_utils.h topk.h ./filters/bloom_filter.h ./filters/murmur3.h ./filters/pointer_set.h
>>>>>>> bf2b6b799b105c8e77dc53b6c9b2fd24cf3d30f9

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
