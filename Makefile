CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude -O2
LDFLAGS = -lcurl

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# Source files
APP_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/movie.c $(SRC_DIR)/ds.c $(SRC_DIR)/json_parser.c $(SRC_DIR)/tmdb.c $(SRC_DIR)/ui.c
TEST_SRCS = $(TEST_DIR)/test_runner.c $(TEST_DIR)/test_ds.c $(TEST_DIR)/test_json.c $(TEST_DIR)/test_tmdb.c \
            $(SRC_DIR)/movie.c $(SRC_DIR)/ds.c $(SRC_DIR)/json_parser.c $(SRC_DIR)/tmdb.c $(SRC_DIR)/ui.c

# Binaries
APP_BIN = cinematch.exe
TEST_BIN = test_runner.exe

.PHONY: all test clean run

all: $(APP_BIN)

$(APP_BIN): $(APP_SRCS)
	$(CC) $(CFLAGS) $(APP_SRCS) $(LDFLAGS) -o $(APP_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_SRCS) $(LDFLAGS) -o $(TEST_BIN)

run: $(APP_BIN)
	./$(APP_BIN)

clean:
	rm -f $(APP_BIN) $(TEST_BIN) *.o
