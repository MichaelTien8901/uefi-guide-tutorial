---
layout: default
title: "B.5 CI/CD Integration"
permalink: /appendices/project-mu/cicd/
nav_order: 5
parent: "Appendix B: Project Mu"
grand_parent: "Appendices"
---

# Section B.5: CI/CD Integration

Project Mu is designed with CI/CD in mind. This section provides templates for Azure Pipelines and GitHub Actions.

---

## Azure Pipelines

```yaml
# azure-pipelines.yml

trigger:
  - main
  - release/*

pool:
  vmImage: 'ubuntu-latest'

variables:
  - name: STUART_PLATFORM
    value: Platforms/MyPlatformPkg/PlatformBuild.py

stages:
  - stage: Build
    jobs:
      - job: BuildFirmware
        strategy:
          matrix:
            DEBUG_GCC:
              TARGET: DEBUG
              TOOLCHAIN: GCC5
            RELEASE_GCC:
              TARGET: RELEASE
              TOOLCHAIN: GCC5

        steps:
          - task: UsePythonVersion@0
            inputs:
              versionSpec: '3.10'
            displayName: 'Set Python Version'

          - script: |
              pip install -r pip-requirements.txt
            displayName: 'Install Python Dependencies'

          - script: |
              stuart_setup -c $(STUART_PLATFORM)
            displayName: 'Stuart Setup'

          - script: |
              stuart_update -c $(STUART_PLATFORM)
            displayName: 'Stuart Update'

          - script: |
              stuart_build -c $(STUART_PLATFORM) \
                TARGET=$(TARGET) \
                TOOL_CHAIN_TAG=$(TOOLCHAIN)
            displayName: 'Stuart Build'

          - task: PublishBuildArtifacts@1
            inputs:
              pathtoPublish: 'Build'
              artifactName: 'firmware-$(TARGET)-$(TOOLCHAIN)'
            displayName: 'Publish Artifacts'

  - stage: Test
    dependsOn: Build
    jobs:
      - job: UnitTests
        steps:
          - script: |
              pip install -r pip-requirements.txt
              stuart_setup -c $(STUART_PLATFORM)
              stuart_update -c $(STUART_PLATFORM)
              stuart_ci_build -c $(STUART_PLATFORM)
            displayName: 'Run Unit Tests'

          - task: PublishTestResults@2
            inputs:
              testResultsFormat: 'JUnit'
              testResultsFiles: '**/test-results.xml'
            displayName: 'Publish Test Results'
```

---

## GitHub Actions

```yaml
# .github/workflows/build.yml

name: Build Firmware

on:
  push:
    branches: [ main, release/* ]
  pull_request:
    branches: [ main ]

env:
  STUART_PLATFORM: Platforms/MyPlatformPkg/PlatformBuild.py

jobs:
  build:
    strategy:
      matrix:
        target: [DEBUG, RELEASE]
        os: [ubuntu-latest, windows-latest]
        include:
          - os: ubuntu-latest
            toolchain: GCC5
          - os: windows-latest
            toolchain: VS2019

    runs-on: ${{ matrix.os }}

    steps:
      - name: Checkout
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Set up Python
        uses: actions/setup-python@v5
        with:
          python-version: '3.10'

      - name: Install Dependencies
        run: pip install -r pip-requirements.txt

      - name: Stuart Setup
        run: stuart_setup -c ${{ env.STUART_PLATFORM }}

      - name: Stuart Update
        run: stuart_update -c ${{ env.STUART_PLATFORM }}

      - name: Stuart Build
        run: |
          stuart_build -c ${{ env.STUART_PLATFORM }} \
            TARGET=${{ matrix.target }} \
            TOOL_CHAIN_TAG=${{ matrix.toolchain }}

      - name: Upload Artifacts
        uses: actions/upload-artifact@v4
        with:
          name: firmware-${{ matrix.target }}-${{ matrix.toolchain }}
          path: Build/**/*.fd

  test:
    needs: build
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - uses: actions/setup-python@v5
        with:
          python-version: '3.10'

      - name: Run CI Build with Tests
        run: |
          pip install -r pip-requirements.txt
          stuart_setup -c ${{ env.STUART_PLATFORM }}
          stuart_update -c ${{ env.STUART_PLATFORM }}
          stuart_ci_build -c ${{ env.STUART_PLATFORM }}

      - name: Publish Test Results
        uses: EnricoMi/publish-unit-test-result-action@v2
        if: always()
        with:
          files: '**/test-results.xml'
```

---

## CI/CD Best Practices

### 1. Use Matrix Builds

Build for multiple configurations in parallel:

```yaml
strategy:
  matrix:
    target: [DEBUG, RELEASE]
    arch: [IA32, X64]
    toolchain: [GCC5, VS2019]
```

### 2. Cache Dependencies

Speed up builds by caching Python packages:

```yaml
- name: Cache pip packages
  uses: actions/cache@v3
  with:
    path: ~/.cache/pip
    key: ${{ runner.os }}-pip-${{ hashFiles('pip-requirements.txt') }}
```

### 3. Artifact Retention

Configure artifact retention policies:

```yaml
- name: Upload Artifacts
  uses: actions/upload-artifact@v4
  with:
    name: firmware
    path: Build/**/*.fd
    retention-days: 30
```

### 4. Branch Protection

Configure required status checks:

- `stuart_ci_build` must pass
- Unit tests must pass
- Code review required

---

## pip-requirements.txt

```
edk2-pytool-library>=0.20.0
edk2-pytool-extensions>=0.27.0
edk2-basetools>=0.1.0
```

---

## Next Section

[B.6 Running on QEMU]({% link appendices/project-mu/b-6-qemu.md %})
