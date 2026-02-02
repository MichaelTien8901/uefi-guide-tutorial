---
layout: default
title: "B.7 Migration from EDK2"
permalink: /appendices/project-mu/migration/
nav_order: 7
parent: "Appendix B: Project Mu"
grand_parent: "Appendices"
---

# Section B.7: Migration from EDK2

This section provides a step-by-step guide for migrating existing EDK2 projects to Project Mu.

---

## Step 1: Understand the Differences

| EDK2 | Project Mu | Migration Action |
|:-----|:-----------|:-----------------|
| `source edksetup.sh` | `stuart_setup` + `stuart_update` | Update build scripts |
| `build` command | `stuart_build` | Update CI/CD pipelines |
| `Conf/target.txt` | `PlatformBuild.py` | Create Python config |
| Manual submodules | Automatic dependency management | Define in `GetDependencies()` |
| Single repo | Multi-repo | Split platform from core |

---

## Step 2: Create Platform Package

```bash
# Create new platform based on OEM sample
git clone https://github.com/microsoft/mu_oem_sample.git my-platform
cd my-platform

# Remove sample-specific code
rm -rf Platforms/SamplePkg

# Create your platform structure
mkdir -p Platforms/MyPlatformPkg/{Library,Drivers,Include}
```

---

## Step 3: Convert DSC File

### EDK2 DSC

```ini
[Defines]
  PLATFORM_NAME           = MyPlatform
  PLATFORM_GUID           = 12345678-1234-1234-1234-123456789ABC
  PLATFORM_VERSION        = 1.0
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/MyPlatform
  SUPPORTED_ARCHITECTURES = X64
  BUILD_TARGETS           = DEBUG|RELEASE

[LibraryClasses]
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
```

### Project Mu DSC

```ini
[Defines]
  PLATFORM_NAME           = MyPlatform
  PLATFORM_GUID           = 12345678-1234-1234-1234-123456789ABC
  PLATFORM_VERSION        = 1.0
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/MyPlatform
  SUPPORTED_ARCHITECTURES = X64
  BUILD_TARGETS           = DEBUG|RELEASE|NOOPT

# Include Mu-specific definitions
!include MU_BASECORE/MdePkg/MdeLibs.dsc.inc

[LibraryClasses]
  # Use Mu-enhanced debug library when available
  DebugLib|MsCorePkg/Library/DebugLib/DebugLib.inf

  # Or fall back to base library
  # DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

  # Mu-specific libraries
  MathLib|MsCorePkg/Library/MathLib/MathLib.inf
  JsonLiteParserLib|MsCorePkg/Library/JsonLiteParserLib/JsonLiteParserLib.inf

[LibraryClasses.common.DXE_DRIVER]
  # Mu graphics support (optional)
  UiRectangleLib|MsGraphicsPkg/Library/UiRectangleLib/UiRectangleLib.inf
```

---

## Step 4: Create PlatformBuild.py

See the complete example in [B.3 Stuart Build System]({% link appendices/project-mu/b-3-stuart-build.md %}).

---

## Step 5: Update Package Paths

```python
# In PlatformBuild.py
def GetPackagesPath(self):
    return [
        # Mu core packages
        str(self.WorkspaceRoot / "MU_BASECORE"),
        str(self.WorkspaceRoot / "Common" / "MU_TIANO"),
        str(self.WorkspaceRoot / "Common" / "MU"),

        # Your platform
        str(self.WorkspaceRoot / "Platforms"),

        # Silicon vendor code
        str(self.WorkspaceRoot / "Silicon" / "Intel"),
    ]
```

---

## Step 6: Migrate Build Pipeline

### Before (EDK2)

```bash
source edksetup.sh
build -p MyPlatform.dsc -a X64 -t GCC5 -b DEBUG
```

### After (Project Mu)

```bash
stuart_setup -c Platforms/MyPlatformPkg/PlatformBuild.py
stuart_update -c Platforms/MyPlatformPkg/PlatformBuild.py
stuart_build -c Platforms/MyPlatformPkg/PlatformBuild.py
```

---

## Migration Checklist

| Task | Status |
|:-----|:-------|
| Create `PlatformBuild.py` | ☐ |
| Update DSC to include Mu paths | ☐ |
| Add `.gitmodules` for Mu repos | ☐ |
| Create `pip-requirements.txt` | ☐ |
| Update CI/CD scripts | ☐ |
| Test build with Stuart | ☐ |
| Validate output firmware | ☐ |

---

## Common Migration Issues

### Issue: Library not found

**Problem:** Build fails with "cannot find library instance"

**Solution:** Update package paths in `GetPackagesPath()` or add library mapping to DSC.

### Issue: INF path changes

**Problem:** INF file paths reference old locations

**Solution:** Update paths to use Mu repository structure:
- `MdePkg/` → `MU_BASECORE/MdePkg/`
- `MsCorePkg/` → `Common/MU/MsCorePkg/`

### Issue: Build variables not recognized

**Problem:** Build fails with undefined variables

**Solution:** Set variables in `SetPlatformEnv()` method of `PlatformBuild.py`.

---

## Next Section

[B.8 Troubleshooting & References]({% link appendices/project-mu/b-8-troubleshooting.md %})
