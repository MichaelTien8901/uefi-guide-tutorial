---
layout: default
title: "Appendix E: Glossary"
permalink: /appendices/e-glossary/
nav_order: 5
parent: "Appendices"
---

# Appendix E: Glossary

Comprehensive glossary of UEFI terminology, acronyms, and error codes.

## UEFI Terminology

### A

| Term | Definition |
|------|------------|
| **ACPI** | Advanced Configuration and Power Interface - power management and hardware discovery |
| **AML** | ACPI Machine Language - bytecode for ACPI control methods |
| **ASL** | ACPI Source Language - high-level language compiled to AML |

### B

| Term | Definition |
|------|------------|
| **BDS** | Boot Device Selection - phase that discovers boot devices and launches OS |
| **BL1** | Boot Loader stage 1 (ARM) - ROM-resident trusted boot |
| **BL2** | Boot Loader stage 2 (ARM) - trusted boot loader |
| **BL31** | EL3 Runtime Firmware (ARM) - secure monitor |
| **BL32** | Secure-EL1 Payload (ARM) - optional secure OS |
| **BL33** | Non-secure EL2/1 Payload (ARM) - usually UEFI |
| **BMC** | Baseboard Management Controller - out-of-band management |
| **Boot Services** | Functions available before ExitBootServices() |
| **BSA** | Base System Architecture (ARM) - hardware requirements |
| **BSP** | Board Support Package / Bootstrap Processor |

### C

| Term | Definition |
|------|------------|
| **Capsule** | Firmware update package delivered to UEFI |
| **ConIn** | Console Input device handle |
| **ConOut** | Console Output device handle |

### D

| Term | Definition |
|------|------------|
| **DEC** | Package Declaration file (.dec) |
| **DSC** | Platform Description file (.dsc) |
| **DXE** | Driver Execution Environment - main firmware phase |
| **DXE Core** | Central dispatcher for DXE phase |
| **DXE Driver** | Driver loaded during DXE phase |

### E

| Term | Definition |
|------|------------|
| **EDK2** | EFI Development Kit II - open source UEFI implementation |
| **EFI** | Extensible Firmware Interface (predecessor to UEFI) |
| **EL0-EL3** | ARM Exception Levels (0=User, 1=OS, 2=Hypervisor, 3=Secure Monitor) |
| **ESP** | EFI System Partition - FAT partition for boot files |
| **Event** | Asynchronous notification mechanism in UEFI |

### F

| Term | Definition |
|------|------------|
| **FD** | Flash Device - binary firmware image |
| **FDF** | Flash Description File (.fdf) |
| **FFS** | Firmware File System - file system within FV |
| **FIP** | Firmware Image Package (ARM TF-A) |
| **FIT** | Firmware Interface Table |
| **FV** | Firmware Volume - container for firmware files |
| **FVB** | Firmware Volume Block protocol |

### G

| Term | Definition |
|------|------------|
| **GCD** | Global Coherency Domain - memory/IO space management |
| **GIC** | Generic Interrupt Controller (ARM) |
| **GOP** | Graphics Output Protocol - graphical display |
| **GPT** | GUID Partition Table - disk partitioning scheme |
| **GUID** | Globally Unique Identifier - 128-bit identifier |

### H

| Term | Definition |
|------|------------|
| **Handle** | Opaque pointer to a collection of protocols |
| **Handle Database** | Collection of all handles and protocols |
| **HII** | Human Interface Infrastructure - UI framework |
| **HOB** | Hand-Off Block - data passed between phases |
| **HVC** | Hypervisor Call (ARM EL2) |

### I

| Term | Definition |
|------|------------|
| **INF** | Module Information file (.inf) |
| **Image** | Executable loaded by UEFI (PE/COFF format) |
| **IPMI** | Intelligent Platform Management Interface |

### L

| Term | Definition |
|------|------------|
| **Library Class** | Abstract interface for code reuse |
| **Library Instance** | Concrete implementation of library class |
| **LoadedImage** | Protocol describing a loaded UEFI image |

### M

| Term | Definition |
|------|------------|
| **MdePkg** | Module Development Environment Package - core EDK2 |
| **MdeModulePkg** | Core UEFI modules package |
| **Memory Map** | Description of system memory layout |
| **MMIO** | Memory-Mapped I/O |

### N

| Term | Definition |
|------|------------|
| **NVRAM** | Non-Volatile RAM - stores UEFI variables |

### O

| Term | Definition |
|------|------------|
| **OVMF** | Open Virtual Machine Firmware - QEMU UEFI |

### P

