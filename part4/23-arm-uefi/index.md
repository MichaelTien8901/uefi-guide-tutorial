---
layout: default
title: "Chapter 23: ARM UEFI Development"
permalink: /part4/arm-uefi/
nav_order: 23
parent: "Part 4: Advanced Topics"
has_children: true
---

# Chapter 23: ARM UEFI Development

ARM-based UEFI development presents unique challenges and opportunities compared to x86 platforms. This comprehensive chapter covers the complete ARM UEFI development journey from environment setup through production porting.

## Chapter Overview

| Section | Topic | Focus |
|---------|-------|-------|
| [23.1](23-1-arm-environment/) | Environment Setup | Cross-compilation toolchains, QEMU, hardware debuggers |
| [23.2](23-2-boot-architecture/) | Boot Architecture | Exception levels, TF-A integration, BL stages |
| [23.3](23-3-trusted-firmware/) | Trusted Firmware-A | BL1-BL33 flow, secure world, PSCI |
| [23.4](23-4-systemready/) | SystemReady Compliance | IR/ES/SR certification, ACS testing |
| [23.5](23-5-configuration/) | Platform Configuration | PCD settings, memory map, GIC configuration |
| [23.6](23-6-dt-acpi/) | Device Tree & ACPI | DT bindings, ACPI tables, hybrid approaches |
| [23.7](23-7-debugging/) | Debugging Techniques | JTAG, ETM tracing, exception analysis |
| [23.8](23-8-porting-guide/) | Porting Guide | New platform bringup, validation checklist |

## ARM vs x86 UEFI Comparison

```mermaid
graph TB
    subgraph "x86 Boot Flow"
        X1[Reset Vector] --> X2[SEC/CAR]
        X2 --> X3[PEI]
        X3 --> X4[DXE]
        X4 --> X5[BDS]
        X5 --> X6[OS]
    end

    subgraph "ARM Boot Flow"
        A1[BL1 - ROM] --> A2[BL2 - Trusted Boot]
        A2 --> A3[BL31 - Runtime Services]
        A2 --> A4[BL32 - Secure Payload]
        A2 --> A5[BL33 - UEFI]
        A5 --> A6[OS]
        A3 -.-> A6
    end

    style X1 fill:#f9f,stroke:#333
    style A1 fill:#9ff,stroke:#333
```

## Exception Level Architecture

ARM processors use Exception Levels (EL0-EL3) for privilege separation:

```mermaid
graph TB
    subgraph "Exception Levels"
        EL3[EL3: Secure Monitor<br/>TF-A BL31, SMC Handler]
        EL2[EL2: Hypervisor<br/>Optional Hypervisor]
        EL1[EL1: Kernel/UEFI<br/>UEFI Firmware, OS Kernel]
        EL0[EL0: User<br/>Applications]
    end

    EL3 --> EL2
    EL2 --> EL1
    EL1 --> EL0

    subgraph "Secure World"
        S_EL1[S-EL1: Secure OS<br/>OP-TEE]
        S_EL0[S-EL0: Secure Apps<br/>Trusted Applications]
    end

    EL3 --> S_EL1
    S_EL1 --> S_EL0

    style EL3 fill:#f99,stroke:#333
    style S_EL1 fill:#f99,stroke:#333
```

## Key Differences from x86

| Aspect | x86 UEFI | ARM UEFI |
|--------|----------|----------|
| Reset Vector | Fixed 0xFFFFFFF0 | Configurable (ROM defined) |
| Early Code | SEC phase in CAR | BL1 in ROM, BL2 in SRAM |
| Runtime Services | SMM-based | PSCI/SMC to EL3 |
| Memory Init | PEI phase | BL2 or separate firmware |
| Secure Services | SMM isolation | TrustZone (EL3/S-EL1) |
| Hardware Description | ACPI only | Device Tree, ACPI, or hybrid |
| Power Management | ACPI S-states | PSCI interface |
| Interrupt Controller | APIC/x2APIC | GIC v2/v3/v4 |

## Trusted Firmware-A Boot Stages

```mermaid
sequenceDiagram
    participant ROM as BL1 (ROM)
    participant BL2 as BL2 (Trusted Boot)
    participant BL31 as BL31 (EL3 Runtime)
    participant BL32 as BL32 (Secure OS)
    participant BL33 as BL33 (UEFI)
    participant OS as Operating System

    ROM->>ROM: Initialize CPU, Security
    ROM->>BL2: Load & Authenticate
    BL2->>BL2: Initialize DRAM
    BL2->>BL31: Load & Authenticate
    BL2->>BL32: Load & Authenticate (Optional)
    BL2->>BL33: Load & Authenticate
    BL2->>BL31: Transfer Control
    BL31->>BL31: Initialize Runtime Services
    BL31->>BL32: Initialize Secure OS
    BL31->>BL33: Jump to UEFI (EL2/EL1)
    BL33->>BL33: SEC → PEI → DXE → BDS
    BL33->>OS: Boot OS
    OS->>BL31: PSCI/SMC calls
```

