CC = gcc
OUT = test
CFLAGS = -Wall -g -O3

OBJECTS = $(SOURCES:.c=.o)
SOURCES = test_main.c input_buffer.c
HEADERS = input_buffer.h

test : $(OBJECTS)
	$(CC) -g $(OBJECTS) -o $(OUT)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
