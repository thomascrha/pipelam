pipelam(1) "April 2025" "User Commands"

# NAME

pipelam - lightweight overlay notification system for Wayland compositors

# SYNOPSIS

*pipelam* [_OPTIONS_] _<pipe_path>_

# DESCRIPTION

*pipelam* is a lightweight overlay notification system for wlroots-based Wayland compositors, designed to display text and image overlays. It is typically used to show volume levels, brightness indicators, and other status information through a named pipe.

Similar to wob (Wayland Overlay Bar), but built using GTK4 and gtk4-layer-shell instead of raw Wayland protocols.

# OPTIONS

*-l, --log-level*=_LEVEL_
	Set log level (DEBUG, INFO, WARNING, ERROR, PANIC). Default is INFO.

*-r, --runtime-behaviour*=_TYPE_
	Set runtime behaviour (queue, replace, overlay). Default is replace.

*-t, --window-timeout*=_MS_
	Set window timeout in milliseconds. Default is 600.

*-a, --anchor*=_POS_
	Set window anchor position (bottom-left, bottom-right, top-left, top-right, center). Default is center.

*-L, --margin-left*=_PIXELS_
	Set left margin in pixels. Default is 100.

*-R, --margin-right*=_PIXELS_
	Set right margin in pixels. Default is 0.

*-T, --margin-top*=_PIXELS_
	Set top margin in pixels. Default is 100.

*-B, --margin-bottom*=_PIXELS_
	Set bottom margin in pixels. Default is 0.

*-v, --version*
	Display version information.

*-h, --help*
	Display help message.

# EXAMPLES

Basic usage:

```
mkfifo /tmp/pipelam.fifo
pipelam /tmp/pipelam.fifo &
echo "Hello, World!" > /tmp/pipelam.fifo
```

Display a notification at the top-right of the screen:

```
pipelam --anchor=top-right --margin-top=50 --margin-right=50 /tmp/pipelam.fifo &
```

Send a JSON-formatted message:

```
jq -n --arg text "Hello, World" '{type: "text", expression: $text}' -c > /tmp/pipelam.fifo
```

# JSON PAYLOAD FORMAT

Messages sent to pipelam can be formatted as JSON for advanced control:

```
{
  "type": "text",              # "text" or "image"
  "expression": "Hello World", # Text or image path
  "settings": {
    "window_timeout": 600,     # Milliseconds
    "anchor": "center",        # Position anchor
    "margin_left": 100,        # Margin in pixels
    "margin_right": 0,
    "margin_top": 100,
    "margin_bottom": 0
  }
}
```

For text overlays, the expression field supports Pango markup.

# ENVIRONMENT

*PIPELAM_LOG_LEVEL*
	Set log level (DEBUG, INFO, WARNING, ERROR, PANIC)

*PIPELAM_RUNTIME_BEHAVIOUR*
	Set runtime behaviour (queue, replace, overlay)

*PIPELAM_WINDOW_TIMEOUT*
	Set window timeout in milliseconds

*PIPELAM_ANCHOR*
	Set window anchor position (BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT, CENTER)

*PIPELAM_MARGIN_LEFT*
	Set left margin in pixels

*PIPELAM_MARGIN_RIGHT*
	Set right margin in pixels

*PIPELAM_MARGIN_TOP*
	Set top margin in pixels

*PIPELAM_MARGIN_BOTTOM*
	Set bottom margin in pixels

*PIPELAM_CONFIG_FILE_PATH*
	Override the default configuration file path

# FILES

_/etc/pipelam/pipelam.toml_
	System-wide configuration file

_$HOME/.config/pipelam/pipelam.toml_
	User configuration file

# SEE ALSO

pipelam.toml(5)

# BUGS

Report bugs to: https://github.com/thomascrha/pipelam/issues

# AUTHOR

thomascrha
