# Variables
CFLAGS          := -g -Wall -Wextra -Wpedantic -std=c17
CC              := clang

# Default target
.DEFAULT_GOAL   := help
FILES           := src/main.c src/window.c src/log.c src/config.c src/message.c
FORMAT_STYLE    := "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 200}"
OUTPUT          := build/pipelam
ARGS            := /tmp/pipelam.fifo

# Test file s
TEST_FILES      := tests/test_main.c tests/test_config.c tests/test_message.c tests/test_cmdline_options.c src/config.c src/log.c src/message.c
TEST_OUTPUT     := build/test_runner

# json.h lib
JSON_H_LIB_URL  := https://raw.githubusercontent.com/sheredom/json.h/cfdee7c025081ce722644f3ac286e1e27ad16f82/json.h

# Get pkg-config flags
GTK4_CFLAGS             := $(shell pkg-config --cflags gtk4)
GTK4_LIBS               := $(shell pkg-config --libs gtk4)
GTK4_LAYER_SHELL_CFLAGS := $(shell pkg-config --cflags gtk4-layer-shell-0)
GTK4_LAYER_SHELL_LIBS   := $(shell pkg-config --libs gtk4-layer-shell-0)


# Targets
.PHONY: all format build_dir clean rebuild build_test help run run_test download_json_h build test

help: ## Display this help message
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'

all: build_dir download_json_h $(OUTPUT) ## Build the project

download_json_h: ## Download the json.h external single header lib
	@if [ ! -f src/json.h ]; then \
		echo "Downloading json.h..."; \
		curl -L $(JSON_H_LIB_URL) -o src/json.h; \
	fi

build_dir: ## Create build directory if it doesn't exist
	@if [ ! -d build ]; then \
		mkdir build; \
	fi

clean: ## Remove built executables
	rm -f $(OUTPUT) $(TEST_OUTPUT)

build_test: build_dir $(TEST_OUTPUT) ## Build the test suite

rebuild: clean all ## Clean and build the project

build: all ## Build the project without cleaning

format: ## Format the code using clang-format
	clang-format -i $(FILES) --style=$(FORMAT_STYLE)

$(TEST_OUTPUT): $(TEST_FILES)
	$(CC) $(CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) $(GTK4_CFLAGS) -o $(TEST_OUTPUT) $(TEST_FILES) $(GTK4_LAYER_SHELL_LIBS) $(GTK4_LIBS)

$(OUTPUT): $(FILES)
	$(CC) $(CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) $(GTK4_CFLAGS) -o $(OUTPUT) $(FILES) $(GTK4_LAYER_SHELL_LIBS) $(GTK4_LIBS)

test: rebuild build_test ## Rebuild the project and run tests
	./$(TEST_OUTPUT)
	@if [ $$? -eq 0 ]; then \
		echo -e "\n=== All tests completed successfully! ==="; \
	else \
		echo -e "\n=== Tests failed! ==="; \
		exit 1; \
	fi
	@echo -e "\n=== Project built and tested successfully ==="

run: build ## Run the project
	./$(OUTPUT) $(ARGS)

