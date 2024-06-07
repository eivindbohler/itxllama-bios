#! /usr/bin/env python
# Write 6 byte MAC address into ROM file address {size-0x50}.
import sys
import os
def main():
    print("write-mac-address.py : Writing MAC address data into ROM file")
    rom_file, mac_file = sys.argv[1], sys.argv[2]
    print("ROM file: %s, MAC address file: %s" % (rom_file, mac_file))
    mac_data = open(mac_file, "rb").read()
    if len(mac_data) != 6:
        print("MAC address data file length error")
        sys.exit(1)
    fs = os.stat(rom_file).st_size
    if fs not in [256 * 1024, 512 * 1024, 1024 * 1024, 2048 * 1024, 4096 * 1024]:
        print("ROM file length error")
        sys.exit(1)
    rom_f = open(rom_file, "r+b")
    rom_f.seek(-0x50, 2)  # seek from the end of file
    rom_f.write(mac_data)
    print("MAC address written successfully")

if __name__ == "__main__":
    main()
