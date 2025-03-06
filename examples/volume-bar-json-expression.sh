#!/bin/bash

# read in current volume - extract just the numeric part
VOLUME=$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -o '[0-9]\+\.\?[0-9]*' | head -1)

# Check if volume is a decimal (like 0.45) and convert to percentage
if [[ $VOLUME < 1 ]]; then
    VOLUME=$(echo "scale=0; $VOLUME * 100" | bc)
fi

# echo "Volume: $VOLUME"
# ▁  ▂  ▃  ▄  ▅  ▆  ▇  █
# 0,15,30,45,60,75,90,100

# calculate the number of blocks to display
NUM_BLOCKS=$(echo "scale=0; $VOLUME / 15" | bc)
if [ $NUM_BLOCKS -gt 7 ]; then
    NUM_BLOCKS=7
fi

# echo "Blocks: $NUM_BLOCKS"

BLOCKS="▂▃▄▅▆▇█"

# build the volume bar string
VOLUME_BAR=""
for ((i=0; i<$NUM_BLOCKS+1; i++)); do
    # Use the correct character from BLOCKS string
    VOLUME_BAR="${VOLUME_BAR}${BLOCKS:i:1}"
done

# Print debug information
# echo "Volume: $VOLUME_BAR"

# Format the volume with leading zero for numbers under 10
FORMATTED_VOLUME="${VOLUME%.*}"
if [ "$FORMATTED_VOLUME" -lt 10 ]; then
    FORMATTED_VOLUME="0$FORMATTED_VOLUME"
fi
FORMATTED_VOLUME="${FORMATTED_VOLUME}%"

# Calculate number of characters needed for display (including % sign)
CHARS_NEEDED=3  # Default for 2 digits + % sign
if [ "${VOLUME%.*}" -eq 100 ]; then
    CHARS_NEEDED=4  # Special case for "100%"
fi

# Create padding only if we have more blocks than characters needed
LEFT_PAD=""
if [ "$NUM_BLOCKS" -gt "$CHARS_NEEDED" ]; then
    # Add padding only at the front, with the difference between blocks and chars needed
    PADDING_COUNT=$((NUM_BLOCKS - CHARS_NEEDED))
    for ((i=0; i<PADDING_COUNT; i++)); do
        LEFT_PAD="${LEFT_PAD} "
    done
fi

# Create the expression with volume bar included and fully transparent background
# Display volume with proper padding
EXPRESSION="<span font_desc='JetBrains Mono 50'>${LEFT_PAD}${FORMATTED_VOLUME}</span>\r<span font_desc='JetBrains Mono 50'>${VOLUME_BAR}</span>"

# generate JSON string with emoji
JSON_STRING="{\"type\": \"text\", \"expression\": \"${EXPRESSION}\"}" #, \"settings\": {\"anchor\": \"center\"}}"

echo $JSON_STRING
