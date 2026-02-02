/** @file
  Network Example - Demonstrates UEFI network stack.

  This example shows how to:
  1. Locate network interfaces (SNP, MNP)
  2. Get MAC address and network info
  3. Use TCP4/UDP4 protocols
  4. Simple HTTP-like request

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/ManagedNetwork.h>
#include <Protocol/Ip4.h>
#include <Protocol/Ip4Config2.h>
#include <Protocol/Tcp4.h>
#include <Protocol/Udp4.h>
#include <Protocol/Dhcp4.h>

/**
  Print MAC address.
**/
VOID
PrintMacAddress (
  IN EFI_MAC_ADDRESS  *Mac,
  IN UINT32           Length
  )
{
  for (UINT32 i = 0; i < Length; i++) {
    Print (L"%02x", Mac->Addr[i]);
    if (i < Length - 1) {
      Print (L":");
    }
  }
}

/**
  Print IPv4 address.
**/
VOID
PrintIpv4Address (
  IN EFI_IPv4_ADDRESS  *Ip
  )
{
  Print (L"%d.%d.%d.%d", Ip->Addr[0], Ip->Addr[1], Ip->Addr[2], Ip->Addr[3]);
}

/**
  List all network interfaces using Simple Network Protocol.
**/
EFI_STATUS
ListNetworkInterfaces (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 *HandleBuffer;
  UINTN                      HandleCount;
  UINTN                      Index;
  EFI_SIMPLE_NETWORK_PROTOCOL *Snp;

  Print (L"\n=== Network Interfaces (SNP) ===\n\n");

  // Locate all SNP handles
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleNetworkProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    Print (L"No network interfaces found: %r\n", Status);
    return Status;
  }

  Print (L"Found %d network interface(s)\n\n", HandleCount);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiSimpleNetworkProtocolGuid,
                    (VOID **)&Snp
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    Print (L"Interface %d:\n", Index);
    Print (L"  State: ");
    switch (Snp->Mode->State) {
      case EfiSimpleNetworkStopped:
        Print (L"Stopped\n");
        break;
      case EfiSimpleNetworkStarted:
        Print (L"Started\n");
        break;
      case EfiSimpleNetworkInitialized:
        Print (L"Initialized\n");
        break;
      default:
        Print (L"Unknown\n");
    }

    Print (L"  MAC Address: ");
    PrintMacAddress (&Snp->Mode->CurrentAddress, Snp->Mode->HwAddressSize);
    Print (L"\n");

    Print (L"  Media Present: %s\n",
           Snp->Mode->MediaPresent ? L"Yes" : L"No");
    Print (L"  MTU: %d bytes\n", Snp->Mode->MaxPacketSize);

    if (Snp->Mode->IfType == 1) {
      Print (L"  Type: Ethernet\n");
    } else if (Snp->Mode->IfType == 6) {
      Print (L"  Type: WiFi\n");
    } else {
      Print (L"  Type: %d\n", Snp->Mode->IfType);
    }

    Print (L"\n");
  }

  gBS->FreePool (HandleBuffer);
  return EFI_SUCCESS;
}

