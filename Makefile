# Check for OS, if not macos assume linux
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	shasum = shasum -a 512
else
	shasum = sha512sum
endif

IMAGE=ghcr.io/tillitis/tkey-builder:4

OBJCOPY ?= llvm-objcopy

P := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
LIBDIR ?= $(P)/../tkey-libs

CC = clang

INCLUDE = $(LIBDIR)/include

# If you want libcommon's qemu_puts() et cetera to output something on our QEMU
# debug port, use -DQEMU_DEBUG below
CFLAGS = -target riscv32-unknown-none-elf -march=rv32iczmmul -mabi=ilp32 -mcmodel=medany \
   -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf \
   -fno-builtin-putchar -nostdlib -mno-relax -flto -g \
   -Wall -Werror=implicit-function-declaration \
   -I $(INCLUDE) -I $(LIBDIR) #-DQEMU_DEBUG

ifneq ($(TKEY_SIGNER_APP_NO_TOUCH),)
CFLAGS := $(CFLAGS) -DTKEY_SIGNER_APP_NO_TOUCH
endif

AS = clang
ASFLAGS = -target riscv32-unknown-none-elf -march=rv32iczmmul -mabi=ilp32 -mcmodel=medany -mno-relax

LDFLAGS=-T $(LIBDIR)/app.lds -L $(LIBDIR) -lcommon -lcrt0


.PHONY: all
all: signer/app.bin check-signer-hash

# Create compile_commands.json for clangd and LSP
.PHONY: clangd
clangd: compile_commands.json
compile_commands.json:
	$(MAKE) clean
	bear -- make signer/app.bin

# Turn elf into bin for device
%.bin: %.elf
	$(OBJCOPY) --input-target=elf32-littleriscv --output-target=binary $^ $@
	chmod a-x $@

show-%-hash: %/app.bin
	@echo "Device app digest:"
	@$(shasum) $$(dirname $^)/app.bin

check-signer-hash: signer/app.bin show-signer-hash
	@echo "Expected device app digest: "
	@cat signer/app.bin.sha512
	$(shasum) -c signer/app.bin.sha512

CLANG_TIDY = clang-tidy

.PHONY: check
check:
	$(CLANG_TIDY) -header-filter=.* -checks=cert-* signer/*.[ch] -- $(CFLAGS)

# Simple ed25519 signer app
SIGNEROBJS=signer/main.o signer/app_proto.o
signer/app.elf: $(SIGNEROBJS)
	$(CC) $(CFLAGS) $(SIGNEROBJS) $(LDFLAGS) -L $(LIBDIR)/monocypher -lmonocypher -I $(LIBDIR) -o $@
$(SIGNEROBJS): $(INCLUDE)/tkey/tk1_mem.h signer/app_proto.h

.PHONY: clean
clean:
	rm -f signer/app.bin signer/app.elf $(SIGNEROBJS)

# Uses ../.clang-format
FMTFILES=signer/*.[ch]

.PHONY: fmt
fmt:
	clang-format --dry-run --ferror-limit=0 $(FMTFILES)
	clang-format --verbose -i $(FMTFILES)
.PHONY: checkfmt
checkfmt:
	clang-format --dry-run --ferror-limit=0 --Werror $(FMTFILES)

.PHONY: podman
podman:
	podman run --arch=amd64 --rm --mount type=bind,source=$(CURDIR),target=/src --mount type=bind,source=$(LIBDIR),target=/tkey-libs -w /src -it $(IMAGE) make -j
