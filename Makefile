.PHONY: run clean debug

objects  = lithp.c mpc.c
compiler = gcc
options  = -std=c99 -ledit -lm

all: lithp

lithp:
	$(compiler) -o lithp $(objects) $(options)

run:
	@$(compiler) -o tmp $(objects) $(options)
	@./tmp
	@rm tmp

debug:
	$(compiler) -o tmp $(objects) $(options) -g
	gdb tmp

clean:
	rm lithp tmp
