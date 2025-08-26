// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <beenft.h>
#include <validation.h>
#include <chainparams.h>
#include <script/standard.h>
#include <utilstrencodings.h>
#include <consensus/validation.h>

#include <algorithm>

// Validate a bee NFT token transaction
bool IsValidBeeNFTTokenTransaction(const CTransaction& tx, std::string& error) {
    // Must have at least one OP_RETURN output with OP_BEE_TOKEN
    bool foundBeeToken = false;
    
    for (const CTxOut& txout : tx.vout) {
        if (txout.scriptPubKey.size() >= 2 && 
            txout.scriptPubKey[0] == OP_RETURN && 
            txout.scriptPubKey[1] == OP_BEE_TOKEN) {
            foundBeeToken = true;
            
            // Validate OP_RETURN data size
            if (txout.scriptPubKey.size() > BEE_NFT_MAX_DATA_SIZE + 2) {
                error = "Bee NFT token data exceeds maximum size";
                return false;
            }
            
            // Parse and validate token data
            std::vector<BeeNFTToken> tokens;
            if (!ParseBeeNFTTokenTransaction(tx, tokens, error)) {
                return false;
            }
            
            // Validate each token
            for (const BeeNFTToken& token : tokens) {
                // Check original BCT exists and is valid
                if (token.originalBCT.IsNull()) {
                    error = "Invalid original BCT hash";
                    return false;
                }
                
                // Check maturity and expiry heights make sense
                if (token.maturityHeight >= token.expiryHeight) {
                    error = "Invalid bee lifecycle: maturity >= expiry";
                    return false;
                }
                
                // Check bee index is reasonable (max 1000 bees per BCT)
                if (token.beeIndex >= 1000) {
                    error = "Bee index too high";
                    return false;
                }
                
                // Validate owner address format
                if (token.currentOwner.empty() || !IsValidDestinationString(token.currentOwner)) {
                    error = "Invalid owner address";
                    return false;
                }
            }
            break;
        }
    }
    
    if (!foundBeeToken) {
        error = "No bee NFT token marker found";
        return false;
    }
    
    return true;
}

// Validate a bee NFT transfer transaction
bool IsValidBeeNFTTransferTransaction(const CTransaction& tx, std::string& error) {
    // Must have at least one OP_RETURN output with OP_BEE_TRANSFER
    bool foundBeeTransfer = false;
    
    for (const CTxOut& txout : tx.vout) {
        if (txout.scriptPubKey.size() >= 2 && 
            txout.scriptPubKey[0] == OP_RETURN && 
            txout.scriptPubKey[1] == OP_BEE_TRANSFER) {
            foundBeeTransfer = true;
            
            // Validate OP_RETURN data size
            if (txout.scriptPubKey.size() > BEE_NFT_MAX_DATA_SIZE + 2) {
                error = "Bee NFT transfer data exceeds maximum size";
                return false;
            }
            
            // Parse and validate transfer data
            std::vector<BeeNFTTransfer> transfers;
            if (!ParseBeeNFTTransferTransaction(tx, transfers, error)) {
                return false;
            }
            
            // Validate each transfer
            for (const BeeNFTTransfer& transfer : transfers) {
                // Check bee NFT ID is valid
                if (transfer.beeNFTId.IsNull()) {
                    error = "Invalid bee NFT ID";
                    return false;
                }
                
                // Validate address formats
                if (transfer.fromOwner.empty() || !IsValidDestinationString(transfer.fromOwner)) {
                    error = "Invalid from owner address";
                    return false;
                }
                
                if (transfer.toOwner.empty() || !IsValidDestinationString(transfer.toOwner)) {
                    error = "Invalid to owner address";
                    return false;
                }
                
                // Check transfer is not to self
                if (transfer.fromOwner == transfer.toOwner) {
                    error = "Cannot transfer bee NFT to same address";
                    return false;
                }
            }
            break;
        }
    }
    
    if (!foundBeeTransfer) {
        error = "No bee NFT transfer marker found";
        return false;
    }
    
    return true;
}

