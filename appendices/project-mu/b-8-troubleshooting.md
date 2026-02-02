---
layout: default
title: "B.8 Troubleshooting & References"
permalink: /appendices/project-mu/troubleshooting/
nav_order: 8
parent: "Appendix B: Project Mu"
grand_parent: "Appendices"
---

# Section B.8: Troubleshooting & References

Common issues, debugging tips, code examples, and reference links.

---

## Common Issues

| Issue | Cause | Solution |
|:------|:------|:---------|
| `stuart_setup not found` | Stuart not installed | `pip install edk2-pytool-extensions` |
| Submodule download fails | Network/URL issues | Check network, verify repository URLs |
| Build fails: missing package | Package path not configured | Add to `GetPackagesPath()` |
| `TOOL_CHAIN_TAG not set` | Environment not configured | Set in `SetPlatformEnv()` or command line |
| CI timeout | Build too slow | Increase timeout, enable parallel builds |
| Missing library class | DSC not configured | Add library mapping to DSC |
| Python version error | Wrong Python version | Use Python 3.8+ |
| Git submodule error | Submodule not initialized | Run `stuart_update` |

---

## Debug Build Issues

```bash
# Verbose output
stuart_build -c PlatformBuild.py --verbose

# Check environment variables
stuart_build -c PlatformBuild.py --dump-env

# Clean build
stuart_build -c PlatformBuild.py --clean

# Skip certain steps
stuart_build -c PlatformBuild.py --skip-pre-build
```

---

## Checking Dependencies

```bash
# List all dependencies
stuart_setup -c PlatformBuild.py --list

# Force re-download
stuart_setup -c PlatformBuild.py --force

# Check package paths
python -c "
from Platforms.MyPlatformPkg.PlatformBuild import PlatformBuilder
pb = PlatformBuilder()
for p in pb.GetPackagesPath():
    print(p)
"
```

---

## Examples

### Complete Platform Example

See the [mu_tiano_platforms](https://github.com/microsoft/mu_tiano_platforms) repository for complete working examples:

- **QemuQ35Pkg** - x86_64 Q35 platform
- **QemuSbsaPkg** - ARM64 SBSA platform

### Minimal UEFI Application

```c
// SimpleApp.c

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
SimpleAppEntry (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    Print(L"Hello from Project Mu!\n");
    Print(L"Press any key to exit...\n");

    // Wait for key press
    EFI_INPUT_KEY  Key;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

    return EFI_SUCCESS;
}
```

```ini
# SimpleApp.inf

[Defines]
  INF_VERSION    = 0x00010005
  BASE_NAME      = SimpleApp
  FILE_GUID      = ABCD1234-1234-1234-1234-123456789ABC
  MODULE_TYPE    = UEFI_APPLICATION
  VERSION_STRING = 1.0
  ENTRY_POINT    = SimpleAppEntry

[Sources]
  SimpleApp.c

[Packages]
  MdePkg/MdePkg.dec

[LibraryClasses]
  UefiApplicationEntryPoint
  UefiLib
  UefiBootServicesTableLib
```

---

## References

### Official Resources

- [Project Mu Documentation](https://microsoft.github.io/mu/) - Official documentation
- [Project Mu GitHub](https://github.com/microsoft?q=mu_&type=all) - All Mu repositories
- [Stuart Documentation](https://www.tianocore.org/edk2-pytool-extensions/) - Build system guide

### Repositories

| Repository | Description |
|:-----------|:------------|
| [mu_basecore](https://github.com/microsoft/mu_basecore) | Core UEFI from EDK2 |
| [mu_plus](https://github.com/microsoft/mu_plus) | Microsoft extensions |
| [mu_tiano_plus](https://github.com/microsoft/mu_tiano_plus) | Enhanced TianoCore |
| [mu_tiano_platforms](https://github.com/microsoft/mu_tiano_platforms) | Reference platforms |
| [mu_feature_dfci](https://github.com/microsoft/mu_feature_dfci) | DFCI feature |
| [mu_oem_sample](https://github.com/microsoft/mu_oem_sample) | OEM sample code |
| [mu_devops](https://github.com/microsoft/mu_devops) | CI/CD and Rust containers |

### Related Documentation

- [DFCI Feature Documentation](https://microsoft.github.io/mu/dyn/mu_feature_dfci/DfciPkg/Docs/Dfci_Feature/)
- [Rust in Project Mu](https://microsoft.github.io/mu/WhatAndWhy/rust/)
- [edk2-pytool-extensions](https://github.com/tianocore/edk2-pytool-extensions) - Stuart source

### Blog Posts and Articles

- [Introducing Project Mu - Windows Developer Blog](https://blogs.windows.com/windowsdeveloper/2018/12/19/%E2%80%AFintroducing-project-mu/)
- [Rust Support for UEFI - Microsoft Tech Community](https://techcommunity.microsoft.com/blog/surfaceitpro/rust-support-for-uefi-development-through-project-mu/3970582)

---

{: .note }
> **Version Compatibility**: This guide is based on Project Mu release/202402. Commands and configurations may vary for different releases.
