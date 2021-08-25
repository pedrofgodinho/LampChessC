CC=gcc
CFLAGS=-Wall -Wextra
DEPS = tables.h utils.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chess: chess.o tables.o utils.o
	$(CC) -o chess chess.o tables.o utils.o

chess.debug: *.c *.h
	$(CC) -g -o chess-debug chess.o tables.o utils.o

run: chess
	./chess

run_debug: chess.debug
	./chess.debug

clean:
	rm -f chess chess.debug *.o
