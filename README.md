# pipelam

A lightweight GTK4-based notification system for displaying text, images, and progress bars in wlroots compositors. Pipelam is inspired by [wob](https://github.com/francma/wob).

## Features

This tool differs from wob in several different ways:

- It uses gkt4 and gtk4-layer-shell for displaying the overlay.
- This overlay is customisable using the JSON payload you write into the FIFO file pipelam is listening to.
  - It as three different types of overlays: `text`, `image` and `wob` (an homage to the that project).
  - The overlay can be displayed in three different modes:
    - `queue`: The overlay is queued and displayed one after the other.
    - `replace`: The overlay replaces the previous overlay.
    - `overlay`: The overlay is overlayed on top of each other.
  - You can set the position, size, colour, font, text, image, etc. of the overlay.
  - The overlay uses [pango](https://developer.gnome.org/pango/stable/) for text rendering.

## Dependencies

At this stage you will need to build the project from source. You will need the following dependencies:

- [gtk4-layer-shell](https://github.com/wmww/gtk4-layer-shell)
- [json.h](https://raw.githubusercontent.com/sheredom/json.h/cfdee7c025081ce722644f3ac286e1e27ad16f82/json.h)
  - NOTE: This is downloaded as part of the build process and is not a hard dependency.

### Development dependencies

- [make](https://www.gnu.org/software/make/)
- bear (optional, for generating compile_commands.json)
- clang-format

## Installation

To build and install the project, run the following commands:

```
make build
make docs # optional if you want the docs
sudo make install
make install_systemd # optional if you want to install the systemd service and socket
```

## Running

To run the project, you will need to create a FIFO file that pipelam will listen to. You can do this by running the following command:

NOTE: When running using systemd the pipe path by default is `/run/user/$UID/pipelam.fifo`

```shell
mkfifo /tmp/pipelam.fifo
```

Then you can run the following command to start pipelam:

```shell
pipelam /tmp/pipelam.fifo
```

To just use the wob overlay you can just send down an integer

```
echo 50 > /tmp/pipelam.fifo
```

You can just send text without any formatting

```shell
echo "Hello, World!" > /tmp/pipelam.fifo
```

Then you can write a JSON payload to the FIFO file to display the overlay. Here is an example payload:

```json
{
    "type": "text",
    "expression": "<span foreground=\"blue\" size=\"x-large\">Hello World!</span> is <i>cool</i>!"
}
```

```shell
cat examples/example.json > /tmp/pipelam.fifo
```

## sway

Add the following to your sway config to integrate with pipelam

```shell
set $PIPELAM_SOCK /run/user/1000/pipelam.fifo
bindsym XF86AudioRaiseVolume exec wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+ && wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK
bindsym XF86AudioLowerVolume exec wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- && wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK
bindsym XF86AudioMute exec wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && (wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -q MUTED && echo 0 > $WOBSOCK) || wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK

bindsym XF86MonBrightnessDown exec brightnessctl set 5%- | sed -En 's/.*\(([0-9]+)%\).*/\1/p' > $PIPELAM_SOCK
bindsym XF86MonBrightnessUp exec brightnessctl set +5% | sed -En 's/.*\(([0-9]+)%\).*/\1/p' > $PIPELAM_SOCK
```

## Json Payload

1. The JSON payload must be a valid JSON object. It also can't be pretty printed - it must be a single line. If it can't be parsed as JSON the input is treated as type `wob` if the input is an integer, if it can't be parsed as an integer it will lastly default to `text`.
2. The object must contain a `type` key with a value of either `text`, `image`, or `wob`.
3. The object must contain an `expression` key with a value:
  - For `text`: The text you want to display (supports Pango markup)
  - For `image`: The path to the image you want to show
  - For `wob`: A numeric value between 0-200 representing the progress bar percentage
4. The object also supports an `version` key, at this point this doesn't do anything - but it is there for future compatibility. The default value is 0.
5. For all optional keys these must be contained in an inner object with the key `settings`. Please refer to the [Configuration](#Configuration) section for the available options.
6. If the keys in the `settings` object are not provided, the default values will be used. If the `settings` object contains keys it doesn't recognise or can't interpret the value it simply uses the default.

### Text Expression

The expression for type `text` supports [pango markup](https://docs.gtk.org/Pango/pango_markup.html). This allows you to customise the appearance of the text.

## Configuration

pipelam has two configuration files that can be used to customise the appearance of the overlay. The default one is located at `/etc/pipelam/pipelam.toml` and a user one can be placed in `$HOME/.config/pipelam/pipelam.toml`

1. If no configuration file is found, the default values will be used.
2. If a configuration file is found, they will be loaded in this order (with the last one taking precedence):
  - The default configuration file is loaded first. (`/etc/pipelam/pipelam.toml`)
  - The user configuration file is loaded second. (`$HOME/.config/pipelam/pipelam.toml`)
3. Environment variables take precedence over the configuration file. If an environment variable is set, it will override the value in the configuration file. These environment variables are the name of the attribute in the config file prefaced with `PIPELAM_` - for example `log_level` is overwritten by the environment variable `PIPELAM_LOG_LEVEL`.
  - Note: there is one undocumented environment variable - `PIPELAM_CONFIG_FILE_PATH` - this allows you to set an arbitrary file path for the config.

### Configuration Options

The configuration file `config/pipelam.toml` with the following options:

```toml
# If you are editing this file inside /etc/pipelam/pipelam.toml please refain from doing that - you can override the
# default config by creating a file in ~/.config/pipelam/pipelam.toml. The file /etc/pipelam/pipelam.toml will be
# deleted during the unintall process
# This is also not a valid toml file - It's been called .toml for formatting reasons - all this file is a key value list
# please see this for the implementation https://github.com/welljsjs/Config-Parser-C
log_level = "INFO" # The log level for the application; INFO means the application will log all messages; DEBUG means the
                 # application will log all messages including debug messages; WARNING means the application will log all
                 # messages including warning messages; ERROR means the application will log all messages including error
                 # messages; PANIC means the application will log all messages including panic messages. (Default: INFO)
runtime_behaviour = "replace" # The runtime behaviour of gtk windows; replace means the windows are replaced; queue means
                            # the windows are queued and displayed one after the other;  overlay means the windows are
                            # overlayed on top of each other. (Default: replace)

# Window behaviour
window_timeout = 600 # The time in milliseconds before the overlay is hidden. (Default: 600) (Can be overridden by the JSON payload)
anchor = "center" # The anchor point of the overlay; center means the overlay is centered on the screen; top-left means the
                # overlay is anchored to the top left corner of the screen; top-right means the overlay is anchored to the
                # top right corner of the screen; bottom-left means the overlay is anchored to the bottom left corner of
                # the screen; bottom-right means the overlay is anchored to the bottom right corner of the screen.
                # (Default: center) (Can be overridden by the JSON payload)
margin_left = 100 # The margin from the left edge of the screen. (Default: 100) (Can be overridden by the JSON payload)
margin_right = 0 # The margin from the right edge of the screen. (Default: 0) (Can be overridden by the JSON payload)
margin_top = 100 # The margin from the top edge of the screen. (Default: 100) (Can be overridden by the JSON payload)
margin_bottom = 0 # The margin from the bottom edge of the screen. (Default: 0) (Can be overridden by the JSON payload)

# WOB bar behaviour
# Note: Any color can use css color strings or hexvalues with a # before the number - see wob_foreground_color
wob_bar_height = 25 # The height of the wob bar. (Default: 25) (Can be overridden by the JSON payload)
wob_bar_width = 350 # The width of the wob bar. (Default: 350) (Can be overridden by the JSON payload)
wob_border_color = "white" # The color of the border of the wob bar. (Default: white) (Can be overridden by the JSON payload)
wob_background_color = "black" # The color of the background of the wob bar. (Default: black) (Can be overridden by the JSON payload)
wob_foreground_color = "#FFFFFF" # The color of the foreground of the wob bar for values 0-100%. (Default: white) (Can be overridden by the JSON payload)
wob_overflow_color = "red" # The color of the foreground of the wob bar for values 101-200%. (Default: red) (Can be overridden by the JSON payload)
wob_box_color = "black" # The color of the box containing the wob bar. (Default: black) (Can be overridden by the JSON payload)

# WOB component-specific padding/margin
wob_box_padding = 0 # The padding of the box containing the wob bar in pixels. (Default: 0) (Can be overridden by the JSON payload)
wob_border_padding = 4 # The padding of the border of the wob bar in pixels. (Default: 4) (Can be overridden by the JSON payload)
wob_border_margin = 4 # The margin of the border of the wob bar in pixels. (Default: 4) (Can be overridden by the JSON payload)
wob_background_padding = 4 # The padding of the background of the wob bar in pixels. (Default: 4) (Can be overridden by the JSON payload)
wob_foreground_padding = 4 # The padding of the foreground of the wob bar in pixels. (Default: 4) (Can be overridden by the JSON payload)
wob_foreground_overflow_padding = 4 # The padding of the foreground overflow of the wob bar in pixels. (Default: 4) (Can be overridden by the JSON payload)
```

### WOB mode styling

```console
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
    wob_bar_heightrsed as JSON the input is treated as type
```

## Command Line Options

pipelam supports various command line options to customize its behavior without modifying the configuration file:

```console
Usage: pipelam [OPTIONS] <pipe_path>
Options:
  -l, --log-level=LEVEL        Set log level (DEBUG, INFO, WARNING, ERROR, PANIC)
  -r, --runtime-behaviour=TYPE Set runtime behaviour (queue, replace, overlay)
  -t, --window-timeout=MS      Set window timeout in milliseconds
  -a, --anchor=POS             Set window anchor position (bottom-left, bottom-right, top-left, top-right, center)
  -L, --margin-left=PIXELS     Set left margin in pixels
  -R, --margin-right=PIXELS    Set right margin in pixels
  -T, --margin-top=PIXELS      Set top margin in pixels
  -B, --margin-bottom=PIXELS   Set bottom margin in pixels

WOB bar options:
  --wob-bar-height=PIXELS      Set WOB bar height in pixels
  --wob-bar-width=PIXELS       Set WOB bar width in pixels
  --wob-border-color=COLOR     Set WOB border color
  --wob-background-color=COLOR Set WOB background color
  --wob-foreground-color=COLOR Set WOB foreground color
  --wob-overflow-color=COLOR   Set WOB overflow color
  --wob-box-color=COLOR        Set WOB box color
  --wob-box-padding=PIXELS     Set WOB box padding in pixels
  --wob-border-padding=PIXELS  Set WOB border padding in pixels
  --wob-border-margin=PIXELS   Set WOB border margin in pixels
  --wob-background-padding=PIXELS Set WOB background padding in pixels
  --wob-foreground-padding=PIXELS Set WOB foreground padding in pixels
  --wob-foreground-overflow-padding=PIXELS Set WOB foreground overflow padding in pixels

  -v, --version                Display version information
  -h, --help                   Display this help message
```

Example usage:
```console
pipelam --log-level=DEBUG --anchor=top-right --margin-top=50 --margin-right=50 /tmp/pipelam.fifo
```

## Makefile

```console
Usage: make [target]

Targets:
  build           Build and download all deps for the project
  build_dir       Create build directory if it doesn't exist
  build_test      Build the test suite
  clean           Remove built executables and object files
  debug           Build with enhanced debugging symbols for GDB
  derun           Run the project
  docs            Generate man pages from scdoc
  download_json_h Download the json.h external single header lib
  format          Format the code using clang-format
  help            Display this help message
  install         Install pipelam to the system
  install_systemd Install systemd (user) service and timer
  rebuild         Clean and build the project
  release         Create a release NOTE: VERSION is required. Usage: make release VERSION=X.Y.Z
  run             Run the project
  test            Rebuild the project and run tests
  uninstall       Uninstall pipelam from the system
```

# TODO's

- [ ] Add a feature to display some text with the wob type to describe the progress bar (even emojis and fontawesome)
- [ ] Clean up the examples and make more permutations of them
- [ ] Using the examples make a demo video for the readme
- [ ] Add to Arch user repositories
- [x] Add better installation step including describing the man and systemd installation
- [x] Add customisation of the wob type
- [x] Add overflow behaviour that turns red for wob mode

