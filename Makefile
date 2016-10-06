CC=clang
CFLAGS=-c -Wall -I/usr/local/include/hidapi
LDFLAGS=-lhidapi
SOURCES=cadmousectl.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=cadmousectl

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@