| Term | Definition |
|------|------------|
| **PCD** | Platform Configuration Database - build-time/runtime config |
| **PE/COFF** | Portable Executable - UEFI executable format |
| **PEI** | Pre-EFI Initialization - early boot phase |
| **PEIM** | PEI Module |
| **PI** | Platform Initialization - lower-level specification |
| **PPI** | PEI-to-PEI Interface - services in PEI phase |
| **Protocol** | Interface exposed via handle database |
| **PSCI** | Power State Coordination Interface (ARM) |

### R

| Term | Definition |
|------|------------|
| **RAS** | Reliability, Availability, Serviceability |
| **RSDP** | Root System Description Pointer (ACPI) |
| **Runtime Driver** | Driver persisting after ExitBootServices() |
| **Runtime Services** | Functions available after ExitBootServices() |

### S

| Term | Definition |
|------|------------|
| **SBSA** | Server Base System Architecture (ARM) |
| **SCT** | Self Certification Test - UEFI compliance tests |
| **SEC** | Security phase - first code executed |
| **Secure Boot** | Signature verification for boot images |
| **SMC** | Secure Monitor Call (ARM EL3) |
| **SMBIOS** | System Management BIOS - hardware info tables |
| **SMM** | System Management Mode (x86) |
| **SMM Driver** | Driver running in SMM |
| **StdErr** | Standard Error output |

### T

| Term | Definition |
|------|------------|
| **TF-A** | Trusted Firmware-A (ARM reference implementation) |
| **TPL** | Task Priority Level - interrupt priority |
| **TPM** | Trusted Platform Module - security coprocessor |
| **TrustZone** | ARM security architecture |
| **TSL** | Transient System Load - boot phase |

### U

| Term | Definition |
|------|------------|
| **UEFI** | Unified Extensible Firmware Interface |
| **UEFI Application** | Standalone executable (exits after use) |
| **UEFI Driver** | Persistent module following driver model |

### V

| Term | Definition |
|------|------------|
| **Variable** | Persistent name-value pair in NVRAM |
| **VFR** | Visual Forms Representation - HII forms language |

## EFI_STATUS Codes

### Success Codes

| Code | Value | Description |
|------|-------|-------------|
| `EFI_SUCCESS` | 0 | Operation completed successfully |

### Error Codes (High bit set)

| Code | Value | Description |
|------|-------|-------------|
| `EFI_LOAD_ERROR` | 1 | Image failed to load |
| `EFI_INVALID_PARAMETER` | 2 | Parameter is incorrect |
| `EFI_UNSUPPORTED` | 3 | Operation not supported |
| `EFI_BAD_BUFFER_SIZE` | 4 | Buffer is wrong size |
| `EFI_BUFFER_TOO_SMALL` | 5 | Buffer is too small |
| `EFI_NOT_READY` | 6 | No data pending |
| `EFI_DEVICE_ERROR` | 7 | Physical device error |
| `EFI_WRITE_PROTECTED` | 8 | Cannot write to device |
| `EFI_OUT_OF_RESOURCES` | 9 | Resource allocation failed |
| `EFI_VOLUME_CORRUPTED` | 10 | File system is corrupted |
| `EFI_VOLUME_FULL` | 11 | No space on volume |
| `EFI_NO_MEDIA` | 12 | No medium in device |
| `EFI_MEDIA_CHANGED` | 13 | Medium has changed |
| `EFI_NOT_FOUND` | 14 | Item was not found |
| `EFI_ACCESS_DENIED` | 15 | Access was denied |
| `EFI_NO_RESPONSE` | 16 | Server not responding |
| `EFI_NO_MAPPING` | 17 | No mapping exists |
| `EFI_TIMEOUT` | 18 | Operation timed out |
| `EFI_NOT_STARTED` | 19 | Protocol not started |
| `EFI_ALREADY_STARTED` | 20 | Protocol already started |
| `EFI_ABORTED` | 21 | Operation was aborted |
| `EFI_ICMP_ERROR` | 22 | ICMP error received |
| `EFI_TFTP_ERROR` | 23 | TFTP error received |
| `EFI_PROTOCOL_ERROR` | 24 | Protocol error occurred |
| `EFI_INCOMPATIBLE_VERSION` | 25 | Version incompatibility |
| `EFI_SECURITY_VIOLATION` | 26 | Security check failed |
| `EFI_CRC_ERROR` | 27 | CRC check failed |
| `EFI_END_OF_MEDIA` | 28 | End of media reached |
| `EFI_END_OF_FILE` | 31 | End of file reached |
| `EFI_INVALID_LANGUAGE` | 32 | Language not supported |
| `EFI_COMPROMISED_DATA` | 33 | Security compromised |
| `EFI_IP_ADDRESS_CONFLICT` | 34 | IP address conflict |
| `EFI_HTTP_ERROR` | 35 | HTTP error occurred |

