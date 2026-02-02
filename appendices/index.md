---
layout: default
title: "Appendices"
nav_order: 7
has_children: true
permalink: /appendices/
---

# Appendices
{: .fs-9 }

Reference materials, tools, and additional resources.
{: .fs-6 .fw-300 }

---

## Contents

| Appendix | Description |
|:---------|:------------|
| **[A. EDK2 Build System](a-edk2-build-system/)** | INF, DEC, DSC, FDF files and build commands |
| **[B. Project Mu](b-project-mu/)** | Microsoft's UEFI core, stuart build system |
| **[C. Debugging](c-debugging/)** | DEBUG macros, serial output, GDB setup |
| **[D. References](d-references/)** | Specifications, documentation, community links |
| **[E. Glossary](e-glossary/)** | UEFI terminology and error codes |
| **[F. ARM Quick Reference](f-arm-reference/)** | ARM registers, PSCI, GIC, SystemReady checklist |

## Quick Reference Cards

### Common EFI_STATUS Codes

| Code | Value | Meaning |
|:-----|:------|:--------|
| `EFI_SUCCESS` | 0 | Operation completed successfully |
| `EFI_LOAD_ERROR` | 1 | Image load failed |
| `EFI_INVALID_PARAMETER` | 2 | Invalid parameter passed |
| `EFI_UNSUPPORTED` | 3 | Operation not supported |
| `EFI_BAD_BUFFER_SIZE` | 4 | Buffer not correct size |
| `EFI_BUFFER_TOO_SMALL` | 5 | Buffer too small for data |
| `EFI_NOT_READY` | 6 | No data pending |
| `EFI_DEVICE_ERROR` | 7 | Physical device error |
| `EFI_WRITE_PROTECTED` | 8 | Device write protected |
| `EFI_OUT_OF_RESOURCES` | 9 | Resource allocation failed |
| `EFI_NOT_FOUND` | 14 | Item not found |
| `EFI_ACCESS_DENIED` | 15 | Access denied |
| `EFI_TIMEOUT` | 18 | Operation timed out |
| `EFI_ALREADY_STARTED` | 20 | Protocol already started |
| `EFI_ABORTED` | 21 | Operation aborted |
| `EFI_SECURITY_VIOLATION` | 26 | Security violation |

### EDK2 Module Types

| MODULE_TYPE | Phase | Description |
|:------------|:------|:------------|
| `SEC` | SEC | Security/startup module |
| `PEI_CORE` | PEI | PEI Foundation |
| `PEIM` | PEI | PEI Module |
| `DXE_CORE` | DXE | DXE Foundation |
| `DXE_DRIVER` | DXE | DXE phase driver |
| `DXE_RUNTIME_DRIVER` | DXE/RT | Runtime driver |
| `DXE_SMM_DRIVER` | DXE/SMM | SMM driver |
| `SMM_CORE` | SMM | SMM Foundation |
| `UEFI_DRIVER` | DXE | UEFI driver model driver |
| `UEFI_APPLICATION` | DXE | UEFI application |

### Memory Types

| Type | Description | Persistence |
|:-----|:------------|:------------|
| `EfiLoaderCode` | Loader code | Freed after ExitBootServices |
| `EfiLoaderData` | Loader data | Freed after ExitBootServices |
| `EfiBootServicesCode` | Boot services code | Freed after ExitBootServices |
| `EfiBootServicesData` | Boot services data | Freed after ExitBootServices |
| `EfiRuntimeServicesCode` | Runtime code | Preserved |
| `EfiRuntimeServicesData` | Runtime data | Preserved |
| `EfiConventionalMemory` | Free memory | Available |
| `EfiACPIReclaimMemory` | ACPI tables | Reclaimable |
| `EfiACPIMemoryNVS` | ACPI NVS | Preserved |
| `EfiReservedMemoryType` | Reserved | Not usable |

### Useful UEFI Shell Commands

```bash
# System information
ver                    # UEFI version
memmap                 # Memory map
smbiosview             # SMBIOS tables
acpiview               # ACPI tables

# Device/Driver info
dh -v                  # Handles and protocols
drivers                # Loaded drivers
devices                # Device list
devtree                # Device tree

# Boot configuration
bcfg boot dump         # Show boot options
bcfg boot add 0 fs0:\app.efi "My App"
bcfg boot rm 0         # Remove boot option

# Variables
dmpstore               # Dump all variables
setvar Name -guid GUID -bs -rt =L"Value"

# File operations
ls fs0:\               # List directory
cp file1 file2         # Copy file
type file.txt          # Display file
edit file.txt          # Edit file

# Execution
fs0:\app.efi           # Run application
load fs0:\driver.efi   # Load driver
unload handle          # Unload driver
```

### Build Commands

```bash
# Setup environment
source edksetup.sh     # Linux/macOS
edksetup.bat           # Windows

# Build commands
build                           # Build default target
build -a X64                    # Build for X64
build -a X64 -t GCC5           # Use GCC5 toolchain
build -a X64 -p Pkg/Pkg.dsc    # Build specific platform
build -a X64 -m Pkg/Mod.inf    # Build specific module
build -a X64 -b RELEASE        # Release build
build -a X64 -b DEBUG          # Debug build
build clean                     # Clean build

# Common options
-a ARCH      # Architecture (IA32, X64, AARCH64, ARM)
-t TOOL      # Toolchain (GCC5, VS2019, CLANG38)
-p DSC       # Platform description file
-m INF       # Module information file
-b TARGET    # Build target (DEBUG, RELEASE, NOOPT)
-n THREADS   # Number of build threads
-y FILE      # Build report file
```

---

{: .note }
> These appendices serve as quick references while working through the tutorials.
