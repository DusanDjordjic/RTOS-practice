CC=gcc
CFLAGS=-Wall -Wextra
LFLAGS=-pthread
BIN=main
SRC=main.c
OBJ=main.o

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LFALGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)
