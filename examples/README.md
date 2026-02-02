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
    │
    │   # Part 1: Getting Started
    ├── HelloWorld/           # First UEFI application
    │
    │   # Part 2: Core Concepts
    ├── DriverExample/        # UEFI driver binding protocol
    ├── ProtocolExample/      # Custom protocol creation
    ├── MemoryExample/        # Memory allocation services
    ├── ServicesExample/      # Boot and Runtime services
    │
    │   # Part 3: Essential Services
    ├── ConsoleExample/       # Console I/O and colors
    ├── GopExample/           # Graphics Output Protocol
    ├── FileSystemExample/    # File system access
    ├── BlockIoExample/       # Block device and partitions
    ├── NetworkExample/       # Network stack basics
    ├── VariableExample/      # UEFI variables
    │
    │   # Part 5: Practical Projects
    ├── ShellCommand/         # Custom shell command
    ├── BootMenu/             # Graphical boot menu
    ├── NetworkApp/           # Network application
    └── BootLoader/           # Custom boot loader
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