## PSCI (Power State Coordination Interface)

ARM platforms use PSCI for power management instead of SMM:

```c
// PSCI Function IDs (SMC64 versions)
#define PSCI_VERSION                 0x84000000
#define PSCI_CPU_SUSPEND_AARCH64     0xC4000001
#define PSCI_CPU_OFF                 0x84000002
#define PSCI_CPU_ON_AARCH64          0xC4000003
#define PSCI_SYSTEM_OFF              0x84000008
#define PSCI_SYSTEM_RESET            0x84000009
#define PSCI_SYSTEM_RESET2_AARCH64   0xC4000012

// Invoking PSCI from UEFI
EFI_STATUS
EFIAPI
ArmPsciCpuOn (
  IN UINTN  TargetCpu,
  IN UINTN  EntryPoint
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;

  ArmSmcArgs.Arg0 = PSCI_CPU_ON_AARCH64;
  ArmSmcArgs.Arg1 = TargetCpu;      // MPIDR of target CPU
  ArmSmcArgs.Arg2 = EntryPoint;     // Entry point address
  ArmSmcArgs.Arg3 = 0;              // Context ID

  ArmCallSmc (&ArmSmcArgs);

  return (ArmSmcArgs.Arg0 == PSCI_SUCCESS) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}
```

## GIC (Generic Interrupt Controller)

ARM uses GIC for interrupt management:

```mermaid
graph LR
    subgraph "GIC v3 Architecture"
        D[Distributor<br/>GICD] --> R1[Redistributor<br/>GICR CPU0]
        D --> R2[Redistributor<br/>GICR CPU1]
        D --> R3[Redistributor<br/>GICR CPUn]

        R1 --> C1[CPU Interface<br/>ICC_*]
        R2 --> C2[CPU Interface<br/>ICC_*]
        R3 --> C3[CPU Interface<br/>ICC_*]
    end

    SPI[SPI: Shared<br/>Peripheral Int] --> D
    LPI[LPI: Locality<br/>Specific Int] --> R1
    PPI[PPI: Private<br/>Peripheral Int] --> R1
    SGI[SGI: Software<br/>Generated Int] --> R1
```

## Platform Examples

| Platform | SoC | EDK2 Package | TF-A Support |
|----------|-----|--------------|--------------|
| Raspberry Pi 4 | BCM2711 | edk2-platforms/RPi4 | Community |
| NVIDIA Jetson | Tegra | edk2-nvidia | NVIDIA fork |
| NXP i.MX8 | i.MX8M | edk2-platforms/NXP | Mainline |
| Ampere Altra | Altra | edk2-platforms/Ampere | Mainline |
| QEMU ARM | virt | ArmVirtPkg | N/A |

## Quick Start with QEMU

```bash
# Build UEFI for QEMU ARM64
cd edk2
source edksetup.sh
build -a AARCH64 -t GCC5 -p ArmVirtPkg/ArmVirtQemu.dsc -b DEBUG

# Run with QEMU
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 2G \
    -drive if=pflash,format=raw,file=QEMU_EFI.fd \
    -serial stdio \
    -net none
```

## Learning Path

```mermaid
graph LR
    A[Environment Setup] --> B[QEMU Testing]
    B --> C[Boot Architecture]
    C --> D[TF-A Integration]
    D --> E[Platform Config]
    E --> F[DT/ACPI Tables]
    F --> G[SystemReady Cert]
    G --> H[Hardware Porting]

    style A fill:#9f9,stroke:#333
    style H fill:#f99,stroke:#333
```

## EDK2 ARM Packages

| Package | Purpose |
|---------|---------|
| `ArmPkg` | Core ARM support (CPU, GIC, Timer, MMU) |
| `ArmPlatformPkg` | Platform abstraction layer |
| `ArmVirtPkg` | QEMU virtual machine support |
| `EmbeddedPkg` | Embedded systems utilities |

## References

- [ARM Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest)
- [TF-A Documentation](https://trustedfirmware-a.readthedocs.io/)
- [PSCI Specification](https://developer.arm.com/documentation/den0022/latest)
- [SystemReady Program](https://www.arm.com/architecture/system-architectures/systemready-certification-program)
- [EDK2 ArmPkg](https://github.com/tianocore/edk2/tree/master/ArmPkg)
- [EDK2 Platforms - ARM](https://github.com/tianocore/edk2-platforms/tree/master/Platform/ARM)

---

*Continue to [Section 23.1: Environment Setup](23-1-arm-environment/) to configure your ARM development environment.*
