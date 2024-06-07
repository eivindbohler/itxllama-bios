#ifndef __LLAMA_BOOT_DISK_STRINGS
#define __LLAMA_BOOT_DISK_STRINGS

static const char llama_boot_disk_string_sd[] =
"/pci@i0cf8/*@c/drive@0/disk@0\n";

static const char llama_boot_disk_string_usb[] =
"/pci@i0cf8/usb@a,1/storage@1/*@0/*@0,0\n"
"/pci@i0cf8/usb@a,1/usb-*@1\n"
"/pci@i0cf8/usb@a,1/storage@2/*@0/*@0,0\n"
"/pci@i0cf8/usb@a,1/usb-*@2\n"
"/pci@i0cf8/usb@a,1/hub@1/usb-*@1\n"
"/pci@i0cf8/usb@a,1/hub@1/usb-*@2\n"
"/pci@i0cf8/usb@a,1/hub@1/usb-*@3\n"
"/pci@i0cf8/usb@a,1/hub@1/usb-*@4\n";

static const char llama_boot_disk_string_sata[] =
"/pci@i0cf8/*@c/drive@1/disk@0\n";

static const char llama_boot_disk_string_vfloppy[] =
"/rom@floppyimg/*\n";

#endif // __LLAMA_BOOT_DISK_STRINGS