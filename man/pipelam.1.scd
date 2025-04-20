pipelam(1)

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

## Bar Styling Options

*--box-height*=_PIXELS_
	Set box height in pixels. Default is 25.

*--box-width*=_PIXELS_
	Set box width in pixels. Default is 350.

*--border-color*=_COLOR_
	Set border color (CSS color string or hex value). Default is "white".

*--background-color*=_COLOR_
	Set background color (CSS color string or hex value). Default is "black".

*--foreground-color*=_COLOR_
	Set foreground color for normal range (0-100%). Default is "white".

*--overflow-color*=_COLOR_
	Set foreground color for overflow range (101-200%). Default is "red".

*--box-color*=_COLOR_
	Set box color (CSS color string or hex value). Default is "black".

*--box-padding*=_PIXELS_
	Set box padding in pixels. Default is 0.

*--border-padding*=_PIXELS_
	Set border padding in pixels. Default is 4.

*--border-margin*=_PIXELS_
	Set border margin in pixels. Default is 4.

*--background-padding*=_PIXELS_
	Set background padding in pixels. Default is 4.

*--foreground-padding*=_PIXELS_
	Set foreground padding in pixels. Default is 4.

*--foreground-overflow-padding*=_PIXELS_
	Set foreground overflow padding in pixels. Default is 4.

See the visual layout of the bar components in the *pipelam.toml(5)* man page.

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

Send a WOB progress bar showing 75%:

```
echo 75 > /tmp/pipelam.fifo
```

Or with JSON format:

```
jq -n '{type: "wob", expression: "75"}' -c > /tmp/pipelam.fifo
```

Customizing WOB bar appearance:

```
pipelam --wob-bar-width=400 --wob-foreground-color="green" --wob-overflow-color="orange" /tmp/pipelam.fifo &
```

# JSON PAYLOAD FORMAT

Messages sent to pipelam can be formatted as JSON for advanced control. The basic format is:

```
{
    "type": "text|image|wob",
    "expression": "content",
    "settings": {
        "window_timeout": 800,
        "anchor": "center|top-left|top-right|bottom-left|bottom-right",
        "margin_left": 100,
        "margin_right": 0,
        "margin_top": 100,
        "margin_bottom": 0
    }
}
```

For WOB type, the expression should be a numeric value between 0-200 representing percentage.

Additional WOB-specific settings can be included in the settings object:

```
{
    "type": "wob",
    "expression": "75",
    "settings": {
        "bar_width": 350,
        "bar_height": 25,
        "border_color": "white",
        "background_color": "black",
        "foreground_color": "white",
        "overflow_color": "red",
        "box_color": "black",
        "box_padding": 0,
        "border_padding": 4,
        "border_margin": 4,
        "background_padding": 4,
        "foreground_padding": 4,
        "foreground_overflow_padding": 4
    }
}
```

The "text" type supports Pango markup for rich text formatting.

# ENVIRONMENT VARIABLES

The following environment variables can be used to override settings:

*PIPELAM_LOG_LEVEL*
	Override the log level setting

*PIPELAM_RUNTIME_BEHAVIOUR*
	Override the runtime behaviour setting

*PIPELAM_WINDOW_TIMEOUT*
	Override the window timeout setting

*PIPELAM_ANCHOR*
	Override the anchor setting

*PIPELAM_MARGIN_LEFT*
	Override the margin_left setting

*PIPELAM_MARGIN_RIGHT*
	Override the margin_right setting

*PIPELAM_MARGIN_TOP*
	Override the margin_top setting

*PIPELAM_MARGIN_BOTTOM*
	Override the margin_bottom setting

*PIPELAM_BOX_HEIGHT*
	Override the box_height setting

*PIPELAM_BOX_WIDTH*
	Override the box_width setting

*PIPELAM_BORDER_COLOR*
	Override the border_color setting

*PIPELAM_BACKGROUND_COLOR*
	Override the background_color setting

*PIPELAM_FOREGROUND_COLOR*
	Override the foreground_color setting

*PIPELAM_OVERFLOW_COLOR*
	Override the overflow_color setting

*PIPELAM_BOX_COLOR*
	Override the box_color setting

*PIPELAM_BOX_PADDING*
	Override the box_padding setting

*PIPELAM_BORDER_PADDING*
	Override the border_padding setting

*PIPELAM_BORDER_MARGIN*
	Override the border_margin setting

*PIPELAM_BACKGROUND_PADDING*
	Override the background_padding setting

*PIPELAM_FOREGROUND_PADDING*
	Override the foreground_padding setting

*PIPELAM_FOREGROUND_OVERFLOW_PADDING*
	Override the foreground_overflow_padding setting

*PIPELAM_CONFIG_FILE_PATH*
	Use a configuration file at a custom location

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
