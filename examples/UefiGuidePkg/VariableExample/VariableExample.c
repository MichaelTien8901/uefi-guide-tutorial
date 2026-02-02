/** @file
  UEFI Variables Example - Demonstrates UEFI variable services.

  This example shows how to:
  1. Read standard UEFI variables
  2. Create and delete custom variables
  3. Enumerate all variables
  4. Understand variable attributes

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
#include <Library/PrintLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>

//
// Custom variable GUID for this example
//
#define UEFI_GUIDE_VARIABLE_GUID \
  { 0x12345678, 0xABCD, 0xEF00, { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 } }

EFI_GUID gUefiGuideVariableGuid = UEFI_GUIDE_VARIABLE_GUID;

/**
  Print variable attributes.
**/
VOID
PrintAttributes (
  IN UINT32  Attributes
  )
{
  Print (L"    Attributes: 0x%08x\n", Attributes);
  Print (L"      ");

  if (Attributes & EFI_VARIABLE_NON_VOLATILE) {
    Print (L"NV ");
  }
  if (Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS) {
    Print (L"BS ");
  }
  if (Attributes & EFI_VARIABLE_RUNTIME_ACCESS) {
    Print (L"RT ");
  }
  if (Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    Print (L"HW_ERR ");
  }
  if (Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) {
    Print (L"AUTH_TIME ");
  }
  if (Attributes & EFI_VARIABLE_APPEND_WRITE) {
    Print (L"APPEND ");
  }

  Print (L"\n");
}

/**
  Read and display standard UEFI variables.
**/
EFI_STATUS
ReadStandardVariables (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       DataSize;
  UINT32      Attributes;
  VOID        *Data;

  Print (L"\n=== Standard UEFI Variables ===\n");

  //
  // BootCurrent
  //
  Print (L"\n1. BootCurrent:\n");
  UINT16 BootCurrent;
  DataSize = sizeof(BootCurrent);
  Status = gRT->GetVariable (
                  L"BootCurrent",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &BootCurrent
                  );
  if (!EFI_ERROR (Status)) {
    Print (L"    Value: Boot%04x\n", BootCurrent);
    PrintAttributes (Attributes);
  } else {
    Print (L"    Not available: %r\n", Status);
  }

  //
  // BootOrder
  //
  Print (L"\n2. BootOrder:\n");
  DataSize = 0;
  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &DataSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Data = AllocatePool (DataSize);
    if (Data != NULL) {
      Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid,
                                 &Attributes, &DataSize, Data);
      if (!EFI_ERROR (Status)) {
        Print (L"    Value: ");
        UINT16 *BootOrder = (UINT16 *)Data;
        for (UINTN i = 0; i < DataSize / sizeof(UINT16); i++) {
          Print (L"Boot%04x ", BootOrder[i]);
        }
        Print (L"\n");
        PrintAttributes (Attributes);
      }
      FreePool (Data);
    }
  } else {
    Print (L"    Not available: %r\n", Status);
  }

  //
  // SecureBoot
  //
  Print (L"\n3. SecureBoot:\n");
  UINT8 SecureBoot;
  DataSize = sizeof(SecureBoot);
  Status = gRT->GetVariable (
                  L"SecureBoot",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &SecureBoot
                  );
  if (!EFI_ERROR (Status)) {
    Print (L"    Value: %d (%s)\n", SecureBoot, SecureBoot ? L"Enabled" : L"Disabled");
    PrintAttributes (Attributes);
  } else {
    Print (L"    Not available: %r\n", Status);
  }

  //
  // SetupMode
  //
  Print (L"\n4. SetupMode:\n");
  UINT8 SetupMode;
  DataSize = sizeof(SetupMode);
  Status = gRT->GetVariable (
                  L"SetupMode",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &SetupMode
                  );
  if (!EFI_ERROR (Status)) {
    Print (L"    Value: %d (%s)\n", SetupMode, SetupMode ? L"Setup Mode" : L"User Mode");
    PrintAttributes (Attributes);
  } else {
    Print (L"    Not available: %r\n", Status);
  }

  //
  // Timeout
  //
  Print (L"\n5. Timeout:\n");
  UINT16 Timeout;
  DataSize = sizeof(Timeout);
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &Timeout
                  );
  if (!EFI_ERROR (Status)) {
    Print (L"    Value: %d seconds\n", Timeout);
    PrintAttributes (Attributes);
  } else {
    Print (L"    Not available: %r\n", Status);
  }

  return EFI_SUCCESS;
}

