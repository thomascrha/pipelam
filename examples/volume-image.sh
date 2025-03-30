#!/bin/bash

VOLUME=$(echo "scale=0; $(wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -o '[0-9]\+\.\?[0-9]*' | head -1) * 100" | bc | awk '{print int($1)}')

# if up or down is passed as argument, adjust volume accordingly
if [ "$1" == "up" ]; then
	wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+
elif [ "$1" == "down" ]; then
	wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-
elif [ "$1" == "mute" ]; then
	wpctl set-volume @DEFAULT_AUDIO_SINK@ 0
fi

if [ "$VOLUME" -le 0 ]; then
	IMAGE_PATH="/home/tcrha/Projects/pipelam/examples/volume-0.png"
elif [ "$VOLUME" -le 40 ]; then
	IMAGE_PATH="/home/tcrha/Projects/pipelam/examples/volume-30.png"
elif [ "$VOLUME" -le 80 ]; then
	IMAGE_PATH="/home/tcrha/Projects/pipelam/examples/volume-70.png"
else
	IMAGE_PATH="/home/tcrha/Projects/pipelam/examples/volume-100.png"
fi

JSON_STRING="{\"type\": \"image\", \"expression\": \"${IMAGE_PATH}\", \"settings\": {\"anchor\": \"top-left\"}}"

echo $JSON_STRING
