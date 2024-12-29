CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRCS = src/main.c src/chip8.c src/rom.c src/graphics.c
OBJS = $(SRCS:.c=.o)
OUTPUT = chip8

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTPUT)

run: $(OUTPUT)
	./$(OUTPUT) pong.rom