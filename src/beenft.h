// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CASCOIN_BEENFT_H
#define CASCOIN_BEENFT_H

#include <amount.h>
#include <uint256.h>
#include <script/script.h>
#include <support/allocators/secure.h>

#include <string>
#include <vector>
#include <map>

/*
Cascoin: Bee NFT System

Individual Bee Tokenization allows transferring specific bees from BCTs as NFT-like tokens.
Each bee from a BCT can be tokenized and transferred independently, enabling:
- Bee trading markets
- Granular mining rights transfer
- Individual bee ownership tracking
- Integration with DeFi systems

Architecture:
1. BCT creates N bees (existing system)
2. Owner can tokenize individual bees via OP_BEE_TOKEN transaction
3. Bee tokens can be transferred via OP_BEE_TRANSFER transaction
4. Mining system checks bee token ownership for mining rights
5. Rewards go to current bee token owner

Each bee NFT contains:
- Original BCT transaction ID
- Bee index within that BCT (0 to N-1)
- Maturity and expiry heights
- Current owner address
- Mining performance history
*/

// Maximum data size for bee NFT transactions
const int BEE_NFT_MAX_DATA_SIZE = 80;

// Bee NFT Token structure
struct BeeNFTToken {
    uint256 originalBCT;        // Original BCT transaction hash
    uint32_t beeIndex;          // Index of this bee within the BCT (0-based)
    uint32_t maturityHeight;    // Block height when bee matures for mining
    uint32_t expiryHeight;      // Block height when bee expires
    uint32_t tokenizedHeight;   // Block height when bee was tokenized
    std::string currentOwner;   // Current owner's address
    
    // Serialization for storage and network transmission
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(originalBCT);
        READWRITE(beeIndex);
        READWRITE(maturityHeight);
        READWRITE(expiryHeight);
        READWRITE(tokenizedHeight);
        READWRITE(currentOwner);
    }
    
    // Generate unique bee NFT ID
    uint256 GetBeeNFTId() const {
        CHashWriter ss(SER_GETHASH, 0);
        ss << originalBCT << beeIndex;
        return ss.GetHash();
    }
    
    // Check if bee is currently mature for mining
    bool IsMature(int currentHeight) const {
        return currentHeight >= maturityHeight && currentHeight < expiryHeight;
    }
    
    // Check if bee has expired
    bool IsExpired(int currentHeight) const {
        return currentHeight >= expiryHeight;
    }
};

// Bee NFT Transfer record
struct BeeNFTTransfer {
    uint256 beeNFTId;           // Unique bee NFT identifier
    std::string fromOwner;      // Previous owner address
    std::string toOwner;        // New owner address
    uint32_t transferHeight;    // Block height of transfer
    uint256 transferTxId;       // Transaction hash of transfer
    CAmount transferFee;        // Fee paid for transfer
    
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(beeNFTId);
        READWRITE(fromOwner);
        READWRITE(toOwner);
        READWRITE(transferHeight);
        READWRITE(transferTxId);
        READWRITE(transferFee);
    }
};

// Bee NFT mining performance record
struct BeeNFTMiningRecord {
    uint256 beeNFTId;           // Unique bee NFT identifier
    uint32_t blockHeight;       // Height of mined block
    uint256 blockHash;          // Hash of mined block
    CAmount rewardAmount;       // Mining reward received
    uint32_t difficulty;        // Mining difficulty at time of block
    
    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(beeNFTId);
        READWRITE(blockHeight);
        READWRITE(blockHash);
        READWRITE(rewardAmount);
        READWRITE(difficulty);
    }
};

// Validation functions
bool IsValidBeeNFTTokenTransaction(const CTransaction& tx, std::string& error);
bool IsValidBeeNFTTransferTransaction(const CTransaction& tx, std::string& error);

// Parsing functions for bee NFT transactions
bool ParseBeeNFTTokenTransaction(const CTransaction& tx, std::vector<BeeNFTToken>& tokens, std::string& error);
bool ParseBeeNFTTransferTransaction(const CTransaction& tx, std::vector<BeeNFTTransfer>& transfers, std::string& error);

// Utility functions
std::vector<unsigned char> SerializeBeeNFTToken(const BeeNFTToken& token);
bool DeserializeBeeNFTToken(const std::vector<unsigned char>& data, BeeNFTToken& token);

std::vector<unsigned char> SerializeBeeNFTTransfer(const BeeNFTTransfer& transfer);
bool DeserializeBeeNFTTransfer(const std::vector<unsigned char>& data, BeeNFTTransfer& transfer);

// Create bee NFT token transaction script
CScript CreateBeeNFTTokenScript(const std::vector<BeeNFTToken>& tokens);

// Create bee NFT transfer transaction script  
CScript CreateBeeNFTTransferScript(const std::vector<BeeNFTTransfer>& transfers);

#endif // CASCOIN_BEENFT_H
