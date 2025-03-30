#!/bin/bash

# Set strict error handling
set -e

echo "=== Rebuilding project ==="
bear -- make rebuild

echo -e "\n=== Building and running tests ==="
bear -- make test

# Check the exit status of the tests
if [ $? -eq 0 ]; then
    echo -e "\n=== All tests completed successfully! ==="
    exit 0
else
    echo -e "\n=== Tests failed! ==="
    exit 1
fi

