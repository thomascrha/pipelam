# pipelam

A lightweight overlay volume/backlight/progress/anything hugely inspired and based on [wob](https://github.com/francma/wob) for wlroots based compositors.

## Features

This tool differs from wob in several different ways:

 - It uses gkt4 and gtk4-layer-shell for displaying the overlay.
 - This overlay is customisable using the JSON payload you write into the FIFO file pipelam is listening to.
    - It as two different types of overlays: `text` and `image`.
    - You can set the position, size, colour, font, text, image, etc. of the overlay.
    - The overlay uses [pango](https://developer.gnome.org/pango/stable/) for text rendering.

## Installation

At this stage you will need to build the project from source. You will need the following dependencies:

 - gtk4
 - gtk4-layer-shell
 - wlroots (for the protocol headers)
 - clang
 - bear (optional, for generating compile_commands.json)
 - [make](https://www.gnu.org/software/make/)

To build the project, run the following commands:

```
make
```

This will build the project and place the binary in the `build` directory.

## Running

To run the project, you will need to create a FIFO file that pipelam will listen to. You can do this by running the following command:

Note: If no FIFO file is provided, pipelam will create one at the path you specify.
```shell
mkfifo /tmp/pipelam.fifo
```

Then you can run the following command to start pipelam:

```shell
./build/pipelam /tmp/pipelam.fifo
```

Then you can write a JSON payload to the FIFO file to display the overlay. Here is an example payload:

```json
{
    "type": "text",
    "text": "Hello, World!",
}
```

```shell
jq -n --arg text "Hello, World" '{type: "text", expression: $text}' -c > /tmp/pipelam.fifo
```

### Examples

There are several example scripts in the `examples` directory that you can use to test the overlay. You can run these scripts by running the following command:

#### Text Overlay
```shell
./examples/volume-bar-expression.sh > /tmp/pipelam.fifo
```

```shell
./examples/volume-text-expression.sh > /tmp/pipelam.fifo
```

## Json Payload

1. The JSON payload must be a valid JSON object. It also can't be pretty printed - it must be a single line. If it can't be parsed as JSON the input is treated as type `text` and the text is just displayed.
2. The object must contain a `type` key with a value of either `text` or `image`.
3. The object must contain an `expression` key with a value of the text you want to display or the path to the image you want to show.
4. For all optional keys these must be contained in an inner object with the key `settings`. This object can contain the following keys:
    - `window_timeout`: The time in milliseconds before the overlay is hidden.
    - `anchor`: The anchor point of the overlay. This can be one of the following values:
        - `top-left`
        - `top-right`
        - `bottom-left`
        - `bottom-right`
        - `center`
    - `margin_left`: The margin from the left edge of the screen.
    - `margin_right`: The margin from the right edge of the screen.
    - `margin_top`: The margin from the top edge of the screen.
    - `margin_bottom`: The margin from the bottom edge of the screen.
5. If the keys in the `settings` object are not provided, the default values will be used. If the `settings` object contains keys it doesn't recognise or can't interpret the value it simply uses the default.

### Text Expression

The expression for type `text` supports [pango markup](https://docs.gtk.org/Pango/pango_markup.html). This allows you to customise the appearance of the text.

## Configuration

pipelam has two configuration files that can be used to customise the appearance of the overlay. The default one is located at `/etc/pipelam/config` and a user one can be placed in `$HOME/.config/pipelam/config`

1. If no configuration file is found, the default values will be used.
2. If a configuration file is found, they will be loaded in this order (with the last one taking precedence):
    - The default configuration file is loaded first. (`/etc/pipelam/config`)
    - The user configuration file is loaded second. (`$HOME/.config/pipelam/config`)
3. Environment variables take precedence over the configuration file. If an environment variable is set, it will override the value in the configuration file. These environment variables are the name of the attribute in the config file prefaced with `pipelam_` - for example `log_level` is overwritten by the environment variable `pipelam_LOG_LEVEL`.
    - Note: there is one undocumented environment variable - `pipelam_CONFIG_FILE_PATH` - this allows you to set an arbitrary file path for the config

### Configuration Options

The configuration file is a TOML file with the following options:
```toml
log_level = "INFO" # The log level for the application. (Default: "INFO")
window_timeout = 600 # The time in milliseconds before the overlay is hidden. (Default: 600) (Can be overridden by the JSON payload)
anchor = "center" # The anchor point of the overlay. (Default: "center") (Can be overridden by the JSON payload)
margin_left = 100 # The margin from the left edge of the screen. (Default: 100) (Can be overridden by the JSON payload)
margin_right = 0 # The margin from the right edge of the screen. (Default: 0) (Can be overridden by the JSON payload)
margin_top = 100 # The margin from the top edge of the screen. (Default: 100) (Can be overridden by the JSON payload)
margin_bottom = 0 # The margin from the bottom edge of the screen. (Default: 0) (Can be overridden by the JSON payload)
```

