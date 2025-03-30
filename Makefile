# Variables
CFLAGS       := -g -Wall -Wextra -std=c17
CC           := clang
FILES        := src/main.c src/window.c src/log.c src/config.c src/message.c
FORMAT_STYLE := "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 200}"
OUTPUT       := build/pipelam

# Test files
TEST_FILES   := tests/test_main.c tests/test_config.c tests/test_message.c src/config.c src/log.c src/message.c
TEST_OUTPUT  := build/test_runner

# Get pkg-config flags
GTK4_CFLAGS := $(shell pkg-config --cflags gtk4)
GTK4_LIBS := $(shell pkg-config --libs gtk4)
GTK4_LAYER_SHELL_CFLAGS := $(shell pkg-config --cflags gtk4-layer-shell-0)
GTK4_LAYER_SHELL_LIBS := $(shell pkg-config --libs gtk4-layer-shell-0)
# Targets
.PHONY: all format build_dir clean rebuild test

all: format build_dir $(OUTPUT)
# Targets
.PHONY: all format build_dir clean rebuild test

all: format build_dir $(OUTPUT)

build_dir:
	mkdir -p ./build
	mkdir -p ./build
clean:
	rm -f $(OUTPUT) $(TEST_OUTPUT)

test: build_dir $(TEST_OUTPUT)
	./$(TEST_OUTPUT)

$(TEST_OUTPUT): $(TEST_FILES)
	$(CC) $(CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) $(GTK4_CFLAGS) -o $(TEST_OUTPUT) $(TEST_FILES) $(GTK4_LAYER_SHELL_LIBS) $(GTK4_LIBS)
	clang-format -i $(FILES) --style=$(FORMAT_STYLE)

$(OUTPUT): $(FILES)
	$(CC) $(CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) $(GTK4_CFLAGS) -o $(OUTPUT) $(FILES) $(GTK4_LAYER_SHELL_LIBS) $(GTK4_LIBS)
clean:
	rm -f $(OUTPUT) $(TEST_OUTPUT)
	rm -f $(OUTPUT)
