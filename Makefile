CC=gcc
CFLAGS=-Wall -Wextra -Werror=pedantic -Wundef -Wshadow -Wformat=2 -Wfloat-equal -Wunreachable-code -Wpedantic -std=gnu17 -Wno-unused-parameter -O3
LINKFLAGS=-lm
DEBUGFLAGS=-ggdb -O0
TARGET=benchmark

$(TARGET):
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET) $(LINKFLAGS)

debug:
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).debug $(LINKFLAGS) $(DEBUGFLAGS)

clean:
	rm $(TARGET)
