CC = gcc
CFLAGS = -O3 -Wall -g
HEADERS = main.c functii.c functii_s.c
EXEC = tema2

.PHONY = build run clean

build: $(EXEC)

$(EXEC): $(HEADERS)
	$(CC) $(CFLAGS) $(HEADERS) -o $@

run: build
	./$(EXEC) ${ARGS}

clean:
	rm -f $(EXEC)