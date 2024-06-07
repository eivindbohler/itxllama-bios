#include "x86.h"
#include "util.h"
#include "llama/llama_nbsb.h"

#define PCI_GET_CF8(bus, dev, fun)  (0x80000000UL + ((u32)(bus) << 16) + ((u32)(dev) << 11) + ((u32)(fun) << 8))

static u32 pci_in32(u32 addr)
{
  outl(addr, 0x0CF8);
  return inl(0x0CFC);
}

static void pci_out32(u32 addr, u32 value)
{
  outl(addr, 0x0CF8);
  outl(value, 0x0CFC);
}

static u8 pci_in8(u32 addr)
{
  int i = (int)(addr & 0x03L) << 3; // * 8
  outl(addr & 0xFFFFFFFCL, 0x0CF8);
  return (u8)(inl(0x0CFC) >> i);
}

static void pci_out8(u32 addr, u8 value)
{
  int i = (int)(addr & 0x03L) << 3; // * 8
  outl(addr & 0xFFFFFFFCL, 0x0CF8);
  outl((inl(0x0CFC) & ~(0xFFL << i)) | ((u32)value << i), 0x0CFC);
}

u32 llama_nbsb_read32(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset)
{
  return pci_in32(PCI_GET_CF8(0, nbsb, func) + (u32)offset);
}

void llama_nbsb_write32(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset, u32 value)
{
  pci_out32(PCI_GET_CF8(0, nbsb, func) + (u32)offset, value);
}

u8 llama_nbsb_read8(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset) {
  return pci_in8(PCI_GET_CF8(0, nbsb, func) + (u32)offset);
}

void llama_nbsb_write8(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset, u8 value)
{
  pci_out8(PCI_GET_CF8(0, nbsb, func) + (u32)offset, value);
}