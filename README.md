# os-development

This is my poorly named personal OS project.

I only work on this when I have the notion so things can sometimes be quiet for years at a time!

## Aims

The idea behind this was an entirely stream-based OS. Each stream exports a specific 'interface' which doesn't need to be defined until runtime, which includes everything from file formats to kernel driver interfaces.

Further to this networking would be transparent, and instances running on distinct platforms (multiple servers and PCs, embedded boards running sensors, actuators or video in/out) could all transparently be accessed from anywhere on the network, by configuring a mesh.

## Setup

As per https://wiki.osdev.org/GCC_Cross-Compiler

### Install via your package manager/ports/homebrew:
mtools
gmp
mpfr
libmpc

### Downloads

binutils: https://www.gnu.org/software/binutils/

gcc: http://gcc.gnu.org/install/download.html

grub (if necessary): https://www.gnu.org/software/grub/grub-download.html

objconv (for grub): http://www.agner.org/optimize/objconv.zip

unicode font (for grub): http://unifoundry.com/pub/unifont/unifont-12.1.02/

### Set up

```
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

### binutils

```
mkdir build-binutils
cd build-binutils
../binutils-<ver>/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make && make install
```

### gcc

```
which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
mkdir build-gcc
cd build-gcc
../gcc-<ver>/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc && all-target-libgcc && install-gcc && install-target-libgcc
```

### objconv

```
mkdir objconv_pack
cd objconv_pack
unzip ../objconv.zip
cd ..
mkdir objconv
cd objconv
unzip ../objconv_pack/source.zip
g++ -o objconv -O2 *.cpp --prefix="$PREFIX"
cp objconv "$PREFIX/bin"
```

### grub

```
../grub-2.02/configure --prefix="$PREFIX" --target=$TARGET
make && make install
```

### Install a font for grub

```
tar xfz unifont-12.1.02.tar.gz
grub-mkfont -o $PREFIX/share/grub/unicode.pf2 unifont-12.1.02/font/precompiled/unifont-12.1.02.ttf
```

## Building

`Kernel`, `Library` and `Conductor` have makefiles, and should be built in that order. After building them, run `makeiso` to generate the output ISO which should then work in Bochs, VirtualBox and Qemu.

## Documentation

Doxygen output is at:
https://electric-monk.github.io/os-development/html/index.html
