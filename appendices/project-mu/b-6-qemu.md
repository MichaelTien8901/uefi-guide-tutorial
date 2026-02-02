---
layout: default
title: "B.6 Running on QEMU"
permalink: /appendices/project-mu/qemu/
nav_order: 6
parent: "Appendix B: Project Mu"
grand_parent: "Appendices"
---

# Section B.6: Running on QEMU

Project Mu provides reference platforms for QEMU that enable testing without physical hardware.

---

## Building QEMU Q35 Platform

```bash
# Clone mu_tiano_platforms
git clone https://github.com/microsoft/mu_tiano_platforms.git
cd mu_tiano_platforms

# Setup and build
stuart_setup -c Platforms/QemuQ35Pkg/PlatformBuild.py
stuart_update -c Platforms/QemuQ35Pkg/PlatformBuild.py
stuart_build -c Platforms/QemuQ35Pkg/PlatformBuild.py TOOL_CHAIN_TAG=GCC5

# Output files
ls Build/QemuQ35Pkg/DEBUG_GCC5/FV/
# QEMUQ35_CODE.fd  - Firmware code
# QEMUQ35_VARS.fd  - Variable store
```

---

## Running with QEMU

### Basic Execution

```bash
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_VARS.fd \
    -serial stdio \
    -nographic
```

### With Debugging Enabled

```bash
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_VARS.fd \
    -serial stdio \
    -s -S  # GDB server on port 1234, pause at start
```

### With Virtual Disk for Testing Applications

```bash
# Create directory for EFI applications
mkdir hda-contents
cp MyApp.efi hda-contents/

# Run QEMU with FAT filesystem
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_VARS.fd \
    -drive format=raw,file=fat:rw:./hda-contents \
    -serial stdio
```

---

## Running on ARM SBSA Platform

```bash
# Build ARM SBSA platform
stuart_setup -c Platforms/QemuSbsaPkg/PlatformBuild.py
stuart_update -c Platforms/QemuSbsaPkg/PlatformBuild.py
stuart_build -c Platforms/QemuSbsaPkg/PlatformBuild.py TOOL_CHAIN_TAG=GCC5

# Run with QEMU
qemu-system-aarch64 \
    -machine sbsa-ref \
    -m 2G \
    -cpu max \
    -drive if=pflash,format=raw,readonly=on,file=Build/QemuSbsaPkg/DEBUG_GCC5/FV/SBSA_CODE.fd \
    -drive if=pflash,format=raw,file=Build/QemuSbsaPkg/DEBUG_GCC5/FV/SBSA_VARS.fd \
    -serial stdio \
    -nographic
```

---

## QEMU Options Reference

| Option | Description |
|:-------|:------------|
| `-machine q35,smm=on` | Use Q35 chipset with SMM support |
| `-m 256M` | Allocate 256MB RAM |
| `-drive if=pflash,...` | Flash drive for firmware |
| `-serial stdio` | Redirect serial to terminal |
| `-nographic` | No graphical display |
| `-s` | Enable GDB server on port 1234 |
| `-S` | Pause CPU at startup |
| `-drive format=raw,file=fat:rw:./dir` | Create FAT filesystem from directory |

---

## Debugging with GDB

```bash
# Terminal 1: Start QEMU with GDB server
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=Build/QemuQ35Pkg/DEBUG_GCC5/FV/QEMUQ35_VARS.fd \
    -serial stdio \
    -s -S

# Terminal 2: Connect GDB
gdb
(gdb) target remote localhost:1234
(gdb) symbol-file Build/QemuQ35Pkg/DEBUG_GCC5/X64/MyDriver.debug
(gdb) break DriverEntryPoint
(gdb) continue
```

---

## Common QEMU Configurations

### With Network Support

```bash
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=QEMUQ35_VARS.fd \
    -netdev user,id=net0 \
    -device e1000,netdev=net0 \
    -serial stdio
```

### With USB Support

```bash
qemu-system-x86_64 \
    -machine q35,smm=on \
    -m 256M \
    -drive if=pflash,format=raw,readonly=on,file=QEMUQ35_CODE.fd \
    -drive if=pflash,format=raw,file=QEMUQ35_VARS.fd \
    -usb \
    -device usb-storage,drive=usb0 \
    -drive id=usb0,format=raw,file=usb.img \
    -serial stdio
```

---

## Next Section

[B.7 Migration from EDK2]({% link appendices/project-mu/b-7-migration.md %})
