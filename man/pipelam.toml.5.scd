pipelam.toml(5)

# NAME

pipelam.toml - configuration file for pipelam

# SYNOPSIS

*/etc/pipelam/pipelam.toml*
*$HOME/.config/pipelam/pipelam.toml*

# DESCRIPTION

*pipelam.toml* is the configuration file for *pipelam*(1), a lightweight overlay notification system for Wayland compositors.

The configuration file uses TOML format. Options specified in the user configuration file ($HOME/.config/pipelam/pipelam.toml) override those in the system configuration file (/etc/pipelam/pipelam.toml). Both configuration files are optional; if they do not exist, default values will be used.

Note: Configuration ordering is as follows (highest precedence first):
1. Environment variables (starting with *PIPELAM_*)
2. Command line options
3. User configuration file ($HOME/.config/pipelam/pipelam.toml)
4. System configuration file (/etc/pipelam/pipelam.toml)

# CONFIG OPTIONS

## Basic Settings

*log_level = INFO*
	The logging verbosity level of the application. Valid values include:
	*DEBUG* - Log all messages including detailed debug information
	*INFO* - Log informational messages, warnings, and errors (default)
	*WARNING* - Log warnings and errors only
	*ERROR* - Log errors only
	*PANIC* - Log only critical errors that cause program termination

*runtime_behaviour = replace*
	Controls how multiple notifications are handled:
	*queue* - Windows are queued and displayed one after another
	*replace* - New windows replace existing ones (default)
	*overlay* - Windows are displayed simultaneously, overlaid on top of each other

## Window Settings

*window_timeout = 600*
	The time in milliseconds before the overlay is automatically hidden (default: 600).

*anchor = center*
	The anchor point of the overlay on the screen. Valid values include:
	*center* - Center of the screen (default)
	*top-left* - Top left corner
	*top-right* - Top right corner
	*bottom-left* - Bottom left corner
	*bottom-right* - Bottom right corner

*margin_left = 100*
	The margin in pixels from the left edge of the screen (default: 100).

*margin_right = 0*
	The margin in pixels from the right edge of the screen (default: 0).

*margin_top = 100*
	The margin in pixels from the top edge of the screen (default: 100).

*margin_bottom = 0*
	The margin in pixels from the bottom edge of the screen (default: 0).

## WOB Bar Settings

*wob_bar_width = 500*
	The width of the WOB progress bar in pixels (default: 500).

*wob_bar_height = 40*
	The height of the WOB progress bar in pixels (default: 40).

*wob_border_color = white*
	The color of the WOB border (CSS color string or hex value, default: white).

*wob_background_color = black*
	The background color of the WOB bar (CSS color string or hex value, default: black).

*wob_foreground_color = white*
	The foreground color for normal range (0-100%) of the WOB bar (CSS color string or hex value, default: white).

*wob_overflow_color = red*
	The foreground color for overflow range (101-200%) of the WOB bar (CSS color string or hex value, default: red).

*wob_box_color = black*
	The color of the box containing the WOB bar (CSS color string or hex value, default: black).

*wob_box_padding = 0*
	The padding of the box containing the WOB bar in pixels (default: 0).

*wob_border_padding = 4*
	The padding of the border of the WOB bar in pixels (default: 4).

*wob_border_margin = 4*
	The margin of the border of the WOB bar in pixels (default: 4).

*wob_background_padding = 4*
	The padding of the background of the WOB bar in pixels (default: 4).

*wob_foreground_padding = 4*
	The padding of the foreground of the WOB bar in pixels (default: 4).

*wob_foreground_overflow_padding = 4*
	The padding of the foreground overflow section of the WOB bar in pixels (default: 4).

# EXAMPLE

A full configuration file example:

```
# pipelam configuration file

# Basic Settings
# Logging level: DEBUG, INFO, WARNING, ERROR, PANIC
log_level = INFO

# Runtime behavior: queue, replace, overlay
runtime_behaviour = replace

# Window Settings
# Window display duration in milliseconds
window_timeout = 800

# Positioning
anchor = bottom-right
margin_left = 0
margin_right = 20
margin_top = 0
margin_bottom = 20

# WOB Bar Settings
wob_bar_width = 350
wob_bar_height = 25
wob_border_color = "white"
wob_background_color = "black"
wob_foreground_color = "#FFFFFF"
wob_overflow_color = "red"
wob_box_color = "black"

# WOB padding settings
wob_box_padding = 0
wob_border_padding = 8
wob_border_margin = 8
wob_background_padding = 8
wob_foreground_padding = 8
wob_foreground_overflow_padding = 8
```

# ENVIRONMENT VARIABLES

The following environment variables can be used to override settings in the configuration files:

*PIPELAM_LOG_LEVEL*
	Override the *log_level* setting

*PIPELAM_RUNTIME_BEHAVIOUR*
	Override the *runtime_behaviour* setting

