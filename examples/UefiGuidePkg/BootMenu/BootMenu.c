/** @file
  Graphical Boot Menu Example - Demonstrates custom boot menu UI.

  This example shows how to:
  1. Enumerate boot options
  2. Create a graphical menu using GOP
  3. Handle keyboard navigation
  4. Boot selected option

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
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/LoadedImage.h>
#include <Guid/GlobalVariable.h>

#define MAX_BOOT_OPTIONS  20
#define MENU_START_Y      100
#define MENU_ITEM_HEIGHT  30
#define MENU_PADDING      20

//
// Boot option structure
//
typedef struct {
  UINT16     OptionNumber;
  CHAR16     Description[128];
  BOOLEAN    IsValid;
} BOOT_OPTION_ENTRY;

//
// Menu state
//
typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop;
  UINTN                         Width;
  UINTN                         Height;
  BOOT_OPTION_ENTRY             Options[MAX_BOOT_OPTIONS];
  UINTN                         OptionCount;
  UINTN                         SelectedIndex;
} MENU_STATE;

//
// Colors
//
EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorBackground = { 0x30, 0x30, 0x30, 0x00 };  // Dark gray
EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorNormal     = { 0x80, 0x80, 0x80, 0x00 };  // Gray
EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorSelected   = { 0xFF, 0xA0, 0x00, 0x00 };  // Orange
EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorTitle      = { 0xFF, 0xFF, 0xFF, 0x00 };  // White
EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorHighlight  = { 0x50, 0x50, 0x60, 0x00 };  // Highlight bg

/**
  Fill rectangle with color.
**/
VOID
FillRect (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL       *Gop,
  IN UINTN                              X,
  IN UINTN                              Y,
  IN UINTN                              Width,
  IN UINTN                              Height,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *Color
  )
{
  Gop->Blt (Gop, Color, EfiBltVideoFill, 0, 0, X, Y, Width, Height, 0);
}

/**
  Draw text at position (uses console for simplicity).
**/
VOID
DrawText (
  IN UINTN    X,
  IN UINTN    Y,
  IN CHAR16   *Text,
  IN UINTN    Attribute
  )
{
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  gST->ConOut->SetCursorPosition (gST->ConOut, X / 8, Y / 16);  // Approximate
  Print (L"%s", Text);
}

/**
  Load boot options from UEFI variables.
**/
EFI_STATUS
LoadBootOptions (
  IN OUT MENU_STATE  *State
  )
{
  EFI_STATUS  Status;
  UINT16      *BootOrder;
  UINTN       BootOrderSize;
  UINTN       Index;
  CHAR16      VarName[16];
  UINT8       *OptionData;
  UINTN       OptionSize;

  State->OptionCount = 0;

  // Get BootOrder variable
  BootOrderSize = 0;
  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  NULL
                  );

  if (Status != EFI_BUFFER_TOO_SMALL) {
    return EFI_NOT_FOUND;
  }

  BootOrder = AllocatePool (BootOrderSize);
  if (BootOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  BootOrder
                  );

  if (EFI_ERROR (Status)) {
    FreePool (BootOrder);
    return Status;
  }

  // Load each boot option
  for (Index = 0; Index < BootOrderSize / sizeof(UINT16) && Index < MAX_BOOT_OPTIONS; Index++) {
    UnicodeSPrint (VarName, sizeof(VarName), L"Boot%04x", BootOrder[Index]);

    OptionSize = 0;
    Status = gRT->GetVariable (VarName, &gEfiGlobalVariableGuid, NULL, &OptionSize, NULL);

    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }

    OptionData = AllocatePool (OptionSize);
    if (OptionData == NULL) {
      continue;
    }

    Status = gRT->GetVariable (VarName, &gEfiGlobalVariableGuid, NULL, &OptionSize, OptionData);

    if (!EFI_ERROR (Status) && OptionSize > sizeof(UINT32) + sizeof(UINT16)) {
      // Parse EFI_LOAD_OPTION structure
      // Attributes (4 bytes) + FilePathListLength (2 bytes) + Description (null-terminated)
      CHAR16 *Description = (CHAR16 *)(OptionData + sizeof(UINT32) + sizeof(UINT16));

      State->Options[State->OptionCount].OptionNumber = BootOrder[Index];
      StrnCpyS (State->Options[State->OptionCount].Description,
                sizeof(State->Options[0].Description) / sizeof(CHAR16),
                Description,
                sizeof(State->Options[0].Description) / sizeof(CHAR16) - 1);
      State->Options[State->OptionCount].IsValid = TRUE;
      State->OptionCount++;
    }

    FreePool (OptionData);
  }

  FreePool (BootOrder);
  return State->OptionCount > 0 ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  Draw the boot menu.
