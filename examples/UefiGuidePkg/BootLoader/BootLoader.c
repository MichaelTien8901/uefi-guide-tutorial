/** @file
  Custom Boot Loader Example - Demonstrates loading and starting OS kernel.

  This example shows how to:
  1. Load a kernel image from disk
  2. Set up memory for kernel
  3. Pass boot parameters to kernel
  4. Exit boot services and transfer control

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/FileInfo.h>
#include <Guid/Acpi.h>

//
// Boot information structure to pass to kernel
//
#pragma pack(1)
typedef struct {
  UINT32                        Signature;
  UINT32                        Version;
  UINT64                        MemoryMapAddr;
  UINT64                        MemoryMapSize;
  UINT64                        DescriptorSize;
  UINT64                        FramebufferAddr;
  UINT32                        FramebufferWidth;
  UINT32                        FramebufferHeight;
  UINT32                        FramebufferPitch;
  UINT32                        FramebufferBpp;
  UINT64                        AcpiRsdp;
  CHAR8                         CommandLine[256];
} BOOT_INFO;
#pragma pack()

#define BOOT_INFO_SIGNATURE  0x544F4F42  // "BOOT"
#define BOOT_INFO_VERSION    1

/**
  Find ACPI RSDP table.
**/
VOID *
FindAcpiRsdp (
  VOID
  )
{
  UINTN  Index;

  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (&gST->ConfigurationTable[Index].VendorGuid, &gEfiAcpi20TableGuid) ||
        CompareGuid (&gST->ConfigurationTable[Index].VendorGuid, &gEfiAcpi10TableGuid)) {
      return gST->ConfigurationTable[Index].VendorTable;
    }
  }

  return NULL;
}

/**
  Get framebuffer information from GOP.
**/
EFI_STATUS
GetFramebufferInfo (
  OUT BOOT_INFO  *BootInfo
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop;

  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  (VOID **)&Gop
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Warning: GOP not available\n");
    BootInfo->FramebufferAddr = 0;
    return Status;
  }

  BootInfo->FramebufferAddr = Gop->Mode->FrameBufferBase;
  BootInfo->FramebufferWidth = Gop->Mode->Info->HorizontalResolution;
  BootInfo->FramebufferHeight = Gop->Mode->Info->VerticalResolution;
  BootInfo->FramebufferPitch = Gop->Mode->Info->PixelsPerScanLine * 4;
  BootInfo->FramebufferBpp = 32;

  Print (L"Framebuffer: %dx%d at 0x%lx\n",
         BootInfo->FramebufferWidth,
         BootInfo->FramebufferHeight,
         BootInfo->FramebufferAddr);

  return EFI_SUCCESS;
}

/**
  Load kernel file from disk.
**/
EFI_STATUS
LoadKernel (
  IN  EFI_HANDLE         ImageHandle,
  IN  CHAR16             *KernelPath,
  OUT VOID               **KernelBuffer,
  OUT UINTN              *KernelSize
  )
{
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem;
  EFI_FILE_PROTOCOL                *Root;
  EFI_FILE_PROTOCOL                *KernelFile;
  EFI_FILE_INFO                    *FileInfo;
  UINTN                            InfoSize;
  VOID                             *Buffer;

  *KernelBuffer = NULL;
  *KernelSize = 0;

  // Get loaded image to find boot device
  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Get file system
  Status = gBS->HandleProtocol (
                  LoadedImage->DeviceHandle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **)&FileSystem
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Open root
  Status = FileSystem->OpenVolume (FileSystem, &Root);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Open kernel file
  Print (L"Loading kernel: %s\n", KernelPath);

  Status = Root->Open (
                   Root,
                   &KernelFile,
                   KernelPath,
                   EFI_FILE_MODE_READ,
                   0
                   );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to open kernel file: %r\n", Status);
    Root->Close (Root);
    return Status;
  }

  // Get file size
  InfoSize = sizeof(EFI_FILE_INFO) + 256 * sizeof(CHAR16);
  FileInfo = AllocatePool (InfoSize);
  if (FileInfo == NULL) {
    KernelFile->Close (KernelFile);
    Root->Close (Root);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = KernelFile->GetInfo (KernelFile, &gEfiFileInfoGuid, &InfoSize, FileInfo);
  if (EFI_ERROR (Status)) {
    FreePool (FileInfo);
    KernelFile->Close (KernelFile);
    Root->Close (Root);
    return Status;
  }

  *KernelSize = (UINTN)FileInfo->FileSize;
  Print (L"Kernel size: %d bytes\n", *KernelSize);
  FreePool (FileInfo);

  // Allocate buffer for kernel (aligned pages)
  UINTN Pages = EFI_SIZE_TO_PAGES (*KernelSize);
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiLoaderData,
                  Pages,
                  (EFI_PHYSICAL_ADDRESS *)&Buffer
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to allocate memory for kernel\n");
    KernelFile->Close (KernelFile);
    Root->Close (Root);
    return Status;
  }

  // Read kernel
  Status = KernelFile->Read (KernelFile, KernelSize, Buffer);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to read kernel: %r\n", Status);
    gBS->FreePages ((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, Pages);
    KernelFile->Close (KernelFile);
    Root->Close (Root);
    return Status;
  }

  *KernelBuffer = Buffer;
  Print (L"Kernel loaded at 0x%lx\n", (UINT64)(UINTN)Buffer);

  KernelFile->Close (KernelFile);
  Root->Close (Root);

  return EFI_SUCCESS;
}