/**
  Create, read, and delete a custom variable.
**/
EFI_STATUS
DemoCustomVariable (
  VOID
  )
{
  EFI_STATUS  Status;
  CHAR8       TestData[] = "Hello from UEFI Variable Example!";
  CHAR8       ReadBuffer[64];
  UINTN       DataSize;
  UINT32      Attributes;

  Print (L"\n=== Custom Variable Demo ===\n");

  //
  // Create a custom variable
  //
  Print (L"\n1. Creating custom variable 'TestVar'...\n");

  Status = gRT->SetVariable (
                  L"TestVar",
                  &gUefiGuideVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(TestData),
                  TestData
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to create variable: %r\n", Status);
    if (Status == EFI_SECURITY_VIOLATION) {
      Print (L"   (May be blocked by Secure Boot policy)\n");
    }
    return Status;
  }

  Print (L"   Variable created successfully\n");

  //
  // Read it back
  //
  Print (L"\n2. Reading back 'TestVar'...\n");

  DataSize = sizeof(ReadBuffer);
  Status = gRT->GetVariable (
                  L"TestVar",
                  &gUefiGuideVariableGuid,
                  &Attributes,
                  &DataSize,
                  ReadBuffer
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"   Value: %a\n", ReadBuffer);
    Print (L"   Size: %d bytes\n", DataSize);
    PrintAttributes (Attributes);
  } else {
    Print (L"   Failed to read: %r\n", Status);
  }

  //
  // Delete the variable
  //
  Print (L"\n3. Deleting 'TestVar'...\n");

  Status = gRT->SetVariable (
                  L"TestVar",
                  &gUefiGuideVariableGuid,
                  0,     // Attributes = 0
                  0,     // DataSize = 0
                  NULL   // Data = NULL
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"   Variable deleted\n");
  } else {
    Print (L"   Failed to delete: %r\n", Status);
  }

  //
  // Verify deletion
  //
  Print (L"\n4. Verifying deletion...\n");

  DataSize = sizeof(ReadBuffer);
  Status = gRT->GetVariable (
                  L"TestVar",
                  &gUefiGuideVariableGuid,
                  NULL,
                  &DataSize,
                  ReadBuffer
                  );

  if (Status == EFI_NOT_FOUND) {
    Print (L"   Variable not found (deleted successfully)\n");
  } else {
    Print (L"   Unexpected status: %r\n", Status);
  }

  return EFI_SUCCESS;
}

