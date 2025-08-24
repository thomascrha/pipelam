#!/bin/bash

PIPELAM_FIFO_DIR="/tmp/pipelam_demo"

start_pipelams() {
    mkdir -p "$PIPELAM_FIFO_DIR"
    local pids=()
    for mode in "overlay" "queue" "replace"; do
        echo "Starting pipelam in $mode mode..."
        pipelam --runtime-behaviour="$mode" --log-level=DEBUG "$PIPELAM_FIFO_DIR/pipelam-$mode.fifo" &
        pids+=($!)
    done
    echo ${#pids[@]}
}

pids=$(start_pipelams)

# stop_pipelams() {
#     local pids=$1
#     for pid in $pids; do
#         kill $pid
#     done
#     rm -rf "$PIPELAM_FIFO_DIR"
# }
#
# trap 'stop_pipelams "$pids"' EXIT
#
#
# # Define colors
# BG_COLOR="#282c34"
# TEXT_COLOR="#abb2bf"
# HIGHLIGHT_COLOR="#61afef"
# ACCENT_COLOR="#c678dd"
#
# PIPELAM_FIFO="//1000/pipelam.fifo"
#
# # --- Text Demo ---
# JSON_PAYLOAD_TEXT=$(cat << EOF
# {
#   "type": "text",
#   "expression": "<span font_desc='Sans Bold 20' color='${TEXT_COLOR}'><span color='${HIGHLIGHT_COLOR}'>A lightweight</span> <span color='${ACCENT_COLOR}'>GTK4-based</span> notification system for displaying\r <span color='${HIGHLIGHT_COLOR}'>text</span>, <span color='${HIGHLIGHT_COLOR}'>images</span>, and <span color='${HIGHLIGHT_COLOR}'>progress bars</span> in <span color='${ACCENT_COLOR}'>wlroots compositors</span>.</span>",
#   "settings" :{
#    "window_timeout": 5000,
#    "background": "${BG_COLOR}",
#    "border_color": "${ACCENT_COLOR}",
#    "border_size": 2,
#    "padding": 20,
#    "width": 500,
#    "height": 100
#   }
# }
# EOF
# )
# echo "$JSON_PAYLOAD_TEXT" | jq -c > "$PIPELAM_FIFO"
# sleep 6
#
# # --- WOB Demos ---
# JSON_PAYLOAD_TEXT=$(cat << EOF
# {
#   "type": "text",
#   "expression": "<span font_desc='Sans Bold 20' color='${TEXT_COLOR}'>Now, let's demonstrate the <span color='${HIGHLIGHT_COLOR}'>WOB</span> (Wayland Overlay Bar) mode.</span>",
#   "settings" :{
#    "window_timeout": 5000,
#    "background": "${BG_COLOR}",
#    "border_color": "${ACCENT_COLOR}",
#    "border_size": 2,
#    "padding": 20,
#    "width": 500,
#    "height": 100,
#    "runtime_behaviour": "overlay"
#   }
# }
# EOF
# )
# echo "$JSON_PAYLOAD_TEXT" | jq -c > "$PIPELAM_FIFO"
# sleep 2
#
# echo "1. Sending a simple percentage (75%)..."
# # echo 75 > "$PIPELAM_FIFO"
# echo '{"type": "wob", "expression": "75", "settings": {"runtime_behaviour": "overlay"}}' | jq -c > "$PIPELAM_FIFO"
# #
# # echo "2. Animating the progress bar from 0 to 100%..."
# # for i in $(seq 0 10 100); do
# #   echo "$i" > "$PIPELAM_FIFO"
# #   sleep 0.1
# # done
# # sleep 3
# #
# # echo "3. Showing an 'overflow' value at 150%..."
# # echo 150 > "$PIPELAM_FIFO"
# # sleep 3
# #
# # echo "4. Displaying a fully customized WOB bar via JSON..."
# # JSON_PAYLOAD_WOB=$(cat << EOF
# # {
# #   "type": "wob",
# #   "expression": "85",
# #   "settings": {
# #     "anchor": "bottom-right",
# #     "margin_bottom": 50,
# #     "margin_right": 50,
# #     "window_timeout": 5000,
# #     "wob_bar_width": 400,
# #     "wob_bar_height": 30,
# #     "wob_border_color": "${HIGHLIGHT_COLOR}",
# #     "wob_background_color": "${BG_COLOR}",
# #     "wob_foreground_color": "${ACCENT_COLOR}",
# #     "wob_overflow_color": "#e06c75",
# #     "wob_border_padding": 5,
# #     "wob_border_margin": 5
# #   }
# # }
# # EOF
# # )
# # echo "$JSON_PAYLOAD_WOB" | jq -c > "$PIPELAM_FIFO"
# # sleep 2
# #
# # echo "Demo finished!"
#
#
