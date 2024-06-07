#!/bin/bash

ITX_LLAMA_BOARD_REVISION="v1-rev-E"

ADD_SEABIOS_CONFIG=1

IGNORE_COREBOOT_SEABIOS_CONFIG_CONFLICT=1

BUILD_DATE=`date +"%Y%m%d-%H%M%S"`
OUTPUT_NAME="itx-llama-$ITX_LLAMA_BOARD_REVISION-$BUILD_DATE"
CROSSBAR="crossbar-itx-llama-$ITX_LLAMA_BOARD_REVISION.bin"
MAC_ADDRESS="mac-address.bin"

BOOTORDER="bootorder-sd-usb"
BOOT_MENU_WAIT=5000
#SCREEN_AND_DEBUG=1
#PS2_KEYBOARD_SPINUP=500

BASE_DIR=`dirname $0`

(cd ${BASE_DIR} && cp config/coreboot/coreboot.config coreboot/.config) || exit 1
(cd ${BASE_DIR} && cp config/seabios/seabios.config seabios/.config) || exit 1

. ${BASE_DIR}/cbfs-func || exit 1

if ! [ -z ITX_LLAMA_BOARD_REVISION ]; then
  echo "$ITX_LLAMA_BOARD_REVISION" > seabios/.boardrevision
fi

if ! [ -z BUILD_DATE ]; then
  echo "$BUILD_DATE" > seabios/.builddate
fi

# coreboot .config set payload to none, payload is selected by option file.
if grep ^"CONFIG_PAYLOAD_NONE=y"$ ${BASE_DIR}/coreboot/.config > /dev/null; then
  # get CONFIG_CBFS_PREFIX setting from coreboot config file.
  if ! S=`grep ^CONFIG_CBFS_PREFIX= ${BASE_DIR}/coreboot/.config` > /dev/null; then
    echo "Missing CONFIG_CBFS_PREFIX in coreboot .config"
    exit 1
  fi
  eval $S
  # Add SeaBIOS payload by option file.
  if ! [ -z $PAYLOAD_SEABIOS ]; then
    # if [ -z $SEABIOS_COMMIT ]; then
    #   echo "Missing SEABIOS_COMMIT setting."
    #   exit 1
    # fi
    NEED_BUILD_SEABIOS_LATER=1
  fi
else
  # coreboot .config set payload is not none, PAYLOAD_* in target file is invalid.
  if ! [ -z $PAYLOAD_SEABIOS ]; then
    echo "PAYLOAD_SEABIOS is set, and payload in coreboot .config file is not none."
    exit 1
  fi
  if ! [ -z $PAYLOAD_ELF ]; then
    echo "PAYLOAD_ELF is set, and payload in coreboot .config file is not none."
    exit 1
  fi
fi

# coreboot .config set payload to ./seabios directory, need to build SeaBIOS before coreboot.
if grep ^"CONFIG_PAYLOAD_FILE=\"../seabios/out/bios.bin.elf"\"$ ${BASE_DIR}/coreboot/.config > /dev/null; then
  # if [ -z $SEABIOS_COMMIT ]; then
  #   echo "Missing SEABIOS_COMMIT setting."
  #   exit 1
  # fi
  NEED_BUILD_SEABIOS_FIRST=1
fi

# if need to build SeaBIOS, check if coreboot has conflict option with SeaBIOS.
if [ -z $IGNORE_COREBOOT_SEABIOS_CONFIG_CONFLICT ]; then
  if ! [ -z $NEED_BUILD_SEABIOS_FIRST ] || ! [ -z $NEED_BUILD_SEABIOS_LATER ] ;then
    if grep ^"CONFIG_VGA_ROM_RUN=y"$ ${BASE_DIR}/coreboot/.config > /dev/null; then
      echo "Error : coreboot CONFIG_VGA_ROM_ROM=y is conflict with SeaBIOS."
      exit 1
    fi
    if grep ^"CONFIG_ON_DEVICE_ROM_RUN=y"$ ${BASE_DIR}/coreboot/.config > /dev/null; then
      echo "Error : coreboot CONFIG_ON_DEVICE_ROM_RUN=y is conflict with SeaBIOS."
      exit 1
    fi
  fi
fi