*PIPELAM_WINDOW_TIMEOUT*
	Override the *window_timeout* setting

*PIPELAM_ANCHOR*
	Override the *anchor* setting

*PIPELAM_MARGIN_LEFT*
	Override the *margin_left* setting

*PIPELAM_MARGIN_RIGHT*
	Override the *margin_right* setting

*PIPELAM_MARGIN_TOP*
	Override the *margin_top* setting

*PIPELAM_MARGIN_BOTTOM*
	Override the *margin_bottom* setting

*PIPELAM_WOB_BAR_HEIGHT*
	Override the *wob_bar_height* setting

*PIPELAM_WOB_BAR_WIDTH*
	Override the *wob_bar_width* setting

*PIPELAM_WOB_BORDER_COLOR*
	Override the *wob_border_color* setting

*PIPELAM_WOB_BACKGROUND_COLOR*
	Override the *wob_background_color* setting

*PIPELAM_WOB_FOREGROUND_COLOR*
	Override the *wob_foreground_color* setting

*PIPELAM_WOB_OVERFLOW_COLOR*
	Override the *wob_overflow_color* setting

*PIPELAM_WOB_BOX_COLOR*
	Override the *wob_box_color* setting

*PIPELAM_WOB_BOX_PADDING*
	Override the *wob_box_padding* setting

*PIPELAM_WOB_BORDER_PADDING*
	Override the *wob_border_padding* setting

*PIPELAM_WOB_BORDER_MARGIN*
	Override the *wob_border_margin* setting

*PIPELAM_WOB_BACKGROUND_PADDING*
	Override the *wob_background_padding* setting

*PIPELAM_WOB_FOREGROUND_PADDING*
	Override the *wob_foreground_padding* setting

*PIPELAM_WOB_FOREGROUND_OVERFLOW_PADDING*
	Override the *wob_foreground_overflow_padding* setting

*PIPELAM_CONFIG_FILE_PATH*
	Use a configuration file at a custom location

# JSON MESSAGES

When sending JSON-formatted messages to pipelam, you can include a `settings` object to override configuration for a specific notification:

```
{
  "type": "text",
  "expression": "Hello, World!",
  "settings": {
    "window_timeout": 1000,
    "anchor": "top-right"
  }
}
```

For WOB-type messages, you can specify a numeric value directly or use JSON format:

```
echo 75 > /tmp/pipelam.fifo
```

Or with full customization:

```
{
  "type": "wob",
  "expression": "75",
  "settings": {
    "wob_bar_width": 400,
    "wob_foreground_color": "green"
  }
}
```

Values from 0-100 show normal progress, while 101-200 show overflow progress with the overflow color.

# WOB BAR STYLING

The WOB progress bar consists of nested components, each with its own color and padding settings. The visual structure is illustrated below:

```
+---------------------------------------------------------+
|                      wob_box_color                      | ← wob_box_padding
|   +---------------------------------------------------+ |
|   |                 wob_border_color                  | | ← wob_border_margin
|   |   +---------------------------------------------+ | |
|   |   |            wob_background_color             | | | ← wob_border_padding
|   |   |   +---------------------------------------+ | | |
|   |   |   |          wob_foreground_color         | | | | ← wob_background_padding
|   |   |   |    ( over 100% wob_overflow_color)    | | | |
|   |   |   |                                       | | | | ← wob_foreground_padding
|   |   |   +---------------------------------------+ | | |
|   |   |                                             | | |
|   |   +---------------------------------------------+ | |
|   |                                                   | |
|   +---------------------------------------------------+ |
|                                                         |
+---------------------------------------------------------+
    ←------------------ wob_bar_width ------------------→
    ↑
    |
    wob_bar_height
```

## Component Descriptions

1. **Box Layer**: The outermost container with *wob_box_color* background
	- Controlled by *wob_box_padding*

2. **Border Layer**: The border container with *wob_border_color* background
	- Controlled by *wob_border_padding* (internal padding)
	- Controlled by *wob_border_margin* (external margin)

3. **Background Layer**: The background of the progress bar with *wob_background_color*
	- Controlled by *wob_background_padding*

4. **Foreground Layer**: The filled portion of the progress bar
	- Normal range (0-100%): Uses *wob_foreground_color*
	- Overflow range (101-200%): Uses *wob_overflow_color*
	- Controlled by *wob_foreground_padding* (normal range)
	- Controlled by *wob_foreground_overflow_padding* (overflow range)

The overall dimensions are controlled by:
	- *wob_bar_width*: Total width of the progress bar
	- *wob_bar_height*: Total height of the progress bar

# NOTES

Configuration options can also be overridden by command-line options specified when running *pipelam*(1). Command-line options have the highest precedence, followed by environment variables, user configuration file, and system configuration file.

When sending JSON-formatted messages to pipelam, per-notification settings can be specified that override all configuration file and command-line settings for that specific notification.

# SEE ALSO

pipelam(1)

# AUTHOR

thomascrha
