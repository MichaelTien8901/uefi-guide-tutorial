---
layout: default
title: Home
nav_order: 1
description: "UEFI Development Guide - Learn firmware development from beginner to professional"
permalink: /
---

# UEFI Development Guide

Welcome to the comprehensive UEFI firmware development guide. This tutorial takes you from beginner to professional, covering everything from environment setup to advanced SMM drivers and platform porting.

## What You'll Learn

- **Getting Started**: Set up your development environment and build your first UEFI application
- **Core Concepts**: Understand UEFI's driver model, protocols, handles, and memory services
- **Essential Services**: Master console I/O, graphics, file systems, networking, and variables
- **Advanced Topics**: PEI/DXE internals, SMM, Secure Boot, TPM, ACPI, capsule updates, RAS
- **Practical Projects**: Build real applications - shell commands, boot menus, network tools

## Quick Start

1. [Set up your environment]({% link part1/02-environment-setup.md %})
2. [Build your first application]({% link part1/03-hello-world.md %})
3. [Understand the architecture]({% link part1/01-introduction.md %})

## How This Guide is Organized

| Section | Description | Difficulty |
|---------|-------------|------------|
| [Part 1: Getting Started]({% link part1/index.md %}) | Environment setup, first build, QEMU testing | Beginner |
| [Part 2: Core Concepts]({% link part2/index.md %}) | Driver model, protocols, memory, services | Beginner-Intermediate |
| [Part 3: Essential Services]({% link part3/index.md %}) | Console, graphics, files, network, variables | Intermediate |
| [Part 4: Advanced Topics]({% link part4/index.md %}) | PEI, DXE, SMM, security, ACPI, RAS, ARM | Advanced |
| [Part 5: Practical Projects]({% link part5/index.md %}) | Shell commands, boot menu, network app | Advanced |
| [Appendices]({% link appendices/index.md %}) | Build system, debugging, references, glossary | Reference |

## Prerequisites

- Basic C programming knowledge
- Linux/Windows command line familiarity
- Embedded systems concepts (helpful but not required)

## Sample Code

All examples in this guide are tested and available in the [examples/](https://github.com/MichaelTien8901/uefi-guide-tutorial/tree/main/examples) directory. Each example includes:

- Complete, buildable code
- INF file for EDK2 integration
- Verification steps for QEMU

## Contributing

Found an error? Want to add content? See our [Contributing Guide](https://github.com/MichaelTien8901/uefi-guide-tutorial/blob/main/CONTRIBUTING.md).

---

{: .note }
This guide is community-maintained and not officially affiliated with the UEFI Forum. For official documentation, visit [uefi.org](https://uefi.org/).
