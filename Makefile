CC = gcc
OUT = n_gram
CFLAGS = -Wall -g -O2

OBJECTS = trie.o trie_node.o main.o string_utils.o ./topk/topk.o hashtable.o ./filters/hash_pointer_set.o ./filters/bloom_filter.o ./filters/murmur3.o ./filters/pointer_set.o ./filters/filter_manager.o ./job_scheduler/jobscheduler.o ./topk/topk_hash.o
SOURCES = trie.c trie_node.c main.c string_utils.c ./topk/topk.c hashtable.c ./filters/hash_pointer_set.c ./filters/bloom_filter.c ./filters/murmur3.c ./filters/pointer_set.c ./filters/filter_manager.c ./job_scheduler/jobscheduler.c ./topk/topk_hash.c
HEADERS = control_panel.h trie.h trie_node.h string_utils.h ./topk/topk.h hashtable.h ./filters/hash_pointer_set.h ./filters/bloom_filter.h ./filters/murmur3.h ./filters/pointer_set.h ./filters/filter_manager.h ./job_scheduler/jobscheduler.h ./topk/topk_hash.h

comp : $(OBJECTS)
	$(CC) -g $(OBJECTS) -lpthread -o $(OUT)

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJECTS) $(OUT)
