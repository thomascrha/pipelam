#!/bin/bash

set -xe

# build dependencies ubuntu 22.04
# sudo apt install meson ninja-build libwayland-dev libgtk-4-dev gobject-introspection libgirepository1.0-dev gtk-doc-tools python3 valac

# TODO look into static linking of gtk4-layer-shell and butil it into the binary - im ok with gtk4 being dynamic
# TODO improve the check for needing install, check to see if gtk4-layer-shell is installed and if not install it
# GTK4_LAYER_SHELL_VERSION="1.0.2"
# GTK4_LAYER_SHELL_DIR="gtk4-layer-shell-${GTK4_LAYER_SHELL_VERSION}"
# if [[ ! -d $GTK4_LAYER_SHELL_DIR ]]; then
#     wget https://github.com/wmww/gtk4-layer-shell/archive/refs/tags/v${GTK4_LAYER_SHELL_VERSION}.tar.gz -O gtk4-layer-shell.tar.gz
#     tar -xvf gtk4-layer-shell.tar.gz
#     cd $GTK4_LAYER_SHELL_DIR
#     meson setup -Dexamples=true -Ddocs=true -Dtests=true build
#     ninja -C build
#     sudo ninja -C build install
#     sudo ldconfig
#     cd ..
#     rm gtk4-layer-shell.tar.gz
# fi


# CFLAGS=""
CFLAGS="-Wall -Wextra -Werror -pedantic -std=c11"
CC=clang

$CC $CFLAGS $(pkg-config --cflags gtk4-layer-shell-0) $(pkg-config --cflags gtk4) -o bow src/main.c src/window.c $(pkg-config --libs gtk4-layer-shell-0) $(pkg-config --libs gtk4)