/**
  Get memory map and prepare for ExitBootServices.
**/
EFI_STATUS
GetMemoryMapForExit (
  OUT EFI_MEMORY_DESCRIPTOR  **MemoryMap,
  OUT UINTN                  *MemoryMapSize,
  OUT UINTN                  *MapKey,
  OUT UINTN                  *DescriptorSize
  )
{
  EFI_STATUS  Status;
  UINT32      DescriptorVersion;

  *MemoryMapSize = 0;
  Status = gBS->GetMemoryMap (MemoryMapSize, NULL, MapKey,
                              DescriptorSize, &DescriptorVersion);

  if (Status != EFI_BUFFER_TOO_SMALL) {
    return EFI_LOAD_ERROR;
  }

  // Add extra space for allocation
  *MemoryMapSize += 2 * (*DescriptorSize);

  *MemoryMap = AllocatePool (*MemoryMapSize);
  if (*MemoryMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->GetMemoryMap (MemoryMapSize, *MemoryMap, MapKey,
                              DescriptorSize, &DescriptorVersion);

  return Status;
}

/**
  Display demonstration of boot process (without actually booting).
**/
EFI_STATUS
DemoBootProcess (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS             Status;
  BOOT_INFO              BootInfo;
  EFI_MEMORY_DESCRIPTOR  *MemoryMap;
  UINTN                  MemoryMapSize;
  UINTN                  MapKey;
  UINTN                  DescriptorSize;

  Print (L"\n=== Boot Loader Demo ===\n\n");
  Print (L"This demonstrates the boot process without actually booting.\n\n");

  // Initialize boot info
  ZeroMem (&BootInfo, sizeof(BootInfo));
  BootInfo.Signature = BOOT_INFO_SIGNATURE;
  BootInfo.Version = BOOT_INFO_VERSION;
  AsciiStrCpyS (BootInfo.CommandLine, sizeof(BootInfo.CommandLine),
                "console=ttyS0 root=/dev/sda1");

  // Get framebuffer info
  Print (L"Step 1: Getting framebuffer info...\n");
  GetFramebufferInfo (&BootInfo);

  // Find ACPI RSDP
  Print (L"\nStep 2: Finding ACPI RSDP...\n");
  BootInfo.AcpiRsdp = (UINT64)(UINTN)FindAcpiRsdp ();
  if (BootInfo.AcpiRsdp != 0) {
    Print (L"ACPI RSDP found at 0x%lx\n", BootInfo.AcpiRsdp);
  } else {
    Print (L"ACPI RSDP not found\n");
  }

  // Get memory map
  Print (L"\nStep 3: Getting memory map...\n");
  Status = GetMemoryMapForExit (&MemoryMap, &MemoryMapSize, &MapKey, &DescriptorSize);
  if (!EFI_ERROR (Status)) {
    BootInfo.MemoryMapAddr = (UINT64)(UINTN)MemoryMap;
    BootInfo.MemoryMapSize = MemoryMapSize;
    BootInfo.DescriptorSize = DescriptorSize;

    // Count memory entries
    UINTN EntryCount = MemoryMapSize / DescriptorSize;
    Print (L"Memory map: %d entries at 0x%lx\n", EntryCount, BootInfo.MemoryMapAddr);

    FreePool (MemoryMap);
  }

  // Show what would happen next
  Print (L"\nStep 4: Would load kernel from disk...\n");
  Print (L"  Example: LoadKernel(ImageHandle, L\"\\\\EFI\\\\kernel.elf\", ...);\n");

  Print (L"\nStep 5: Would call ExitBootServices...\n");
  Print (L"  Status = gBS->ExitBootServices(ImageHandle, MapKey);\n");

  Print (L"\nStep 6: Would jump to kernel entry point...\n");
  Print (L"  typedef VOID (*KERNEL_ENTRY)(BOOT_INFO *);\n");
  Print (L"  KERNEL_ENTRY KernelEntry = (KERNEL_ENTRY)KernelEntryPoint;\n");
  Print (L"  KernelEntry(&BootInfo);\n");

  Print (L"\n=== Boot Info Structure ===\n");
  Print (L"Signature:      0x%08x ('BOOT')\n", BootInfo.Signature);
  Print (L"Version:        %d\n", BootInfo.Version);
  Print (L"MemoryMapAddr:  0x%lx\n", BootInfo.MemoryMapAddr);
  Print (L"MemoryMapSize:  %ld\n", BootInfo.MemoryMapSize);
  Print (L"DescriptorSize: %ld\n", BootInfo.DescriptorSize);
  Print (L"Framebuffer:    0x%lx\n", BootInfo.FramebufferAddr);
  Print (L"Resolution:     %dx%d\n", BootInfo.FramebufferWidth, BootInfo.FramebufferHeight);
  Print (L"ACPI RSDP:      0x%lx\n", BootInfo.AcpiRsdp);
  Print (L"CommandLine:    %a\n", BootInfo.CommandLine);

  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
BootLoaderMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"Custom Boot Loader Example\n");
  Print (L"==========================\n");

  // Run demo
  DemoBootProcess (ImageHandle);

  Print (L"\n=== Important Notes ===\n\n");
  Print (L"1. ExitBootServices() can only be called once\n");
  Print (L"2. After ExitBootServices(), only Runtime Services are available\n");
  Print (L"3. Memory map must be fresh (MapKey must match)\n");
  Print (L"4. Kernel must be position-independent or loaded at expected address\n");
  Print (L"5. Consider using SetVirtualAddressMap() for runtime services\n");

  Print (L"\nBoot loader demo completed.\n");

  return EFI_SUCCESS;
}
