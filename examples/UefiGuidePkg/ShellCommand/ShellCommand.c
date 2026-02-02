/** @file
  Custom Shell Command Example - Demonstrates creating UEFI Shell commands.

  This example shows how to:
  1. Create a loadable shell command
  2. Parse command line arguments
  3. Use shell library functions
  4. Handle shell command protocol

  Usage in shell: load ShellCommand.efi
                  sysinfo [-v] [-h]

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Protocol/Shell.h>
#include <Guid/GlobalVariable.h>

//
// Command name for registration
//
#define COMMAND_NAME  L"sysinfo"

/**
  Print UEFI firmware information.
**/
VOID
PrintFirmwareInfo (
  IN BOOLEAN  Verbose
  )
{
  Print (L"\n=== UEFI Firmware Information ===\n\n");

  // UEFI Version
  Print (L"UEFI Specification: %d.%d",
         gST->Hdr.Revision >> 16,
         gST->Hdr.Revision & 0xFFFF);
  if ((gST->Hdr.Revision & 0xFFFF) % 10 != 0) {
    Print (L".%d", (gST->Hdr.Revision & 0xFFFF) % 10);
  }
  Print (L"\n");

  // Firmware vendor and revision
  Print (L"Firmware Vendor: %s\n", gST->FirmwareVendor);
  Print (L"Firmware Revision: 0x%08x\n", gST->FirmwareRevision);

  if (Verbose) {
    // Configuration table count
    Print (L"Configuration Tables: %d\n", gST->NumberOfTableEntries);

    // Boot Services revision
    Print (L"Boot Services Revision: 0x%08x\n", gBS->Hdr.Revision);

    // Runtime Services revision
    Print (L"Runtime Services Revision: 0x%08x\n", gRT->Hdr.Revision);
  }
}

/**
  Print memory information.
**/
VOID
PrintMemoryInfo (
  IN BOOLEAN  Verbose
  )
{
  EFI_STATUS             Status;
  EFI_MEMORY_DESCRIPTOR  *MemoryMap;
  UINTN                  MemoryMapSize;
  UINTN                  MapKey;
  UINTN                  DescriptorSize;
  UINT32                 DescriptorVersion;
  UINT64                 TotalMemory = 0;
  UINT64                 FreeMemory = 0;

  Print (L"\n=== Memory Information ===\n\n");

  // Get memory map
  MemoryMapSize = 0;
  Status = gBS->GetMemoryMap (&MemoryMapSize, NULL, &MapKey,
                              &DescriptorSize, &DescriptorVersion);

  if (Status != EFI_BUFFER_TOO_SMALL) {
    Print (L"Failed to get memory map\n");
    return;
  }

  MemoryMapSize += 2 * DescriptorSize;
  MemoryMap = AllocatePool (MemoryMapSize);
  if (MemoryMap == NULL) {
    return;
  }

  Status = gBS->GetMemoryMap (&MemoryMapSize, MemoryMap, &MapKey,
                              &DescriptorSize, &DescriptorVersion);

  if (!EFI_ERROR (Status)) {
    EFI_MEMORY_DESCRIPTOR *Entry = MemoryMap;
    UINTN EntryCount = MemoryMapSize / DescriptorSize;

    for (UINTN i = 0; i < EntryCount; i++) {
      UINT64 Size = Entry->NumberOfPages * 4096;

      if (Entry->Type == EfiConventionalMemory) {
        FreeMemory += Size;
      }
      TotalMemory += Size;

      Entry = NEXT_MEMORY_DESCRIPTOR (Entry, DescriptorSize);
    }

    Print (L"Total Memory: %ld MB\n", TotalMemory / (1024 * 1024));
    Print (L"Free Memory: %ld MB\n", FreeMemory / (1024 * 1024));

    if (Verbose) {
      Print (L"Memory Map Entries: %d\n", EntryCount);
      Print (L"Descriptor Size: %d bytes\n", DescriptorSize);
    }
  }

  FreePool (MemoryMap);
}

