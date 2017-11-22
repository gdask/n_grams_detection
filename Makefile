CC = gcc
OUT = n_gram
CFLAGS = -Wall -g -O3

OBJECTS = trie.o trie_node.o main.o string_utils.o bloom_filter.o murmur3.o topk.o hashtable.o
SOURCES = trie.c trie_node.c main.c string_utils.c bloom_filter.c murmur3.c topk.c hashtable.c
HEADERS = trie.h trie_node.h string_utils.h bloom_filter.h murmur3.h topk.h hashtable.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
