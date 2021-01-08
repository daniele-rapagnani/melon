#!/bin/bash

SCRIPT="$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"

FILES=$(find "$SCRIPT/fixtures" -name "*.ma")
OLDIFS="$IFS"
IFS=$'\n'

if [ -z "$BUILD_DIR" ]; then
BUILD_DIR="build"
else
echo "Using build dir: $BUILD_DIR"
fi

MELON_VERSION=$("$SCRIPT/../$BUILD_DIR/melon" --version)

if [ -z "$MELON_VERSION" ]; then
	echo "Can't read melon version" 1>&2
	exit 1
fi

BITS=64

if echo "$MELON_VERSION" | grep -q "32bit"; then
	BITS=32
fi

echo "Building for ${BITS}bit"

for FILE in $FILES; do
	DIRNAME=$(dirname "$FILE")
	BASENAME=$(basename "$FILE")
	mkdir -p "$DIRNAME/$BITS"
	OUTFILE="$DIRNAME/$BITS/${BASENAME%.*}.mbc"
	echo "Compiling $OUTFILE"
	if ! "$SCRIPT/../$BUILD_DIR/assembler" -f "$FILE" -o "$OUTFILE"; then
		echo "Error compiling, aborting." 1>&2
		exit 1
	fi
done

mkdir -p "fixtures/vm/language/modules/$BITS"
"$SCRIPT/../$BUILD_DIR/melon" -f fixtures/vm/language/modules/module_obj.ms -o "fixtures/vm/language/modules/$BITS/module_obj.mbc"
