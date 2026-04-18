#!/bin/bash
# CopperOS dependency installer for dnf systems

sudo dnf install -y \
    make \
    gcc \
    qemu-system-x86 \
    grub2-tools \
    xorriso \
    nasm \
    gdb \
    clang \
    musl-gcc \
    curl-devel \
    openssl-devel \
    zlib-devel \
    pkgconfig \
    glibc-static \
    libstdc++-static \
    ncurses-devel \
    ncurses-compat-libs

# Some distros might need these additional packages
sudo dnf install -y \
    wget \
    bc \
    cpio \
    diffutils \
    findutils \
    which

echo "All dependencies installed."
echo "CopperOS includes a prebuilt BusyBox with:"
echo "  - Traffic Control (tc) disabled"
echo "  - Standalone shell enabled"
echo "  - Prefer applets enabled"