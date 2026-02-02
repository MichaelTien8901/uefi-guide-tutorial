---
layout: default
title: "Appendix C: Debugging"
permalink: /appendices/c-debugging/
nav_order: 3
parent: "Appendices"
---

# Appendix C: Debugging

Comprehensive guide to debugging UEFI firmware using DEBUG macros, serial output, and GDB.

## Debug Output Configuration

### PCDs for Debug Output

```ini
# In platform DSC file
[PcdsFixedAtBuild]
  #
  # Debug property mask (controls DEBUG macro behavior)
  #
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F
  #   BIT0 - DEBUG_ASSERT_ENABLED       - Enable ASSERT
  #   BIT1 - DEBUG_PRINT_ENABLED        - Enable DEBUG prints
  #   BIT2 - DEBUG_CODE_ENABLED         - Enable DEBUG_CODE blocks
  #   BIT3 - CLEAR_MEMORY_ENABLED       - Clear allocations
  #   BIT4 - ASSERT_BREAKPOINT_ENABLED  - Breakpoint on ASSERT
  #   BIT5 - ASSERT_DEADLOOP_ENABLED    - Dead loop on ASSERT

  #
  # Debug print error level (which messages to print)
  #
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8040004F
  #   BIT0  - DEBUG_INIT      (0x00000001) - Initialization
  #   BIT1  - DEBUG_WARN      (0x00000002) - Warnings
  #   BIT2  - DEBUG_LOAD      (0x00000004) - Image loading
  #   BIT3  - DEBUG_FS        (0x00000008) - Filesystem
  #   BIT4  - DEBUG_POOL      (0x00000010) - Pool allocation
  #   BIT5  - DEBUG_PAGE      (0x00000020) - Page allocation
  #   BIT6  - DEBUG_INFO      (0x00000040) - Informational
  #   BIT7  - DEBUG_DISPATCH  (0x00000080) - Driver dispatch
  #   BIT10 - DEBUG_BM        (0x00000400) - Boot Manager
  #   BIT12 - DEBUG_BLKIO     (0x00001000) - Block I/O
  #   BIT14 - DEBUG_NET       (0x00004000) - Network
  #   BIT18 - DEBUG_VARIABLE  (0x00040000) - Variables
  #   BIT22 - DEBUG_EVENT     (0x00400000) - Events
  #   BIT30 - DEBUG_VERBOSE   (0x40000000) - Verbose
  #   BIT31 - DEBUG_ERROR     (0x80000000) - Errors

  # Report status code (POST codes, progress)
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  #   BIT0 - REPORT_STATUS_CODE_PROPERTY_PROGRESS_CODE_ENABLED
  #   BIT1 - REPORT_STATUS_CODE_PROPERTY_ERROR_CODE_ENABLED
  #   BIT2 - REPORT_STATUS_CODE_PROPERTY_DEBUG_CODE_ENABLED
```

### Serial Port Configuration

```ini
[PcdsFixedAtBuild]
  # Serial port base address
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x3F8

  # Baud rate
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  # Port settings
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1      # No parity
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1    # 1 stop bit
```

### Debug Library Selection

```ini
[LibraryClasses]
  # Serial port debug output
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf

  # Or: Console debug output
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf

  # Or: Null (disable debug)
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
```

## DEBUG Macro Usage

### Basic Usage

```c
#include <Library/DebugLib.h>

VOID
ExampleFunction (
    VOID
    )
{
    // Simple message
    DEBUG((DEBUG_INFO, "Hello from UEFI\n"));

    // With format specifiers
    DEBUG((DEBUG_INFO, "Value: %d, Hex: 0x%x, String: %s\n",
           Value, HexVal, String));

    // Unicode string
    DEBUG((DEBUG_INFO, "Unicode: %s\n", L"Wide String"));

    // GUID
    DEBUG((DEBUG_INFO, "GUID: %g\n", &gMyGuid));

    // EFI_STATUS
    DEBUG((DEBUG_INFO, "Status: %r\n", Status));

    // Pointer
    DEBUG((DEBUG_INFO, "Pointer: %p\n", Pointer));

    // Error level examples
    DEBUG((DEBUG_ERROR, "Error message\n"));
    DEBUG((DEBUG_WARN, "Warning message\n"));
    DEBUG((DEBUG_VERBOSE, "Verbose message\n"));
}
```

### Format Specifiers

