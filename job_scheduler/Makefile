CC = gcc
OUT = test
CFLAGS = -Wall -g -lpthread

OBJECTS = $(SOURCES:.c=.o)
SOURCES = test_scheduler.c jobscheduler.c alt_scheduler.c
HEADERS = jobscheduler.h alt_scheduler.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -lpthread -o $(OUT)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
