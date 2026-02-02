---
layout: default
title: "Part 3: Essential Services"
nav_order: 4
has_children: true
permalink: /part3/
---

# Part 3: Essential Services
{: .fs-9 }

Working with UEFI I/O, storage, network, and variable services.
{: .fs-6 .fw-300 }

---

## What You'll Learn

| Chapter | Topics |
|:--------|:-------|
| **[8. Console I/O]({% link part3/08-console-io.md %})** | Text input/output, cursor control, colors |
| **[9. Graphics (GOP)]({% link part3/09-graphics-output.md %})** | Framebuffer, video modes, drawing operations |
| **[10. File System]({% link part3/10-file-system.md %})** | Reading/writing files, directory operations |
| **[11. Block I/O]({% link part3/11-block-io.md %})** | Sector access, storage devices, partitions |
| **[12. Network Stack]({% link part3/12-network.md %})** | TCP/UDP, DHCP, network configuration |
| **[13. UEFI Variables]({% link part3/13-variables.md %})** | Persistent storage, boot options, Secure Boot |

## Service Architecture

```mermaid
flowchart LR
    subgraph UEFI Application
        APP[Your Application]
    end

    subgraph Console Services
        STIP[Simple Text Input]
        STOP[Simple Text Output]
        GOP[Graphics Output Protocol]
    end

    subgraph Storage Services
        SFS[Simple File System]
        BLOCKIO[Block I/O Protocol]
        DISKIO[Disk I/O Protocol]
    end

    subgraph Network Services
        SNP[Simple Network Protocol]
        TCP[TCP4/TCP6 Protocol]
        UDP[UDP4/UDP6 Protocol]
        DHCP[DHCP4 Protocol]
    end

    subgraph Variable Services
        GETVAR[GetVariable]
        SETVAR[SetVariable]
    end

    APP --> STIP
    APP --> STOP
    APP --> GOP
    APP --> SFS
    APP --> BLOCKIO
    APP --> SNP
    APP --> TCP
    APP --> GETVAR
    APP --> SETVAR

    style APP fill:#3498db,color:#fff
    style GOP fill:#2ecc71,color:#fff
    style TCP fill:#e67e22,color:#fff
    style GETVAR fill:#9b59b6,color:#fff
```

## Storage Stack

```mermaid
flowchart TB
    subgraph Applications
        FS_APP[File System Access]
        RAW_APP[Raw Disk Access]
    end

    subgraph Protocols
        SFS[Simple File System]
        DISKIO[Disk I/O]
        BLOCKIO[Block I/O]
        PARTITION[Partition Driver]
    end

    subgraph Drivers
        AHCI[AHCI/SATA]
        NVME[NVMe]
        USB[USB Storage]
    end

    subgraph Hardware
        HDD[Hard Disk]
        SSD[SSD]
        FLASH[USB Flash]
    end

    FS_APP --> SFS
    SFS --> DISKIO
    RAW_APP --> BLOCKIO
    DISKIO --> BLOCKIO
    BLOCKIO --> PARTITION
    PARTITION --> AHCI
    PARTITION --> NVME
    PARTITION --> USB
    AHCI --> HDD
    NVME --> SSD
    USB --> FLASH

    style SFS fill:#2ecc71,color:#fff
    style BLOCKIO fill:#3498db,color:#fff
```

## Network Protocol Stack

```mermaid
flowchart TB
    subgraph Application Layer
        HTTP[HTTP Boot]
        TFTP[TFTP/MTFTP]
        CUSTOM[Custom App]
    end

    subgraph Transport
        TCP4[TCP4]
        TCP6[TCP6]
        UDP4[UDP4]
        UDP6[UDP6]
    end

    subgraph Network
        IP4[IPv4]
        IP6[IPv6]
        ARP[ARP]
        DHCP[DHCP4/6]
    end

    subgraph Link
        MNP[Managed Network]
        SNP[Simple Network]
    end

    subgraph Hardware
        NIC[Network Card]
    end

    HTTP --> TCP4
    TFTP --> UDP4
    CUSTOM --> TCP4
    CUSTOM --> UDP4
    TCP4 --> IP4
    TCP6 --> IP6
    UDP4 --> IP4
    UDP6 --> IP6
    IP4 --> ARP
    IP4 --> MNP
    DHCP --> IP4
    ARP --> MNP
    MNP --> SNP
    SNP --> NIC

    style TCP4 fill:#3498db,color:#fff
    style IP4 fill:#2ecc71,color:#fff
    style SNP fill:#e67e22,color:#fff
```

---

## Key Concepts

### Protocol Discovery Pattern

All services follow a consistent pattern:

```c
EFI_STATUS
UseService (VOID)
{
  EFI_STATUS Status;
  EFI_HANDLE *Handles;
  UINTN HandleCount;
  SOME_PROTOCOL *Protocol;

  // 1. Find handles with the protocol
  Status = gBS->LocateHandleBuffer(
             ByProtocol,
             &gSomeProtocolGuid,
             NULL,
             &HandleCount,
             &Handles
           );

  // 2. Get protocol from handle
  Status = gBS->HandleProtocol(
             Handles[0],
             &gSomeProtocolGuid,
             (VOID **)&Protocol
           );

  // 3. Use the protocol
  Protocol->SomeFunction(...);

  // 4. Cleanup
  gBS->FreePool(Handles);

  return Status;
}
```

### Service Binding Pattern

Network protocols use Service Binding:

```c
// 1. Get Service Binding
gBS->HandleProtocol(Handle, &gServiceBindingGuid, &ServiceBinding);

// 2. Create child instance
ServiceBinding->CreateChild(ServiceBinding, &ChildHandle);

// 3. Get protocol from child
gBS->HandleProtocol(ChildHandle, &gProtocolGuid, &Protocol);

// 4. Use protocol...

// 5. Destroy child when done
ServiceBinding->DestroyChild(ServiceBinding, ChildHandle);
```

---

{: .note }
> **Practical Focus:** This section emphasizes working code examples. Each chapter includes complete, buildable examples you can run in QEMU.

{: .warning }
> **Prerequisites:** Ensure you have completed Parts 1 and 2. You should be comfortable with the UEFI driver model and protocol system.
