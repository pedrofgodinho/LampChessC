CC=gcc
WCC=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Wextra -Ofast
DEPS = tables.h utils.h chess.h uci.h ai.h
DEPSC = tables.c utils.c chess.c uci.c ai.c
OBJ = tables.o utils.o uci.o chess.o ai.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: chess chess.exe

chess: $(OBJ)
	$(CC) -o chess $(OBJ) $(CFLAGS)

chess.exe: $(DEPS) $(DEPSC)
	$(WCC) -o chess.exe $(DEPS) $(DEPSC) $(CFLAGS)

run: chess
	./chess

run.exe: chess.exe
	./chess.exe

clean:
	rm -f chess *.o chess.exe
