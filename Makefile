PROGS	= huffman_compression
OBJ	= huffman_compression.o huffman_tree.o huffman.o
CC	= gcc
FLAGS	= -Wall $(DEBUG) -lncurses

all: $(PROGS)

debug: all
debug: DEBUG=-g

huffman_compression: $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)
%.o: %.c
	$(CC) -c $^ $(FLAGS)

.PHONY: clean
clean:
	$(RM) $(wildcard *.o)
	$(RM) $(PROGS)
