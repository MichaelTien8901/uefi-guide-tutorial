/** @file
  Protocol Example - Demonstrates creating and using custom protocols.

  This example shows how to:
  1. Define a custom protocol with GUID
  2. Implement the protocol interface
  3. Install the protocol on a handle
  4. Locate and use protocols

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

//
// Define a custom protocol GUID
// Generate your own GUID for real protocols
//
#define EXAMPLE_PROTOCOL_GUID \
  { 0x12345678, 0x1234, 0x1234, { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } }

EFI_GUID gExampleProtocolGuid = EXAMPLE_PROTOCOL_GUID;

//
// Forward declarations for protocol functions
//
typedef struct _EXAMPLE_PROTOCOL EXAMPLE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EXAMPLE_GET_VALUE)(
  IN  EXAMPLE_PROTOCOL  *This,
  OUT UINT32            *Value
  );

typedef
EFI_STATUS
(EFIAPI *EXAMPLE_SET_VALUE)(
  IN  EXAMPLE_PROTOCOL  *This,
  IN  UINT32            Value
  );

//
// Protocol structure definition
//
struct _EXAMPLE_PROTOCOL {
  UINT32              Revision;
  EXAMPLE_GET_VALUE   GetValue;
  EXAMPLE_SET_VALUE   SetValue;
  UINT32              StoredValue;  // Private data
};

//
// Protocol function implementations
//
EFI_STATUS
EFIAPI
ExampleGetValue (
  IN  EXAMPLE_PROTOCOL  *This,
  OUT UINT32            *Value
  )
{
  if (This == NULL || Value == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Value = This->StoredValue;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ExampleSetValue (
  IN  EXAMPLE_PROTOCOL  *This,
  IN  UINT32            Value
  )
{
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  This->StoredValue = Value;
  Print (L"Protocol: Value set to %d\n", Value);
  return EFI_SUCCESS;
}

//
// Protocol instance
//
EXAMPLE_PROTOCOL gExampleProtocol = {
  0x00010000,      // Revision 1.0
  ExampleGetValue,
  ExampleSetValue,
  0                // Initial stored value
};

/**
  Demonstrate protocol installation and usage.

  @param[in]  ImageHandle    The image handle.
  @param[in]  SystemTable    The system table.

  @retval EFI_SUCCESS        Application ran successfully.
**/
EFI_STATUS
EFIAPI
ProtocolExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        ProtocolHandle;
  EXAMPLE_PROTOCOL  *Protocol;
  UINT32            Value;

  Print (L"Protocol Example - Custom Protocol Demo\n");
  Print (L"========================================\n\n");

  //
  // Step 1: Install our protocol on a new handle
  //
  Print (L"1. Installing custom protocol...\n");

  ProtocolHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &ProtocolHandle,
                  &gExampleProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gExampleProtocol
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to install protocol: %r\n", Status);
    return Status;
  }
  Print (L"   Protocol installed on handle %p\n\n", ProtocolHandle);

  //
  // Step 2: Locate the protocol (as another component would)
  //
  Print (L"2. Locating protocol...\n");

  Status = gBS->LocateProtocol (
                  &gExampleProtocolGuid,
                  NULL,
                  (VOID **)&Protocol
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to locate protocol: %r\n", Status);
    goto Cleanup;
  }
  Print (L"   Protocol found at %p\n", Protocol);
  Print (L"   Protocol revision: 0x%08x\n\n", Protocol->Revision);

  //
  // Step 3: Use the protocol
  //
  Print (L"3. Using protocol functions...\n");

  // Set a value
  Status = Protocol->SetValue (Protocol, 42);
  if (EFI_ERROR (Status)) {
    Print (L"   SetValue failed: %r\n", Status);
    goto Cleanup;
  }

  // Get the value back
  Status = Protocol->GetValue (Protocol, &Value);
  if (EFI_ERROR (Status)) {
    Print (L"   GetValue failed: %r\n", Status);
    goto Cleanup;
  }
  Print (L"   Retrieved value: %d\n\n", Value);

  //
  // Step 4: Demonstrate HandleProtocol
  //
  Print (L"4. Using HandleProtocol...\n");

  Status = gBS->HandleProtocol (
                  ProtocolHandle,
                  &gExampleProtocolGuid,
                  (VOID **)&Protocol
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   HandleProtocol failed: %r\n", Status);
    goto Cleanup;
  }
  Print (L"   Got protocol from specific handle\n\n");

  Print (L"Protocol example completed successfully!\n");

Cleanup:
  //
  // Step 5: Uninstall the protocol
  //
  Print (L"5. Cleaning up - uninstalling protocol...\n");

  Status = gBS->UninstallProtocolInterface (
                  ProtocolHandle,
                  &gExampleProtocolGuid,
                  &gExampleProtocol
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to uninstall: %r\n", Status);
  } else {
    Print (L"   Protocol uninstalled\n");
  }

  return EFI_SUCCESS;
}
