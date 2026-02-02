# Project Instructions for Claude

## Internal Links Pattern

This Jekyll site uses `{% link %}` tags for all internal links. This ensures:
- Links are validated at build time (Jekyll errors if file doesn't exist)
- URLs automatically use the page's permalink if one is set
- Links work correctly on the live GitHub Pages site

### Syntax

```markdown
[Link Text]({% link path/to/file.md %})
```

### Path Rules

1. **Paths are relative to Jekyll root** (the `docs/` folder)
   - Correct: `{% link part1/01-introduction.md %}`
   - Wrong: `{% link docs/part1/01-introduction.md %}`

2. **Use the actual file path**, not the permalink
   - Correct: `{% link part4/23-arm-uefi/index.md %}`
   - Wrong: `{% link part4/arm-uefi/index.md %}`

3. **Jekyll resolves to permalink** if one is set
   - File: `part4/23-arm-uefi/index.md` with `permalink: /part4/arm-uefi/`
   - Generated URL: `/part4/arm-uefi/`

### Examples

```markdown
# Same directory
[Next Chapter]({% link part1/02-environment-setup.md %})

# Different directory
[Part 3]({% link part3/index.md %})

# Subdirectory with custom permalink
[ARM UEFI]({% link part4/23-arm-uefi/index.md %})
# Resolves to /part4/arm-uefi/ (the permalink)
```

### External Links

For GitHub example code links, use full URLs:
```markdown
[Source Code](https://github.com/MichaelTien8901/uefi-guide-tutorial/tree/main/examples/UefiGuidePkg/ExampleName)
```

## File Structure

```
docs/                    # Jekyll root
├── _config.yml
├── index.md
├── part1/
│   ├── index.md        # permalink: /part1/
│   ├── 01-introduction.md
│   └── ...
├── part4/
│   ├── index.md        # permalink: /part4/
│   └── 23-arm-uefi/    # Folder name has prefix
│       └── index.md    # permalink: /part4/arm-uefi/ (clean URL)
└── examples/
    └── UefiGuidePkg/   # UEFI example code
```

## Common Mistakes to Avoid

1. **Don't use `.md` URLs directly** - They won't work on the live site
2. **Don't use `docs/` prefix** - The docs folder IS the Jekyll root
3. **Don't use relative paths like `../`** - Use full path from Jekyll root
4. **Don't guess permalinks** - Use the file path; Jekyll resolves to permalink
