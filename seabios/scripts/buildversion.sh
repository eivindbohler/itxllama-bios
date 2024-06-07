#!/bin/sh
# Script to generate a C file with version information.
OUTFILE="$1"
VAR16MODE="$2"

if [ -f .boardrevision ]; then
  ITX_LLAMA_BOARD_REVISION=`cat .boardrevision`
  rm .boardrevision
else
  ITX_LLAMA_BOARD_REVISION="?"
fi

if [ -f .builddate ]; then
  BUILD_DATE=`cat .builddate`
  rm .builddate
else
  BUILD_DATE="?"
fi

VERSION="ITX-Llama ${ITX_LLAMA_BOARD_REVISION} - ${BUILD_DATE}"
echo "\nVersion: ${VERSION}\n"

# Build header file
if [ "$VAR16MODE" = "VAR16" ]; then
    cat > ${OUTFILE} <<EOF
#include "types.h"
char VERSION[] VAR16 = "${VERSION}";
EOF
else
    cat > ${OUTFILE} <<EOF
char VERSION[] = "${VERSION}";
EOF
fi
