CC=gcc
CFLAGS=-O2 -Wall -w
LDFLAGS=
TARGET=run

SOURCES=src/client.o src/rc4.c src/fastexp.c

OBJECTS=$(SOURCES:.c=.o)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
