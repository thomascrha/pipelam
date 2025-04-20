#!/bin/bash
# Adapted from gist.githubusercontent.com/monkeywithacupcake/9e0092733302668b2a4adbbfb1d35748/raw/5ecc5ee613300b64de351977622b99e28c37f586/catchphrase.sh

# set -xe
BACKGROUND_COLOUR=LightGray
TEXT_COLOUR=aquamarine4
FONT=JetBrains-Mono-ExtraBold
OVERALL_SIZE=1000
TEXT_SIZE=800

echo "What is your catchphrase?"
read THE_PHRASE

SHORT_PHRASE=$(echo $THE_PHRASE | cut -c 1-20 | tr ' ' '_')

convert -size ${OVERALL_SIZE}x${OVERALL_SIZE} xc:$BACKGROUND_COLOUR \
 +repage -font $FONT -size ${TEXT_SIZE}x${TEXT_SIZE} -fill $TEXT_COLOUR -background None \
    -gravity center caption:"$THE_PHRASE" \
 +repage -gravity Center -composite -strip $SHORT_PHRASE.jpg

xdg-open ${PWD}/$SHORT_PHRASE.jpg
