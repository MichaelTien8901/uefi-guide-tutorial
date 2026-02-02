/** @file
  Common definitions for UEFI Guide examples.

  Copyright (c) 2024, UEFI Guide Contributors. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef UEFI_GUIDE_H_
#define UEFI_GUIDE_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

//
// Helper macro for printing with newline
//
#define PRINT(fmt, ...)  Print(L##fmt L"\r\n", ##__VA_ARGS__)

//
// Helper macro for error checking
//
#define CHECK_STATUS(Status, Label) \
  do { \
    if (EFI_ERROR(Status)) { \
      DEBUG((DEBUG_ERROR, "%a:%d Status = %r\n", __FUNCTION__, __LINE__, Status)); \
      goto Label; \
    } \
  } while (FALSE)

//
// Wait for keypress helper
//
#define WAIT_FOR_KEY() \
  do { \
    EFI_INPUT_KEY Key; \
    UINTN Index; \
    Print(L"\r\nPress any key to continue...\r\n"); \
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index); \
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key); \
  } while (FALSE)

#endif // UEFI_GUIDE_H_