/**
  Print time information.
**/
VOID
PrintTimeInfo (
  VOID
  )
{
  EFI_STATUS  Status;
  EFI_TIME    Time;

  Print (L"\n=== Time Information ===\n\n");

  Status = gRT->GetTime (&Time, NULL);
  if (!EFI_ERROR (Status)) {
    Print (L"Current Time: %04d-%02d-%02d %02d:%02d:%02d\n",
           Time.Year, Time.Month, Time.Day,
           Time.Hour, Time.Minute, Time.Second);
    Print (L"Timezone: UTC%+d\n", Time.TimeZone == 2047 ? 0 : Time.TimeZone / 60);
    Print (L"Daylight: %s\n", Time.Daylight ? L"Yes" : L"No");
  } else {
    Print (L"Failed to get time: %r\n", Status);
  }
}

/**
  Print boot information.
**/
VOID
PrintBootInfo (
  IN BOOLEAN  Verbose
  )
{
  EFI_STATUS  Status;
  UINT16      BootCurrent;
  UINTN       DataSize;

  Print (L"\n=== Boot Information ===\n\n");

  // Get current boot option
  DataSize = sizeof(BootCurrent);
  Status = gRT->GetVariable (
                  L"BootCurrent",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &BootCurrent
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"Current Boot Option: Boot%04x\n", BootCurrent);
  }

  if (Verbose) {
    // Get Secure Boot state
    UINT8 SecureBoot = 0;
    DataSize = sizeof(SecureBoot);
    Status = gRT->GetVariable (
                    L"SecureBoot",
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &DataSize,
                    &SecureBoot
                    );

    if (!EFI_ERROR (Status)) {
      Print (L"Secure Boot: %s\n", SecureBoot ? L"Enabled" : L"Disabled");
    }
  }
}

/**
  Print usage information.
**/
VOID
PrintUsage (
  VOID
  )
{
  Print (L"\nUsage: %s [-v] [-h]\n\n", COMMAND_NAME);
  Print (L"Display system information.\n\n");
  Print (L"Options:\n");
  Print (L"  -v, --verbose    Show detailed information\n");
  Print (L"  -h, --help       Show this help message\n");
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
ShellCommandMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  LIST_ENTRY    *ParamPackage;
  BOOLEAN       Verbose;
  BOOLEAN       ShowHelp;
  CONST CHAR16  *Problem;

  //
  // Initialize Shell library
  //
  Status = ShellInitialize ();
  if (EFI_ERROR (Status)) {
    Print (L"Shell library initialization failed\n");
    // Fall back to non-shell mode
    PrintFirmwareInfo (FALSE);
    PrintMemoryInfo (FALSE);
    PrintTimeInfo ();
    PrintBootInfo (FALSE);
    return EFI_SUCCESS;
  }

  //
  // Parse command line
  //
  Status = ShellCommandLineParse (
             NULL,
             &ParamPackage,
             &Problem,
             TRUE
             );

  if (EFI_ERROR (Status)) {
    if (Problem != NULL) {
      Print (L"Error: %s\n", Problem);
      FreePool ((VOID *)Problem);
    }
    return Status;
  }

  //
  // Check for flags
  //
  Verbose = ShellCommandLineGetFlag (ParamPackage, L"-v") ||
            ShellCommandLineGetFlag (ParamPackage, L"--verbose");

  ShowHelp = ShellCommandLineGetFlag (ParamPackage, L"-h") ||
             ShellCommandLineGetFlag (ParamPackage, L"--help");

  if (ShowHelp) {
    PrintUsage ();
    ShellCommandLineFreeVarList (ParamPackage);
    return EFI_SUCCESS;
  }

  //
  // Display system information
  //
  Print (L"\n*** System Information ***\n");

  PrintFirmwareInfo (Verbose);
  PrintMemoryInfo (Verbose);
  PrintTimeInfo ();
  PrintBootInfo (Verbose);

  Print (L"\n");

  ShellCommandLineFreeVarList (ParamPackage);
  return EFI_SUCCESS;
}