### Warning Codes

| Code | Value | Description |
|------|-------|-------------|
| `EFI_WARN_UNKNOWN_GLYPH` | 1 | Glyph not found |
| `EFI_WARN_DELETE_FAILURE` | 2 | Delete operation failed |
| `EFI_WARN_WRITE_FAILURE` | 3 | Write operation failed |
| `EFI_WARN_BUFFER_TOO_SMALL` | 4 | Buffer is too small |
| `EFI_WARN_STALE_DATA` | 5 | Data may be stale |
| `EFI_WARN_FILE_SYSTEM` | 6 | File system warning |
| `EFI_WARN_RESET_REQUIRED` | 7 | Reset is required |

### Error Handling Macros

```c
// Check if status is an error
#define EFI_ERROR(Status)  (((INTN)(RETURN_STATUS)(Status)) < 0)

// Common pattern
EFI_STATUS Status;
Status = SomeFunction();
if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed: %r\n", Status));
    return Status;
}

// Using ASSERT
ASSERT_EFI_ERROR(Status);  // Halts on error in DEBUG builds
```

## Memory Types

| Type | Value | Description | After ExitBootServices |
|------|-------|-------------|------------------------|
| `EfiReservedMemoryType` | 0 | Not usable | Reserved |
| `EfiLoaderCode` | 1 | UEFI app code | Available |
| `EfiLoaderData` | 2 | UEFI app data | Available |
| `EfiBootServicesCode` | 3 | Boot services code | Available |
| `EfiBootServicesData` | 4 | Boot services data | Available |
| `EfiRuntimeServicesCode` | 5 | Runtime code | Preserved |
| `EfiRuntimeServicesData` | 6 | Runtime data | Preserved |
| `EfiConventionalMemory` | 7 | Free memory | Available |
| `EfiUnusableMemory` | 8 | Error-prone memory | Unusable |
| `EfiACPIReclaimMemory` | 9 | ACPI tables | Reclaimable |
| `EfiACPIMemoryNVS` | 10 | ACPI NVS | Preserved |
| `EfiMemoryMappedIO` | 11 | MMIO | Mapped |
| `EfiMemoryMappedIOPortSpace` | 12 | MMIO port | Mapped |
| `EfiPalCode` | 13 | PAL code (Itanium) | Preserved |
| `EfiPersistentMemory` | 14 | Persistent memory | Preserved |
| `EfiUnacceptedMemoryType` | 15 | TDX unaccepted | Must accept |

### Memory Attributes

| Attribute | Bit | Description |
|-----------|-----|-------------|
| `EFI_MEMORY_UC` | 0 | Uncacheable |
| `EFI_MEMORY_WC` | 1 | Write Combining |
| `EFI_MEMORY_WT` | 2 | Write Through |
| `EFI_MEMORY_WB` | 3 | Write Back |
| `EFI_MEMORY_UCE` | 4 | Uncacheable Exported |
| `EFI_MEMORY_WP` | 12 | Write Protected |
| `EFI_MEMORY_RP` | 13 | Read Protected |
| `EFI_MEMORY_XP` | 14 | Execute Protected |
| `EFI_MEMORY_NV` | 15 | Non-Volatile |
| `EFI_MEMORY_MORE_RELIABLE` | 16 | More reliable |
| `EFI_MEMORY_RO` | 17 | Read Only |
| `EFI_MEMORY_SP` | 18 | Specific Purpose |
| `EFI_MEMORY_CPU_CRYPTO` | 19 | CPU Crypto |
| `EFI_MEMORY_RUNTIME` | 63 | Runtime mapping needed |

## Boot Phase Events

### TPL Levels

| Level | Value | Description |
|-------|-------|-------------|
| `TPL_APPLICATION` | 4 | Normal application level |
| `TPL_CALLBACK` | 8 | Callback priority |
| `TPL_NOTIFY` | 16 | Notification level |
| `TPL_HIGH_LEVEL` | 31 | Highest priority (interrupts disabled) |

### Event Types

| Event | Description |
|-------|-------------|
| `EVT_TIMER` | Timer-based event |
| `EVT_RUNTIME` | Survives ExitBootServices |
| `EVT_NOTIFY_WAIT` | Checked via WaitForEvent |
| `EVT_NOTIFY_SIGNAL` | Notification on signal |
| `EVT_SIGNAL_EXIT_BOOT_SERVICES` | ExitBootServices notification |
| `EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE` | SetVirtualAddressMap notification |

## Protocol GUIDs

### Common System Protocols

