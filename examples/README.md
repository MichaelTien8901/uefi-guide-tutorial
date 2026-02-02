# UEFI Guide Examples

This directory contains working code examples for the UEFI Development Guide tutorial.

## Structure

```
examples/
└── UefiGuidePkg/
    ├── UefiGuidePkg.dsc      # Platform description (build file)
    ├── UefiGuidePkg.dec      # Package declaration
    ├── Include/              # Common headers
    │   └── UefiGuide.h
    ├── HelloWorld/           # Part 1: First application
    ├── DriverExample/        # Part 2: Driver model
    ├── ProtocolExample/      # Part 2: Protocols
    ├── MemoryExample/        # Part 2: Memory services
    ├── ConsoleExample/       # Part 3: Console I/O
    ├── GopExample/           # Part 3: Graphics
    ├── FileSystemExample/    # Part 3: File access
    ├── NetworkExample/       # Part 3: Networking
    ├── ShellCommand/         # Part 5: Shell command project
    ├── BootMenu/             # Part 5: Boot menu project
    ├── NetworkApp/           # Part 5: Network app project
    └── BootLoader/           # Part 5: Boot loader project
```

## Building

### Prerequisites

1. EDK2 source code cloned and set up
2. Build tools installed (see tutorial Part 1, Chapter 2)

### Build Steps

```bash
# Navigate to EDK2 workspace
cd /path/to/edk2

# Set up build environment
source edksetup.sh   # Linux/macOS
# or
edksetup.bat         # Windows

# Copy or symlink UefiGuidePkg to workspace
ln -s /path/to/examples/UefiGuidePkg UefiGuidePkg

# Build all examples
build -p UefiGuidePkg/UefiGuidePkg.dsc -a X64 -t GCC5 -b DEBUG

# Build a single module
build -p UefiGuidePkg/UefiGuidePkg.dsc -m UefiGuidePkg/HelloWorld/HelloWorld.inf -a X64 -t GCC5 -b DEBUG
```

### Output Location

Built EFI files are located at:
```
Build/UefiGuidePkg/DEBUG_GCC5/X64/*.efi
```

## Running in QEMU

See the `scripts/` directory for QEMU launch scripts:

```bash
# Linux/macOS
./scripts/run-qemu.sh

# Windows
scripts\run-qemu.bat
```

## Testing EDK2 Tags

These examples are tested against:
- **EDK2 Stable Tag**: `edk2-stable202311`
- **OVMF**: Included with EDK2

## License

BSD-2-Clause-Patent (same as EDK2)
