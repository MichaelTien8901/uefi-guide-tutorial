/** @file
  Hello World - A minimal UEFI application

  This is the simplest possible UEFI application. It demonstrates:
  - UEFI application entry point
  - Using the System Table
  - Console output with Print()
  - Returning status codes

  Copyright (c) 2024, UEFI Guide Contributors. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

/**
  Entry point for the Hello World UEFI application.

  @param[in] ImageHandle    Handle for this image.
  @param[in] SystemTable    Pointer to the EFI System Table.

  @retval EFI_SUCCESS       Application completed successfully.
**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  //
  // Print message to console
  // Print() is a UefiLib helper that uses SystemTable->ConOut
  //
  Print(L"Hello, UEFI World!\n");
  Print(L"\n");

  //
  // Display some system information
  //
  Print(L"Firmware Vendor: %s\n", SystemTable->FirmwareVendor);
  Print(L"Firmware Revision: 0x%08x\n", SystemTable->FirmwareRevision);
  Print(L"\n");

  //
  // Show UEFI specification version from the System Table header
  //
  Print(L"UEFI Specification: %d.%d.%d\n",
    (SystemTable->Hdr.Revision >> 16) & 0xFFFF,
    (SystemTable->Hdr.Revision >> 8) & 0xFF,
    SystemTable->Hdr.Revision & 0xFF
  );

  Print(L"\n");
  Print(L"Press any key to exit...\n");

  //
  // Wait for a keypress
  //
  {
    EFI_INPUT_KEY Key;
    UINTN         Index;

    // Wait for keyboard event
    SystemTable->BootServices->WaitForEvent(
      1,
      &SystemTable->ConIn->WaitForKey,
      &Index
    );

    // Read the key (clears the event)
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
  }

  return EFI_SUCCESS;
}