| Protocol | Description |
|----------|-------------|
| `gEfiLoadedImageProtocolGuid` | Information about loaded image |
| `gEfiDevicePathProtocolGuid` | Device path for a handle |
| `gEfiSimpleTextInProtocolGuid` | Text input (keyboard) |
| `gEfiSimpleTextOutProtocolGuid` | Text output (console) |
| `gEfiGraphicsOutputProtocolGuid` | Graphics framebuffer |
| `gEfiBlockIoProtocolGuid` | Block device access |
| `gEfiDiskIoProtocolGuid` | Disk I/O with byte offsets |
| `gEfiSimpleFileSystemProtocolGuid` | File system access |
| `gEfiUnicodeCollationProtocolGuid` | String comparison |

### Driver Protocols

| Protocol | Description |
|----------|-------------|
| `gEfiDriverBindingProtocolGuid` | Driver binding interface |
| `gEfiComponentNameProtocolGuid` | Component name (obsolete) |
| `gEfiComponentName2ProtocolGuid` | Component name (current) |
| `gEfiDriverDiagnosticsProtocolGuid` | Driver diagnostics |
| `gEfiDriverConfiguration2ProtocolGuid` | Driver configuration |
| `gEfiPlatformDriverOverrideProtocolGuid` | Driver override |

### Network Protocols

| Protocol | Description |
|----------|-------------|
| `gEfiSimpleNetworkProtocolGuid` | Raw network access |
| `gEfiManagedNetworkProtocolGuid` | Managed network |
| `gEfiArpProtocolGuid` | ARP protocol |
| `gEfiIp4ProtocolGuid` | IPv4 protocol |
| `gEfiTcp4ProtocolGuid` | TCPv4 protocol |
| `gEfiUdp4ProtocolGuid` | UDPv4 protocol |
| `gEfiDhcp4ProtocolGuid` | DHCPv4 client |
| `gEfiDns4ProtocolGuid` | DNS resolver |
| `gEfiHttpProtocolGuid` | HTTP client |

### Security Protocols

| Protocol | Description |
|----------|-------------|
| `gEfiSecurityArchProtocolGuid` | Security architecture |
| `gEfiSecurity2ArchProtocolGuid` | Security2 architecture |
| `gEfiTcg2ProtocolGuid` | TPM 2.0 interface |
| `gEfiRngProtocolGuid` | Random number generator |
| `gEfiHashProtocolGuid` | Hash algorithms |
| `gEfiHash2ProtocolGuid` | Hash2 algorithms |
| `gEfiPkcs7VerifyProtocolGuid` | PKCS7 verification |

## Variable Attributes

| Attribute | Value | Description |
|-----------|-------|-------------|
| `EFI_VARIABLE_NON_VOLATILE` | 0x00000001 | Stored in NVRAM |
| `EFI_VARIABLE_BOOTSERVICE_ACCESS` | 0x00000002 | Accessible in boot services |
| `EFI_VARIABLE_RUNTIME_ACCESS` | 0x00000004 | Accessible at runtime |
| `EFI_VARIABLE_HARDWARE_ERROR_RECORD` | 0x00000008 | Hardware error log |
| `EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS` | 0x00000010 | Deprecated |
| `EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS` | 0x00000020 | Authenticated |
| `EFI_VARIABLE_APPEND_WRITE` | 0x00000040 | Append to variable |

### Standard Variables

| Variable | Vendor GUID | Description |
|----------|-------------|-------------|
| `BootOrder` | gEfiGlobalVariableGuid | Boot option order |
| `BootXXXX` | gEfiGlobalVariableGuid | Boot option N |
| `BootNext` | gEfiGlobalVariableGuid | Next boot option |
| `BootCurrent` | gEfiGlobalVariableGuid | Current boot option |
| `Timeout` | gEfiGlobalVariableGuid | Boot timeout |
| `PlatformLang` | gEfiGlobalVariableGuid | Platform language |
| `ConIn` | gEfiGlobalVariableGuid | Console input devices |
| `ConOut` | gEfiGlobalVariableGuid | Console output devices |
| `ErrOut` | gEfiGlobalVariableGuid | Error output devices |
| `PK` | gEfiGlobalVariableGuid | Platform Key |
| `KEK` | gEfiGlobalVariableGuid | Key Exchange Keys |
| `db` | gEfiImageSecurityDatabaseGuid | Allowed signatures |
| `dbx` | gEfiImageSecurityDatabaseGuid | Forbidden signatures |
| `SecureBoot` | gEfiGlobalVariableGuid | Secure Boot status |
| `SetupMode` | gEfiGlobalVariableGuid | Setup Mode status |

## References

- [UEFI Specification](https://uefi.org/specifications) - Official specification
- [EDK2 Documentation](https://github.com/tianocore/tianocore.github.io/wiki) - TianoCore wiki
- [PI Specification](https://uefi.org/specifications) - Platform Initialization
