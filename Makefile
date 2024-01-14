IDIR=include
SDIR=src
TDIR=test
CC=gcc
CFLAGS=-I$(IDIR) $(shell pkg-config --cflags glib-2.0) -Wall
DEBUG_FLAGS=-g3 -fsanitize=undefined -fno-sanitize-recover
RELEASE_FLAGS=-O3

LIBS=$(shell pkg-config --libs glib-2.0)

DIJKSTRA_DEPS=$(TDIR)/demo.c $(SDIR)/dijkstra.c

default: release

release: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(RELEASE_FLAGS) $(LIBS)

debug: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(DEBUG_FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f demo *.o
