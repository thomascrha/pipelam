#!/bin/bash

LEVEL=$1

LOW_BRIGHTNESS= # two bars
MEDIUM_BRIGHTNESS= # three bars
HIGH_BRIGHTNESS= # four bars

# if the LEVEL is less than 33 then show LOW_BRIGHTNESS
# if the LEVEL is less than 66 then show MEDIUM_BRIGHTNESS
# if the LEVEL is greater than 66 then show HIGH_BRIGHTNESS

if [[ $LEVEL -lt 33 ]]; then
	echo $LOW_BRIGHTNESS
elif [[ $LEVEL -lt 66 ]]; then
	echo $MEDIUM_BRIGHTNESS
else
	echo $HIGH_BRIGHTNESS
fi
