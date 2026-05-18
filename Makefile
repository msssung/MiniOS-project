CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

TARGET = minios

SRC = \
src/main.c \
src/parser.c \
src/command.c \
scr/filesystem.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: all
	./$(TARGET)
