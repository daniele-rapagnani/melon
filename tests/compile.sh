#!/bin/bash

SCRIPT="$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"

FILES=$(find "$SCRIPT/fixtures" -name "*.ma")
OLDIFS="$IFS"
IFS=$'\n'

for FILE in $FILES; do
	OUTFILE="${FILE%.*}.mbc"
	echo "Compiling $OUTFILE"
	if ! "$SCRIPT/../build/assembler" -f "$FILE" -o "$OUTFILE"; then
		echo "Error compiling, aborting." 1>&2
		exit 1
	fi
done

../build/melon -f fixtures/vm/language/modules/module_obj.ms -o fixtures/vm/language/modules/module_obj.mbc
