all:
	gcc -oFast *.c *.h -o chess

debug:
	gcc -g *.c *.h -o chess.debug

run: all
	./chess

run_debug: debug
	./chess.debug

clean:
	rm chess
	rm chess.debug
