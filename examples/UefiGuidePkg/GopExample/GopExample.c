/** @file
  Graphics Output Protocol Example - Demonstrates UEFI graphics.

  This example shows how to:
  1. Locate and use GOP (Graphics Output Protocol)
  2. Query and set video modes
  3. Draw pixels and rectangles
  4. Use Blt (Block Transfer) operations

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/GraphicsOutput.h>

//
// Color definitions (BGRA format for GOP)
//
#define COLOR_BLACK    { 0x00, 0x00, 0x00, 0x00 }
#define COLOR_WHITE    { 0xFF, 0xFF, 0xFF, 0x00 }
#define COLOR_RED      { 0x00, 0x00, 0xFF, 0x00 }
#define COLOR_GREEN    { 0x00, 0xFF, 0x00, 0x00 }
#define COLOR_BLUE     { 0xFF, 0x00, 0x00, 0x00 }
#define COLOR_YELLOW   { 0x00, 0xFF, 0xFF, 0x00 }
#define COLOR_CYAN     { 0xFF, 0xFF, 0x00, 0x00 }
#define COLOR_MAGENTA  { 0xFF, 0x00, 0xFF, 0x00 }

/**
  Draw a filled rectangle using GOP Blt.
**/
EFI_STATUS
DrawRectangle (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL       *Gop,
  IN UINTN                              X,
  IN UINTN                              Y,
  IN UINTN                              Width,
  IN UINTN                              Height,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *Color
  )
{
  return Gop->Blt (
                Gop,
                Color,
                EfiBltVideoFill,
                0, 0,           // Source X, Y (ignored for fill)
                X, Y,           // Destination X, Y
                Width, Height,
                0               // Delta (ignored for fill)
                );
}

/**
  Display available video modes.
**/
VOID
ShowVideoModes (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINTN                                 SizeOfInfo;
  UINT32                                Mode;

  Print (L"\nAvailable video modes:\n");
  Print (L"Mode   Resolution    Pixels/Line  Format\n");
  Print (L"----   ----------    -----------  ------\n");

  for (Mode = 0; Mode < Gop->Mode->MaxMode; Mode++) {
    Status = Gop->QueryMode (Gop, Mode, &SizeOfInfo, &Info);
    if (EFI_ERROR (Status)) {
      continue;
    }

    CHAR16 *PixelFormat;
    switch (Info->PixelFormat) {
      case PixelRedGreenBlueReserved8BitPerColor:
        PixelFormat = L"RGBR";
        break;
      case PixelBlueGreenRedReserved8BitPerColor:
        PixelFormat = L"BGRR";
        break;
      case PixelBitMask:
        PixelFormat = L"Mask";
        break;
      case PixelBltOnly:
        PixelFormat = L"Blt";
        break;
      default:
        PixelFormat = L"????";
    }

    Print (L"%c%3d   %4d x %4d   %5d        %s\n",
           (Mode == Gop->Mode->Mode) ? L'*' : L' ',
           Mode,
           Info->HorizontalResolution,
           Info->VerticalResolution,
           Info->PixelsPerScanLine,
           PixelFormat);

    gBS->FreePool (Info);
  }

  Print (L"\n* = current mode\n");
}

/**
  Draw a simple graphics demo.
**/
EFI_STATUS
DrawGraphicsDemo (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  )
{
  EFI_STATUS                    Status;
  UINTN                         Width;
  UINTN                         Height;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Black = COLOR_BLACK;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Colors[8];
  UINTN                         i;
  UINTN                         BoxWidth;
  UINTN                         BoxHeight;

  Width = Gop->Mode->Info->HorizontalResolution;
  Height = Gop->Mode->Info->VerticalResolution;

  Print (L"\nDrawing graphics demo at %d x %d...\n", Width, Height);

  // Clear screen to black
  Status = DrawRectangle (Gop, 0, 0, Width, Height, &Black);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Initialize colors
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL TempColors[] = {
    COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW,
    COLOR_CYAN, COLOR_MAGENTA, COLOR_WHITE, COLOR_BLACK
  };
  CopyMem (Colors, TempColors, sizeof(Colors));

  // Draw colored rectangles
  BoxWidth = Width / 4;
  BoxHeight = Height / 3;

  for (i = 0; i < 8; i++) {
    UINTN X = (i % 4) * BoxWidth;
    UINTN Y = (i / 4) * BoxHeight + BoxHeight;  // Start from second row

    Status = DrawRectangle (Gop, X + 10, Y + 10,
                           BoxWidth - 20, BoxHeight - 20,
                           &Colors[i]);
    if (EFI_ERROR (Status)) {
      Print (L"Failed to draw rectangle %d: %r\n", i, Status);
    }
  }

  // Draw a gradient bar at the top
  for (UINTN x = 0; x < Width; x++) {
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pixel;
    Pixel.Blue = (UINT8)((x * 255) / Width);
    Pixel.Green = (UINT8)(((Width - x) * 255) / Width);
    Pixel.Red = 128;
    Pixel.Reserved = 0;

    Gop->Blt (Gop, &Pixel, EfiBltVideoFill,
              0, 0, x, 50, 1, 20, 0);
  }

  return EFI_SUCCESS;
}

/**
  Save and restore screen region demo.
**/
EFI_STATUS
DemoScreenCapture (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Buffer;
  UINTN                         Width = 200;
  UINTN                         Height = 100;
  UINTN                         BufferSize;

  Print (L"\nScreen capture demo...\n");

  // Allocate buffer for screen region
  BufferSize = Width * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  Buffer = AllocatePool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Capture a region of the screen
  Status = Gop->Blt (
                  Gop,
                  Buffer,
                  EfiBltVideoToBltBuffer,
                  100, 100,       // Source X, Y
                  0, 0,           // Buffer X, Y
                  Width, Height,
                  Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to capture: %r\n", Status);
    FreePool (Buffer);
    return Status;
  }

  // Draw the captured region at a different location
  Status = Gop->Blt (
                  Gop,
                  Buffer,
                  EfiBltBufferToVideo,
                  0, 0,           // Buffer X, Y
                  400, 200,       // Destination X, Y
                  Width, Height,
                  Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                  );

  FreePool (Buffer);

  if (EFI_ERROR (Status)) {
    Print (L"Failed to restore: %r\n", Status);
    return Status;
  }

  Print (L"Captured and copied screen region\n");
  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
GopExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop;
  EFI_INPUT_KEY                 Key;
  UINTN                         Index;

  Print (L"Graphics Output Protocol Example\n");
  Print (L"=================================\n");

  //
  // Locate GOP
  //
  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  (VOID **)&Gop
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to locate GOP: %r\n", Status);
    Print (L"(GOP may not be available in text-only console)\n");
    return Status;
  }

  Print (L"GOP located successfully\n");
  Print (L"Framebuffer: 0x%lx, Size: %d bytes\n",
         Gop->Mode->FrameBufferBase,
         Gop->Mode->FrameBufferSize);

  //
  // Show available modes
  //
  ShowVideoModes (Gop);

  Print (L"\nPress any key to start graphics demo...\n");
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

  //
  // Draw graphics
  //
  Status = DrawGraphicsDemo (Gop);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Demo screen capture
  //
  Status = DemoScreenCapture (Gop);

  //
  // Wait for keypress before exiting
  //
  Print (L"\nPress any key to exit...\n");
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

  return EFI_SUCCESS;
}
