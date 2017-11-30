CC = gcc
OUT = n_gram
CFLAGS = -Wall -g -O3

OBJECTS = trie.o trie_node.o main.o string_utils.o topk_hash.o hashtable.o ../filters/bloom_filter.o ../filters/murmur3.o ../filters/pointer_set.o
SOURCES = trie.c trie_node.c main.c string_utils.c topk_hash.c hashtable.c ../filters/bloom_filter.c ../filters/murmur3.c ../filters/pointer_set.c 
HEADERS = trie.h trie_node.h string_utils.h topk_hash.h hashtable.h ../filters/bloom_filter.h ../filters/murmur3.h ../filters/pointer_set.h 

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
