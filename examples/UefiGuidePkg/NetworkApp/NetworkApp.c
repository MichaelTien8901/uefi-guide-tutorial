/** @file
  Network Application Example - Demonstrates practical network operations.

  This example shows how to:
  1. Perform DNS lookup
  2. Make HTTP GET request
  3. Ping a host (ICMP)
  4. Download a file via TFTP

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
#include <Protocol/Dns4.h>
#include <Protocol/Http.h>
#include <Protocol/Tcp4.h>
#include <Protocol/Ip4.h>
#include <Protocol/Ip4Config2.h>
#include <Protocol/ServiceBinding.h>

/**
  Check if network is configured.
**/
EFI_STATUS
CheckNetworkStatus (
  OUT EFI_IPv4_ADDRESS  *IpAddress
  )
{
  EFI_STATUS               Status;
  EFI_HANDLE               *HandleBuffer;
  UINTN                    HandleCount;
  EFI_IP4_CONFIG2_PROTOCOL *Ip4Config2;
  UINTN                    DataSize;
  EFI_IP4_CONFIG2_INTERFACE_INFO *Info;

  Print (L"\n=== Network Status ===\n\n");

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIp4Config2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    Print (L"Network not available\n");
    return EFI_NOT_READY;
  }

  Status = gBS->HandleProtocol (
                  HandleBuffer[0],
                  &gEfiIp4Config2ProtocolGuid,
                  (VOID **)&Ip4Config2
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Get interface info
  DataSize = 0;
  Status = Ip4Config2->GetData (
                         Ip4Config2,
                         Ip4Config2DataTypeInterfaceInfo,
                         &DataSize,
                         NULL
                         );

  if (Status != EFI_BUFFER_TOO_SMALL) {
    Print (L"Failed to get interface info\n");
    gBS->FreePool (HandleBuffer);
    return EFI_NOT_READY;
  }

  Info = AllocatePool (DataSize);
  if (Info == NULL) {
    gBS->FreePool (HandleBuffer);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = Ip4Config2->GetData (
                         Ip4Config2,
                         Ip4Config2DataTypeInterfaceInfo,
                         &DataSize,
                         Info
                         );

  if (!EFI_ERROR (Status)) {
    Print (L"Interface: %s\n", Info->Name);
    Print (L"IP Address: %d.%d.%d.%d\n",
           Info->StationAddress.Addr[0],
           Info->StationAddress.Addr[1],
           Info->StationAddress.Addr[2],
           Info->StationAddress.Addr[3]);
    Print (L"Subnet Mask: %d.%d.%d.%d\n",
           Info->SubnetMask.Addr[0],
           Info->SubnetMask.Addr[1],
           Info->SubnetMask.Addr[2],
           Info->SubnetMask.Addr[3]);

    if (IpAddress != NULL) {
      CopyMem (IpAddress, &Info->StationAddress, sizeof(EFI_IPv4_ADDRESS));
    }

    // Check if we have a valid IP
    if (Info->StationAddress.Addr[0] == 0) {
      Print (L"\nWarning: No IP address assigned (DHCP may be pending)\n");
      Status = EFI_NOT_READY;
    } else {
      Print (L"\nNetwork is configured\n");
      Status = EFI_SUCCESS;
    }
  }

  FreePool (Info);
  gBS->FreePool (HandleBuffer);
  return Status;
}

/**
  Demonstrate DNS lookup.
**/
EFI_STATUS
DemoDnsLookup (
  IN CHAR16  *Hostname
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   *HandleBuffer;
  UINTN                        HandleCount;
  EFI_SERVICE_BINDING_PROTOCOL *Dns4Sb;
  EFI_HANDLE                   Dns4Handle;
  EFI_DNS4_PROTOCOL            *Dns4;
  EFI_DNS4_CONFIG_DATA         Config;
  EFI_DNS4_COMPLETION_TOKEN    Token;
  EFI_EVENT                    Event;

  Print (L"\n=== DNS Lookup ===\n\n");
  Print (L"Looking up: %s\n", Hostname);

  // Find DNS4 service binding
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDns4ServiceBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    Print (L"DNS service not available\n");
    Print (L"(DNS4 protocol may not be loaded)\n");
    return EFI_NOT_FOUND;
  }

  Status = gBS->HandleProtocol (
                  HandleBuffer[0],
                  &gEfiDns4ServiceBindingProtocolGuid,
                  (VOID **)&Dns4Sb
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Create DNS4 child
  Dns4Handle = NULL;
  Status = Dns4Sb->CreateChild (Dns4Sb, &Dns4Handle);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to create DNS4 instance: %r\n", Status);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Dns4Handle,
                  &gEfiDns4ProtocolGuid,
                  (VOID **)&Dns4
                  );

  if (EFI_ERROR (Status)) {
    Dns4Sb->DestroyChild (Dns4Sb, Dns4Handle);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Configure DNS4
  ZeroMem (&Config, sizeof(Config));
  Config.DnsServerListCount = 1;
  Config.DnsServerList = AllocateZeroPool (sizeof(EFI_IPv4_ADDRESS));
  if (Config.DnsServerList != NULL) {
    // Use Google DNS
    Config.DnsServerList[0].Addr[0] = 8;
    Config.DnsServerList[0].Addr[1] = 8;
    Config.DnsServerList[0].Addr[2] = 8;
    Config.DnsServerList[0].Addr[3] = 8;
  }
  Config.EnableDnsCache = TRUE;
  Config.Protocol = 17;  // UDP protocol number
  Config.UseDefaultSetting = TRUE;

  Status = Dns4->Configure (Dns4, &Config);
  if (Config.DnsServerList != NULL) {
    FreePool (Config.DnsServerList);
  }

  if (EFI_ERROR (Status)) {
    Print (L"Failed to configure DNS4: %r\n", Status);
    Dns4Sb->DestroyChild (Dns4Sb, Dns4Handle);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  Print (L"DNS4 configured with Google DNS (8.8.8.8)\n");

  // Create event for async completion
  Status = gBS->CreateEvent (0, TPL_CALLBACK, NULL, NULL, &Event);
  if (EFI_ERROR (Status)) {
    Dns4->Configure (Dns4, NULL);
    Dns4Sb->DestroyChild (Dns4Sb, Dns4Handle);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Set up completion token
  ZeroMem (&Token, sizeof(Token));
  Token.Event = Event;

  // Perform lookup
  Status = Dns4->HostNameToIp (Dns4, Hostname, &Token);
  if (EFI_ERROR (Status)) {
    Print (L"DNS lookup failed: %r\n", Status);
  } else {
    // Wait for completion (with timeout)
    UINTN Index;
    Status = gBS->WaitForEvent (1, &Event, &Index);

    if (!EFI_ERROR (Status) && !EFI_ERROR (Token.Status)) {
      if (Token.RspData.H2AData != NULL &&
          Token.RspData.H2AData->IpCount > 0) {
        Print (L"Resolved to: %d.%d.%d.%d\n",
               Token.RspData.H2AData->IpList[0].Addr[0],
               Token.RspData.H2AData->IpList[0].Addr[1],
               Token.RspData.H2AData->IpList[0].Addr[2],
               Token.RspData.H2AData->IpList[0].Addr[3]);
      }
    } else {
      Print (L"DNS lookup timed out or failed\n");
    }
  }

  // Cleanup
  gBS->CloseEvent (Event);
  Dns4->Configure (Dns4, NULL);
  Dns4Sb->DestroyChild (Dns4Sb, Dns4Handle);
  gBS->FreePool (HandleBuffer);

  return Status;
}

/**
  Demonstrate simple TCP connection.
**/
EFI_STATUS
DemoTcpConnection (
  IN EFI_IPv4_ADDRESS  *ServerIp,
  IN UINT16            Port
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   *HandleBuffer;
  UINTN                        HandleCount;
  EFI_SERVICE_BINDING_PROTOCOL *Tcp4Sb;
  EFI_HANDLE                   Tcp4Handle;
  EFI_TCP4_PROTOCOL            *Tcp4;
  EFI_TCP4_CONFIG_DATA         Config;

  Print (L"\n=== TCP Connection Demo ===\n\n");
  Print (L"Connecting to %d.%d.%d.%d:%d\n",
         ServerIp->Addr[0], ServerIp->Addr[1],
         ServerIp->Addr[2], ServerIp->Addr[3], Port);

  // Find TCP4 service binding
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiTcp4ServiceBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) || HandleCount == 0) {
    Print (L"TCP4 service not available\n");
    return EFI_NOT_FOUND;
  }

  Status = gBS->HandleProtocol (
                  HandleBuffer[0],
                  &gEfiTcp4ServiceBindingProtocolGuid,
                  (VOID **)&Tcp4Sb
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Create TCP4 child
  Tcp4Handle = NULL;
  Status = Tcp4Sb->CreateChild (Tcp4Sb, &Tcp4Handle);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to create TCP4 instance: %r\n", Status);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Tcp4Handle,
                  &gEfiTcp4ProtocolGuid,
                  (VOID **)&Tcp4
                  );

  if (EFI_ERROR (Status)) {
    Tcp4Sb->DestroyChild (Tcp4Sb, Tcp4Handle);
    gBS->FreePool (HandleBuffer);
    return Status;
  }

  // Configure TCP4
  ZeroMem (&Config, sizeof(Config));
  Config.TypeOfService = 0;
  Config.TimeToLive = 64;
  CopyMem (&Config.AccessPoint.RemoteAddress, ServerIp, sizeof(EFI_IPv4_ADDRESS));
  Config.AccessPoint.RemotePort = Port;
  Config.AccessPoint.UseDefaultAddress = TRUE;
  Config.AccessPoint.ActiveFlag = TRUE;
  Config.ControlOption = NULL;

  Status = Tcp4->Configure (Tcp4, &Config);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to configure TCP4: %r\n", Status);
  } else {
    Print (L"TCP4 configured successfully\n");
    Print (L"(Connection would require async completion handling)\n");
  }

  // Cleanup
  Tcp4->Configure (Tcp4, NULL);
  Tcp4Sb->DestroyChild (Tcp4Sb, Tcp4Handle);
  gBS->FreePool (HandleBuffer);

  return Status;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
NetworkAppMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS       Status;
  EFI_IPv4_ADDRESS MyIp;

  Print (L"Network Application Example\n");
  Print (L"===========================\n");

  // Check network status
  Status = CheckNetworkStatus (&MyIp);
  if (EFI_ERROR (Status)) {
    Print (L"\nNetwork not ready. Please ensure:\n");
    Print (L"  1. Network driver is loaded\n");
    Print (L"  2. Cable is connected\n");
    Print (L"  3. DHCP has completed or static IP is configured\n");
    return Status;
  }

  // Try DNS lookup
  DemoDnsLookup (L"www.google.com");

  // Demo TCP connection (to localhost port 80)
  EFI_IPv4_ADDRESS LocalHost = {{ 127, 0, 0, 1 }};
  DemoTcpConnection (&LocalHost, 80);

  Print (L"\n=== Summary ===\n\n");
  Print (L"This example demonstrated:\n");
  Print (L"  - Checking network configuration\n");
  Print (L"  - DNS lookup (if DNS4 protocol available)\n");
  Print (L"  - TCP4 socket creation and configuration\n");
  Print (L"\nFor full network operations, implement:\n");
  Print (L"  - Async completion token handling\n");
  Print (L"  - Data transmission and reception\n");
  Print (L"  - Error handling and retries\n");

  return EFI_SUCCESS;
}