(
  cd ${BASE_DIR} || exit 1
  if ! [ -z $NEED_BUILD_SEABIOS_FIRST ]; then
    echo "Building SeaBIOS..."
    (cd seabios && ../xgcc-seabios-make.sh) || exit 1
    echo
  fi
  echo "Building coreboot..."
  (cd coreboot && make) || exit 1
  echo
  if ! [ -z $NEED_BUILD_SEABIOS_LATER ]; then
    echo "Building SeaBIOS..."
    (cd seabios && ../xgcc-seabios-make.sh) || exit 1
    echo
    if [ -z $PAYLOAD_COMPRESS ]; then
      echo "Add SeaBIOS payload."
      cbfs_add_payload seabios/out/bios.bin.elf -n ${CONFIG_CBFS_PREFIX}/payload
    else
      echo "Add SeaBIOS payload (compressed)."
      cbfs_add_payload seabios/out/bios.bin.elf -n ${CONFIG_CBFS_PREFIX}/payload -c $PAYLOAD_COMPRESS
    fi
    echo
  fi
  if ! [ -z $PAYLOAD_ELF ]; then
    echo "Add ELF payload."
    cbfs_add_payload $PAYLOAD_ELF -n ${CONFIG_CBFS_PREFIX}/payload -c $PAYLOAD_COMPRESS
  fi

  # if need to add SeaBIOS config into CBFS.
  if ! [ -z $ADD_SEABIOS_CONFIG ]; then
    if ! [ -z $NEED_BUILD_SEABIOS_FIRST ] || ! [ -z $NEED_BUILD_SEABIOS_LATER ] ;then
      echo "Add SeaBIOS config file."
      echo "# This image was built using git revision" `cd seabios && git rev-parse HEAD` > seabios/out/config.tmp
      # Add git revision info and remove comment line in SeaBIOS config file.
      # Copied from coreboot src/arch/x86/Makefile.inc .
      sed -e '/^#/d' -e '/^ *$$/d' seabios/.config >> seabios/out/config.tmp
      cbfs_add seabios/out/config.tmp -n seabios_config
      rm -f seabios/out/config.tmp
    fi
  fi

  # Add floppy image as ramdisk
  echo "Adding ramdisk floppy image"
  cbfs_add ./win98_1440k.img.lzma -n floppyimg/win98_1440k.img.lzma -t raw

  echo "Writing crossbar data..."
  ./write-crossbar.py coreboot/build/coreboot.rom crossbar/${CROSSBAR} && \
  echo "Writing MAC address data..." && \
  ./write-mac-address.py coreboot/build/coreboot.rom ${MAC_ADDRESS} && \
  echo "Adding coreboot bootorder/flags/roms..."
  (
    [ -z "$BOOTORDER" ] || \
    cbfs_add seabios-etc/${BOOTORDER} -n bootorder
  ) && \
  (
    [ -z "$BOOT_MENU_WAIT" ] || \
    cbfs_add_int ${BOOT_MENU_WAIT} etc/boot-menu-wait
  ) && \
  (
    [ -z "$SCREEN_AND_DEBUG" ] || \
    cbfs_add_int ${SCREEN_AND_DEBUG} etc/screen-and-debug
  ) && \
  (
    [ -z "$PS2_KEYBOARD_SPINUP" ] || \
    cbfs_add_int ${PS2_KEYBOARD_SPINUP} etc/ps2-keyboard-spinup
  ) && \
  echo "Copy output ROM file to out/${OUTPUT_NAME}.rom" && \
  mkdir -p out && \
  cp coreboot/build/coreboot.rom out/${OUTPUT_NAME}.rom && \
  cbfs_print out/${OUTPUT_NAME}.rom
  echo ""
  echo "Padding rom file to 8 MB..."
  if [ `uname` == 'Darwin' ]; then
    ROM_FILE_SIZE=`stat -f%z "out/${OUTPUT_NAME}.rom"`
  else
    ROM_FILE_SIZE=`du -b "out/${OUTPUT_NAME}.rom" | cut -f1`
  fi
  PADDING_SIZE=$((0x800000-ROM_FILE_SIZE)) && \
  dd if=/dev/zero ibs=1 count=$PADDING_SIZE | LC_ALL=C tr "\000" "\377" >out/padding.bin && \
  cat out/padding.bin out/${OUTPUT_NAME}.rom >out/${OUTPUT_NAME}-padded.rom && \
  rm out/padding.bin && \
  echo ""
  echo "Done!"
)
