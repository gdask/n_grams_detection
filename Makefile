CC = gcc
OUT = n_gram
CFLAGS = -Wall -g

OBJECTS = trie.o main.o string_utils.o
SOURCES = trie.c main.c string_utils.c
HEADERS = trie.h string_utils.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean: 
	rm -f $(OBJECTS) $(OUT)