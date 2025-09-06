## Cascoin Core v3.1.5 – Enhanced Labyrinth Experience and Localization

### Overview
- **Feature enhancement release**: Significantly improves The Labyrinth user experience
- **Auto-refresh functionality**: The Labyrinth view now updates automatically when blocks are found and mice are created
- **Language system overhaul**: Fixes mixed German/English interface and properly detects system language
- **UI improvements**: Enhanced readability and user interface polish
- **Performance enhancements**: Optimized update mechanisms for better responsiveness

### Highlights
- **Automated Labyrinth Updates**: No more manual refreshing - the labyrinth view updates automatically on new blocks and transactions
- **System Language Detection**: Application now starts in your system's language instead of defaulting to English
- **Fixed Disappearing Mice**: Resolved critical issue where mice would vanish during wallet operations
- **Real BCT Data**: BCT NFT system now uses actual wallet data instead of dummy test information
- **Unified Data Storage**: All Cascoin data now consistently stored in standard directory (~/.cascoin/)
- **Fixed UI Elements**: Improved visibility and usability of the "Create Mice" interface
- **Periodic Refresh**: Background refresh ensures labyrinth status stays current even during network issues
- **Enhanced User Experience**: Smoother, more responsive interface with fewer manual interactions required

### New Features

#### Automatic Labyrinth Refresh System
- **Real-time Updates**: The Labyrinth view automatically refreshes when new blocks are found
- **Transaction-triggered Updates**: Mouse status updates immediately when wallet transactions occur
- **Periodic Background Refresh**: 60-second background updates ensure data stays current
- **Smart Debouncing**: Prevents excessive updates during periods of high activity
- **Status Synchronization**: Both global summary and individual mouse data stay synchronized

#### Enhanced Language Support
- **System Language Detection**: Application automatically detects and uses your system's default language
- **Proper Fallback**: Graceful fallback to English if system locale cannot be determined
- **Fixed Mixed Languages**: Resolved issues where German and English text appeared simultaneously
- **Improved Translations**: Added missing translations for Labyrinth-specific terms
- **Consistent Terminology**: Standardized German translations throughout the interface

### Bug Fixes

#### Labyrinth Auto-Update Issues
- **Fixed**: Labyrinth view not updating when new blocks were found
- **Fixed**: Mouse status not refreshing when new mice were created
- **Fixed**: Manual refresh required to see maturation status changes
- **Fixed**: Global summary becoming outdated during extended sessions
- **Fixed**: Mice disappearing from labyrinth view during wallet operations
- **Root Cause**: Missing connections between block/transaction notifications and UI updates, plus wallet lock contention
- **Solution**: Implemented comprehensive auto-update system with multiple triggers and wallet lock retry mechanism

#### BCT Database and Data Integrity
- **Fixed**: Mice disappearing temporarily when wallet was busy (syncing, building, etc.)
- **Fixed**: BCT database stored in inconsistent location separate from other Cascoin data
- **Fixed**: BCT NFT system showing dummy/test data instead of real wallet BCT data
- **Fixed**: Inconsistent data between labyrinth view and NFT tokenization system
- **Root Cause**: TRY_LOCK causing empty results when wallet busy, plus inconsistent storage paths
- **Solution**: Wallet lock retry system with delays, unified data directory, and real-time wallet synchronization

#### Language and Localization
- **Fixed**: Application always starting in English regardless of system language
- **Fixed**: Mixed German/English text throughout the interface
- **Fixed**: Hardcoded English strings in Labyrinth functionality
- **Fixed**: Missing translations for mouse-related terminology
- **Root Cause**: Hard-coded English default and untranslated strings
- **Solution**: System locale detection and comprehensive translation coverage

#### User Interface Improvements
- **Fixed**: "Create Mice" input field text was invisible (white text on white background)
- **Fixed**: Poor contrast made input field difficult to read
- **Fixed**: Missing visual feedback during input
- **Root Cause**: CSS styling conflicts with dark theme background
- **Solution**: High-contrast color scheme with proper background and text colors

### Technical Implementation

#### Auto-Update Architecture
- **Block Change Detection**: Connected `ClientModel::numBlocksChanged` signal to refresh labyrinth data
- **Transaction Updates**: Enhanced `WalletModel::updateTransaction()` to trigger labyrinth refreshes
- **Debounced Updates**: 1-second timer prevents excessive refresh calls during rapid changes
- **Periodic Refresh**: 60-second timer ensures data freshness during extended sessions
- **Smart Updates**: Only refreshes when necessary to maintain performance
- **Wallet Lock Retry**: 3-retry system with 50ms delays prevents empty results during wallet operations

#### Language System Improvements
- **System Locale Detection**: Modified `GetLangTerritory()` to use `QLocale::system().name()`
- **Translation Coverage**: Added `tr()` wrapper to previously hardcoded strings
- **German Terminology**: Proper translations for adventure mice, resting mice, and labyrinth terms
- **Fallback Mechanism**: Graceful handling when system locale detection fails

