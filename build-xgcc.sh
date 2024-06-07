#! /bin/sh

if ! [ -d coreboot/util/crossgcc/ ]; then
  echo No crossgcc source directory.
  exit 1
fi

if [ -d coreboot/util/crossgcc/xgcc/ ]; then
  echo crossgcc already exist.
  exit 1
fi

echo Build crossgcc.
(cd coreboot/util/crossgcc/ && ./buildgcc -p i386-elf -j 12)