/**
  Enumerate all variables in the system.
**/
EFI_STATUS
EnumerateAllVariables (
  VOID
  )
{
  EFI_STATUS  Status;
  CHAR16      VariableName[256];
  EFI_GUID    VendorGuid;
  UINTN       NameSize;
  UINTN       Count = 0;
  UINTN       GlobalCount = 0;
  UINTN       ImageSecurityCount = 0;
  UINTN       OtherCount = 0;

  Print (L"\n=== Variable Enumeration ===\n\n");
  Print (L"Enumerating all variables (first 20):\n\n");

  VariableName[0] = L'\0';

  while (TRUE) {
    NameSize = sizeof(VariableName);
    Status = gRT->GetNextVariableName (&NameSize, VariableName, &VendorGuid);

    if (Status == EFI_NOT_FOUND) {
      break;  // No more variables
    }

    if (EFI_ERROR (Status)) {
      Print (L"Error enumerating: %r\n", Status);
      break;
    }

    Count++;

    // Categorize by GUID
    if (CompareGuid (&VendorGuid, &gEfiGlobalVariableGuid)) {
      GlobalCount++;
      if (Count <= 20) {
        Print (L"  [Global] %s\n", VariableName);
      }
    } else if (CompareGuid (&VendorGuid, &gEfiImageSecurityDatabaseGuid)) {
      ImageSecurityCount++;
      if (Count <= 20) {
        Print (L"  [ImageSecurity] %s\n", VariableName);
      }
    } else {
      OtherCount++;
      if (Count <= 20) {
        Print (L"  [Other] %s\n", VariableName);
      }
    }

    if (Count == 20) {
      Print (L"  ...\n");
    }
  }

  Print (L"\nVariable Statistics:\n");
  Print (L"  Total variables: %d\n", Count);
  Print (L"  Global variables: %d\n", GlobalCount);
  Print (L"  Image Security variables: %d\n", ImageSecurityCount);
  Print (L"  Other vendor variables: %d\n", OtherCount);

  return EFI_SUCCESS;
}

/**
  Show variable storage information.
**/
EFI_STATUS
ShowVariableInfo (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT64      MaxStorageSize;
  UINT64      RemainingStorageSize;
  UINT64      MaxVariableSize;

  Print (L"\n=== Variable Storage Info ===\n\n");

  Status = gRT->QueryVariableInfo (
                  EFI_VARIABLE_NON_VOLATILE |
                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                  EFI_VARIABLE_RUNTIME_ACCESS,
                  &MaxStorageSize,
                  &RemainingStorageSize,
                  &MaxVariableSize
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"Non-Volatile Variable Storage:\n");
    Print (L"  Maximum Storage Size: %ld bytes (%ld KB)\n",
           MaxStorageSize, MaxStorageSize / 1024);
    Print (L"  Remaining Storage: %ld bytes (%ld KB)\n",
           RemainingStorageSize, RemainingStorageSize / 1024);
    Print (L"  Maximum Variable Size: %ld bytes\n", MaxVariableSize);
    Print (L"  Storage Used: %d%%\n",
           (UINT32)(100 - (RemainingStorageSize * 100 / MaxStorageSize)));
  } else {
    Print (L"QueryVariableInfo not supported: %r\n", Status);
  }

  // Query volatile storage
  Status = gRT->QueryVariableInfo (
                  EFI_VARIABLE_BOOTSERVICE_ACCESS |
                  EFI_VARIABLE_RUNTIME_ACCESS,
                  &MaxStorageSize,
                  &RemainingStorageSize,
                  &MaxVariableSize
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"\nVolatile Variable Storage:\n");
    Print (L"  Maximum Storage Size: %ld bytes (%ld KB)\n",
           MaxStorageSize, MaxStorageSize / 1024);
    Print (L"  Remaining Storage: %ld bytes\n", RemainingStorageSize);
    Print (L"  Maximum Variable Size: %ld bytes\n", MaxVariableSize);
  }

  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
VariableExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"UEFI Variables Example\n");
  Print (L"======================\n");

  // Read standard variables
  ReadStandardVariables ();

  // Show storage info
  ShowVariableInfo ();

  // Enumerate variables
  EnumerateAllVariables ();

  // Demo custom variable
  DemoCustomVariable ();

  Print (L"\n=== Variable Attributes Reference ===\n\n");
  Print (L"EFI_VARIABLE_NON_VOLATILE              - Persist across reboot\n");
  Print (L"EFI_VARIABLE_BOOTSERVICE_ACCESS        - Accessible during boot\n");
  Print (L"EFI_VARIABLE_RUNTIME_ACCESS            - Accessible by OS\n");
  Print (L"EFI_VARIABLE_HARDWARE_ERROR_RECORD     - Hardware error log\n");
  Print (L"EFI_VARIABLE_TIME_BASED_AUTHENTICATED  - Secure variable\n");

  Print (L"\nVariable example completed!\n");

  return EFI_SUCCESS;
}
