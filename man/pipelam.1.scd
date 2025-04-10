pipelam.toml(5) "April 2025" "File Formats Manual"

# NAME

pipelam.toml - configuration file for pipelam

# SYNOPSIS

*/etc/pipelam/pipelam.toml*
*$HOME/.config/pipelam/pipelam.toml*

# DESCRIPTION

*pipelam.toml* is the configuration file for *pipelam*(1), a lightweight overlay notification system for Wayland compositors.

The configuration file uses TOML format. Options specified in the user configuration file ($HOME/.config/pipelam/pipelam.toml) override those in the system configuration file (/etc/pipelam/pipelam.toml). Both configuration files are optional; if they do not exist, default values will be used.

Additionally, environment variables that start with *PIPELAM_* followed by the uppercase setting name will override settings from any configuration file.

# CONFIG OPTIONS

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
	*replace* - New windows replace existing ones
	*overlay* - Windows are displayed simultaneously, overlaid on top of each other (default)

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

# EXAMPLE

A full configuration file example:

```
# pipelam configuration file

# Logging level: DEBUG, INFO, WARNING, ERROR, PANIC
log_level = INFO

# Runtime behavior: queue, replace, overlay
runtime_behaviour = replace

# Window display duration in milliseconds
window_timeout = 800

# Positioning
anchor = bottom-right
margin_left = 0
margin_right = 20
margin_top = 0
margin_bottom = 20
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

*PIPELAM_CONFIG_FILE_PATH*
	Use a configuration file at a custom location

# NOTES

Configuration options can also be overridden by command-line options specified when running *pipelam*(1). Command-line options have the highest precedence, followed by environment variables, user configuration file, and system configuration file.

When sending JSON-formatted messages to pipelam, per-notification settings can be specified that override all configuration file and command-line settings for that specific notification.

# SEE ALSO

pipelam(1)

# AUTHOR

thomascrha