#### BCT Database Architecture Overhaul
- **Unified Data Directory**: Moved BCT database from `~/.local/share/Cascoin/` to standard `~/.cascoin/` directory
- **Automatic Migration**: Seamless migration from old location to new location with backup preservation
- **Real-time Synchronization**: BCT database now syncs with actual wallet BCT data instead of using dummy data
- **Wallet Integration**: `HiveTableModel` automatically updates BCT database when wallet data changes
- **Global Instance Management**: Centralized BCT database access for system-wide data consistency

#### UI Enhancements
- **CSS Styling**: Comprehensive QSpinBox styling for optimal readability
- **Color Scheme**: Light gray background (#f7fafc) with dark text (#1a202c) for maximum contrast
- **Focus States**: Blue highlight (#3182ce) for better input field visibility
- **Button Styling**: Enhanced up/down button appearance with hover effects

### User Impact
- **Seamless Experience**: Labyrinth data updates automatically without manual intervention
- **Native Language**: Interface appears in your system's language from first startup
- **Improved Readability**: All input fields and text are clearly visible and easy to read
- **Reduced Clicks**: No more manual refresh buttons needed for current data
- **Better Responsiveness**: Immediate feedback when mice are created or mature

### Developer Notes
- **New Slot**: Added `onBlocksChanged()` to `HiveDialog` class
- **Enhanced Model**: Extended wallet model with automatic hive table updates
- **Timer Management**: Proper timer cleanup and debouncing mechanisms
- **Translation Updates**: Regenerated translation files with new translatable strings
- **CSS Architecture**: Modular styling approach for better maintainability

### Upgrade Notes
- **Automatic Configuration**: New language detection works immediately upon upgrade
- **No Data Migration**: Existing wallet and labyrinth data remain unchanged
- **Backward Compatibility**: Can safely downgrade if needed (manual refresh returns)
- **Settings Preservation**: User preferences and custom language settings respected
- **Fresh UI**: Enhanced interface elements take effect immediately

### System Requirements
- **Qt6 Framework**: Continues to require Qt6 for modern interface support
- **Language Packs**: System language packs improve localization experience
- **Performance**: Minimal additional resource usage from auto-update features
- **Cross Platform**: Available for Windows, Linux, and macOS

### Translation Improvements
- **German Localization**:
  - "adventure mice" → "Abenteuer-Mäuse"
  - "resting mice" → "ruhende Mäuse"  
  - "No live mice currently in wallet" → "Derzeit keine lebenden Mäuse im Portemonnaie"
  - "The Labyrinth is not enabled on the network" → "Das Labyrinth ist im Netzwerk nicht aktiviert"
  - "to create %1 mice" → "um %1 Mäuse zu erstellen"

### Performance Optimizations
- **Smart Refresh**: Only updates when data actually changes
- **Debounced Calls**: Prevents UI lag during periods of high blockchain activity
- **Efficient Queries**: Optimized data retrieval for labyrinth statistics
- **Background Processing**: Non-blocking updates maintain interface responsiveness

### Changelog
- [Feature][Labyrinth] Automatic refresh when new blocks are found
- [Feature][Labyrinth] Automatic refresh when wallet transactions change
- [Feature][Labyrinth] Periodic 60-second background refresh for data consistency
- [Feature][Language] System language detection replaces hard-coded English default
- [Feature][Language] Comprehensive German translation for Labyrinth functionality
- [Feature][BCT Database] Automatic migration to standard Cascoin data directory
- [Feature][BCT Database] Real-time synchronization between wallet and BCT NFT system
- [Fix][Labyrinth] Mice no longer disappear during wallet operations (sync, build, etc.)
- [Fix][BCT Database] Moved from inconsistent `~/.local/share/` to standard `~/.cascoin/` directory
- [Fix][BCT NFT] Replaced dummy/test data with real wallet BCT information
- [Fix][UI] Create mice input field visibility with high-contrast styling
- [Fix][UI] Proper color scheme for input fields in dark theme areas
- [Fix][Wallet] Wallet lock retry mechanism prevents data loss during concurrent operations
- [Enhancement][Performance] Debounced update system prevents excessive refresh calls
- [Enhancement][Performance] Wallet lock retry with delays for better reliability
- [Enhancement][UX] Eliminated need for manual labyrinth refresh buttons
- [Enhancement][Data Integrity] Consistent data between labyrinth view and NFT system
- [Maintenance] Updated Qt6 translation files with new translatable strings

---

### Migration Guide
This release introduces automatic features that work immediately upon installation. No user action is required to benefit from the new auto-update and language detection features.

#### BCT Database Migration
- **Automatic Process**: BCT database automatically migrates from old location to standard Cascoin directory on first startup
- **Data Preservation**: All existing BCT cache data is safely transferred to the new location
- **Backup Creation**: Original file is kept as backup in old location with `.migrated` extension
- **Real Data Sync**: Dummy data is automatically replaced with real wallet BCT information
- **No User Action**: Migration happens transparently in the background

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).

### Installation Notes
- **Standard Update**: Replace existing binaries with v3.1.5
- **Automatic Benefits**: New features activate immediately upon first launch
- **Language Detection**: Interface will appear in your system language automatically
- **Data Preservation**: All existing wallet and labyrinth data remains intact
