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

- [GTK4](https://docs.gtk.org/gtk4/)
- [gtk4-layer-shell](https://github.com/wmww/gtk4-layer-shell)
- [json.h](https://raw.githubusercontent.com/sheredom/json.h/cfdee7c025081ce722644f3ac286e1e27ad16f82/json.h)
  - NOTE: This is downloaded as part of the build process and is not a hard dependency.

### Development dependencies

- [make](https://www.gnu.org/software/make/)
- clang-format (for formatting the code) [optional]
- scdoc (for generating man pages) [optional]

## Installation

To build and install the project, run the following commands:

```
make build
make docs # optional if you want the docs
sudo make install
make install-systemd # optional if you want to install the systemd service and socket
```

### Arch Linux

[![pipelam](https://img.shields.io/aur/version/pipelam?style=flat-square)](https://aur.archlinux.org/packages/pipelam/)

### sway

Add the following to your sway config to integrate with pipelam

```shell
# if using the systemd service
set $PIPELAM_SOCK /run/user/1000/pipelam.fifo
bindsym XF86AudioRaiseVolume exec wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+ && wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK
bindsym XF86AudioLowerVolume exec wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- && wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK
bindsym XF86AudioMute exec wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle && (wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -q MUTED && echo 0 > $WOBSOCK) || wpctl get-volume @DEFAULT_AUDIO_SINK@ | sed 's/[^0-9]//g' > $PIPELAM_SOCK

bindsym XF86MonBrightnessDown exec brightnessctl set 5%- | sed -En 's/.*\(([0-9]+)%\).*/\1/p' > $PIPELAM_SOCK
bindsym XF86MonBrightnessUp exec brightnessctl set +5% | sed -En 's/.*\(([0-9]+)%\).*/\1/p' > $PIPELAM_SOCK
```

## Documentation

```shell
man 1 pipelam
man 5 pipelam.toml
```

# TODO's

- [ ] Add a feature to display some text with the wob type to describe the progress bar (even emojis and fontawesome)
- [ ] Clean up the examples and make more permutations of them
- [ ] Using the examples make a demo video for the readme
- [x] Add to Arch user repositories
- [x] Add better installation step including describing the man and systemd installation
- [x] Add customisation of the wob type
- [x] Add overflow behaviour that turns red for wob mode

