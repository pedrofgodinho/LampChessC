all: *.c *.h
	gcc -oFast *.c *.h -o chess

debug: *.c *.h
	gcc -g *.c *.h -o chess.debug

run: all
	./chess

run_debug: debug
	./chess.debug

clean:
	rm -f chess chess.debug
