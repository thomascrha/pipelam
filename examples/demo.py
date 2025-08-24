#!/usr/bin/env python3

import pathlib
import subprocess
import os
import time
import json

PIPELAM_DEMO_DIR = pathlib.Path("/tmp/pipelam_demo")
BG_COLOR="#282c34"
TEXT_COLOR="#abb2bf"
HIGHLIGHT_COLOR="#61afef"
ACCENT_COLOR="#c678dd"

def start_pipelams():
    PIPELAM_DEMO_DIR.mkdir(parents=True, exist_ok=True)
    pids = []

    for mode in ["overlay", "queue", "replace"]:
        fifo_path = PIPELAM_DEMO_DIR / f"pipelam-{mode}.fifo"
        os.mkfifo(fifo_path, mode=0o600)

        process = subprocess.Popen(["pipelam", f"--runtime-behaviour={mode}", "--log-level=DEBUG", fifo_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        pids.append(process.pid)

    return pids

def stop_pipelams(pids):
    for pid in pids:
        os.kill(pid, 9)

    os.system("rm -rf /tmp/pipelam_demo")

def send_message(mode, message):
    fifo_path = PIPELAM_DEMO_DIR / f"pipelam-{mode}.fifo"
    with open(fifo_path, 'w') as fifo:
        fifo.write(message)
        fifo.flush()

pipelam_pids = start_pipelams()
window_timeout = 5000
message: dict = {
    "type": "text",
    "expression": f"<span font_desc='Sans Bold 20' color='{TEXT_COLOR}'><span color='{HIGHLIGHT_COLOR}'>Pipelam - A lightweight</span> <span color='{ACCENT_COLOR}'>GTK4-based</span> notification system for displaying\r    <span color='{HIGHLIGHT_COLOR}'>text</span>, <span color='{HIGHLIGHT_COLOR}'>images</span>, and <span color='{HIGHLIGHT_COLOR}'>progress bars</span> in <span color='{ACCENT_COLOR}'>wlroots compositors</span>.</span>",
    "settings": {
        "window_timeout": window_timeout,
        "background": BG_COLOR,
        "border_color": ACCENT_COLOR,
        "border_size": 2,
        "padding": 20,
        "width": 500,
        "height": 100
    }
}
send_message("queue", json.dumps(message))
time.sleep(window_timeout / 1000)


# display a message in overlay mode saying we are going to demo wob and give a brief description
window_timeout = 5000
message = {
    "type": "text",
    "expression": f"<span font_desc='Sans Bold 20' color='{TEXT_COLOR}'><span color='{HIGHLIGHT_COLOR}'>Wob</span> mode is omage to the project Wob. It can display a progress bar in a <span color='{ACCENT_COLOR}'>non-intrusive</span> way. For example the value 75%:</span>",
    "settings": {
        "window_timeout": window_timeout,
        "background": BG_COLOR,
        "border_color": ACCENT_COLOR,
        "border_size": 2,
        "padding": 20,
        "width": 500,
        "height": 100
    }
}
send_message("overlay", json.dumps(message))
time.sleep(window_timeout / 1000 / 2)

# display a progress bar in overlay mode with wob 75% filled
window_timeout = 2000
message = {
    "type": "wob",
    "expression": "75",
    "settings": {
        "window_timeout": window_timeout,
        "background": BG_COLOR,
        "border_color": ACCENT_COLOR,
        "border_size": 2,
        "padding": 20,
        "width": 500,
        "height": 100,
        "anchor": "top-right",
        "margin_top": 620,
        "margin_right": 900,

    }
}

send_message("overlay", json.dumps(message))
time.sleep(window_timeout / 1000)

time.sleep(0.5)

# show and overflow wob progress bar in overlay mode with a message fiest saying 150%
window_timeout = 3000
message = {
    "type": "text",
    "expression": f"<span font_desc='Sans Bold 20' color='{TEXT_COLOR}'><span color='{HIGHLIGHT_COLOR}'>Wob</span> mode can also display overflow values. For example 150%</span>",
    "settings": {
        "window_timeout": window_timeout,
        "background": BG_COLOR,
        "border_color": ACCENT_COLOR,
        "border_size": 2,
        "padding": 20,
        "width": 500,
        "height": 100
    }
}
send_message("overlay", json.dumps(message))
time.sleep(window_timeout / 1000 / 2)

window_timeout = 1500
message = {
    "type": "wob",
    "expression": "150",
    "settings": {
        "window_timeout": window_timeout,
        "background": BG_COLOR,
        "border_color": ACCENT_COLOR,
        "border_size": 2,
        "padding": 20,
        "width": 500,
        "height": 100,
        "anchor": "top-right",
        "margin_top": 620,
        "margin_right": 900,
    }
}
send_message("overlay", json.dumps(message))
time.sleep(window_timeout / 1000)

stop_pipelams(pipelam_pids)
