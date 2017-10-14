CC = gcc
OUT = n_gram
CFLAGS = -Wall -g

OBJECTS = trie_node.o main.o string_utils.o
SOURCES = trie_node.c main.c string_utils.c
HEADERS = trie_node.h string_utils.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean: 
	rm -f $(OBJECTS) $(OUT)