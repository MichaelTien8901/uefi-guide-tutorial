---
layout: default
title: "Part 1: Getting Started"
nav_order: 2
has_children: true
permalink: /part1/
---

# Part 1: Getting Started
{: .fs-9 }

Your journey into UEFI development begins here.
{: .fs-6 .fw-300 }

---

## What You'll Learn

In this section, you'll build a solid foundation for UEFI development:

| Chapter | Topics |
|:--------|:-------|
| **[1. Introduction]({% link part1/01-introduction.md %})** | UEFI architecture, boot stages, services overview |
| **[2. Environment Setup]({% link part1/02-environment-setup.md %})** | EDK2, toolchains, QEMU/OVMF configuration |
| **[3. Hello World]({% link part1/03-hello-world.md %})** | Your first UEFI application |

## Prerequisites

Before starting, ensure you have:

- **Basic C programming knowledge** - pointers, structures, functions
- **Command-line familiarity** - navigating directories, running commands
- **A development machine** - Windows 10/11, Ubuntu 22.04+, or macOS 13+

## Learning Path

```mermaid
flowchart TD
    A[Start Here] --> B[1. Understand UEFI Architecture]
    B --> C[2. Set Up Development Environment]
    C --> D[3. Build Hello World]
    D --> E[Continue to Part 2: Core Concepts]

    style A fill:#3498db,color:#fff
    style E fill:#2ecc71,color:#fff
```

## Time Investment

| Chapter | Estimated Time |
|:--------|:---------------|
| Introduction | 30-45 minutes reading |
| Environment Setup | 1-2 hours (varies by platform) |
| Hello World | 30 minutes |

---

{: .note }
> **Skill Level:** Beginner
> **Prerequisites:** Basic C programming, command-line familiarity
