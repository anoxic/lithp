.PHONY: run clean

objects = lithp.c mpc.c

all: lithp

lithp:
	cc -std=c99 -Wall -o lithp $(objects) -ledit -lm

run:
	@cc -std=c99 -Wall -o tmp $(objects) -ledit -lm
	@./tmp
	@rm tmp

clean:
	rm lithp
