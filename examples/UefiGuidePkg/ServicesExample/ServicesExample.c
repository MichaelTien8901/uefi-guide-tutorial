/** @file
  Boot and Runtime Services Example - Demonstrates UEFI services usage.

  This example shows how to:
  1. Use Boot Services (events, timers, protocols)
  2. Use Runtime Services (time, variables, reset)
  3. Understand service table structure
  4. Handle ExitBootServices transition

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
#include <Guid/GlobalVariable.h>

//
// Timer callback context
//
typedef struct {
  UINTN   Count;
  BOOLEAN Done;
} TIMER_CONTEXT;

/**
  Timer event callback function.
**/
VOID
EFIAPI
TimerCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  TIMER_CONTEXT *Ctx = (TIMER_CONTEXT *)Context;

  Ctx->Count++;
  Print (L"  Timer tick #%d\n", Ctx->Count);

  if (Ctx->Count >= 5) {
    Ctx->Done = TRUE;
  }
}

/**
  Demonstrate Boot Services - Events and Timers.
**/
EFI_STATUS
DemoBootServicesEvents (
  VOID
  )
{
  EFI_STATUS     Status;
  EFI_EVENT      TimerEvent;
  TIMER_CONTEXT  Context;
  UINTN          Index;

  Print (L"\n=== Boot Services: Events & Timers ===\n\n");

  //
  // Create a timer event
  //
  Print (L"1. Creating periodic timer event...\n");

  Context.Count = 0;
  Context.Done = FALSE;

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  TimerCallback,
                  &Context,
                  &TimerEvent
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to create event: %r\n", Status);
    return Status;
  }

  //
  // Set timer - 500ms periodic
  //
  Print (L"2. Setting periodic timer (500ms intervals)...\n");

  Status = gBS->SetTimer (
                  TimerEvent,
                  TimerPeriodic,
                  5000000  // 500ms in 100ns units
                  );

  if (EFI_ERROR (Status)) {
    Print (L"   Failed to set timer: %r\n", Status);
    gBS->CloseEvent (TimerEvent);
    return Status;
  }

  //
  // Wait for timer callbacks
  //
  Print (L"3. Waiting for 5 timer ticks...\n");

  while (!Context.Done) {
    // Poll for events
    gBS->WaitForEvent (1, &TimerEvent, &Index);
  }

  //
  // Cancel timer and close event
  //
  Print (L"4. Canceling timer...\n");

  gBS->SetTimer (TimerEvent, TimerCancel, 0);
  gBS->CloseEvent (TimerEvent);

  Print (L"   Timer demo completed\n");

  return EFI_SUCCESS;
}

/**
  Demonstrate Boot Services - Task Priority Levels.
**/
VOID
DemoTaskPriorityLevels (
  VOID
  )
{
  EFI_TPL  OldTpl;

  Print (L"\n=== Boot Services: Task Priority Levels ===\n\n");

  Print (L"TPL Levels:\n");
  Print (L"  TPL_APPLICATION  = %d (normal execution)\n", TPL_APPLICATION);
  Print (L"  TPL_CALLBACK     = %d (timer callbacks)\n", TPL_CALLBACK);
  Print (L"  TPL_NOTIFY       = %d (high priority)\n", TPL_NOTIFY);
  Print (L"  TPL_HIGH_LEVEL   = %d (highest, interrupts disabled)\n", TPL_HIGH_LEVEL);

  //
  // Demonstrate raising and restoring TPL
  //
  Print (L"\n1. Raising TPL to TPL_NOTIFY...\n");

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  Print (L"   Previous TPL: %d\n", OldTpl);

  // Critical section - interrupts at lower priority are blocked
  Print (L"   (In critical section at TPL_NOTIFY)\n");

  Print (L"2. Restoring TPL...\n");
  gBS->RestoreTPL (OldTpl);
  Print (L"   TPL restored to %d\n", OldTpl);
}

/**
  Demonstrate Runtime Services - Time.
**/
EFI_STATUS
DemoRuntimeServicesTime (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_TIME          Time;
  EFI_TIME_CAPABILITIES Capabilities;

  Print (L"\n=== Runtime Services: Time ===\n\n");

  //
  // Get current time
  //
  Print (L"1. Getting current time...\n");

  Status = gRT->GetTime (&Time, &Capabilities);
  if (EFI_ERROR (Status)) {
    Print (L"   Failed: %r\n", Status);
    return Status;
  }

  Print (L"   Date: %04d-%02d-%02d\n", Time.Year, Time.Month, Time.Day);
  Print (L"   Time: %02d:%02d:%02d.%09d\n",
         Time.Hour, Time.Minute, Time.Second, Time.Nanosecond);
  Print (L"   Timezone: UTC%+d\n",
         Time.TimeZone == EFI_UNSPECIFIED_TIMEZONE ? 0 : Time.TimeZone / 60);
  Print (L"   Daylight: 0x%02x\n", Time.Daylight);

  Print (L"\n2. Time capabilities:\n");
  Print (L"   Resolution: %d (counts/second)\n", Capabilities.Resolution);
  Print (L"   Accuracy: %d (error rate in ppm)\n", Capabilities.Accuracy);
  Print (L"   Sets to zero: %s\n", Capabilities.SetsToZero ? L"Yes" : L"No");

  return EFI_SUCCESS;
}

