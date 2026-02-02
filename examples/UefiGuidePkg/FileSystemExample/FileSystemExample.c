/** @file
  File System Example - Demonstrates UEFI file system access.

  This example shows how to:
  1. Locate Simple File System protocol
  2. Open volumes and directories
  3. Read and write files
  4. List directory contents
  5. Get file information

  Copyright (c) 2024, UEFI Guide Tutorial. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>

/**
  List directory contents.
**/
EFI_STATUS
ListDirectory (
  IN EFI_FILE_PROTOCOL  *Dir,
  IN CHAR16             *DirName
  )
{
  EFI_STATUS     Status;
  EFI_FILE_INFO  *FileInfo;
  UINTN          BufferSize;
  UINTN          FileCount = 0;
  UINTN          DirCount = 0;

  Print (L"\nDirectory listing for %s:\n", DirName);
  Print (L"----------------------------------------\n");
  Print (L"%-30s %10s  %s\n", L"Name", L"Size", L"Type");
  Print (L"----------------------------------------\n");

  // Allocate buffer for file info
  BufferSize = sizeof(EFI_FILE_INFO) + 256 * sizeof(CHAR16);
  FileInfo = AllocatePool (BufferSize);
  if (FileInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Reset directory position
  Dir->SetPosition (Dir, 0);

  // Read directory entries
  while (TRUE) {
    UINTN Size = BufferSize;
    Status = Dir->Read (Dir, &Size, FileInfo);

    if (EFI_ERROR (Status) || Size == 0) {
      break;  // No more entries
    }

    CHAR16 *Type;
    if (FileInfo->Attribute & EFI_FILE_DIRECTORY) {
      Type = L"<DIR>";
      DirCount++;
    } else {
      Type = L"";
      FileCount++;
    }

    Print (L"%-30s %10ld  %s\n",
           FileInfo->FileName,
           FileInfo->FileSize,
           Type);
  }

  Print (L"----------------------------------------\n");
  Print (L"%d file(s), %d dir(s)\n", FileCount, DirCount);

  FreePool (FileInfo);
  return EFI_SUCCESS;
}

/**
  Read and display file contents.
**/
EFI_STATUS
ReadFile (
  IN EFI_FILE_PROTOCOL  *Root,
  IN CHAR16             *FileName
  )
{
  EFI_STATUS        Status;
  EFI_FILE_PROTOCOL *File;
  CHAR8             Buffer[512];
  UINTN             BufferSize;
  EFI_FILE_INFO     *FileInfo;
  UINTN             InfoSize;

  Print (L"\nReading file: %s\n", FileName);

  // Open file
  Status = Root->Open (
                   Root,
                   &File,
                   FileName,
                   EFI_FILE_MODE_READ,
                   0
                   );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to open file: %r\n", Status);
    return Status;
  }

  // Get file info
  InfoSize = sizeof(EFI_FILE_INFO) + 256 * sizeof(CHAR16);
  FileInfo = AllocatePool (InfoSize);
  if (FileInfo != NULL) {
    Status = File->GetInfo (File, &gEfiFileInfoGuid, &InfoSize, FileInfo);
    if (!EFI_ERROR (Status)) {
      Print (L"File size: %ld bytes\n", FileInfo->FileSize);
      Print (L"Created: %04d-%02d-%02d %02d:%02d:%02d\n",
             FileInfo->CreateTime.Year,
             FileInfo->CreateTime.Month,
             FileInfo->CreateTime.Day,
             FileInfo->CreateTime.Hour,
             FileInfo->CreateTime.Minute,
             FileInfo->CreateTime.Second);
    }
    FreePool (FileInfo);
  }

  // Read file contents (first 500 bytes)
  Print (L"\nContents (first 500 bytes):\n");
  Print (L"----------------------------------------\n");

  BufferSize = sizeof(Buffer) - 1;
  Status = File->Read (File, &BufferSize, Buffer);

  if (!EFI_ERROR (Status) && BufferSize > 0) {
    Buffer[BufferSize] = '\0';
    // Print as ASCII
    for (UINTN i = 0; i < BufferSize; i++) {
      if (Buffer[i] == '\n') {
        Print (L"\n");
      } else if (Buffer[i] >= 0x20 && Buffer[i] < 0x7F) {
        Print (L"%c", (CHAR16)Buffer[i]);
      } else if (Buffer[i] == '\t') {
        Print (L"  ");
      }
    }
    Print (L"\n");
  }

  Print (L"----------------------------------------\n");

  File->Close (File);
  return EFI_SUCCESS;
}

