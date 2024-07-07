#!/bin/sh

CFLAGS="-Wall -Wextra -Werror -pedantic -std=c11"
LDFLAGS="-L./gtk4-layer-shell/build/src -lgtk4-layer-shell"
INCLUDES="-I./gtk4-layer-shell/include -I./gtk4-layer-shell/build/include"

gcc $CFLAGS $(pkg-config --cflags gtk4) $INCLUDES -o bow simple-example.c $LDFLAGS $(pkg-config --libs gtk4)

# gcc -I./gtk4-layer-shell/include -I./gtk4-layer-shell/build/include $(pkg-config --cflags gtk4) -o bow simple-example.c -L./gtk4-layer-shell/build/src -lgtk4-layer-shell $(pkg-config --libs gtk4)
# gcc -Wall -Wextra -Werror -pedantic -std=c11 -o bow main.c
