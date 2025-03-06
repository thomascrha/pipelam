# bow

A lightweight overlay volume/backlight/progress/anything hugely inspired and based on [wob](https://github.com/francma/wob) for wlroots based compositors.

## Features

This tool differs from wob in several different ways:

 - It uses gkt4 and gtk4-layer-shell for displaying the overlay.
 - This overlay is customisable using the JSON payload you write into the FIFO file bow is listening to.
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
