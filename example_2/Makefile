CC=gcc
CFLAGS=-Wall -Wextra -g
LFLAGS=-pthread -g
BIN=main
SRC=main.c ring.c
OBJ=main.o ring.o

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LFALGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)
