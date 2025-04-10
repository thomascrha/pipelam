#!/bin/bash

# Colors for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Path to the pipelam binary - adjust if needed
PIPELAM_BIN="../build/pipelam"
PIPE_PATH="/tmp/pipelam_test_pipe"

# Test image path - adjust to point to a real image
TEST_IMAGE1="../examples/example.png"
TEST_IMAGE2="../examples/example1.png"

# Make sure to clean up on exit
cleanup() {
    echo -e "${BLUE}Cleaning up...${NC}"
    killall pipelam 2>/dev/null
    rm -f "$PIPE_PATH"
    echo -e "${GREEN}Done!${NC}"
    exit 0
}

trap cleanup EXIT INT TERM

# Create named pipe if it doesn't exist
if [ ! -e "$PIPE_PATH" ]; then
    mkfifo "$PIPE_PATH"
fi

# Function to send a message to pipelam
send_message() {
    echo "$1" > "$PIPE_PATH"
    echo -e "${BLUE}Sent: ${YELLOW}$1${NC}"
}

# Function to wrap a message in JSON with anchor point
wrap_message() {
    local message=$1
    local type=$2
    local anchor=$3

    echo "{\"expression\":\"$message\",\"type\":\"$type\",\"settings\":{\"anchor\":\"$anchor\",\"window_timeout\":3000}}"
}

# Function to run a test
run_test() {
    local mode=$1
    local test_name=$2
    local message1=$3
    local type=$4
    local message2=${5:-$message1}  # Default to first message if second not provided

    json_message1=$(wrap_message "$message1" "$type" "top-left")
    json_message2=$(wrap_message "$message2" "$type" "bottom-right")

    echo -e "\n${GREEN}==========================================${NC}"
    echo -e "${GREEN}Testing: ${YELLOW}$test_name${NC} in ${YELLOW}$mode${NC} mode"
    echo -e "${GREEN}==========================================${NC}"

    echo -e "${BLUE}Starting pipelam in $mode mode...${NC}"
    $PIPELAM_BIN --runtime-behaviour="$mode" --log-level=DEBUG "$PIPE_PATH" &

    # Give pipelam time to start
    sleep 1

    # Send first message (top-left)
    echo -e "${BLUE}Sending message 1 (top-left):${NC}"
    send_message "$json_message1"

    # Wait 1 second between messages
    sleep 1

    # Send second message (bottom-right)
    echo -e "${BLUE}Sending message 2 (bottom-right):${NC}"
    send_message "$json_message2"

    echo -n "Press Enter to continue to the next test..."
    read

    # Kill the current pipelam instance
    killall pipelam 2>/dev/null
    sleep 1
}

# check the current working directory ends in tests
if [[ ! $PWD =~ tests$ ]]; then
    echo -e "${YELLOW}Please run this script from the tests directory.${NC}"
    exit 1
fi

# rebuild pipelam to make sure we are testing the latest version
cd ../
make rebuild
cd tests

# Start testing each mode with each message type
echo -e "${GREEN}Pipelam Test Suite${NC}"
echo -e "${YELLOW}Press Enter to start testing...${NC}"
read

# Test modes
MODES=("queue" "replace" "overlay")

for mode in "${MODES[@]}"; do
    # Test TEXT messages
    run_test "$mode" "Markup text" "<span font_desc='Roboto 100' foreground='red'>Red text test</span>" "text" "<span font_desc='Roboto 100' foreground='blue'>Blue text test</span>"

    # Test WOB messages with different values
    run_test "$mode" "WOB values" "25" "wob" "75"

    run_test "$mode" "Image display" "$TEST_IMAGE1" "image" "$TEST_IMAGE2"
done

echo -e "\n${GREEN}All tests completed!${NC}"
# cleanup

