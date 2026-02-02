## @file
#  UEFI Guide Tutorial Package Description File
#
#  This package contains example code for the UEFI Development Guide tutorial.
#  Build with: build -p UefiGuidePkg/UefiGuidePkg.dsc
#
#  Copyright (c) 2024, UEFI Guide Contributors. All rights reserved.
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiGuidePkg
  PLATFORM_GUID                  = 3A5C8E2D-1B4F-4D9A-8C7E-6F5A4B3C2D1E
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x00010017
  OUTPUT_DIRECTORY               = Build/UefiGuidePkg
  SUPPORTED_ARCHITECTURES        = IA32|X64|AARCH64|RISCV64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]
  #
  # Entry Point Libraries
  #
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf

  #
  # Common Libraries
  #
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf

  #
  # File and Filesystem Libraries
  #
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf

  #
  # Shell Libraries (for shell applications)
  #
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

  #
  # Network Libraries
  #
  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf
  HttpLib|NetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf

[Components]
  #
  # Part 1: Getting Started
  #
  UefiGuidePkg/HelloWorld/HelloWorld.inf

  #
  # Part 2: Core Concepts (uncomment as examples are added)
  #
  # UefiGuidePkg/DriverExample/DriverExample.inf
  # UefiGuidePkg/ProtocolExample/ProtocolExample.inf
  # UefiGuidePkg/MemoryExample/MemoryExample.inf
  # UefiGuidePkg/ServicesExample/ServicesExample.inf

  #
  # Part 3: Essential Services
  #
  # UefiGuidePkg/ConsoleExample/ConsoleExample.inf
  # UefiGuidePkg/GopExample/GopExample.inf
  # UefiGuidePkg/FileSystemExample/FileSystemExample.inf
  # UefiGuidePkg/BlockIoExample/BlockIoExample.inf
  # UefiGuidePkg/NetworkExample/NetworkExample.inf
  # UefiGuidePkg/VariablesExample/VariablesExample.inf

  #
  # Part 5: Practical Projects
  #
  # UefiGuidePkg/ShellCommand/ShellCommand.inf
  # UefiGuidePkg/BootMenu/BootMenu.inf
  # UefiGuidePkg/NetworkApp/NetworkApp.inf
  # UefiGuidePkg/BootLoader/BootLoader.inf

[BuildOptions]
  #
  # Enable all warnings and treat warnings as errors
  #
  GCC:*_*_*_CC_FLAGS = -Wall -Werror
  MSFT:*_*_*_CC_FLAGS = /W4 /WX
