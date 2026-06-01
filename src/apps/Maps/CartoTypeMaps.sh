##!/bin/sh
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$SCRIPT_DIR:$LD_LIBRARY_PATH"

# Point to the root plugin folder instead of just platforms
export QT_PLUGIN_PATH="$SCRIPT_DIR/plugins"

exec "$SCRIPT_DIR/CartoTypeMaps" "$@"
