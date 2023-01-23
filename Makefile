CC=gcc
CFLAGS=-pedantic -Wall -Werror
LINKFLAGS=-lm
DEBUGFLAGS=-ggdb -O0
TARGET=benchmark

$(TARGET):
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET) $(LINKFLAGS)

debug:
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).debug $(LINKFLAGS) $(DEBUGFLAGS)

clean:
	rm $(TARGET)
