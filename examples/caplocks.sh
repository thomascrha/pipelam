#!/bin/bash

# DOES NOT WORK CURRENTLY
# check cat /sys/class/leds/input*::capslock/brightness every second and translate the vlaue 0 or 1 to on (1) off (0) and the print that out eery second

while true; do
    # read the value of the capslock led
    value=$(cat /sys/class/leds/input*::capslock/brightness)

    # translate the value to on or off
    if [ "$value" -eq 1 ]; then
        EXPRESSION="<span font_desc='Roboto 100'>Capslock is ON 🎶</span>"

        # generate JSON string with emoji
        JSON_STRING="{\"type\": \"text\", \"expression\": \"${EXPRESSION}\", \"settings\": {\"anchor\": \"top-right\", \"window_timeout\": 1007}}"

        echo $JSON_STRING
    fi

    # wait for 1 second
    sleep 1
done
