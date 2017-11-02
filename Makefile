CC = gcc
OUT = n_gram
CFLAGS = -Wall -g

OBJECTS = trie.o trie_node.o main.o string_utils.o
SOURCES = trie.c trie_node.c main.c string_utils.c
HEADERS = trie.h trie_node.h string_utils.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
