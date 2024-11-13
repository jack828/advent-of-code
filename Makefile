CC=gcc
CFLAGS=-Wall -Wextra -Werror=pedantic -Wundef -Wshadow -Wformat=2 -Wfloat-equal -Wunreachable-code -Wpedantic -std=gnu17 -Wno-unused-parameter -O3
LINKFLAGS=-lm
LINKFLAGS+=`pkg-config --libs libuv`
DEBUGFLAGS=-ggdb -O0
TARGET=benchmark

.phony: debug clean benchmark

benchmark: benchmark.c
	$(CC) benchmark.c $(CFLAGS) -o benchmark $(LINKFLAGS)

mon: mon.c
	$(CC) mon.c $(CFLAGS) -o mon $(LINKFLAGS)

debug: $(TARGET)
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).debug $(LINKFLAGS) $(DEBUGFLAGS)

clean:
	rm -f benchmark mon benchmark.debug mon.debug
