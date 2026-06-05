# SVD Editor

A lightweight, browser-based editor for ARM CMSIS **System View Description (SVD)** files. No installation, no build step — open `SvdEditor.html` and start editing.

![screenshot placeholder](docs/screenshot.png)

---

## What is an SVD file?

SVD is an XML format standardised by ARM/CMSIS that describes a microcontroller's memory-mapped peripherals, registers, and bit fields. IDEs such as VS Code (via Cortex-Debug), Keil MDK, and Eclipse use SVD files to display live register values during a debug session. Chip vendors ship SVD files with their device packs; this tool lets you inspect and edit them without a full IDE.

---

## Features

- **Load** any `.svd` file directly in the browser — no data leaves your machine
- **Tree view** of the full peripheral → register hierarchy
- **Properties panel** for editing all metadata at every level:
  - **Peripheral** — description, base address, group name, address block (offset / size / usage), interrupt
  - **Register** — description, address offset, size, reset value, access mode
  - **Fields** — name, bit range, access mode, description; add or delete fields without leaving the panel
- **Export** back to a well-formatted, indented `.svd` XML file
- Handles both `<bitRange>` and the legacy `<bitOffset>` / `<bitWidth>` field formats
- Resolves `derivedFrom` peripheral references at load time
- Single self-contained HTML file — works offline after first load if dependencies are cached

---

## Usage

1. Clone or download the repository
2. Open `SvdEditor.html` in any modern browser (Chrome, Firefox, Edge, Safari)
3. Click **Choose File** and select your `.svd` file
4. Navigate the tree on the left; select a peripheral or register to edit its properties on the right
5. Click **Export SVD** to download the modified file

No web server, Node.js, or build tools required.

### Optional icon assets

The tree uses four icon images resolved relative to `SvdEditor.html`. If they are absent the tree falls back to jsTree's default icons. To use custom icons, place the following files alongside `SvdEditor.html`:

| File | Used for |
|---|---|
| `chip.png` | Root node |
| `peripheral.png` | Peripheral nodes |
| `register.png` | Register nodes |
| `error.png` | Excluded/error nodes |

---

## Dependencies

Loaded from CDN at runtime — no local installation needed.

| Library | Version | Licence |
|---|---|---|
| [jQuery](https://jquery.com) | 3.2.1 | MIT |
| [jsTree](https://www.jstree.com) | 3.3.3 | MIT |

---

## Browser support

Any modern browser with ES2020 support (optional chaining, `for...of`, `Object.entries`). Tested on Chrome 120+, Firefox 121+, Edge 120+.

---

## Known limitations

- `derivedFrom` peripherals are fully expanded on export (the attribute is not written back); the output is always a standalone SVD with no cross-references
- Interrupt metadata is displayed and editable but is not written back to the exported SVD yet
- No undo/redo history

---

## Contributing

Pull requests are welcome. Please open an issue first for anything beyond small bug fixes.

---

## Licence

MIT — see [LICENSE](LICENSE).
