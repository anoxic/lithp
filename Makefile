.SUFFIXES:
.PHONY: run clean debug

SHELL   = /bin/sh
objects = lithp.c mpc.c
options = -std=c99 -ledit -lm

run debug: tmpfile := $(shell mktemp)

all: lithp

lithp: $(objects)
	$(CC) -o lithp $(objects) $(options)

run:
	$(CC) -o '$(tmpfile)' $(objects) $(options) && $(tmpfile)

debug:
	$(CC) -o '$(tmpfile)' $(objects) $(options) -g && gdb $(tmpfile)

clean:
	rm lithp tmp
