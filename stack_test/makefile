CC = gcc
CFLAGS = -g -O0 -I.. -L..
LDFLAGS = -L.. -llua -lm -ldl
TARGET = test1 test2 test_global

all: $(TARGET)

test1: test1.c 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test2: test2.c script.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_global: test_global.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

