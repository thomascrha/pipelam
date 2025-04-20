#!/bin/bash

# A simple demonstration script for pipelam
# Displays a stylized message with Pango markup

# Define colors
BG_COLOR="#282c34"
TEXT_COLOR="#abb2bf"
HIGHLIGHT_COLOR="#61afef"
ACCENT_COLOR="#c678dd"

PIPELAM_FIFO="/tmp/pipelam.fifo"

# Create JSON payload and validate with jq before sending
JSON_PAYLOAD=$(cat << EOF
{
  "type": "text",
  "expression": "<span font_desc='Sans Bold 20' color='${TEXT_COLOR}'><span color='${HIGHLIGHT_COLOR}'>A lightweight</span> <span color='${ACCENT_COLOR}'>GTK4-based</span> notification system for displaying\r <span color='${HIGHLIGHT_COLOR}'>text</span>, <span color='${HIGHLIGHT_COLOR}'>images</span>, and <span color='${HIGHLIGHT_COLOR}'>progress bars</span> in <span color='${ACCENT_COLOR}'>wlroots compositors</span>.</span>",
  "settings" :{
   "window_timeout": 5000,
   "background": "${BG_COLOR}",
   "border_color": "${ACCENT_COLOR}",
   "border_size": 2,
   "padding": 20,
   "width": 500,
   "height": 100,
   "corner_radius": 10
  }
}
EOF
)

echo "$JSON_PAYLOAD" | jq -c > $PIPELAM_FIFO

