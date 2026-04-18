#!/bin/bash
#installs every package needed to run the OS on apt systems

set -e

echo "Installing build dependencies for CopperOS..."
sudo apt update
sudo apt install -y \
    build-essential \
    qemu-system-x86 \
    grub-pc-bin \
    grub-common \
    xorriso \
    nasm \
    gdb \
    clang \
    musl-tools \
    binutils \
    libcurl4-openssl-dev \
    libssl-dev \
    zlib1g-dev \
    pkg-config \
    libncursesw6 \
    libncurses-dev

echo "All dependencies installed."
echo "CopperOS includes a prebuilt BusyBox with:"
echo "  - Traffic Control (tc) disabled"
echo "  - Standalone shell enabled"
echo "  - Prefer applets enabled"