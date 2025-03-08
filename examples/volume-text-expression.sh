#!/bin/bash

# read in current volume
VOLUME=$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -o '[0-9]\+\.\?[0-9]*' | head -1)

# Check if volume is a decimal (like 0.45) and convert to percentage
if [[ $VOLUME < 1 ]]; then
    VOLUME=$(echo "scale=0; $VOLUME * 100" | bc)
fi

EXPRESSION="<span font_desc='Roboto 100'>Hello Sylvia 🎶</span>"

# generate JSON string with emoji
JSON_STRING="{\"type\": \"text\", \"expression\": \"${EXPRESSION}\", \"settings\": {\"anchor\": \"top-left\", \"window_timeout\": 15000}}"

echo $JSON_STRING