| Specifier | Type | Description |
|-----------|------|-------------|
| `%d` | INT32 | Signed decimal |
| `%u` | UINT32 | Unsigned decimal |
| `%x` | UINT32 | Lowercase hex |
| `%X` | UINT32 | Uppercase hex |
| `%lx` | UINT64 | 64-bit hex |
| `%ld` | INT64 | 64-bit signed |
| `%s` | CHAR16* | Unicode string |
| `%a` | CHAR8* | ASCII string |
| `%c` | CHAR16 | Character |
| `%g` | EFI_GUID* | GUID |
| `%r` | EFI_STATUS | Status code |
| `%p` | VOID* | Pointer |

### ASSERT Macro

```c
// Simple assert
ASSERT(Pointer != NULL);

// Assert with status
ASSERT_EFI_ERROR(Status);

// Conditional assert
if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Operation failed: %r\n", Status));
    ASSERT(FALSE);
}

// Assert with message (compound)
if (Value > MAX_VALUE) {
    DEBUG((DEBUG_ERROR, "Value %d exceeds maximum %d\n", Value, MAX_VALUE));
    ASSERT(FALSE);
}
```

### DEBUG_CODE Blocks

```c
// Code only compiled in DEBUG builds
DEBUG_CODE_BEGIN();
    // Validation code
    ValidateDataStructure(Data);

    // Hex dump
    UINTN Index;
    DEBUG((DEBUG_VERBOSE, "Buffer contents:\n"));
    for (Index = 0; Index < Size; Index++) {
        DEBUG((DEBUG_VERBOSE, "%02x ", Buffer[Index]));
        if ((Index + 1) % 16 == 0) {
            DEBUG((DEBUG_VERBOSE, "\n"));
        }
    }
DEBUG_CODE_END();
```

## GDB Debugging with QEMU

### QEMU Setup

```bash
# Run QEMU with GDB server
qemu-system-x86_64 \
    -drive if=pflash,format=raw,file=OVMF_CODE.fd,readonly=on \
    -drive if=pflash,format=raw,file=OVMF_VARS.fd \
    -drive format=raw,file=disk.img \
    -serial stdio \
    -s -S    # -s: GDB on port 1234, -S: pause at start
```

### GDB Connection

```bash
# Start GDB
gdb

# Or with multiarch support
gdb-multiarch

# Connect to QEMU
(gdb) target remote localhost:1234
```

### Loading Symbols

```gdb
# Find module load address from DEBUG output:
# "Loading driver at 0x00000000XXXXX000 EntryPoint=0x..."

# Add symbols
(gdb) add-symbol-file Build/.../DEBUG/MyModule/MyModule.debug 0xXXXXX000

# For multiple modules
(gdb) add-symbol-file Build/.../DEBUG/DxeCore/DxeCore.debug 0x7EA8C000
(gdb) add-symbol-file Build/.../DEBUG/MyDriver/MyDriver.debug 0x7E9F1000
```

### GDB Commands

```gdb
# Breakpoints
(gdb) break FunctionName
(gdb) break filename.c:123
(gdb) break *0x7EA8C100          # Address
(gdb) info breakpoints
(gdb) delete 1

# Execution
(gdb) continue                    # Continue execution
(gdb) step                        # Step into
(gdb) next                        # Step over
(gdb) finish                      # Run to return
(gdb) stepi                       # Step instruction

# Examine memory
(gdb) x/16xw 0x7EA8C000          # 16 words hex
(gdb) x/s 0x7EA8C000             # String
(gdb) x/i $rip                    # Instruction at RIP

# Registers
(gdb) info registers
(gdb) print $rax
(gdb) set $rax = 0

# Stack
(gdb) backtrace
(gdb) frame 2
(gdb) info frame
(gdb) info locals

# Variables
(gdb) print VariableName
(gdb) print *Pointer
(gdb) print Status
(gdb) display Variable           # Auto-display
```

### GDB Init Script

```gdb
# ~/.gdbinit or .gdbinit in project

# Connect to QEMU
define qemu-connect
    target remote localhost:1234
end

# Load common symbols
define load-uefi-symbols
    # Adjust paths to your build
    add-symbol-file Build/OvmfX64/DEBUG_GCC5/X64/DxeCore.debug 0x7EA8C000
end

# UEFI-specific commands
define print-guid
    set $g = (EFI_GUID *)$arg0
    printf "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n", \
        $g->Data1, $g->Data2, $g->Data3, \
        $g->Data4[0], $g->Data4[1], $g->Data4[2], $g->Data4[3], \
        $g->Data4[4], $g->Data4[5], $g->Data4[6], $g->Data4[7]
end

# Memory map dump
define dump-memmap
    set $entry = (EFI_MEMORY_DESCRIPTOR *)$arg0
    set $count = $arg1
    set $size = $arg2
    set $i = 0
    while $i < $count
        printf "%016lX - %016lX [%d]\n", \
            $entry->PhysicalStart, \
            $entry->PhysicalStart + $entry->NumberOfPages * 0x1000, \
            $entry->Type
        set $entry = (EFI_MEMORY_DESCRIPTOR *)((char *)$entry + $size)
        set $i = $i + 1
    end
end
```

