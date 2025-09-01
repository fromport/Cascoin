## Cascoin Core v3.1.2 – Memory Optimization Release

### Overview
- **Performance release**: Significant memory leak fixes and RAM usage optimizations
- **Stability improvements**: Enhanced memory management for long-running nodes
- **No breaking changes**: Maintains full compatibility with v3.1.1

### Highlights
- **Memory Leak Fixes**: Resolved critical memory leaks affecting long-running nodes
- **RAM Usage Optimization**: Reduced overall memory footprint by optimizing data structures
- **Enhanced Stability**: Improved performance for nodes running continuously for extended periods
- **Better Resource Management**: More efficient handling of blockchain data and transaction processing

### Performance Improvements

#### Memory Leak Fixes
- **Fixed**: hiveCoinbaseMap memory leak with automatic cleanup when exceeding 1000 entries
- **Fixed**: BCT cache overflow by limiting cache to maximum 1000 entries instead of unlimited growth
- **Fixed**: Thread pool memory leaks by limiting work queue depth to 64 entries maximum
- **Fixed**: NFT system memory leaks with intelligent memory pool management (8MB pool limit)
- **Fixed**: Splash screen memory leaks through improved thread management with shared_ptr

#### RAM Usage Optimization
- **Reduced**: Database cache from 450MB to 200MB default to prevent memory exhaustion
- **Optimized**: NFT table model limited to 500 cached entries with priority-based cleanup
- **Enhanced**: HTTP server thread pool with maximum 2x thread count for active tasks
- **Improved**: Memory allocation patterns with better size limits and automatic cleanup

### Technical Details
- **Cache Limits**: Implemented size limits on previously unbounded caches (hiveCoinbaseMap, BCT cache)
- **Smart Pointer Usage**: Enhanced thread management using shared_ptr for automatic cleanup
- **Memory Pool Management**: Added NFTMemoryManager for intelligent 4KB NFT data handling
- **Queue Depth Limits**: Protected against unbounded task creation in HTTP server
- **Automatic Cleanup**: Added logging and monitoring for memory usage patterns

### User Impact
- **Long-running Nodes**: Significantly improved stability for nodes running 24/7
- **Lower System Requirements**: Reduced RAM requirements for optimal performance
- **Better Responsiveness**: Improved GUI responsiveness during heavy blockchain operations
- **Reduced Crashes**: Fewer out-of-memory related crashes on resource-constrained systems
- **Faster Sync**: More efficient memory usage during initial blockchain synchronization

### System Requirements
- **Recommended RAM**: Reduced from 4GB to 3GB for optimal performance
- **Minimum RAM**: Reduced from 2GB to 1.5GB for basic operation
- **Better Performance**: Improved performance on systems with limited memory

### Compatibility
- **Full Compatibility**: 100% compatible with existing wallets and blockchain data
- **Network Protocol**: No changes to network protocol - seamless upgrade
- **API Compatibility**: All RPC commands and responses remain unchanged
- **Configuration**: Existing configuration files work without modification

### Upgrade Recommendations
- **High Priority**: Recommended for all users, especially those running nodes continuously
- **Mining Pools**: Strongly recommended for improved stability and performance
- **Exchanges**: Recommended for better resource utilization and stability
- **Regular Users**: Benefits include faster startup and better overall performance

---

For the complete list of changes, see the [commit history](https://github.com/casraw/cascoin/commits/master).

### Installation Notes
- **Backup**: Always backup your wallet.dat before upgrading
- **Shutdown**: Cleanly shutdown the previous version before installing v3.1.2
- **Memory**: You may notice reduced memory usage immediately after upgrade
- **Performance**: Full performance benefits may take a few hours of operation to become apparent