/**
  Write a test file.
**/
EFI_STATUS
WriteFile (
  IN EFI_FILE_PROTOCOL  *Root,
  IN CHAR16             *FileName
  )
{
  EFI_STATUS        Status;
  EFI_FILE_PROTOCOL *File;
  CHAR8             *Content = "Hello from UEFI!\r\n"
                               "This file was created by FileSystemExample.\r\n"
                               "UEFI file system access is working.\r\n";
  UINTN             ContentSize;

  Print (L"\nWriting file: %s\n", FileName);

  // Create/overwrite file
  Status = Root->Open (
                   Root,
                   &File,
                   FileName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   0
                   );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to create file: %r\n", Status);
    return Status;
  }

  // Write content
  ContentSize = AsciiStrLen (Content);
  Status = File->Write (File, &ContentSize, Content);

  if (EFI_ERROR (Status)) {
    Print (L"Failed to write: %r\n", Status);
  } else {
    Print (L"Wrote %d bytes\n", ContentSize);
  }

  // Flush and close
  File->Flush (File);
  File->Close (File);

  return Status;
}

/**
  Create a directory.
**/
EFI_STATUS
CreateDirectory (
  IN EFI_FILE_PROTOCOL  *Root,
  IN CHAR16             *DirName
  )
{
  EFI_STATUS        Status;
  EFI_FILE_PROTOCOL *Dir;

  Print (L"\nCreating directory: %s\n", DirName);

  Status = Root->Open (
                   Root,
                   &Dir,
                   DirName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_DIRECTORY
                   );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to create directory: %r\n", Status);
    return Status;
  }

  Print (L"Directory created\n");
  Dir->Close (Dir);

  return EFI_SUCCESS;
}

/**
  Application entry point.
**/
EFI_STATUS
EFIAPI
FileSystemExampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;
  EFI_FILE_PROTOCOL                *Root;

  Print (L"File System Example\n");
  Print (L"===================\n");

  //
  // Get loaded image protocol to find the volume we booted from
  //
  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to get loaded image: %r\n", Status);
    return Status;
  }

  //
  // Get file system from the device we booted from
  //
  Status = gBS->HandleProtocol (
                  LoadedImage->DeviceHandle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **)&FileSystem
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Failed to get file system: %r\n", Status);
    Print (L"(File system may not be available on boot device)\n");
    return Status;
  }

  Print (L"File system located on boot device\n");

  //
  // Open the root directory
  //
  Status = FileSystem->OpenVolume (FileSystem, &Root);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to open volume: %r\n", Status);
    return Status;
  }

  Print (L"Root directory opened\n");

  //
  // List root directory
  //
  Status = ListDirectory (Root, L"\\");

  //
  // Create a test directory
  //
  Status = CreateDirectory (Root, L"\\UefiTest");

  //
  // Write a test file
  //
  Status = WriteFile (Root, L"\\UefiTest\\test.txt");

  //
  // Read it back
  //
  if (!EFI_ERROR (Status)) {
    Status = ReadFile (Root, L"\\UefiTest\\test.txt");
  }

  //
  // List the test directory
  //
  EFI_FILE_PROTOCOL *TestDir;
  Status = Root->Open (Root, &TestDir, L"\\UefiTest", EFI_FILE_MODE_READ, 0);
  if (!EFI_ERROR (Status)) {
    ListDirectory (TestDir, L"\\UefiTest");
    TestDir->Close (TestDir);
  }

  Root->Close (Root);

  Print (L"\nFile system example completed!\n");
  return EFI_SUCCESS;
}
