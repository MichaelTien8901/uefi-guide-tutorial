/** @file
  Block I/O Example - Demonstrates UEFI block device access.

  This example shows how to:
  1. Enumerate block devices
  2. Read device information (size, block size, media)
  3. Read raw blocks from disk
  4. Understand partition schemes (GPT/MBR)

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DiskIo.h>
#include <Protocol/DevicePath.h>

//
// MBR signature location
//
#define MBR_SIGNATURE_OFFSET  510
#define MBR_SIGNATURE         0xAA55

//
// GPT signature
//
#define GPT_SIGNATURE  "EFI PART"

/**
  Display media information for a block device.
**/
VOID
DisplayMediaInfo (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN UINTN                  DeviceIndex
  )
{
  EFI_BLOCK_IO_MEDIA  *Media;
  UINT64              DeviceSize;

  Media = BlockIo->Media;
  DeviceSize = MultU64x32 (Media->LastBlock + 1, Media->BlockSize);

  Print (L"\nDevice %d:\n", DeviceIndex);
  Print (L"  Media ID: %d\n", Media->MediaId);
  Print (L"  Removable: %s\n", Media->RemovableMedia ? L"Yes" : L"No");
  Print (L"  Media Present: %s\n", Media->MediaPresent ? L"Yes" : L"No");
  Print (L"  Logical Partition: %s\n", Media->LogicalPartition ? L"Yes" : L"No");
  Print (L"  Read Only: %s\n", Media->ReadOnly ? L"Yes" : L"No");
  Print (L"  Block Size: %d bytes\n", Media->BlockSize);
  Print (L"  Last Block: %ld\n", Media->LastBlock);
  Print (L"  Total Size: %ld MB (%ld bytes)\n",
         DeviceSize / (1024 * 1024), DeviceSize);

  if (BlockIo->Revision >= EFI_BLOCK_IO_PROTOCOL_REVISION2) {
    Print (L"  Optimal Transfer Length: %d blocks\n",
           Media->OptimalTransferLengthGranularity);
  }

  if (BlockIo->Revision >= EFI_BLOCK_IO_PROTOCOL_REVISION3) {
    Print (L"  Lowest Aligned LBA: %ld\n", Media->LowestAlignedLba);
    Print (L"  Logical Blocks per Physical: %d\n",
           Media->LogicalBlocksPerPhysicalBlock);
  }
}

/**
  Check if device has MBR partition table.
**/
BOOLEAN
CheckMbr (
  IN UINT8  *Buffer
  )
{
  UINT16  Signature;

  Signature = Buffer[MBR_SIGNATURE_OFFSET] |
              (Buffer[MBR_SIGNATURE_OFFSET + 1] << 8);

  return (Signature == MBR_SIGNATURE);
}

/**
  Check if device has GPT partition table.
**/
BOOLEAN
CheckGpt (
  IN UINT8  *Buffer
  )
{
  // GPT header is at LBA 1, starts with "EFI PART"
  return (CompareMem (Buffer, GPT_SIGNATURE, 8) == 0);
}

/**
  Read and analyze partition table.
**/
EFI_STATUS
AnalyzePartitionTable (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN UINTN                  DeviceIndex
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  UINT8       *GptBuffer;
  UINT32      BlockSize;

  BlockSize = BlockIo->Media->BlockSize;

  // Skip logical partitions (they don't have partition tables)
  if (BlockIo->Media->LogicalPartition) {
    Print (L"  (Logical partition - skipping partition table check)\n");
    return EFI_SUCCESS;
  }

  // Allocate buffer for MBR (LBA 0)
  Buffer = AllocateZeroPool (BlockSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Read LBA 0 (MBR or protective MBR for GPT)
  Status = BlockIo->ReadBlocks (
                      BlockIo,
                      BlockIo->Media->MediaId,
                      0,
                      BlockSize,
                      Buffer
                      );

  if (EFI_ERROR (Status)) {
    Print (L"  Failed to read LBA 0: %r\n", Status);
    FreePool (Buffer);
    return Status;
  }

  Print (L"\n  Partition Table Analysis:\n");

  // Check for valid boot signature
  if (!CheckMbr (Buffer)) {
    Print (L"    No valid MBR signature found\n");
    FreePool (Buffer);
    return EFI_SUCCESS;
  }

  // Check for GPT (read LBA 1)
  GptBuffer = AllocateZeroPool (BlockSize);
  if (GptBuffer != NULL) {
    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        1,
                        BlockSize,
                        GptBuffer
                        );

    if (!EFI_ERROR (Status) && CheckGpt (GptBuffer)) {
      Print (L"    GPT (GUID Partition Table) detected\n");

      // Parse GPT header
      UINT64 *FirstUsableLba = (UINT64 *)(GptBuffer + 40);
      UINT64 *LastUsableLba = (UINT64 *)(GptBuffer + 48);
      UINT32 *NumPartitionEntries = (UINT32 *)(GptBuffer + 80);

      Print (L"    First Usable LBA: %ld\n", *FirstUsableLba);
      Print (L"    Last Usable LBA: %ld\n", *LastUsableLba);
      Print (L"    Partition Entries: %d\n", *NumPartitionEntries);
    } else {
      Print (L"    MBR (Master Boot Record) detected\n");

      // Parse MBR partition entries (at offset 446)
      UINT8 *PartitionTable = Buffer + 446;
      for (UINTN i = 0; i < 4; i++) {
        UINT8 *Entry = PartitionTable + (i * 16);
        UINT8 Type = Entry[4];

        if (Type != 0) {
          UINT32 StartLba = *(UINT32 *)(Entry + 8);
          UINT32 SizeLba = *(UINT32 *)(Entry + 12);

          Print (L"    Partition %d: Type=0x%02x, Start=%d, Size=%d sectors\n",
                 i + 1, Type, StartLba, SizeLba);
        }
      }
    }

    FreePool (GptBuffer);
  }

  FreePool (Buffer);
  return EFI_SUCCESS;
}

