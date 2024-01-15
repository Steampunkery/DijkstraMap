IDIR=include
SDIR=src
TDIR=test
CC=gcc
CFLAGS=-I$(IDIR) -Wall
DEBUG_FLAGS=-g3 -fsanitize=undefined -fno-sanitize-recover
RELEASE_FLAGS=-O3 -flto

DIJKSTRA_DEPS=$(TDIR)/demo.c $(SDIR)/dijkstra.c

default: release

release: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(RELEASE_FLAGS)

debug: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(DEBUG_FLAGS)

.PHONY: clean

clean:
	rm -f demo *.o
