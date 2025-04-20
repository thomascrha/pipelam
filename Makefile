# Variables
CFLAGS          := -O3 -Wall -Wextra -Wpedantic -std=c17
CC              := clang

# Installation paths
PREFIX          ?= /usr/local
BINDIR          := $(PREFIX)/bin

# Default target
.DEFAULT_GOAL   := help

# Build directory
BUILD_DIR       := build

# Source files
SRC_DIR         := src
SRC_FILES       := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# External dependencies
GTK4_CFLAGS             := $(shell pkg-config --cflags gtk4)
GTK4_LIBS               := $(shell pkg-config --libs gtk4)
GTK4_LAYER_SHELL_CFLAGS := $(shell pkg-config --cflags gtk4-layer-shell-0)
GTK4_LAYER_SHELL_LIBS   := $(shell pkg-config --libs gtk4-layer-shell-0)

# json.h lib
JSON_H_LIB_URL  := https://raw.githubusercontent.com/sheredom/json.h/cfdee7c025081ce722644f3ac286e1e27ad16f82/json.h

# Test files
TEST_DIR        := tests
TEST_SRC        := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ        := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRC))
TEST_OUTPUT     := $(BUILD_DIR)/test_runner

# Output binary
OUTPUT          := $(BUILD_DIR)/pipelam

build: build_dir download_json_h $(OUTPUT) ## Build and download all deps for the project

help: ## Display this help message
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'

build_dir: ## Create build directory if it doesn't exist
	@mkdir -p $(BUILD_DIR)

download_json_h: ## Download the json.h external single header lib
	@if [ ! -f $(SRC_DIR)/json.h ]; then \
		echo "Downloading json.h..."; \
		curl -L $(JSON_H_LIB_URL) -o $(SRC_DIR)/json.h; \
	fi

# Create object files from C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -c $< -o $@

# Create object files from test C source files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -c $< -o $@

# Build the pipelam executable
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -o $@ $^ $(GTK4_LIBS) $(GTK4_LAYER_SHELL_LIBS)

# Dependencies for object files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/log.h $(SRC_DIR)/message.h $(SRC_DIR)/window.h $(SRC_DIR)/config.h $(SRC_DIR)/cli.h
$(BUILD_DIR)/log.o: $(SRC_DIR)/log.c $(SRC_DIR)/log.h $(SRC_DIR)/config.h
$(BUILD_DIR)/window.o: $(SRC_DIR)/window.c $(SRC_DIR)/window.h $(SRC_DIR)/config.h $(SRC_DIR)/log.h
$(BUILD_DIR)/message.o: $(SRC_DIR)/message.c $(SRC_DIR)/message.h $(SRC_DIR)/config.h $(SRC_DIR)/log.h $(SRC_DIR)/json.h
$(BUILD_DIR)/config.o: $(SRC_DIR)/config.c $(SRC_DIR)/config.h $(SRC_DIR)/log.h $(SRC_DIR)/cli.h
$(BUILD_DIR)/cli.o: $(SRC_DIR)/cli.c $(SRC_DIR)/cli.h $(SRC_DIR)/config.h $(SRC_DIR)/log.h

# Build test runner
$(TEST_OUTPUT): $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES)) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -o $@ $^ $(GTK4_LIBS) $(GTK4_LAYER_SHELL_LIBS)

clean: ## Remove built executables and object files
	rm -rf $(BUILD_DIR)

build_test: build_dir $(TEST_OUTPUT) ## Build the test suite

rebuild: clean build ## Clean and build the project

format: ## Format the code using clang-format
	clang-format -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h $(TEST_DIR)/*.c

test: rebuild build_test ## Rebuild the project and run tests
	./$(TEST_OUTPUT)
	@if [ $$? -eq 0 ]; then \
		echo -e "\n=== All tests completed successfully! ==="; \
	else \
		echo -e "\n=== Tests failed! ==="; \
		exit 1; \
	fi

run: build ## Run the project
	./$(OUTPUT) /tmp/pipelam.fifo


debug: ## Build with enhanced debugging symbols for GDB
	$(MAKE) CFLAGS="-ggdb -Wall -Wextra -Wpedantic -std=c17" build

derun: debug ## Run the project
	./$(OUTPUT) /tmp/pipelam.fifo

release: format ## Create a release NOTE: VERSION is required. Usage: make release VERSION=X.Y.Z
	@echo "Creating release..."
	@if [ -z "$(VERSION)" ]; then \
		echo "Error: VERSION is required. Usage: make release VERSION=X.Y.Z"; \
		exit 1; \
	fi
	@./scripts/create-release.sh $(VERSION)

docs: ## Generate man pages from scdoc
	@mkdir -p build/man
	@scdoc < man/pipelam.1.scd > build/man/pipelam.1
	@scdoc < man/pipelam.toml.5.scd > build/man/pipelam.toml.5

install: ## Install pipelam to the system
	@install -d $(BINDIR)
	@install -m 755 $(OUTPUT) $(BINDIR)/pipelam
	@install -d /etc/pipelam
	@install -m 644 config/pipelam.toml /etc/pipelam/pipelam.toml
	@if [ -f build/man/pipelam.1 ]; then \
		install -d $(PREFIX)/share/man/man1; \
		install -m 644 build/man/pipelam.1 $(PREFIX)/share/man/man1/pipelam.1; \
		if command -v makewhatis >/dev/null 2>&1; then \
			makewhatis $(PREFIX)/share/man; \
		elif command -v mandb >/dev/null 2>&1; then \
			mandb; \
		else \
			echo "Warning: Neither makewhatis nor mandb found. Man page database not updated."; \
		fi; \
	fi
	@if [ -f build/man/pipelam.toml.5 ]; then \
		install -d $(PREFIX)/share/man/man5; \
		install -m 644 build/man/pipelam.toml.5 $(PREFIX)/share/man/man5/pipelam.toml.5; \
		if command -v makewhatis >/dev/null 2>&1; then \
			makewhatis $(PREFIX)/share/man; \
		elif command -v mandb >/dev/null 2>&1; then \
			mandb; \
		else \
			echo "Warning: Neither makewhatis nor mandb found. Man page database not updated."; \
		fi; \
	fi

install_systemd: ## Install systemd (user) service and timer
	@install -d $(HOME)/.config/systemd/user
	@install -m 644 systemd/pipelam.service $(HOME)/.config/systemd/user/pipelam.service
	@install -m 644 systemd/pipelam.socket $(HOME)/.config/systemd/user/pipelam.socket
	@systemctl --user daemon-reload

uninstall: ## Uninstall pipelam from the system
	@rm -f $(BINDIR)/pipelam
	@rm -rf /etc/pipelam
	@rm -f $(PREFIX)/share/man/man1/pipelam.1
	@rm -f $(PREFIX)/share/man/man5/pipelam.toml.5
	@if command -v makewhatis >/dev/null 2>&1; then \
		makewhatis $(PREFIX)/share/man; \
	elif command -v mandb >/dev/null 2>&1; then \
		mandb; \
	else \
		echo "Warning: Neither makewhatis nor mandb found. Man page database not updated."; \
	fi