/**
  Dump first few bytes of a block.
**/
EFI_STATUS
DumpBlockData (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_LBA                Lba,
  IN UINTN                  BytesToShow
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  UINT32      BlockSize;
  UINTN       i;

  BlockSize = BlockIo->Media->BlockSize;

  Buffer = AllocatePool (BlockSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = BlockIo->ReadBlocks (
                      BlockIo,
                      BlockIo->Media->MediaId,
                      Lba,
                      BlockSize,
                      Buffer
                      );

  if (EFI_ERROR (Status)) {
    Print (L"  Failed to read LBA %ld: %r\n", Lba, Status);
    FreePool (Buffer);
    return Status;
  }

  Print (L"\n  First %d bytes of LBA %ld:\n  ", BytesToShow, Lba);
  for (i = 0; i < BytesToShow && i < BlockSize; i++) {
    Print (L"%02x ", Buffer[i]);
    if ((i + 1) % 16 == 0) {
      Print (L"\n  ");
    }
  }
  Print (L"\n");

  FreePool (Buffer);
  return EFI_SUCCESS;
}

/**
  Enumerate all block devices.
**/
EFI_STATUS
EnumerateBlockDevices (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            *HandleBuffer;
  UINTN                 HandleCount;
  UINTN                 Index;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  UINTN                 DeviceCount = 0;

  Print (L"\n=== Enumerating Block Devices ===\n");

  // Locate all handles with BlockIo protocol
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    Print (L"No block devices found: %r\n", Status);
    return Status;
  }

  Print (L"Found %d block device handle(s)\n", HandleCount);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlockIo
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    // Skip devices without media present
    if (!BlockIo->Media->MediaPresent) {
      continue;
    }

    DeviceCount++;

    // Display media info
    DisplayMediaInfo (BlockIo, DeviceCount);

    // Get device path for display
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );

    if (!EFI_ERROR (Status)) {
      CHAR16 *PathStr = ConvertDevicePathToText (DevicePath, FALSE, FALSE);
      if (PathStr != NULL) {
        Print (L"  Device Path: %s\n", PathStr);
        FreePool (PathStr);
      }
    }

    // Analyze partition table (only for non-partition devices)
    if (!BlockIo->Media->LogicalPartition) {
      AnalyzePartitionTable (BlockIo, DeviceCount);

      // Show first 32 bytes of LBA 0
      DumpBlockData (BlockIo, 0, 32);
    }

    // Limit output
    if (DeviceCount >= 5) {
      Print (L"\n(Showing first 5 devices with media present)\n");
      break;
    }
  }

  gBS->FreePool (HandleBuffer);

  Print (L"\nTotal devices with media: %d\n", DeviceCount);

  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
BlockIoExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"Block I/O Example\n");
  Print (L"=================\n");

  EnumerateBlockDevices ();

  Print (L"\n=== Block I/O Operations Summary ===\n\n");
  Print (L"Key Functions:\n");
  Print (L"  BlockIo->ReadBlocks()  - Read sectors\n");
  Print (L"  BlockIo->WriteBlocks() - Write sectors\n");
  Print (L"  BlockIo->FlushBlocks() - Flush cache\n");
  Print (L"  BlockIo->Reset()       - Reset device\n");
  Print (L"\nBlock I/O example completed!\n");

  return EFI_SUCCESS;
}
