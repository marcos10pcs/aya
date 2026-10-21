#!/bin/bash
# this is for AppImage
export PATH="$APPDIR/usr/bin:$PATH"
ARGS="${@:2}"
TOXEC="${1:-Player}"

exec $APPDIR/usr/bin/Aya.$TOXEC $ARGS
