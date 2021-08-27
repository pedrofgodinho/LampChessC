CC=gcc
CFLAGS=-Wall -Wextra -Ofast
DEPS = tables.h utils.h chess.h
OBJ = tables.o utils.o chess.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chess: tables.o utils.o chess.o
	$(CC) -o chess tables.o utils.o chess.o $(CFLAGS)

run: chess
	./chess

clean:
	rm -f chess *.o