/**
  Demonstrate Runtime Services - Variables.
**/
EFI_STATUS
DemoRuntimeServicesVariables (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT16      BootCurrent;
  UINTN       DataSize;
  UINT8       SecureBoot;
  CHAR16      *BootOrder;
  UINTN       BootOrderSize;
  UINTN       i;

  Print (L"\n=== Runtime Services: Variables ===\n\n");

  //
  // Read BootCurrent variable
  //
  Print (L"1. Reading BootCurrent variable...\n");

  DataSize = sizeof(BootCurrent);
  Status = gRT->GetVariable (
                  L"BootCurrent",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &BootCurrent
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"   BootCurrent: Boot%04x\n", BootCurrent);
  } else {
    Print (L"   Not available: %r\n", Status);
  }

  //
  // Read SecureBoot variable
  //
  Print (L"\n2. Reading SecureBoot variable...\n");

  DataSize = sizeof(SecureBoot);
  Status = gRT->GetVariable (
                  L"SecureBoot",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &SecureBoot
                  );

  if (!EFI_ERROR (Status)) {
    Print (L"   SecureBoot: %s\n", SecureBoot ? L"Enabled" : L"Disabled");
  } else {
    Print (L"   Not available: %r\n", Status);
  }

  //
  // Read BootOrder variable
  //
  Print (L"\n3. Reading BootOrder variable...\n");

  BootOrderSize = 0;
  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  NULL
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    BootOrder = AllocatePool (BootOrderSize);
    if (BootOrder != NULL) {
      Status = gRT->GetVariable (
                      L"BootOrder",
                      &gEfiGlobalVariableGuid,
                      NULL,
                      &BootOrderSize,
                      BootOrder
                      );

      if (!EFI_ERROR (Status)) {
        Print (L"   BootOrder: ");
        for (i = 0; i < BootOrderSize / sizeof(UINT16); i++) {
          Print (L"Boot%04x ", BootOrder[i]);
        }
        Print (L"\n");
      }

      FreePool (BootOrder);
    }
  } else {
    Print (L"   Not available: %r\n", Status);
  }

  //
  // Enumerate variables
  //
  Print (L"\n4. Enumerating variables (first 10)...\n");

  CHAR16     VariableName[256];
  EFI_GUID   VendorGuid;
  UINTN      NameSize;
  UINTN      Count = 0;

  VariableName[0] = L'\0';

  while (Count < 10) {
    NameSize = sizeof(VariableName);
    Status = gRT->GetNextVariableName (&NameSize, VariableName, &VendorGuid);

    if (Status == EFI_NOT_FOUND) {
      break;
    }

    if (EFI_ERROR (Status)) {
      break;
    }

    Print (L"   %s\n", VariableName);
    Count++;
  }

  if (Count == 10) {
    Print (L"   ... (more variables exist)\n");
  }

  return EFI_SUCCESS;
}

/**
  Demonstrate Runtime Services - System Reset.
**/
VOID
DemoResetTypes (
  VOID
  )
{
  Print (L"\n=== Runtime Services: Reset System ===\n\n");

  Print (L"Available reset types:\n");
  Print (L"  EfiResetCold     - Full power cycle\n");
  Print (L"  EfiResetWarm     - Reset without power cycle\n");
  Print (L"  EfiResetShutdown - Power off system\n");
  Print (L"  EfiResetPlatformSpecific - Platform-defined reset\n");

  Print (L"\nUsage: gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);\n");
  Print (L"\n(Not executing reset in this demo)\n");
}

/**
  Display service table information.
**/
VOID
ShowServiceTableInfo (
  VOID
  )
{
  Print (L"\n=== UEFI Service Tables ===\n\n");

  Print (L"System Table (gST):\n");
  Print (L"  Address: 0x%p\n", gST);
  Print (L"  Revision: %d.%d\n", gST->Hdr.Revision >> 16, gST->Hdr.Revision & 0xFFFF);
  Print (L"  Firmware Vendor: %s\n", gST->FirmwareVendor);
  Print (L"  Config Tables: %d\n", gST->NumberOfTableEntries);

  Print (L"\nBoot Services (gBS):\n");
  Print (L"  Address: 0x%p\n", gBS);
  Print (L"  Revision: 0x%08x\n", gBS->Hdr.Revision);

  Print (L"\nRuntime Services (gRT):\n");
  Print (L"  Address: 0x%p\n", gRT);
  Print (L"  Revision: 0x%08x\n", gRT->Hdr.Revision);
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
ServicesExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"Boot and Runtime Services Example\n");
  Print (L"==================================\n");

  // Show service table info
  ShowServiceTableInfo ();

  // Demo Boot Services
  DemoBootServicesEvents ();
  DemoTaskPriorityLevels ();

  // Demo Runtime Services
  DemoRuntimeServicesTime ();
  DemoRuntimeServicesVariables ();
  DemoResetTypes ();

  Print (L"\n=== Summary ===\n\n");
  Print (L"Boot Services are available until ExitBootServices() is called.\n");
  Print (L"Runtime Services remain available after OS takes control.\n");
  Print (L"\nServices example completed!\n");

  return EFI_SUCCESS;
}
