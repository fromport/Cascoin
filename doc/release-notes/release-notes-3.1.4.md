## Cascoin Core v3.1.4 – GUI Menu and Translation Hotfix

### Overview
- **Hotfix release**: Addresses non-functional menu items in the Qt GUI and translation issues
- **Critical bug fix**: Resolves unclickable dropdown menu items in the header
- **Translation improvements**: Various localization fixes by fromport
- **No breaking changes**: Maintains full compatibility with v3.1.3

### Highlights
- **Fixed Menu Items**: Restored functionality to About, Options, and Debug window menu items
- **Improved User Experience**: All header dropdown menu items are now fully clickable and functional
- **Translation Fixes**: Various translation issues corrected for better localization
- **Quick Fix**: Minimal change hotfix focusing on GUI accessibility and translations

### Bug Fixes

#### GUI Menu Functionality
- **Fixed**: "About Cascoin" menu item was disabled and unclickable
- **Fixed**: "Options..." settings menu item was disabled and unclickable  
- **Fixed**: "Debug window" menu item was disabled and unclickable
- **Root Cause**: Menu actions were disabled during initialization but never re-enabled when client model loaded
- **Solution**: Added proper enablement of menu actions in `setClientModel()` method

#### Translation Fixes
- **Fixed**: Various translation issues corrected by fromport
- **Improved**: Better localization support and text consistency

### Technical Details
- **File Modified**: `src/qt/bitcoingui.cpp`
- **Change**: Added `setEnabled(true)` calls for `aboutAction`, `optionsAction`, and `openRPCConsoleAction` 
- **Location**: Menu actions are now enabled when the client model is successfully loaded
- **Impact**: All menu items in File, Settings, and Help menus are now fully functional

### User Impact
- **Immediate Fix**: Users can now access settings, about information, and debug console through the menu
- **Better Translations**: Improved text consistency and localization for international users
- **No Data Loss**: This fix does not affect wallet data, blockchain data, or configuration
- **Enhanced Usability**: Complete restoration of GUI menu functionality and better language support

### Upgrade Notes
- **Safe Upgrade**: No database, wallet, or configuration changes
- **Backward Compatible**: Can safely roll back to v3.1.3 if needed
- **Restart Required**: Simple restart after replacing binaries
- **No Migration**: No data migration or re-indexing required

### System Requirements
- **Unchanged**: Same system requirements as v3.1.3
- **Qt6 Compatible**: Continues to support modern Qt6 framework
- **Cross Platform**: Available for Windows, Linux, and macOS

### Changelog
- [Fix][GUI] Enable disabled menu actions (About, Options, Debug window) when client model loads
- [Fix][GUI] Restore full functionality to header dropdown menu items
- [Fix][Translation] Various translation issues corrected by fromport
- [Maintenance] Minor code cleanup in menu initialization sequence

---

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).

### Installation Notes
- **Simple Update**: Replace existing binaries with v3.1.4
- **No Backup Required**: This hotfix does not modify data files
- **Immediate Effect**: Menu functionality restored immediately after restart
