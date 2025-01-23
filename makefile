CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

SRCS = src/main.c src/chip8.c
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
	./$(OUTPUT) test/pong.rom

debug: $(OUTPUT)
	gdb --args ./$(OUTPUT) test/pong.rom
