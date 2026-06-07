CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -lpthread

TARGET = minios

SRC = \
src/main.c \
src/parser.c \
src/command.c \
src/filesystem.c \
src/vstat.c \
src/cd.c \
src/ls.c \
src/mkdir.c \
src/cat.c \
src/pwd.c \
src/rmdir.c \
src/touch.c \
src/cp.c \
src/chmod.c \
src/clear.c \
src/wc.c

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