// Parse bee NFT token transaction data
bool ParseBeeNFTTokenTransaction(const CTransaction& tx, std::vector<BeeNFTToken>& tokens, std::string& error) {
    tokens.clear();
    
    for (const CTxOut& txout : tx.vout) {
        if (txout.scriptPubKey.size() >= 2 && 
            txout.scriptPubKey[0] == OP_RETURN && 
            txout.scriptPubKey[1] == OP_BEE_TOKEN) {
            
            // Extract data after OP_RETURN OP_BEE_TOKEN
            if (txout.scriptPubKey.size() < 4) {
                error = "Bee NFT token data too short";
                return false;
            }
            
            // Get data length
            unsigned char dataLen = txout.scriptPubKey[2];
            if (dataLen + 3 > txout.scriptPubKey.size()) {
                error = "Invalid bee NFT token data length";
                return false;
            }
            
            // Extract raw data
            std::vector<unsigned char> rawData(
                txout.scriptPubKey.begin() + 3,
                txout.scriptPubKey.begin() + 3 + dataLen
            );
            
            // Parse token data (simplified format for MVP)
            if (rawData.size() < 32 + 4 + 4 + 4 + 4) { // BCT hash + 4 x uint32
                error = "Bee NFT token data too short for required fields";
                return false;
            }
            
            BeeNFTToken token;
            
            // Parse original BCT hash (32 bytes)
            std::copy(rawData.begin(), rawData.begin() + 32, token.originalBCT.begin());
            
            // Parse uint32 fields (4 bytes each)
            token.beeIndex = ReadLE32(&rawData[32]);
            token.maturityHeight = ReadLE32(&rawData[36]);
            token.expiryHeight = ReadLE32(&rawData[40]);
            token.tokenizedHeight = ReadLE32(&rawData[44]);
            
            // Parse owner address (remaining bytes as string)
            if (rawData.size() > 48) {
                token.currentOwner = std::string(rawData.begin() + 48, rawData.end());
            }
            
            tokens.push_back(token);
            break; // Only process first bee token output
        }
    }
    
    return !tokens.empty();
}

// Parse bee NFT transfer transaction data
bool ParseBeeNFTTransferTransaction(const CTransaction& tx, std::vector<BeeNFTTransfer>& transfers, std::string& error) {
    transfers.clear();
    
    for (const CTxOut& txout : tx.vout) {
        if (txout.scriptPubKey.size() >= 2 && 
            txout.scriptPubKey[0] == OP_RETURN && 
            txout.scriptPubKey[1] == OP_BEE_TRANSFER) {
            
            // Extract data after OP_RETURN OP_BEE_TRANSFER
            if (txout.scriptPubKey.size() < 4) {
                error = "Bee NFT transfer data too short";
                return false;
            }
            
            // Get data length
            unsigned char dataLen = txout.scriptPubKey[2];
            if (dataLen + 3 > txout.scriptPubKey.size()) {
                error = "Invalid bee NFT transfer data length";
                return false;
            }
            
            // Extract raw data
            std::vector<unsigned char> rawData(
                txout.scriptPubKey.begin() + 3,
                txout.scriptPubKey.begin() + 3 + dataLen
            );
            
            // Parse transfer data
            if (rawData.size() < 32 + 4 + 32 + 8) { // Bee NFT ID + height + tx ID + fee
                error = "Bee NFT transfer data too short for required fields";
                return false;
            }
            
            BeeNFTTransfer transfer;
            
            // Parse bee NFT ID (32 bytes)
            std::copy(rawData.begin(), rawData.begin() + 32, transfer.beeNFTId.begin());
            
            // Parse transfer height (4 bytes)
            transfer.transferHeight = ReadLE32(&rawData[32]);
            
            // Parse transfer tx ID (32 bytes)
            std::copy(rawData.begin() + 36, rawData.begin() + 68, transfer.transferTxId.begin());
            
            // Parse transfer fee (8 bytes)
            transfer.transferFee = ReadLE64(&rawData[68]);
            
            // Parse addresses (remaining bytes, split by null terminator)
            if (rawData.size() > 76) {
                std::string addresses(rawData.begin() + 76, rawData.end());
                size_t nullPos = addresses.find('\0');
                if (nullPos != std::string::npos) {
                    transfer.fromOwner = addresses.substr(0, nullPos);
                    transfer.toOwner = addresses.substr(nullPos + 1);
                } else {
                    error = "Invalid address format in transfer data";
                    return false;
                }
            }
            
            transfers.push_back(transfer);
            break; // Only process first bee transfer output
        }
    }
    
    return !transfers.empty();
}

// Serialize bee NFT token for transaction
std::vector<unsigned char> SerializeBeeNFTToken(const BeeNFTToken& token) {
    std::vector<unsigned char> data;
    data.reserve(80); // Reserve space for efficiency
    
    // Add original BCT hash (32 bytes)
    data.insert(data.end(), token.originalBCT.begin(), token.originalBCT.end());
    
    // Add uint32 fields (4 bytes each)
    unsigned char beeIndexBytes[4];
    WriteLE32(beeIndexBytes, token.beeIndex);
    data.insert(data.end(), beeIndexBytes, beeIndexBytes + 4);
    
    unsigned char maturityBytes[4];
    WriteLE32(maturityBytes, token.maturityHeight);
    data.insert(data.end(), maturityBytes, maturityBytes + 4);
    
    unsigned char expiryBytes[4];
    WriteLE32(expiryBytes, token.expiryHeight);
    data.insert(data.end(), expiryBytes, expiryBytes + 4);
    
    unsigned char tokenizedBytes[4];
    WriteLE32(tokenizedBytes, token.tokenizedHeight);
    data.insert(data.end(), tokenizedBytes, tokenizedBytes + 4);
    
    // Add owner address
    data.insert(data.end(), token.currentOwner.begin(), token.currentOwner.end());
    
    return data;
}

