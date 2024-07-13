IDIR=include
SDIR=src
TDIR=test
CC=gcc
CFLAGS=-I$(IDIR) -Wall
DEBUG_FLAGS=-g3 -fsanitize=undefined -fno-sanitize-recover
RELEASE_FLAGS=-O3 -flto

DIJKSTRA_DEPS=$(TDIR)/demo.c $(SDIR)/dijkstra.c
DIJKSTRA_OBJS=$(DIJKSTRA_DEPS:%.c=%.h)

default: libdijkstra.a

libdijkstra.a: dijkstra.o
	ar rcs $@ $^

dijkstra.o: $(SDIR)/dijkstra.c $(IDIR)/dijkstra.h
	$(CC) -c -o $@ $(SDIR)/dijkstra.c $(CFLAGS) $(RELEASE_FLAGS)

release: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(RELEASE_FLAGS)

debug: $(DIJKSTRA_DEPS) $(IDIR)/dijkstra.h
	$(CC) -o demo $(DIJKSTRA_DEPS) $(CFLAGS) $(DEBUG_FLAGS)

.PHONY: clean

clean:
	rm -f demo *.o *.a
