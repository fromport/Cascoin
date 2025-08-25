## Cascoin Core v3.0.0 – Mice Update

### Overview
- Major modernization of the build/runtime stack (Qt6, Boost, Berkeley DB, C++17) for reliable builds on modern Linux distributions.
- Terminology update: Bees/Honey → Mice/Cheese across UI texts and Hive logic.
- Refreshed build documentation with direct links for Linux/Windows.

### Highlights
- Migrated to Qt6 (GUI and tools: `moc`, `uic`, `rcc`, `lrelease`).
- Updated system dependencies: Boost, Berkeley DB, OpenSSL, libevent, ZMQ, Protobuf, QRencode.
- “Mice Update”: complete switch from Bee/Honey to Mice/Cheese in UI and strings.
- Linux build cleanup: link against system Qt6 and remove accidental Windows/Mingw artifacts from the Linux linker path.

### Breaking/Behavior Changes
- C++17 is now required (e.g., for Qt6 compatibility).
- Wallet/DB: When building with system Berkeley DB and `--with-incompatible-bdb`, wallets are not portable to older official releases. Use Berkeley DB 4.8 for portability.
- Terminology change (Bees/Honey → Mice/Cheese) can affect scripts/automations that parse texts/labels.

### Area Details
- **Core/Consensus**: No consensus changes.
- **Wallet**: Fully functional; see Berkeley DB note above.
- **GUI/UX**: Qt6 migration; updated splash/overview/dialogs; consistent Mice/Cheese terminology.
- **Hive/Mice**: UI and logic switched to Mice/Cheese.
- **Build/Toolchain**:
  - Linux: System Qt6 tools are used; Windows‑specific Qt libs are not linked on Linux.
  - Dependency and compiler flags aligned with modern GCC/glibc.
- **Docs**: New Linux build guide (`doc/build-linux.md`); `README.md` links Linux/Windows build docs.

### Upgrade Notes
- Back up your wallet (`wallet.dat`) before upgrading.
- For source builds:
  - Install Qt6 dev tools; pass `moc/uic/rcc/lrelease` explicitly to `./configure` if needed.
  - Use Berkeley DB 4.8 for portable wallets (or `--with-incompatible-bdb` if portability is not required).

### Build Links
- Linux: `doc/build-linux.md`
- Windows: `doc/build-windows.md`

### Known Issues
- Translations for the new terminology are being expanded.
- Using `--with-incompatible-bdb` limits wallet portability across builds/releases.


