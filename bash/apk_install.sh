#the only reason i added this was because i wanted to test it on my iPhone with iSH

set -e

echo "Installing build dependencies for CopperOS..."
apk update
apk add \
    build-base \
    qemu-system-x86_64 \
    grub-pc \
    xorriso \
    nasm \
    gdb \
    clang \
    musl-dev \
    binutils \
    libcurl-dev \
    openssl-dev \
    zlib-dev \
    pkgconfig \
    ncurses-dev

echo "All dependencies installed."
echo "CopperOS includes a prebuilt BusyBox with:"
echo "  - Traffic Control (tc) disabled"
echo "  - Standalone shell enabled"
echo "  - Prefer applets enabled"