## Source-Level Debugging

### Finding Module Load Addresses

Enable debug prints to see module loading:

```
Loading driver at 0x00007E9F1000 EntryPoint=0x00007E9F1234
Loading driver MyDriver.efi
```

### Automatic Symbol Loading Script

```python
#!/usr/bin/env python3
# load_symbols.py - Parse debug log and generate GDB commands

import re
import sys

def parse_log(log_file, build_dir):
    pattern = r'Loading driver at (0x[0-9A-Fa-f]+).*?Loading driver (.*?)\.efi'

    with open(log_file, 'r') as f:
        content = f.read()

    matches = re.findall(pattern, content, re.DOTALL)

    for addr, name in matches:
        debug_file = f"{build_dir}/DEBUG/{name}/{name}.debug"
        print(f"add-symbol-file {debug_file} {addr}")

if __name__ == "__main__":
    parse_log(sys.argv[1], sys.argv[2])
```

## UEFI Shell Debugging

### Useful Commands

```bash
# Memory examination
mm 0x100000 -w 4 -n 16          # Read 16 dwords at 0x100000
mm 0x100000 0xDEADBEEF -w 4     # Write dword

# Device handles
dh -d                            # Detailed handle info
dh -p gEfiSimpleTextOutProtocolGuid  # Filter by protocol

# Memory map
memmap                           # Current memory map

# Loaded drivers
drivers                          # List drivers
drvdiag                          # Driver diagnostics

# Device tree
devtree                          # Show device hierarchy

# ACPI tables
acpiview                         # Display ACPI tables
acpiview -l                      # List tables
acpiview -s DSDT                 # Specific table

# Variables
dmpstore                         # Dump all variables
dmpstore BootOrder              # Specific variable
setvar MyVar -guid <GUID> =1    # Set variable

# Execution
load MyDriver.efi               # Load driver
unload <handle>                 # Unload driver
```

### Memory Dump Utility

```c
// Include in your module for debugging
VOID
DumpHex (
    IN VOID   *Data,
    IN UINTN   Size
    )
{
    UINT8  *Bytes = (UINT8 *)Data;
    UINTN  i;

    for (i = 0; i < Size; i++) {
        if (i % 16 == 0) {
            DEBUG((DEBUG_INFO, "\n%08X: ", i));
        }
        DEBUG((DEBUG_INFO, "%02X ", Bytes[i]));
    }
    DEBUG((DEBUG_INFO, "\n"));
}
```

## VS Code Integration

### launch.json

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "QEMU UEFI Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd",
            "miDebuggerServerAddress": "localhost:1234",
            "miDebuggerPath": "/usr/bin/gdb",
            "cwd": "${workspaceFolder}",
            "setupCommands": [
                {
                    "text": "-enable-pretty-printing"
                },
                {
                    "text": "set architecture i386:x86-64:intel"
                }
            ]
        }
    ]
}
```

### tasks.json

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build UEFI",
            "type": "shell",
            "command": "source edksetup.sh && build",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "Run QEMU",
            "type": "shell",
            "command": "qemu-system-x86_64 -drive if=pflash,format=raw,file=OVMF_CODE.fd -serial stdio -s -S",
            "isBackground": true
        }
    ]
}
```

## Troubleshooting

| Symptom | Cause | Solution |
|---------|-------|----------|
| No debug output | Wrong DebugLib | Use SerialPort variant |
| Garbled output | Baud mismatch | Match QEMU and PCD |
| GDB won't connect | QEMU not waiting | Add `-S` flag |
| Symbols not loading | Wrong address | Check DEBUG output for load address |
| Breakpoint not hit | Module not loaded | Verify module in build |
| ASSERT hangs | DEADLOOP enabled | Check PcdDebugPropertyMask |

## References

- [EDK2 DebugLib](https://github.com/tianocore/edk2/tree/master/MdePkg/Library)
- [OVMF Debugging](https://github.com/tianocore/tianocore.github.io/wiki/How-to-debug-OVMF-with-QEMU-using-GDB)
- [GDB Manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)
