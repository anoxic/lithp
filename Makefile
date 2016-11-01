.SUFFIXES:
.PHONY: run clean debug

SHELL   = /bin/sh
objects = lithp.c mpc.c
options = -std=c99 -ledit -lm

all: lithp

lithp: $(objects)
	$(CC) -o lithp $(objects) $(options)

run:
	@$(CC) -o tmp $(objects) $(options)
	@./tmp
	@rm tmp

debug:
	$(CC) -o tmp $(objects) $(options) -g
	gdb tmp

clean:
	rm lithp tmp
