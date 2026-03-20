# Novus UEFI Builder
NUB helps you quickly build, test, and flash UEFI applications. Just add it to your PATH and use it anywhere.
Made for personal use. You might have to make some changes to use it, check warning below.
# Usage
```c
nub clean "prog" // cleans build files
nub make "prog" // makes build files
nub run "prog" // run application in QeMu
sudo nub flash "prog" // flash the program to an USB
nub kernel "prog" // build kernel
```
# Warning
This tool is hardcoded to build my custom [NovusKernel](https://github.com/Nullora/NovusKernel) and custom bootloader, to build more UEFI apps like your custom bootloader, make sure you change the hardcoded paths in the kernel() function
If you aim to use the flash feature, make sure to modify line 113 and change **sda1** to your usb name, you can find it using **lsblk** in terminal.
