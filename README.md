# os-development

This is my poorly named personal OS project.

I only work on this when I have the notion so things can sometimes be quiet for years at a time!

## Aims

The idea behind this was an entirely stream-based OS. Each stream exports a specific 'interface' which doesn't need to be defined until runtime, which includes everything from file formats to kernel driver interfaces.

Further to this networking would be transparent, and instances running on distinct platforms (multiple servers and PCs, embedded boards running sensors, actuators or video in/out) could all transparently be accessed from anywhere on the network, by configuring a mesh.

## Setup

A gcc-based cross compiler for x86 is required. I decided to target x86 first as an easy architecture, but intend to add 64-bit, ARM, etc. later.
You'll need grub to generate the bootable ISO.

## Building

`Kernel` and `Conductor` have makefiles. After building them, run `makeiso` to generate the output ISO which should then work in Bochs, VirtualBox and Qemu.

## Documentation

Doxygen output is at:
https://electric-monk.github.io/os-development/html/index.html
