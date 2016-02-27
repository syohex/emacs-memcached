EMACS_ROOT ?= ../..
EMACS ?= emacs

CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)

.PHONY : test

all: memcached-core.so

memcached-core.so: memcached-core.o
	$(LD) -shared $(LDFLAGS) -o $@ $^ -lmemcached

memcached-core.o: memcached-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f memcached-core.so memcached-core.o

test:
	$(EMACS) -Q -batch -L . $(LOADPATH) \
		-l test/test.el \
		-f ert-run-tests-batch-and-exit
