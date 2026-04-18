#!/bin/bash
# CopperOS dependency installer for Arch Linux and Arch-based distros

set -e

echo "Installing build dependencies for CopperOS..."
sudo pacman -S --needed --noconfirm \
    base-devel \
    qemu \
    grub \
    xorriso \
    nasm \
    gdb \
    clang \
    musl \
    curl \
    openssl \
    zlib \
    pkg-config \
    ncurses

echo "All dependencies installed."
echo "CopperOS includes a prebuilt BusyBox with the required custom settings."