// Deserialize bee NFT token from data
bool DeserializeBeeNFTToken(const std::vector<unsigned char>& data, BeeNFTToken& token) {
    if (data.size() < 48) { // Minimum size for required fields
        return false;
    }
    
    // Parse original BCT hash
    std::copy(data.begin(), data.begin() + 32, token.originalBCT.begin());
    
    // Parse uint32 fields
    token.beeIndex = ReadLE32(&data[32]);
    token.maturityHeight = ReadLE32(&data[36]);
    token.expiryHeight = ReadLE32(&data[40]);
    token.tokenizedHeight = ReadLE32(&data[44]);
    
    // Parse owner address
    if (data.size() > 48) {
        token.currentOwner = std::string(data.begin() + 48, data.end());
    }
    
    return true;
}

// Serialize bee NFT transfer for transaction
std::vector<unsigned char> SerializeBeeNFTTransfer(const BeeNFTTransfer& transfer) {
    std::vector<unsigned char> data;
    data.reserve(80); // Reserve space for efficiency
    
    // Add bee NFT ID (32 bytes)
    data.insert(data.end(), transfer.beeNFTId.begin(), transfer.beeNFTId.end());
    
    // Add transfer height (4 bytes)
    unsigned char heightBytes[4];
    WriteLE32(heightBytes, transfer.transferHeight);
    data.insert(data.end(), heightBytes, heightBytes + 4);
    
    // Add transfer tx ID (32 bytes)
    data.insert(data.end(), transfer.transferTxId.begin(), transfer.transferTxId.end());
    
    // Add transfer fee (8 bytes)
    unsigned char feeBytes[8];
    WriteLE64(feeBytes, transfer.transferFee);
    data.insert(data.end(), feeBytes, feeBytes + 8);
    
    // Add addresses with null separator
    data.insert(data.end(), transfer.fromOwner.begin(), transfer.fromOwner.end());
    data.push_back('\0');
    data.insert(data.end(), transfer.toOwner.begin(), transfer.toOwner.end());
    
    return data;
}

// Deserialize bee NFT transfer from data
bool DeserializeBeeNFTTransfer(const std::vector<unsigned char>& data, BeeNFTTransfer& transfer) {
    if (data.size() < 76) { // Minimum size for required fields
        return false;
    }
    
    // Parse bee NFT ID
    std::copy(data.begin(), data.begin() + 32, transfer.beeNFTId.begin());
    
    // Parse transfer height
    transfer.transferHeight = ReadLE32(&data[32]);
    
    // Parse transfer tx ID
    std::copy(data.begin() + 36, data.begin() + 68, transfer.transferTxId.begin());
    
    // Parse transfer fee
    transfer.transferFee = ReadLE64(&data[68]);
    
    // Parse addresses
    if (data.size() > 76) {
        std::string addresses(data.begin() + 76, data.end());
        size_t nullPos = addresses.find('\0');
        if (nullPos != std::string::npos) {
            transfer.fromOwner = addresses.substr(0, nullPos);
            transfer.toOwner = addresses.substr(nullPos + 1);
        } else {
            return false;
        }
    }
    
    return true;
}

// Create bee NFT token transaction script
CScript CreateBeeNFTTokenScript(const std::vector<BeeNFTToken>& tokens) {
    CScript script;
    
    if (tokens.empty()) {
        return script;
    }
    
    // For MVP, only support single token per transaction
    const BeeNFTToken& token = tokens[0];
    std::vector<unsigned char> tokenData = SerializeBeeNFTToken(token);
    
    script << OP_RETURN << OP_BEE_TOKEN << tokenData;
    
    return script;
}

// Create bee NFT transfer transaction script
CScript CreateBeeNFTTransferScript(const std::vector<BeeNFTTransfer>& transfers) {
    CScript script;
    
    if (transfers.empty()) {
        return script;
    }
    
    // For MVP, only support single transfer per transaction
    const BeeNFTTransfer& transfer = transfers[0];
    std::vector<unsigned char> transferData = SerializeBeeNFTTransfer(transfer);
    
    script << OP_RETURN << OP_BEE_TRANSFER << transferData;
    
    return script;
}
