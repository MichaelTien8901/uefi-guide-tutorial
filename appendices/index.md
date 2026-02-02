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
| **[A. EDK2 Build System](a-edk2-build-system.html)** | INF, DEC, DSC, FDF files and build commands |
| **[B. Project Mu](b-project-mu.html)** | Microsoft's UEFI core, stuart build system |
| **[C. Debugging](c-debugging.html)** | DEBUG macros, serial output, GDB setup |
| **[D. References](d-references.html)** | Specifications, documentation, community links |
| **[E. Glossary](e-glossary.html)** | UEFI terminology and error codes |

## Quick Reference

### Common EFI_STATUS Codes

| Code | Value | Meaning |
|:-----|:------|:--------|
| `EFI_SUCCESS` | 0 | Operation completed successfully |
| `EFI_NOT_FOUND` | 14 | Item not found |
| `EFI_OUT_OF_RESOURCES` | 9 | Resource allocation failed |
| `EFI_INVALID_PARAMETER` | 2 | Invalid parameter passed |
| `EFI_UNSUPPORTED` | 3 | Operation not supported |
| `EFI_BUFFER_TOO_SMALL` | 5 | Buffer too small for data |

### EDK2 Module Types

| MODULE_TYPE | Description |
|:------------|:------------|
| `UEFI_APPLICATION` | Standalone UEFI application |
| `UEFI_DRIVER` | UEFI driver (loads and stays resident) |
| `DXE_DRIVER` | DXE phase driver |
| `DXE_RUNTIME_DRIVER` | Runtime driver (survives ExitBootServices) |
| `PEIM` | PEI Module |
| `SEC` | Security phase module |
| `SMM_CORE` | SMM Core |
| `DXE_SMM_DRIVER` | SMM driver |

### Useful UEFI Shell Commands

```shell
# Memory map
memmap

# Display handles and protocols
dh -v

# Show UEFI variables
dmpstore

# Device tree
devtree

# Driver list
drivers

# Boot options
bcfg boot dump
```

---

{: .note }
> These appendices serve as quick references while working through the tutorials.
