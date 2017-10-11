CC = g++
PATH = .././test_env
CFLAGS = -I
DEPS = trie.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)