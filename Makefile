CC=gcc
CFLAGS=-Wall -Wextra -Werror=pedantic -Wundef -Wshadow -Wformat=2 -Wfloat-equal -Wunreachable-code -Wpedantic -std=gnu17 -Wno-unused-parameter -O3
LINKFLAGS=-lm
LINKFLAGS+=`pkg-config --libs libuv`
DEBUGFLAGS=-ggdb -O0
TARGET=benchmark

.phony: debug clean

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET) $(LINKFLAGS)

mon: mon.c
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET) $(LINKFLAGS) $(DEBUGFLAGS)


debug: $(TARGET)
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).debug $(LINKFLAGS) $(DEBUGFLAGS)

clean:
	rm -f $(TARGET)
