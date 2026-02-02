/** @file
  Memory Services Example - Demonstrates UEFI memory allocation.

  This example shows how to:
  1. Allocate and free pool memory
  2. Allocate and free pages
  3. Get the memory map
  4. Use memory allocation library

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

/**
  Display memory type as a string.
**/
CONST CHAR16 *
GetMemoryTypeString (
  IN EFI_MEMORY_TYPE  Type
  )
{
  switch (Type) {
    case EfiReservedMemoryType:      return L"Reserved";
    case EfiLoaderCode:              return L"LoaderCode";
    case EfiLoaderData:              return L"LoaderData";
    case EfiBootServicesCode:        return L"BS Code";
    case EfiBootServicesData:        return L"BS Data";
    case EfiRuntimeServicesCode:     return L"RT Code";
    case EfiRuntimeServicesData:     return L"RT Data";
    case EfiConventionalMemory:      return L"Conventional";
    case EfiUnusableMemory:          return L"Unusable";
    case EfiACPIReclaimMemory:       return L"ACPI Reclaim";
    case EfiACPIMemoryNVS:           return L"ACPI NVS";
    case EfiMemoryMappedIO:          return L"MMIO";
    case EfiMemoryMappedIOPortSpace: return L"MMIO Port";
    case EfiPalCode:                 return L"PAL Code";
    case EfiPersistentMemory:        return L"Persistent";
    default:                         return L"Unknown";
  }
}

/**
  Demonstrate pool memory allocation.
**/
EFI_STATUS
DemoPoolAllocation (
  VOID
  )
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       Size;

  Print (L"\n=== Pool Memory Allocation ===\n\n");

  //
  // Method 1: Direct Boot Services call
  //
  Size = 1024;  // 1 KB
  Print (L"1. AllocatePool (%d bytes)...\n", Size);

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  Size,
                  &Buffer
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed: %r\n", Status);
    return Status;
  }

  Print (L"   Allocated at: 0x%p\n", Buffer);

  // Use the memory
  ZeroMem (Buffer, Size);
  Print (L"   Memory zeroed\n");

  // Free it
  Status = gBS->FreePool (Buffer);
  Print (L"   FreePool: %r\n", Status);

  //
  // Method 2: Using MemoryAllocationLib (preferred)
  //
  Print (L"\n2. AllocateZeroPool (%d bytes)...\n", Size);

  Buffer = AllocateZeroPool (Size);
  if (Buffer == NULL) {
    Print (L"   Failed to allocate\n");
    return EFI_OUT_OF_RESOURCES;
  }

  Print (L"   Allocated at: 0x%p (pre-zeroed)\n", Buffer);

  FreePool (Buffer);
  Print (L"   Freed\n");

  //
  // Method 3: AllocateCopyPool
  //
  CHAR8 SourceData[] = "Hello, UEFI Memory!";
  Print (L"\n3. AllocateCopyPool...\n");

  Buffer = AllocateCopyPool (sizeof(SourceData), SourceData);
  if (Buffer == NULL) {
    Print (L"   Failed to allocate\n");
    return EFI_OUT_OF_RESOURCES;
  }

  Print (L"   Copied string: %a\n", (CHAR8 *)Buffer);

  FreePool (Buffer);
  Print (L"   Freed\n");

  return EFI_SUCCESS;
}

/**
  Demonstrate page allocation.
**/
EFI_STATUS
DemoPageAllocation (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Address;
  UINTN                 Pages;

  Print (L"\n=== Page Allocation ===\n\n");

  Pages = 4;  // 4 pages = 16 KB
  Print (L"1. AllocatePages (%d pages = %d KB)...\n", Pages, Pages * 4);

  //
  // AllocateAnyPages - let firmware choose address
  //
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiBootServicesData,
                  Pages,
                  &Address
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed: %r\n", Status);
    return Status;
  }

  Print (L"   Allocated at: 0x%lx\n", Address);

  // Free the pages
  Status = gBS->FreePages (Address, Pages);
  Print (L"   FreePages: %r\n", Status);

  //
  // AllocateMaxAddress - allocate below a specific address
  //
  Print (L"\n2. AllocatePages (below 4GB)...\n");

  Address = 0xFFFFFFFF;  // Below 4 GB
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  Pages,
                  &Address
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed: %r\n", Status);
    return Status;
  }

  Print (L"   Allocated at: 0x%lx\n", Address);

  Status = gBS->FreePages (Address, Pages);
  Print (L"   FreePages: %r\n", Status);

  //
  // Using library functions
  //
  Print (L"\n3. AllocateAlignedPages (64KB aligned)...\n");

  VOID *AlignedBuffer = AllocateAlignedPages (Pages, SIZE_64KB);
  if (AlignedBuffer == NULL) {
    Print (L"   Failed to allocate\n");
    return EFI_OUT_OF_RESOURCES;
  }

  Print (L"   Allocated at: 0x%p\n", AlignedBuffer);
  Print (L"   Address %% 64KB = %d (should be 0)\n",
         (UINTN)AlignedBuffer % SIZE_64KB);

  FreeAlignedPages (AlignedBuffer, Pages);
  Print (L"   Freed\n");

  return EFI_SUCCESS;
}

/**
  Display the memory map.
**/
EFI_STATUS
DemoMemoryMap (
  VOID
  )
{
  EFI_STATUS             Status;
  EFI_MEMORY_DESCRIPTOR  *MemoryMap;
  EFI_MEMORY_DESCRIPTOR  *Entry;
  UINTN                  MemoryMapSize;
  UINTN                  MapKey;
  UINTN                  DescriptorSize;
  UINT32                 DescriptorVersion;
  UINTN                  Index;
  UINTN                  EntryCount;

  Print (L"\n=== Memory Map ===\n\n");

  //
  // First call to get required buffer size
  //
  MemoryMapSize = 0;
  Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  NULL,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );

  if (Status != EFI_BUFFER_TOO_SMALL) {
    Print (L"Unexpected error: %r\n", Status);
    return Status;
  }

  //
  // Allocate buffer (add extra space for map changes during allocation)
  //
  MemoryMapSize += 2 * DescriptorSize;
  MemoryMap = AllocatePool (MemoryMapSize);
  if (MemoryMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the actual memory map
  //
  Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  MemoryMap,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );

  if (EFI_ERROR (Status)) {
    Print (L"GetMemoryMap failed: %r\n", Status);
    FreePool (MemoryMap);
    return Status;
  }

  EntryCount = MemoryMapSize / DescriptorSize;
  Print (L"Memory map has %d entries (showing first 10):\n\n", EntryCount);
  Print (L"Type           Physical Start   Pages      Attributes\n");
  Print (L"-------------- ---------------- ---------- ----------------\n");

  Entry = MemoryMap;
  for (Index = 0; Index < EntryCount && Index < 10; Index++) {
    Print (L"%-14s %016lx %10ld %016lx\n",
           GetMemoryTypeString (Entry->Type),
           Entry->PhysicalStart,
           Entry->NumberOfPages,
           Entry->Attribute
           );

    Entry = NEXT_MEMORY_DESCRIPTOR (Entry, DescriptorSize);
  }

  if (EntryCount > 10) {
    Print (L"... and %d more entries\n", EntryCount - 10);
  }

  FreePool (MemoryMap);
  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
MemoryExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Print (L"Memory Services Example\n");
  Print (L"=======================\n");

  Status = DemoPoolAllocation ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DemoPageAllocation ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DemoMemoryMap ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Print (L"\nMemory example completed successfully!\n");
  return EFI_SUCCESS;
}
