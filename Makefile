CC=gcc
WCC=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Wextra -Ofast
DEPS = tables.h utils.h chess.h uci.h
OBJ = tables.o utils.o chess.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


chess: tables.o utils.o uci.o chess.o
	$(CC) -o chess tables.o utils.o uci.o chess.o $(CFLAGS)

chess.exe: 
	$(WCC) -o chess.exe *.c *.h $(CFLAGS)

run: chess
	./chess

run.exe: chess.exe
	./chess.exe

clean:
	rm -f chess *.o chess.exe
