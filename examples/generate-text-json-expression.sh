#!/bin/bash

# read in current volume
VOLUME=$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/0[^0-9]//g')

EXPRESSION="<span font_desc='Roboto 100'>${VOLUME} 🎶</span>"

# generate JSON string with emoji
JSON_STRING="{\"type\": \"text\", \"expression\": \"${EXPRESSION}\"}" #, \"settings\": {\"anchor\": \"center\"}}"

echo $JSON_STRING
