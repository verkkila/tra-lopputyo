CC=gcc
CFLAGS=-O -g -ansi -pedantic -Wall -Wextra -Werror -Wmissing-prototypes -Wstrict-prototypes
LDFLAGS=
EXEC=tra-lopputyo
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)