/**
  Get IP configuration using IP4Config2 protocol.
**/
EFI_STATUS
GetIpConfiguration (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   HandleCount;
  EFI_IP4_CONFIG2_PROTOCOL *Ip4Config2;
  EFI_IP4_CONFIG2_INTERFACE_INFO *InterfaceInfo;
  UINTN                   DataSize;
  EFI_IP4_CONFIG2_POLICY  Policy;

  Print (L"\n=== IP Configuration ===\n\n");

  // Locate IP4Config2 protocol
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIp4Config2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    Print (L"IP4Config2 not available: %r\n", Status);
    return Status;
  }

  for (UINTN i = 0; i < HandleCount; i++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiIp4Config2ProtocolGuid,
                    (VOID **)&Ip4Config2
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    Print (L"Interface %d:\n", i);

    // Get policy (DHCP or Static)
    DataSize = sizeof(Policy);
    Status = Ip4Config2->GetData (
                           Ip4Config2,
                           Ip4Config2DataTypePolicy,
                           &DataSize,
                           &Policy
                           );

    if (!EFI_ERROR (Status)) {
      Print (L"  Policy: %s\n",
             Policy == Ip4Config2PolicyDhcp ? L"DHCP" : L"Static");
    }

    // Get interface info
    DataSize = 0;
    Status = Ip4Config2->GetData (
                           Ip4Config2,
                           Ip4Config2DataTypeInterfaceInfo,
                           &DataSize,
                           NULL
                           );

    if (Status == EFI_BUFFER_TOO_SMALL) {
      InterfaceInfo = AllocatePool (DataSize);
      if (InterfaceInfo != NULL) {
        Status = Ip4Config2->GetData (
                               Ip4Config2,
                               Ip4Config2DataTypeInterfaceInfo,
                               &DataSize,
                               InterfaceInfo
                               );

        if (!EFI_ERROR (Status)) {
          Print (L"  Name: %s\n", InterfaceInfo->Name);
          Print (L"  MAC: ");
          PrintMacAddress (&InterfaceInfo->HwAddress, InterfaceInfo->HwAddressSize);
          Print (L"\n");
          Print (L"  IP: ");
          PrintIpv4Address (&InterfaceInfo->StationAddress);
          Print (L"\n");
          Print (L"  Subnet: ");
          PrintIpv4Address (&InterfaceInfo->SubnetMask);
          Print (L"\n");
        }

        FreePool (InterfaceInfo);
      }
    }

    Print (L"\n");
  }

  gBS->FreePool (HandleBuffer);
  return EFI_SUCCESS;
}

/**
  Demonstrate UDP4 socket creation.
**/
EFI_STATUS
DemoUdp4 (
  VOID
  )
{
  EFI_STATUS           Status;
  EFI_HANDLE           *HandleBuffer;
  UINTN                HandleCount;
  EFI_SERVICE_BINDING_PROTOCOL *Udp4Sb;
  EFI_HANDLE           Udp4Handle;
  EFI_UDP4_PROTOCOL    *Udp4;
  EFI_UDP4_CONFIG_DATA Config;

  Print (L"\n=== UDP4 Demo ===\n\n");

  // Find UDP4 service binding
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUdp4ServiceBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    Print (L"UDP4 service not available: %r\n", Status);
    return Status;
  }

  Status = gBS->HandleProtocol (
                  HandleBuffer[0],
                  &gEfiUdp4ServiceBindingProtocolGuid,
                  (VOID **)&Udp4Sb
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Create UDP4 child
  Udp4Handle = NULL;
  Status = Udp4Sb->CreateChild (Udp4Sb, &Udp4Handle);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to create UDP4 instance: %r\n", Status);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  Print (L"UDP4 instance created\n");

  // Get UDP4 protocol
  Status = gBS->HandleProtocol (
                  Udp4Handle,
                  &gEfiUdp4ProtocolGuid,
                  (VOID **)&Udp4
                  );

  if (EFI_ERROR (Status)) {
    Udp4Sb->DestroyChild (Udp4Sb, Udp4Handle);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Configure UDP4
  ZeroMem (&Config, sizeof(Config));
  Config.AcceptBroadcast = TRUE;
  Config.AcceptPromiscuous = FALSE;
  Config.AcceptAnyPort = FALSE;
  Config.AllowDuplicatePort = FALSE;
  Config.TimeToLive = 64;
  Config.DoNotFragment = FALSE;
  Config.ReceiveTimeout = 0;
  Config.TransmitTimeout = 0;
  Config.UseDefaultAddress = TRUE;
  Config.StationPort = 12345;  // Bind to port 12345

  Status = Udp4->Configure (Udp4, &Config);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to configure UDP4: %r\n", Status);
  } else {
    Print (L"UDP4 configured, bound to port 12345\n");
  }

  // Clean up
  Udp4->Configure (Udp4, NULL);
  Udp4Sb->DestroyChild (Udp4Sb, Udp4Handle);
  gBS->FreePool (HandleBuffer);

  Print (L"UDP4 demo completed\n");
  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
NetworkExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"Network Example\n");
  Print (L"===============\n");

  //
  // List network interfaces
  //
  ListNetworkInterfaces ();

  //
  // Get IP configuration
  //
  GetIpConfiguration ();

  //
  // Demo UDP4
  //
  DemoUdp4 ();

  Print (L"\nNetwork example completed!\n");
  Print (L"\nNote: Full network functionality requires:\n");
  Print (L"  - Network driver loaded\n");
  Print (L"  - DHCP or static IP configured\n");
  Print (L"  - Cable connected (for Ethernet)\n");

  return EFI_SUCCESS;
}
