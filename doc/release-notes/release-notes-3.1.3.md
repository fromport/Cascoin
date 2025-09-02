## Cascoin Core v3.1.3 – Maintenance Update

### Overview
- Maintenance release focusing on logging and GUI reliability
- No breaking changes; fully compatible with v3.1.2

### Highlights
- Automatic shrinking of `debug.log`
- GUI "Hide" button fix in the overlay/progress view

### Logging
- `debug.log` is now automatically trimmed to prevent unbounded growth, preserving recent entries and capping disk usage.
- Implemented in `src/util.cpp`; enabled by default with no configuration required.

### Bug Fixes
- GUI: The "Hide" button now reliably dismisses the modal/progress overlay and restores normal interaction.

### Upgrade notes
- No database or wallet format changes.
- Safe to upgrade or roll back between 3.1.2 and 3.1.3.
- Restart required after replacing binaries.

### Changelog
- [Feature] Add automatic shrinking of `debug.log` to cap disk usage.
- [Fix][GUI] Correct "Hide" button behavior in the overlay/progress view.
- [Maintenance] Minor stability and logging robustness improvements.

---

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).

