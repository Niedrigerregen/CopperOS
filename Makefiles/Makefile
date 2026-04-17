# CopperOS Makefile
# Most stuff is statically linked, but search needs dynamic linking because libcurl is a pain

CC = musl-gcc
CFLAGS_STATIC = -static -O2 -s -I$(CURDIR)/include

SRC_DIR = $(CURDIR)/src
ROOTFS  = $(CURDIR)/rootfs
BIN_DIR = $(ROOTFS)/bin
INIT = $(ROOTFS)/init

INIT_SRC = $(SRC_DIR)/init.c $(SRC_DIR)/cd.c
SPECIAL_SRC = $(SRC_DIR)/search.c

COMMAND_SRC := $(filter-out \
	$(INIT_SRC) \
	$(SRC_DIR)/shell.c \
	$(SPECIAL_SRC), \
	$(wildcard $(SRC_DIR)/*.c))

# Package manager source and binary
PKG_MGR_SRC = $(CURDIR)/pkg_manager/src/cop_mng.c
PKG_MGR_BIN = $(BIN_DIR)/cop

.PHONY: all dirs init commands search busybox copy-libs cpio clean pkg terminfo

all: dirs init commands pkg search busybox copy-libs terminfo cpio

cpio:
	cd $(ROOTFS) && find . | cpio -o -H newc > $(CURDIR)/iso/boot/rootfs.cpio
	rm -rf $(ROOTFS)

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(ROOTFS)/data
	mkdir -p $(ROOTFS)/lib
	mkdir -p $(ROOTFS)/lib64
	mkdir -p $(ROOTFS)/dev
	mkdir -p $(ROOTFS)/proc
	mkdir -p $(ROOTFS)/sys
	mkdir -p $(ROOTFS)/tmp
	mkdir -p $(ROOTFS)/etc
	mkdir -p $(ROOTFS)/usr/share/terminfo
	cp $(CURDIR)/data/quotes.txt $(ROOTFS)/data/
	cp $(CURDIR)/data/srch_aliases.txt $(ROOTFS)/data/
	cp $(CURDIR)/data/cop_aliases.txt $(ROOTFS)/data/

init:
	$(CC) $(CFLAGS_STATIC) $(INIT_SRC) -o $(INIT)

commands:
	@for srcfile in $(COMMAND_SRC); do \
		prog=$$(basename $$srcfile .c); \
		$(CC) $(CFLAGS_STATIC) $$srcfile -o $(BIN_DIR)/$$prog; \
		if [ $$? -ne 0 ]; then echo "Error compiling $$prog"; exit 1; fi; \
	done
	$(CC) $(CFLAGS_STATIC) $(SRC_DIR)/shell.c $(SRC_DIR)/cd.c -o $(BIN_DIR)/shell

pkg: $(PKG_MGR_BIN)

$(PKG_MGR_BIN): $(PKG_MGR_SRC)
	@if [ ! -f $(PKG_MGR_SRC) ]; then \
		echo "Error: $(PKG_MGR_SRC) not found"; \
		exit 1; \
	fi
	gcc -O2 -s -I$(CURDIR)/include $(PKG_MGR_SRC) -o $(PKG_MGR_BIN) -lcurl

# Use the prebuilt BusyBox from tools
busybox:
	@echo "Using prebuilt BusyBox from tools/"
	@if [ ! -f $(CURDIR)/tools/busybox ]; then \
		echo "Error: tools/busybox not found. Please ensure the custom BusyBox is present."; \
		exit 1; \
	fi
	cp $(CURDIR)/tools/busybox $(BIN_DIR)/busybox
	@# Create symlinks for the commands needed
	@ln -sf busybox $(BIN_DIR)/ip
	@ln -sf busybox $(BIN_DIR)/udhcpc
	@ln -sf busybox $(BIN_DIR)/ifconfig
	@ln -sf busybox $(BIN_DIR)/route
	@ln -sf busybox $(BIN_DIR)/sh
	@ln -sf busybox $(BIN_DIR)/ash
	@echo "BusyBox added."

# first time linking dynamically - kinda nervous https://tenor.com/view/shy-girl-maid-fingers-nervous-fingers-shying-gif-17287029592820735835
search:
	gcc -O2 -s -I$(CURDIR)/include $(SRC_DIR)/search.c -o $(BIN_DIR)/search -lcurl

copy-libs:
	@echo "Copying dynamic libraries for search..."
	@mkdir -p $(ROOTFS)/lib
	@mkdir -p $(ROOTFS)/lib64
	@ldd $(BIN_DIR)/search 2>/dev/null | grep "=> /" | awk '{print $$3}' | while read lib; do \
		if [ -n "$$lib" ] && [ -f "$$lib" ]; then \
			echo "  Copying $$lib"; \
			cp "$$lib" $(ROOTFS)/lib/; \
		fi \
	done
	@# Copy dynamic linker
	@if [ -f /lib64/ld-linux-x86-64.so.2 ]; then \
		echo "  Copying /lib64/ld-linux-x86-64.so.2 to lib64/"; \
		cp /lib64/ld-linux-x86-64.so.2 $(ROOTFS)/lib64/; \
	elif [ -f /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 ]; then \
		echo "  Copying /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 to lib64/"; \
		cp /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 $(ROOTFS)/lib64/; \
	else \
		echo "  WARNING: Could not find dynamic linker"; \
	fi
	@echo "Library copy complete."

terminfo:
	@cp -r /usr/share/terminfo/* $(ROOTFS)/usr/share/terminfo/ 2>/dev/null || true

clean:
	rm -rf $(ROOTFS) $(CURDIR)/iso/boot/rootfs.cpio