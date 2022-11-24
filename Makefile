CC = gcc
CFLAGS = -D_POSIX_C_SOURCE=200809L -std=c99 -Werror -Wall -Wextra -Wvla -Isrc -pthread -g

SRC = $(wildcard src/*/*.c src/*/*/*.c) # $(wildcard src/*.c) 
MAIN = src/main.c
MAIN_OBJ = $(MAIN:.c=.o)
SRC_OBJ = $(SRC:.c=.o)
EXE = nginl

all: $(EXE)

$(EXE): $(SRC_OBJ) $(MAIN_OBJ)
	$(CC) -o $(EXE) $^

debug: $(SRC_OBJ) $(MAIN_OBJ)
	$(CC) -o $(EXE) -fsanitize=address $^

clean:
	$(RM) -f $(SRC_OBJ) $(TEST_OBJ) $(MAIN_OBJ) $(EXE)
