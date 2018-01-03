CC = gcc
CFLAGS = -g -O0 -I.. -L..
LDFLAGS = -L.. -llua -lm -ldl
TARGET = test1 test2 test_global

all: $(TARGET)

test1: test1.o
	$(CC) -o $@ $^ $(LDFLAGS)

test2: test2.o script.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_global: test_global.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

%o: %c
	$(CC) $(CFLAGS) -c  -o $@ $^ 
