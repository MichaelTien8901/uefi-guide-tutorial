---
layout: default
title: "Appendix D: References"
permalink: /appendices/d-references/
nav_order: 4
parent: "Appendices"
---

# Appendix D: References

Curated collection of specifications, documentation, and community resources for UEFI development.

## Official Specifications

### UEFI Forum Specifications

| Specification | Version | Description |
|---------------|---------|-------------|
| [UEFI Specification](https://uefi.org/specifications) | 2.10 | Core UEFI interface specification |
| [PI Specification](https://uefi.org/specifications) | 1.8 | Platform Initialization (SEC, PEI, DXE, SMM) |
| [ACPI Specification](https://uefi.org/specifications) | 6.5 | Advanced Configuration and Power Interface |
| [UEFI Shell Specification](https://uefi.org/specifications) | 2.2 | Shell environment and commands |
| [UEFI Secure Boot](https://uefi.org/specifications) | - | Secure Boot signing requirements |

### Industry Specifications

| Specification | Organization | Description |
|---------------|--------------|-------------|
| [SMBIOS](https://www.dmtf.org/standards/smbios) | DMTF | System Management BIOS |
| [IPMI](https://www.intel.com/content/www/us/en/products/docs/servers/ipmi/ipmi-second-gen-interface-spec-v2-rev1-1.html) | Intel | Intelligent Platform Management Interface |
| [Redfish](https://www.dmtf.org/standards/redfish) | DMTF | REST-based management API |
| [TCG TPM](https://trustedcomputinggroup.org/resource/tpm-library-specification/) | TCG | Trusted Platform Module |
| [PCIe](https://pcisig.com/specifications) | PCI-SIG | PCI Express specification |

### ARM Specifications

| Specification | Description |
|---------------|-------------|
| [ARM Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest) | ARMv8-A architecture |
| [PSCI](https://developer.arm.com/documentation/den0022/latest) | Power State Coordination Interface |
| [SMCCC](https://developer.arm.com/documentation/den0028/latest) | SMC Calling Convention |
| [BSA](https://developer.arm.com/documentation/den0094/latest) | Base System Architecture |
| [SBSA](https://developer.arm.com/documentation/den0029/latest) | Server Base System Architecture |
| [SystemReady](https://www.arm.com/architecture/system-architectures/systemready-certification-program) | Certification program |

## EDK2 Documentation

### Core Documentation

| Resource | Description |
|----------|-------------|
| [EDK2 GitHub](https://github.com/tianocore/edk2) | Main repository |
| [EDK2 Wiki](https://github.com/tianocore/tianocore.github.io/wiki) | TianoCore wiki |
| [EDK2 Specifications](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-Specifications) | Build file specifications |
| [EDK2 Platforms](https://github.com/tianocore/edk2-platforms) | Platform support packages |

### Package Documentation

| Package | Purpose |
|---------|---------|
| [MdePkg](https://github.com/tianocore/edk2/tree/master/MdePkg) | Core definitions, libraries |
| [MdeModulePkg](https://github.com/tianocore/edk2/tree/master/MdeModulePkg) | DXE drivers, BDS |
| [UefiCpuPkg](https://github.com/tianocore/edk2/tree/master/UefiCpuPkg) | CPU initialization |
| [SecurityPkg](https://github.com/tianocore/edk2/tree/master/SecurityPkg) | Secure Boot, TPM |
| [NetworkPkg](https://github.com/tianocore/edk2/tree/master/NetworkPkg) | Network stack |
| [ShellPkg](https://github.com/tianocore/edk2/tree/master/ShellPkg) | UEFI Shell |
| [ArmPkg](https://github.com/tianocore/edk2/tree/master/ArmPkg) | ARM support |
| [OvmfPkg](https://github.com/tianocore/edk2/tree/master/OvmfPkg) | QEMU/KVM firmware |

### Build Specifications (edk2-docs)

| Document | Content |
|----------|---------|
| [INF Specification](https://edk2-docs.gitbook.io/edk-ii-inf-specification/) | Module information file |
| [DSC Specification](https://edk2-docs.gitbook.io/edk-ii-dsc-specification/) | Platform description |
| [DEC Specification](https://edk2-docs.gitbook.io/edk-ii-dec-specification/) | Package declaration |
| [FDF Specification](https://edk2-docs.gitbook.io/edk-ii-fdf-specification/) | Flash description |
| [Build Specification](https://edk2-docs.gitbook.io/edk-ii-build-specification/) | Build process |

## Project Mu

| Resource | Description |
|----------|-------------|
| [Project Mu Documentation](https://microsoft.github.io/mu/) | Official docs |
| [mu_basecore](https://github.com/microsoft/mu_basecore) | Core fork |
| [mu_plus](https://github.com/microsoft/mu_plus) | Microsoft additions |
| [mu_tiano_plus](https://github.com/microsoft/mu_tiano_plus) | Tiano enhancements |
| [edk2-pytool](https://github.com/tianocore/edk2-pytool-extensions) | Stuart build system |

## Trusted Firmware

| Resource | Description |
|----------|-------------|
| [TF-A Documentation](https://trustedfirmware-a.readthedocs.io/) | Trusted Firmware-A |
| [TF-A GitHub](https://github.com/ARM-software/arm-trusted-firmware) | Source repository |
| [OP-TEE](https://optee.readthedocs.io/) | Open Portable TEE |
| [TF-M](https://trustedfirmware-m.readthedocs.io/) | TF for Cortex-M |

## Tools and Utilities

### Development Tools

| Tool | Purpose | Link |
|------|---------|------|
| QEMU | Emulation | [qemu.org](https://www.qemu.org/) |
| OVMF | QEMU UEFI firmware | [OVMF Wiki](https://github.com/tianocore/tianocore.github.io/wiki/OVMF) |
| IASL | ACPI compiler | [ACPICA](https://acpica.org/) |
| UEFITool | Firmware analysis | [GitHub](https://github.com/LongSoft/UEFITool) |
| Chipsec | Security analysis | [GitHub](https://github.com/chipsec/chipsec) |

### Signing Tools

| Tool | Purpose |
|------|---------|
| `sbsign` | Secure Boot signing (Linux) |
| `pesign` | PE signing (Linux) |
| `signtool` | Windows signing tool |
| OpenSSL | Key generation |

### Testing Tools

| Tool | Purpose |
|------|---------|
| UEFI SCT | Self Certification Tests |
| FWTS | Firmware Test Suite |
| ARM ACS | Architecture Compliance |

## Community Resources

### Forums and Mailing Lists

| Resource | Description |
|----------|-------------|
| [edk2-devel](https://edk2.groups.io/g/devel) | EDK2 development list |
| [edk2-discuss](https://edk2.groups.io/g/discuss) | General discussion |
| [UEFI Forum](https://uefi.org/) | Industry consortium |
| [OSDev Forums](https://forum.osdev.org/) | OS development community |

### Tutorials and Blogs

| Resource | Description |
|----------|-------------|
| [Firmware Security](https://firmwaresecurity.com/) | Security news and analysis |
| [TianoCore Training](https://github.com/tianocore-training) | UEFI training materials |
| [OSDev Wiki](https://wiki.osdev.org/UEFI) | UEFI development guide |

### Books

| Title | Author | Description |
|-------|--------|-------------|
| *Harnessing the UEFI Shell* | Rothman, Zimmer | Shell programming guide |
| *Beyond BIOS* | Zimmer, et al. | EDK2 development |
| *UEFI Implementation* | Vincent | Architecture details |

## Code Examples

### Official Examples

| Repository | Content |
|------------|---------|
| [edk2/MdeModulePkg/Application](https://github.com/tianocore/edk2/tree/master/MdeModulePkg/Application) | Reference applications |
| [edk2/ShellPkg/Application](https://github.com/tianocore/edk2/tree/master/ShellPkg/Application) | Shell commands |
| [edk2-libc](https://github.com/tianocore/edk2-libc) | POSIX applications |

### Community Projects

| Project | Description |
|---------|-------------|
| [rEFInd](https://www.rodsbooks.com/refind/) | Boot manager |
| [Clover](https://github.com/CloverHackyColor/CloverBootloader) | Boot loader |
| [systemd-boot](https://www.freedesktop.org/wiki/Software/systemd/systemd-boot/) | Systemd boot |
| [GRUB2](https://www.gnu.org/software/grub/) | GNU bootloader |

## Quick Links by Topic

### Getting Started
- [EDK2 Getting Started](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
- [OVMF Getting Started](https://github.com/tianocore/tianocore.github.io/wiki/How-to-run-OVMF)

### Driver Development
- [UEFI Driver Model](https://uefi.org/specs/UEFI/2.10/11_Protocols_UEFI_Driver_Model.html)
- [Driver Writer's Guide](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-Developers-Guides)

### Security
- [Secure Boot Overview](https://uefi.org/specs/UEFI/2.10/32_Secure_Boot_and_Driver_Signing.html)
- [TCG EFI Protocol](https://trustedcomputinggroup.org/resource/tcg-efi-protocol-specification/)

### ARM Development
- [EDK2 ArmPkg](https://github.com/tianocore/edk2/tree/master/ArmPkg)
- [ARM System Ready](https://www.arm.com/architecture/system-architectures/systemready-certification-program)

### Debugging
- [OVMF Debugging](https://github.com/tianocore/tianocore.github.io/wiki/How-to-debug-OVMF-with-QEMU-using-GDB)
- [Source Level Debug](https://github.com/tianocore/tianocore.github.io/wiki/SourceLevelDebugPkg)

## Version History

| UEFI Version | Release | Key Features |
|--------------|---------|--------------|
| 2.10 | 2022 | Confidential Computing |
| 2.9 | 2021 | RISC-V support |
| 2.8 | 2019 | HTTP Boot enhancements |
| 2.7 | 2017 | NVMe, SD/MMC |
| 2.6 | 2016 | REST Config |
| 2.5 | 2015 | Enhanced security |
