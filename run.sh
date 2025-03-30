#!/bin/sh

# Default values
FIFO_PATH="/tmp/pipelam.fifo"
REBUILD=0
LOG_LEVEL="INFO"

while [ $# -gt 0 ]; do
    case "$1" in
        -r|--rebuild)
            REBUILD=1
            ;;
        -d|--debug)
            # Take the next argument as the log level
            if [ -n "$2" ]; then
                # Convert to uppercase
                LOG_LEVEL=$(echo "$2" | tr '[:lower:]' '[:upper:]')
                shift
            else
                echo "Error: Log level not provided after -d/--debug flag"
                exit 1
            fi
            ;;
        -h|--help)
            echo "Usage: $0 [path/to/fifo] [-r|--rebuild] [-d|--debug LEVEL] [-h|--help]"
            echo "  path/to/fifo     - Path to the FIFO pipe (default: /tmp/pipelam.fifo)"
            echo "  -r, --rebuild    - Force full rebuild"
            echo "  -d, --debug LEVEL - Set log level (e.g., info, debug, error)"
            echo "  -h, --help       - Show this help message"
            exit 0
            ;;
        *)
            # If it's not a flag, treat as FIFO path
            FIFO_PATH="$1"
            ;;
    esac
    shift

done

# Create default FIFO if it doesn't exist
if [ ! -p "$FIFO_PATH" ]; then
    echo "FIFO at $FIFO_PATH does not exist, creating it now..."
    mkfifo "$FIFO_PATH" || { echo "Failed to create FIFO at $FIFO_PATH"; exit 1; }
fi

# Build and run
if [ "$REBUILD" -eq 1 ]; then
    echo "Performing full rebuild..."
    bear -- make -B && PIPELAM_LOG_LEVEL="$LOG_LEVEL" ./build/pipelam "$FIFO_PATH"
else
    bear -- make && PIPELAM_LOG_LEVEL="$LOG_LEVEL" ./build/pipelam "$FIFO_PATH"
fi

