## Cascoin Core v3.1.1 – Language Hotfix

### Overview
- **Hotfix release**: Fixes language detection issue on English systems
- **No breaking changes**: Maintains full compatibility with v3.1.0

### Bug Fixes

#### Language Detection
- **Fixed**: Application now defaults to English on English systems instead of using system locale
- **Issue**: Previously, the application would display in German on German Windows systems even when English was preferred
- **Solution**: Modified language detection to default to English ("en") unless explicitly overridden by user settings or command-line arguments

### Technical Details
- Modified `GetLangTerritory()` function in `src/qt/bitcoin.cpp`
- Changed default language from `QLocale::system().name()` to `"en"`
- Preserves all existing language override mechanisms:
  - User-configured language settings in GUI (Settings → Display → User Interface language)
  - Command-line `-lang` parameter
  - Saved language preferences

### User Impact
- **English systems**: Application now starts in English by default
- **Existing users**: No change if language was previously configured
- **Multilingual users**: Can still select preferred language through settings
- **Developers**: Can still use `-lang=de_DE` or similar command-line options

### Files Changed
- `src/qt/bitcoin.cpp` - Language detection logic updated

---

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).
