---
layout: default
title: "B.2 Installation & Setup"
permalink: /appendices/project-mu/installation-setup/
nav_order: 2
parent: "Appendix B: Project Mu"
grand_parent: "Appendices"
---

# Section B.2: Installation & Setup

Configure your development environment for Project Mu firmware development.

---

## Prerequisites

### All Platforms

| Component | Minimum Version | Purpose |
|:----------|:----------------|:--------|
| Python | 3.8+ | Build system, Stuart |
| Git | 2.25+ | Version control, submodules |
| pip | 21.0+ | Python package manager |

---

## Linux (Ubuntu/Debian)

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install build essentials
sudo apt install -y \
    build-essential \
    git \
    python3 \
    python3-pip \
    python3-venv \
    uuid-dev \
    nasm \
    iasl \
    gcc-12 \
    g++-12 \
    make \
    qemu-system-x86

# Verify installations
gcc --version
python3 --version
git --version
```

---

## Windows

```powershell
# Install via winget
winget install Python.Python.3.11
winget install Git.Git

# Visual Studio 2019/2022 with:
# - Desktop development with C++
# - Windows 10/11 SDK

# NASM from https://www.nasm.us/
# Add to PATH: C:\Program Files\NASM
```

---

## macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install python@3.11 nasm acpica qemu
```

---

## Installing Stuart Build System

Stuart is Project Mu's Python-based build orchestration system.

```bash
# Create and activate virtual environment (recommended)
python3 -m venv mu_venv
source mu_venv/bin/activate    # Linux/macOS
# mu_venv\Scripts\activate.bat  # Windows

# Install Stuart and related tools
pip install --upgrade pip
pip install edk2-pytool-library edk2-pytool-extensions

# Verify installation
stuart_setup --help
stuart_update --help
stuart_build --help
```

---

## Setting Up Your First Mu Platform

### Option 1: Clone mu_tiano_platforms (Recommended for Learning)

```bash
# Clone the official virtual platform repository
git clone https://github.com/microsoft/mu_tiano_platforms.git
cd mu_tiano_platforms

# Checkout a stable release
git checkout release/202402

# Setup dependencies (downloads submodules)
stuart_setup -c Platforms/QemuQ35Pkg/PlatformBuild.py

# Update (syncs all dependencies)
stuart_update -c Platforms/QemuQ35Pkg/PlatformBuild.py

# Build
stuart_build -c Platforms/QemuQ35Pkg/PlatformBuild.py TOOL_CHAIN_TAG=GCC5
```

### Option 2: Clone mu_oem_sample (For Creating Your Own Platform)

```bash
# Clone OEM sample
git clone https://github.com/microsoft/mu_oem_sample.git my-platform
cd my-platform

# Setup and update
stuart_setup -c Platforms/SamplePkg/PlatformBuild.py
stuart_update -c Platforms/SamplePkg/PlatformBuild.py

# Build
stuart_build -c Platforms/SamplePkg/PlatformBuild.py
```

### Option 3: Create a New Platform from Scratch

```bash
# Create workspace
mkdir my_mu_platform && cd my_mu_platform

# Initialize git
git init

# Add Mu repositories as submodules
git submodule add https://github.com/Microsoft/mu_basecore.git MU_BASECORE
git submodule add https://github.com/Microsoft/mu_plus.git Common/MU
git submodule add https://github.com/Microsoft/mu_tiano_plus.git Common/TIANO

# Create platform directory structure
mkdir -p Platforms/MyPlatformPkg/{Library,Drivers,Include}

# Create pip-requirements.txt
cat > pip-requirements.txt << 'EOF'
edk2-pytool-library>=0.20.0
edk2-pytool-extensions>=0.27.0
edk2-basetools>=0.1.0
EOF

# Install dependencies
pip install -r pip-requirements.txt
```

---

## Environment Verification

Run these commands to verify your setup:

```bash
# 1. Check Python and tools
python3 --version
stuart_setup --help

# 2. Check compilers
gcc --version       # Linux/macOS
# or cl.exe on Windows

nasm -v
iasl -v

# 3. Clone and build test platform
git clone https://github.com/microsoft/mu_tiano_platforms.git
cd mu_tiano_platforms
stuart_setup -c Platforms/QemuQ35Pkg/PlatformBuild.py
stuart_update -c Platforms/QemuQ35Pkg/PlatformBuild.py
stuart_build -c Platforms/QemuQ35Pkg/PlatformBuild.py TOOL_CHAIN_TAG=GCC5

# 4. Verify output exists
ls Build/QemuQ35Pkg/DEBUG_GCC5/FV/*.fd
```

---

## Next Section

[B.3 Stuart Build System]({% link appendices/project-mu/b-3-stuart-build.md %})
