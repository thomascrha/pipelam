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

## WOB Bar Options

*--wob-bar-height*=_PIXELS_
	Set WOB bar height in pixels. Default is 40.

*--wob-bar-width*=_PIXELS_
	Set WOB bar width in pixels. Default is 500.

*--wob-border-color*=_COLOR_
	Set WOB border color (CSS color string or hex value). Default is "white".

*--wob-background-color*=_COLOR_
	Set WOB background color (CSS color string or hex value). Default is "black".

*--wob-foreground-color*=_COLOR_
	Set WOB foreground color for normal range (0-100%). Default is "white".

*--wob-overflow-color*=_COLOR_
	Set WOB foreground color for overflow range (101-200%). Default is "red".

*--wob-box-color*=_COLOR_
	Set WOB box color (CSS color string or hex value). Default is "black".

*--wob-box-padding*=_PIXELS_
	Set WOB box padding in pixels. Default is 0.

*--wob-border-padding*=_PIXELS_
	Set WOB border padding in pixels. Default is 4.

*--wob-border-margin*=_PIXELS_
	Set WOB border margin in pixels. Default is 4.

*--wob-background-padding*=_PIXELS_
	Set WOB background padding in pixels. Default is 4.

*--wob-foreground-padding*=_PIXELS_
	Set WOB foreground padding in pixels. Default is 4.

*--wob-foreground-overflow-padding*=_PIXELS_
	Set WOB foreground overflow padding in pixels. Default is 4.

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
        "wob_bar_width": 350,
        "wob_bar_height": 25,
        "wob_border_color": "white",
        "wob_background_color": "black",
        "wob_foreground_color": "white",
        "wob_overflow_color": "red",
        "wob_box_color": "black",
        "wob_box_padding": 0,
        "wob_border_padding": 4,
        "wob_border_margin": 4,
        "wob_background_padding": 4,
        "wob_foreground_padding": 4,
        "wob_foreground_overflow_padding": 4
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

*PIPELAM_WOB_BAR_HEIGHT*
	Override the wob_bar_height setting

*PIPELAM_WOB_BAR_WIDTH*
	Override the wob_bar_width setting

*PIPELAM_WOB_BORDER_COLOR*
	Override the wob_border_color setting

*PIPELAM_WOB_BACKGROUND_COLOR*
	Override the wob_background_color setting

*PIPELAM_WOB_FOREGROUND_COLOR*
	Override the wob_foreground_color setting

*PIPELAM_WOB_OVERFLOW_COLOR*
	Override the wob_overflow_color setting

*PIPELAM_WOB_BOX_COLOR*
	Override the wob_box_color setting

*PIPELAM_WOB_BOX_PADDING*
	Override the wob_box_padding setting

*PIPELAM_WOB_BORDER_PADDING*
	Override the wob_border_padding setting

*PIPELAM_WOB_BORDER_MARGIN*
	Override the wob_border_margin setting

*PIPELAM_WOB_BACKGROUND_PADDING*
	Override the wob_background_padding setting

*PIPELAM_WOB_FOREGROUND_PADDING*
	Override the wob_foreground_padding setting

*PIPELAM_WOB_FOREGROUND_OVERFLOW_PADDING*
	Override the wob_foreground_overflow_padding setting

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
