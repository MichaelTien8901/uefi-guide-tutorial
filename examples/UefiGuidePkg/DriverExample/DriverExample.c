/** @file
  UEFI Driver Model Example - Demonstrates driver binding protocol.

  This driver shows the basic structure of a UEFI driver that follows
  the driver model with Supported, Start, and Stop functions.

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/PciIo.h>

//
// Forward declarations
//
EFI_STATUS
EFIAPI
DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  );

EFI_STATUS
EFIAPI
DriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  );

//
// Driver Binding Protocol instance
//
EFI_DRIVER_BINDING_PROTOCOL gDriverBindingProtocol = {
  DriverBindingSupported,
  DriverBindingStart,
  DriverBindingStop,
  0x10,  // Version
  NULL,  // ImageHandle - filled in by entry point
  NULL   // DriverBindingHandle - filled in by entry point
};

/**
  Test to see if this driver supports a given controller.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  ControllerHandle     Handle of device to test.
  @param[in]  RemainingDevicePath  Optional parameter use to pick a specific child device.

  @retval EFI_SUCCESS          This driver supports this device.
  @retval EFI_ALREADY_STARTED  This driver is already running on this device.
  @retval other                This driver does not support this device.
**/
EFI_STATUS
EFIAPI
DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS  Status;
  EFI_PCI_IO_PROTOCOL *PciIo;

  //
  // Example: Check if controller has PCI I/O protocol
  // Real drivers would check vendor/device IDs
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Close the protocol - we were just checking if it exists
  //
  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  DEBUG ((DEBUG_INFO, "DriverExample: Supported() called\n"));
  return EFI_SUCCESS;
}

/**
  Start this driver on ControllerHandle.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  ControllerHandle     Handle of device to bind driver to.
  @param[in]  RemainingDevicePath  Optional parameter use to pick a specific child device.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle.
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle.
  @retval other                This driver does not support this device.
**/
EFI_STATUS
EFIAPI
DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS  Status;
  EFI_PCI_IO_PROTOCOL *PciIo;

  DEBUG ((DEBUG_INFO, "DriverExample: Start() called\n"));

  //
  // Open PCI I/O protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Driver would initialize hardware and install protocols here
  //
  Print (L"DriverExample: Managing device\n");

  return EFI_SUCCESS;
}

/**
  Stop this driver on ControllerHandle.

  @param[in]  This              Protocol instance pointer.
  @param[in]  ControllerHandle  Handle of device to stop driver on.
  @param[in]  NumberOfChildren  Number of Handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle.
  @retval other             This driver was not removed from this device.
**/
EFI_STATUS
EFIAPI
DriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "DriverExample: Stop() called\n"));

  //
  // Close protocols opened in Start()
  //
  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  return EFI_SUCCESS;
}

/**
  Entry point for the driver.

  @param[in]  ImageHandle    The image handle.
  @param[in]  SystemTable    The system table.

  @retval EFI_SUCCESS        Driver loaded successfully.
**/
EFI_STATUS
EFIAPI
DriverExampleEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install driver binding protocol
  //
  gDriverBindingProtocol.ImageHandle = ImageHandle;
  gDriverBindingProtocol.DriverBindingHandle = ImageHandle;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gDriverBindingProtocol,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DriverExample: Failed to install binding protocol\n"));
    return Status;
  }

  Print (L"DriverExample: Driver loaded successfully\n");
  return EFI_SUCCESS;
}
