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
    // Log basic info for this specific call to GetPoWHash
    LogPrintf("GetPoWHash (Block: %s): nTime=%u, nVersion=0x%08x, Consensus.powForkTime=%u\n", GetHash().ToString().substr(0,10), nTime, nVersion, Params().GetConsensus().powForkTime);

    if (nTime > Params().GetConsensus().powForkTime) {
        LogPrintf("GetPoWHash: nTime > powForkTime branch taken.\n");
        
        POW_TYPE calculatedPowType = GetPoWType(); // Call it once
        LogPrintf("GetPoWHash: GetPoWType() returned: %d (%s) from nVersion 0x%08x\n", static_cast<int>(calculatedPowType), (calculatedPowType < NUM_BLOCK_TYPES ? POW_TYPE_NAMES[calculatedPowType] : "unknown_type_value"), nVersion);

        // This version check might be part of an older UASF logic for MinotaurX.
        // If MinotaurX is enabled by height (via IsMinotaurXEnabled), this might conflict or be redundant.
        if (nVersion >= 0x20000000) { // Bitcoin core uses this for BIP9 version bits usually.
            LogPrintf("GetPoWHash: nVersion >= 0x20000000 branch. Assuming SHA256. Returning GetHash().\n");
            return GetHash(); // MinotaurX not activated by *this specific version scheme*; assumes sha256
        }
        LogPrintf("GetPoWHash: nVersion < 0x20000000. Proceeding to switch on PoWType.\n");

        switch (calculatedPowType) {
            case POW_TYPE_SHA256:
                LogPrintf("GetPoWHash: Switch case POW_TYPE_SHA256. Returning GetHash().\n");
                return GetHash();
            case POW_TYPE_MINOTAURX:
                LogPrintf("GetPoWHash: Switch case POW_TYPE_MINOTAURX. Returning Minotaur(...).\n");
                return Minotaur(BEGIN(nVersion), END(nNonce), true);
            default:
                LogPrintf("GetPoWHash: Switch case DEFAULT! calculatedPowType=%d. Returning HIGH_HASH.\n", static_cast<int>(calculatedPowType));
                return HIGH_HASH;
        }
    }
    
    LogPrintf("GetPoWHash: nTime <= powForkTime branch. Pre-fork logic (Scrypt). Returning scrypt hash.\n");
    uint256 thash;
    scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
    return thash;
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
