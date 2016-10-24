.PHONY: run clean

all: lithp

lithp:
	cc -std=c99 -Wall -o lithp lithp.c -ledit -lm

run:
	@cc -std=c99 -Wall -o tmp lithp.c -ledit -lm
	@./tmp
	@rm tmp

clean:
	rm lithp
