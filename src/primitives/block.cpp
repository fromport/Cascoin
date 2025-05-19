// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>
#include <utilstrencodings.h>
#include <crypto/common.h>
#include <crypto/scrypt.h>
#include <chainparams.h>    // Cascoin: Hive

#include <crypto/minotaurx/minotaur.h>  // Cascoin: MinotaurX+Hive1.2
#include <validation.h>                 // Cascoin: MinotaurX+Hive1.2
#include <util.h>                       // Cascoin: MinotaurX+Hive1.2

uint256 CBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}

/*
// Cascoin: MinotaurX+Hive1.2: Hash arbitrary data, using internally-managed thread-local memory for YP
uint256 CBlockHeader::MinotaurXHashArbitrary(const char* data) {
    return Minotaur(data, data + strlen(data), true);
}

// Cascoin: MinotaurX+Hive1.2: Hash a string with MinotaurX, using provided YP thread-local memory
uint256 CBlockHeader::MinotaurXHashStringWithLocal(std::string data, yespower_local_t *local) {
    return Minotaur(data.begin(), data.end(), true, local);
}*/

// Cascoin: MinotaurX+Hive1.2: Hash arbitrary data with classical Minotaur
uint256 CBlockHeader::MinotaurHashArbitrary(const char* data) {
    return Minotaur(data, data + strlen(data), false);
}

// Cascoin: MinotaurX+Hive1.2: Hash a string with classical Minotaur
uint256 CBlockHeader::MinotaurHashString(std::string data) {
    return Minotaur(data.begin(), data.end(), false);
}

// Cascoin: MinotaurX+Hive1.2: Get pow hash based on block type and UASF activation
uint256 CBlockHeader::GetPoWHash() const
{
    LogPrintf("GetPoWHash (Block: %s): nTime=%u, nVersion=0x%08x, Consensus.powForkTime=%u\n", GetHash().ToString().substr(0,10), nTime, nVersion, Params().GetConsensus().powForkTime);

    if (nTime > Params().GetConsensus().powForkTime) { // Multi-algo logic is active
        LogPrintf("GetPoWHash: Multi-algo time zone (nTime > powForkTime).\n");

        POW_TYPE calculatedPowType = GetPoWType(); // (nVersion >> 16) & 0xFF
        LogPrintf("GetPoWHash: Raw GetPoWType() from nVersion 0x%08x returned: %d (%s)\n", nVersion, static_cast<int>(calculatedPowType), (calculatedPowType < NUM_BLOCK_TYPES ? POW_TYPE_NAMES[calculatedPowType] : "unknown_type_value"));

        // Explicit check for known algorithm types based on extracted bits
        if (calculatedPowType == POW_TYPE_MINOTAURX) {
            LogPrintf("GetPoWHash: PoW Type bits explicitly indicate MINOTAURX (%d). Using Minotaur hash.\n", static_cast<int>(POW_TYPE_MINOTAURX));
            return Minotaur(BEGIN(nVersion), END(nNonce), true);
        }
        
        // Check for BIP9-style versioning (e.g. 0x20000000 for standard SHA256 blocks if other bits aren't set for specific algos)
        // This was the previous logic's first check after powForkTime.
        if (nVersion >= 0x20000000) {
             LogPrintf("GetPoWHash: nVersion 0x%08x >= 0x20000000 (BIP9 style). Assuming SHA256. Returning GetHash().\n", nVersion);
             return GetHash();
        }

        // If not MinotaurX by explicit bits, and not a high-bit BIP9 version, then default to SHA256.
        // This covers PoWType being explicitly POW_TYPE_SHA256 (0) or any other value not caught above (like 217 from 0x04d98000).
        if (calculatedPowType == POW_TYPE_SHA256) {
             LogPrintf("GetPoWHash: PoW Type bits explicitly indicate SHA256 (%d). Returning GetHash().\n", static_cast<int>(POW_TYPE_SHA256));
        } else {
             LogPrintf("GetPoWHash: PoW Type bits indicate UNKNOWN type %d (and nVersion < 0x20000000). Defaulting to SHA256 for LCC compatibility. Returning GetHash().\n", static_cast<int>(calculatedPowType));
        }
        return GetHash(); // Default to SHA256

    } else { // Pre-multi-algo fork (powForkTime not reached)
        LogPrintf("GetPoWHash: Pre-multi-algo fork (nTime <= powForkTime). Using Scrypt.\n");
        uint256 thash;
        scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
        return thash;
    }
}

// Cascoin: Add helper definition
POW_TYPE CBlockHeader::GetEffectivePoWTypeForHashing(const Consensus::Params& consensusParams) const {
    // Check if we are in the multi-algorithm phase based on block time
    if (nTime > consensusParams.powForkTime) {
        // Multi-algorithm phase logic
        POW_TYPE calculatedPowType = GetPoWType(); // Get raw PoW type from nVersion bits: (nVersion >> 16) & 0xFF

        // Explicitly check for MinotaurX
        if (calculatedPowType == POW_TYPE_MINOTAURX) {
            LogPrintf("GetEffectivePoWTypeForHashing: Multi-algo phase, raw type is MINOTAURX (%d). Effective type: MINOTAURX.\n", static_cast<int>(calculatedPowType));
            return POW_TYPE_MINOTAURX;
        }
        
        // Check for BIP9-style versioning (e.g. 0x20000000 typically implies SHA256 if not MinotaurX)
        if (nVersion >= 0x20000000) {
             LogPrintf("GetEffectivePoWTypeForHashing: Multi-algo phase, nVersion 0x%08x >= 0x20000000 (BIP9 style). Effective type: SHA256.\n", nVersion);
             return POW_TYPE_SHA256;
        }

        // Default for multi-algo phase if not MinotaurX and not a high-bit nVersion:
        // This covers cases where calculatedPowType is POW_TYPE_SHA256 (0) or an "unknown" type (like 41 from 0x0a290000).
        // GetPoWHash would default to SHA256 (GetHash()) in these scenarios.
        LogPrintf("GetEffectivePoWTypeForHashing: Multi-algo phase, raw type is %d (SHA256 or UNKNOWN) and nVersion < 0x20000000. Effective type: SHA256.\n", static_cast<int>(calculatedPowType));
        return POW_TYPE_SHA256;

    } else {
        // Pre-multi-algorithm fork phase: Default to Scrypt (as per original Litecoin Cash behavior pre-Hive/MinotaurX)
        LogPrintf("GetEffectivePoWTypeForHashing: Pre-multi-algo fork (nTime <= powForkTime). Effective type: SCRYPT.\n");
        return POW_TYPE_SCRYPT; // LCC's original PoW was Scrypt
    }
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    // Cascoin: Hive: Include type
    bool isHive = IsHiveMined(Params().GetConsensus());
    s << strprintf("CBlock(type=%s, hash=%s, powHash=%s, powType=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        isHive ? "hive" : "pow",
        GetHash().ToString(),
        GetPoWHash().ToString(),
        isHive ? "n/a" : GetPoWTypeName(),  // Cascoin: MinotaurX+Hive1.2: Include pow type name
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
