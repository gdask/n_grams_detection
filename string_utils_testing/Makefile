CC = gcc
OUT = test
CFLAGS = -Wall -g -O3

OBJECTS = $(SOURCES:.c=.o)
SOURCES = ../string_utils.c example.c
HEADERS = ../string_utils.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