**/
VOID
DrawMenu (
  IN MENU_STATE  *State
  )
{
  UINTN  Index;
  UINTN  Y;

  // Clear screen
  FillRect (State->Gop, 0, 0, State->Width, State->Height, &ColorBackground);

  // Draw title
  gST->ConOut->SetCursorPosition (gST->ConOut, 30, 3);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"=== UEFI Boot Menu ===");

  gST->ConOut->SetCursorPosition (gST->ConOut, 20, 5);
  Print (L"Use UP/DOWN arrows to select, ENTER to boot, ESC to exit");

  // Draw menu items
  Y = MENU_START_Y;
  for (Index = 0; Index < State->OptionCount; Index++) {
    // Draw selection highlight
    if (Index == State->SelectedIndex) {
      FillRect (State->Gop, MENU_PADDING, Y - 2,
                State->Width - 2 * MENU_PADDING, MENU_ITEM_HEIGHT - 4,
                &ColorHighlight);
    }

    // Draw option text
    gST->ConOut->SetCursorPosition (gST->ConOut, 5, Y / 16);

    if (Index == State->SelectedIndex) {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print (L" > ");
    } else {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      Print (L"   ");
    }

    Print (L"Boot%04x: %s", State->Options[Index].OptionNumber,
           State->Options[Index].Description);

    Y += MENU_ITEM_HEIGHT;
  }

  // Draw footer
  gST->ConOut->SetCursorPosition (gST->ConOut, 5, (Y + 40) / 16);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_DARKGRAY | EFI_BACKGROUND_BLACK);
  Print (L"Selected: %d of %d", State->SelectedIndex + 1, State->OptionCount);
}

/**
  Boot the selected option.
**/
EFI_STATUS
BootSelectedOption (
  IN MENU_STATE  *State
  )
{
  UINT16  BootOption;

  if (State->SelectedIndex >= State->OptionCount) {
    return EFI_INVALID_PARAMETER;
  }

  BootOption = State->Options[State->SelectedIndex].OptionNumber;

  Print (L"\n\nBooting Boot%04x: %s\n",
         BootOption, State->Options[State->SelectedIndex].Description);

  // Set BootNext variable
  gRT->SetVariable (
         L"BootNext",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS |
         EFI_VARIABLE_RUNTIME_ACCESS,
         sizeof(BootOption),
         &BootOption
         );

  // Reset to boot the selected option
  Print (L"Resetting system to boot selected option...\n");
  gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);

  return EFI_SUCCESS;  // Should not reach here
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
BootMenuMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS      Status;
  MENU_STATE      State;
  EFI_INPUT_KEY   Key;
  UINTN           Index;
  BOOLEAN         Running;

  ZeroMem (&State, sizeof(State));

  // Locate GOP
  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  (VOID **)&State.Gop
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Graphics not available, using text mode\n");
    State.Gop = NULL;
    State.Width = 800;
    State.Height = 600;
  } else {
    State.Width = State.Gop->Mode->Info->HorizontalResolution;
    State.Height = State.Gop->Mode->Info->VerticalResolution;
  }

  // Clear screen
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  // Load boot options
  Status = LoadBootOptions (&State);
  if (EFI_ERROR (Status) || State.OptionCount == 0) {
    Print (L"No boot options found\n");
    Print (L"Press any key to exit...\n");
    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    return EFI_NOT_FOUND;
  }

  // Main menu loop
  Running = TRUE;
  while (Running) {
    DrawMenu (&State);

    // Wait for key
    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

    if (EFI_ERROR (Status)) {
      continue;
    }

    // Handle key
    switch (Key.ScanCode) {
      case SCAN_UP:
        if (State.SelectedIndex > 0) {
          State.SelectedIndex--;
        }
        break;

      case SCAN_DOWN:
        if (State.SelectedIndex < State.OptionCount - 1) {
          State.SelectedIndex++;
        }
        break;

      case SCAN_ESC:
        Running = FALSE;
        break;

      default:
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
          BootSelectedOption (&State);
        }
        break;
    }
  }

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Boot menu exited\n");

  return EFI_SUCCESS;
}
