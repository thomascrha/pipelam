# Build directory
BUILD_DIR       := build
BUILD_MAN_DIR   := $(BUILD_DIR)/man
BUILD_DEPS_DIR  := $(BUILD_DIR)/deps

# Variables
CFLAGS          := -O3 -Wall -Wextra -Wpedantic -std=c17
INCFLAGS        := -I$(BUILD_DEPS_DIR)

# Installation paths
PREFIX          ?=
BINDIR          := $(PREFIX)/usr/bin

# Default target
.DEFAULT_GOAL   := help

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

FIFO_PATH      := /tmp/pipelam.fifo

build: build-dir download-json-h $(OUTPUT) ## Build and download all deps for the project

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'

build-dir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DEPS_DIR)
	@mkdir -p $(BUILD_MAN_DIR)

download-json-h: ## Download the json.h external single header lib
	@if [ ! -f $(BUILD_DEPS_DIR)/json.h ]; then \
		echo "Downloading json.h..."; \
		curl -L $(JSON_H_LIB_URL) -o $(BUILD_DEPS_DIR)/json.h; \
	fi

# Create object files from C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -c $< -o $@

# Create object files from test C source files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $(INCFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -c $< -o $@

# Build the pipelam executable
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INCFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -o $@ $^ $(GTK4_LIBS) $(GTK4_LAYER_SHELL_LIBS)

# Dependencies for object files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/log.h $(SRC_DIR)/message.h $(SRC_DIR)/window.h $(SRC_DIR)/config.h
$(BUILD_DIR)/log.o: $(SRC_DIR)/log.c $(SRC_DIR)/log.h $(SRC_DIR)/config.h
$(BUILD_DIR)/window.o: $(SRC_DIR)/window.c $(SRC_DIR)/window.h $(SRC_DIR)/config.h $(SRC_DIR)/log.h
$(BUILD_DIR)/message.o: $(SRC_DIR)/message.c $(SRC_DIR)/message.h $(SRC_DIR)/config.h $(SRC_DIR)/log.h
$(BUILD_DIR)/config.o: $(SRC_DIR)/config.c $(SRC_DIR)/config.h $(SRC_DIR)/log.h

# Build test runner
$(TEST_OUTPUT): $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES)) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(INCFLAGS) $(GTK4_CFLAGS) $(GTK4_LAYER_SHELL_CFLAGS) -o $@ $^ $(GTK4_LIBS) $(GTK4_LAYER_SHELL_LIBS)

clean: rm-fifo ## Remove build artifacts
	@rm -rf $(BUILD_DIR)

build-test: build-dir $(TEST_OUTPUT) ## Build the test suite

rebuild: clean build ## Clean and build the project

format: ## Format the code using clang-format
	@clang-format -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h $(TEST_DIR)/*.c

test: rebuild build-test ## Rebuild the project and run tests
	./$(TEST_OUTPUT)
	@if [ $$? -eq 0 ]; then \
		echo -e "\n=== All tests completed successfully! ==="; \
	else \
		echo -e "\n=== Tests failed! ==="; \
		exit 1; \
	fi

fifo:
	@mkfifo $(FIFO_PATH)

rm-fifo:
	@rm -f $(FIFO_PATH)

debug: ## Build with enhanced debugging symbols for GDB
	$(MAKE) CFLAGS="-ggdb $(CFLAGS)" INCFLAGS="$(INCFLAGS)" build

derun: debug fifo ## Run the project in debug mode
	./$(OUTPUT) /tmp/pipelam.fifo

run: build fifo ## Run the project
	./$(OUTPUT) /tmp/pipelam.fifo

release: test format ## Create a release NOTE: VERSION is required. Usage: make release VERSION=X.Y.Z
	@echo "Creating release..."
	@if [ -z "$(VERSION)" ]; then \
		echo "Error: VERSION is required. Usage: make release VERSION=X.Y.Z"; \
		exit 1; \
	fi
	@./scripts/create-release.sh $(VERSION)

docs: build-dir ## Generate man pages from scdoc
	@scdoc < man/pipelam.1.scd > $(BUILD_DOC_DIR)/pipelam.1
	@scdoc < man/pipelam.toml.5.scd > $(BUILD_DOC_DIR)/pipelam.toml.5

install: ## Install pipelam to the system
	@install -d $(BINDIR)
	@install -m 755 $(OUTPUT) $(BINDIR)/pipelam
	@install -d $(PREFIX)/etc/pipelam
	@install -m 644 config/pipelam.toml $(PREFIX)/etc/pipelam/pipelam.toml
	@if [ -f build/man/pipelam.1 ]; then \
		install -d $(PREFIX)/usr/share/man/man1; \
		install -m 644 build/man/pipelam.1 $(PREFIX)/usr/share/man/man1/pipelam.1; \
	fi
	@if [ -f build/man/pipelam.toml.5 ]; then \
		install -d $(PREFIX)/usr/share/man/man5; \
		install -m 644 build/man/pipelam.toml.5 $(PREFIX)/usr/share/man/man5/pipelam.toml.5; \
	fi

install-systemd: ## Install systemd (user) service and socket files
	@install -d $(PREFIX)/usr/lib/systemd/user
	@install -m 644 systemd/pipelam.service $(PREFIX)/usr/lib/systemd/user/pipelam.service
	@install -m 644 systemd/pipelam.socket $(PREFIX)/usr/lib/systemd/user/pipelam.socket

uninstall: ## Uninstall pipelam from the system
	@rm -f $(BINDIR)/pipelam
	@rm -rf $(PREFIX)/etc/pipelam
	@rm -f $(PREFIX)/usr/share/man/man1/pipelam.1
	@rm -f $(PREFIX)/usr/share/man/man5/pipelam.toml.5
	@rm -f $(PREFIX)/usr/lib/systemd/user/pipelam.service
	@rm -f $(PREFIX)/usr/lib/systemd/user/pipelam.socket
