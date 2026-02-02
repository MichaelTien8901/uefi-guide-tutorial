/** @file
  Console I/O Example - Demonstrates UEFI console input/output.

  This example shows how to:
  1. Use SimpleTextOutput protocol for printing
  2. Use SimpleTextInput protocol for keyboard input
  3. Handle console modes and colors
  4. Clear screen and position cursor

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>

/**
  Demonstrate text output features.
**/
EFI_STATUS
DemoTextOutput (
  VOID
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  UINTN                            Columns;
  UINTN                            Rows;
  UINTN                            Mode;
  UINTN                            MaxMode;

  ConOut = gST->ConOut;

  Print (L"\n=== Text Output Demo ===\n\n");

  //
  // Query available modes
  //
  MaxMode = ConOut->Mode->MaxMode;
  Print (L"Available console modes: %d\n", MaxMode);

  for (Mode = 0; Mode < MaxMode; Mode++) {
    if (ConOut->QueryMode (ConOut, Mode, &Columns, &Rows) == EFI_SUCCESS) {
      Print (L"  Mode %d: %d x %d\n", Mode, Columns, Rows);
    }
  }

  Print (L"\nCurrent mode: %d (%d x %d)\n",
         ConOut->Mode->Mode,
         ConOut->Mode->CursorColumn,
         ConOut->Mode->CursorRow);

  //
  // Demonstrate colors
  //
  Print (L"\nColor demonstration:\n");

  // Save original attribute
  UINTN OriginalAttribute = ConOut->Mode->Attribute;

  // Show different foreground colors
  UINTN Colors[] = {
    EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_CYAN,
    EFI_RED, EFI_MAGENTA, EFI_BROWN, EFI_LIGHTGRAY,
    EFI_DARKGRAY, EFI_LIGHTBLUE, EFI_LIGHTGREEN, EFI_LIGHTCYAN,
    EFI_LIGHTRED, EFI_LIGHTMAGENTA, EFI_YELLOW, EFI_WHITE
  };

  CHAR16 *ColorNames[] = {
    L"Black", L"Blue", L"Green", L"Cyan",
    L"Red", L"Magenta", L"Brown", L"LightGray",
    L"DarkGray", L"LightBlue", L"LightGreen", L"LightCyan",
    L"LightRed", L"LightMagenta", L"Yellow", L"White"
  };

  for (UINTN i = 0; i < 16; i++) {
    ConOut->SetAttribute (ConOut, Colors[i] | EFI_BACKGROUND_BLACK);
    Print (L"  %s", ColorNames[i]);
    if ((i + 1) % 4 == 0) {
      Print (L"\n");
    }
  }

  // Restore original attribute
  ConOut->SetAttribute (ConOut, OriginalAttribute);
  Print (L"\n");

  //
  // Demonstrate cursor positioning
  //
  Print (L"Cursor positioning demo:\n");
  Print (L"  Original position\n");

  // Save current position
  UINTN SaveCol = ConOut->Mode->CursorColumn;
  UINTN SaveRow = ConOut->Mode->CursorRow;

  // Move to specific position
  ConOut->SetCursorPosition (ConOut, 40, SaveRow - 1);
  Print (L"<-- Positioned text");

  // Return to saved position
  ConOut->SetCursorPosition (ConOut, SaveCol, SaveRow);
  Print (L"\n");

  return EFI_SUCCESS;
}

/**
  Demonstrate text input features.
**/
EFI_STATUS
DemoTextInput (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn;
  EFI_INPUT_KEY                   Key;
  CHAR16                          Buffer[64];
  UINTN                           Index;

  ConIn = gST->ConIn;

  Print (L"\n=== Text Input Demo ===\n\n");

  //
  // Simple key read
  //
  Print (L"Press any key to continue...\n");

  // Wait for key
  Status = gBS->WaitForEvent (1, &ConIn->WaitForKey, &Index);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = ConIn->ReadKeyStroke (ConIn, &Key);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Print (L"You pressed: ");
  if (Key.UnicodeChar != 0) {
    Print (L"'%c' (Unicode: 0x%04x)\n", Key.UnicodeChar, Key.UnicodeChar);
  } else {
    Print (L"Scan code: 0x%04x\n", Key.ScanCode);
  }

  //
  // Read a line of text
  //
  Print (L"\nEnter some text (up to 60 chars, press Enter): ");

  Index = 0;
  while (Index < 60) {
    // Wait for key
    gBS->WaitForEvent (1, &ConIn->WaitForKey, &Index);
    Status = ConIn->ReadKeyStroke (ConIn, &Key);

    if (EFI_ERROR (Status)) {
      continue;
    }

    // Enter key ends input
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      break;
    }

    // Backspace handling
    if (Key.UnicodeChar == CHAR_BACKSPACE) {
      if (Index > 0) {
        Index--;
        Print (L"\b \b");  // Erase character
      }
      continue;
    }

    // Printable character
    if (Key.UnicodeChar >= L' ') {
      Buffer[Index++] = Key.UnicodeChar;
      Print (L"%c", Key.UnicodeChar);
    }
  }

  Buffer[Index] = L'\0';
  Print (L"\n\nYou entered: \"%s\" (%d characters)\n", Buffer, Index);

  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
ConsoleExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  // Clear screen
  gST->ConOut->ClearScreen (gST->ConOut);

  Print (L"Console I/O Example\n");
  Print (L"===================\n");

  Status = DemoTextOutput ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DemoTextInput ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Print (L"\nConsole example completed!\n");
  return EFI_SUCCESS;
}
