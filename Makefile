CC = g++
OUT = n_gram
CFLAGS = -Wall -g

OBJECTS = trie.o main.o
SOURCES = trie.cpp main.cpp
HEADERS = trie.h 

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean: 
	rm -f $(OBJECTS) $(OUT)