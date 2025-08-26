## Cascoin Core v3.1.0 – CASTOK NFT System

### Overview
- **Major new feature**: Complete BCT NFT System with CASTOK/CASXFR magic bytes
- **Soft Fork implementation**: Backward-compatible with older nodes
- **Enhanced GUI**: Mice NFT management interface with real-time data
- **Wallet stability**: Improved wallet corruption resistance

### Highlights
- **CASTOK NFT Tokenization**: Transform entire BCTs into transferable NFTs
- **CASXFR NFT Transfers**: Send NFTs between addresses with full validation
- **Magic Bytes System**: Uses CASTOK/CASXFR instead of opcodes for soft fork compatibility
- **Enhanced GUI**: Complete NFT management interface in "Mice NFTs" tab
- **Real Transaction Creation**: Full RPC integration with `bctnftokenize` and `bctnftransfer`
- **Consensus Validation**: Soft fork validation ensures network compatibility

### New Features

#### NFT System
- **Complete BCT Tokenization**: Convert entire BCTs into single NFTs
- **NFT Transfer System**: Send NFTs to other addresses with validation
- **Soft Fork Compatibility**: Old nodes see NFTs as standard OP_RETURN data
- **Magic Bytes**: CASTOK (tokenization) and CASXFR (transfer) identifiers

#### RPC Commands
- `bctnftokenize <bct_txid> [owner_address]` - Tokenize complete BCT as NFT
- `bctnftransfer <nft_txid> <to_address>` - Transfer NFT to new owner
- `bctnftlist` - List all owned NFTs
- `miceavailable` - List available BCTs with mice counts

#### GUI Enhancements
- **Mice NFTs Tab**: Complete NFT management interface
- **BCT Selection**: Dropdown with real BCT data and mice counts
- **Progress Feedback**: Loading indicators and status messages
- **Enhanced Splash Screen**: Detailed progress with time estimates
- **Asynchronous Loading**: Non-blocking BCT database initialization

### Technical Improvements
- **Wallet Salvage**: Enhanced wallet corruption recovery
- **D-Bus Stability**: Improved GUI stability with D-Bus fixes
- **Qt6 Compatibility**: Full migration to Qt6 signal/slot syntax
- **Memory Management**: Better handling of large datasets
- **Error Handling**: Comprehensive validation and user feedback

### Consensus Changes
- **Soft Fork**: NFT transactions validate on upgraded nodes only
- **OP_RETURN Usage**: Standard OP_RETURN with magic bytes (no new opcodes)
- **Backward Compatibility**: Old nodes accept NFT transactions as valid
- **Network Adoption**: Gradual rollout without network splits

### Breaking Changes
- **None**: This is a soft fork, fully backward compatible

### Known Issues
- NFT transactions may show as "offline" until sufficient network adoption
- Mining pools should upgrade for optimal NFT transaction relay

### Credits
This release includes contributions from the Cascoin development team for the complete NFT infrastructure implementation.

---

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).
