CC=gcc
DEPS = tables.h utils.h chess.h tables.c utils.c chess.c

chess: $(DEPS)
	$(CC) -Ofast -o chess $(DEPS)

chess.debug: $(DEPS)
	$(CC) -g -o chess.debug $(DEPS)

run: chess
	./chess

run.debug: chess.debug
	./chess.debug

clean:
	rm -f chess chess.debug
