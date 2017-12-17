CC = gcc
OUT = test
CFLAGS = -Wall -g -O3

OBJECTS = $(SOURCES:.c=.o)
SOURCES = ../bloom_filter.c ../murmur3.c example.c
HEADERS = ../bloom_filter.h ../murmur3.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
