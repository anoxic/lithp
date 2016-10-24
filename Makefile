.PHONY: run clean

all: lithp

lithp:
	cc -std=c99 -Wall -o lithp lithp.c

run:
	@cc -std=c99 -Wall -o tmp lithp.c
	@./tmp
	@rm tmp

clean:
	rm lithp
