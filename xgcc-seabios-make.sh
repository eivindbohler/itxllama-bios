#!/bin/sh
HOSTCC=`which gcc`
HOSTCFLAGS=-w
XGCC_BASE=../coreboot/util/crossgcc/xgcc
CC=$XGCC_BASE/bin/i386-elf-gcc
AS=$XGCC_BASE/bin/i386-elf-as
OBJCOPY=$XGCC_BASE/bin/i386-elf-objcopy
OBJDUMP=$XGCC_BASE/bin/i386-elf-objdump
STRIP=$XGCC_BASE/bin/i386-elf-strip
CPP=$XGCC_BASE/bin/i386-elf-cpp
IASL=$XGCC_BASE/bin/iasl
LD=$XGCC_BASE/bin/i386-elf-ld
make CC=$CC AS=$AS OBJCOPY=$OBJCOPY OBJDUMP=$OBJDUMP STRIP=$STRIP CPP=$CPP IASL=$IASL LD=$LD HOSTCC=$HOSTCC HOSTCFLAGS=$HOSTCFLAGS $* -j18
