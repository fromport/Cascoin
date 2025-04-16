// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>
#include <base58.h> // Cascoin: Needed for DecodeDestination()

#include <assert.h>

#include <chainparamsseeds.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Cascoin Official Launch: 13/Apr/2024 - The Future of Hybrid Consensus";
    const CScript genesisOutputScript = CScript() << ParseHex("044bdf8b325e5007390ddf6b4c4309bab7222ac4f0dc0f0e81089f3c87df272ff56f454bf738da22191d4e048e7ebf70ba06270c42f9030c8cf47e6b492c480183") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 840000;
        consensus.BIP16Height = 0; // 87afb798a3ad9378fcd56123c81fb31cfd9a8df4719b9774d71730c16315a092 - October 1, 2012
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("fa09d204a83a768ed5a7c8d441fa62f2043abf420cff1226c7b4329aeb9d51cf");
        consensus.BIP65Height = 0; // bab3041e8977e0dc3eeff63fe707b92bde1dd449d8efafb248c27c8264cc311a
        consensus.BIP66Height = 0; // 7aceee012833fa8952f8835d8b1b3ae233cd6ab08fdb27a771d2bd7bdc491894
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 6048; // 75% of 8064
        consensus.nMinerConfirmationWindow = 8064; // nPowTargetTimespan / nPowTargetSpacing * 4
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1746108117; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 2000000000; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1746108117; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 2000000000; // January 31st, 2018

        // Cascoin: Hive: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].bit = 7;
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].nStartTime = 1746108117; // Dec 26, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].nTimeout = 2000000000; // Dec 26, 2019

        // Cascoin: Hive 1.1: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].nStartTime = 1746108117;  // Sept 20, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].nTimeout = 1600560000;    // Sept 20, 2020

        // Cascoin: MinotaurX+Hive1.2: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].bit = 7;
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].nStartTime = 1746108117;  // 1200 UTC Sept 16, 2021
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].nTimeout = 2000000000;  // Start + 1 year

        // Cascoin: Rialto: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].nStartTime = 2000000000;               // Far future (2033)
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].nTimeout = 2000000000 + 31536000;      // Start + 1 year

        // Cascoin fields
        consensus.powForkTime = 1518982404;                 // Time of PoW hash method change
        consensus.lastScryptBlock = 0;                // Height of last scrypt block
        consensus.powLimitSHA = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");   // Initial hash target at fork
        consensus.slowStartBlocks = 2000;                   // Scale post-fork block reward up over this many blocks
        consensus.premineAmount = 0;                   // Premine amount (less than 1% of issued currency at fork time)
        std::vector<unsigned char> vch = ParseHex("76a914c9f3305556963e2976ccf3348b89a6cc736b6a4e88ac");
        consensus.premineOutputScript = CScript(vch.begin(), vch.end());	// Output script for premine block (CashierDaZEsyBQkuvv4c2uPZFx6m2XTgT)
        consensus.totalMoneySupplyHeight = 6215968;         // Height at which TMS is reached, do not issue rewards past this point

        // Cascoin: Hive: Consensus Fields
        consensus.minBeeCost = 10000;                       // Minimum cost of a bee, used when no more block rewards
        consensus.beeCostFactor = 2500;                     // Bee cost is block_reward/beeCostFactor
        consensus.beeCreationAddress = "CReateCascoinCashWorkerBeeXYs19YQ";        // Unspendable address for bee creation
        consensus.hiveCommunityAddress = "cas1qv2drkwum2ytsl7zzx048ss66a9p7jht7tgcrck";      // Community fund address
        consensus.communityContribFactor = 10;              // Optionally, donate bct_value/maxCommunityContribFactor to community fund
        consensus.beeGestationBlocks = 48*24;               // The number of blocks for a new bee to mature
        consensus.beeLifespanBlocks = 48*24*14;             // The number of blocks a bee lives for after maturation
        consensus.powLimitHive = uint256S("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");  // Highest (easiest) bee hash target
        consensus.minHiveCheckBlock = 1537566;              // Don't bother checking below this height for Hive blocks (not used for consensus/validation checks, just efficiency when looking for potential BCTs)
        consensus.hiveTargetAdjustAggression = 30;          // Snap speed for bee hash target adjustment EMA
        consensus.hiveBlockSpacingTarget = 2;               // Target Hive block frequency (1 out of this many blocks should be Hivemined)
        consensus.hiveBlockSpacingTargetTypical = 3;        // Observed Hive block frequency (1 out of this many blocks are observed to be Hive)
        consensus.hiveBlockSpacingTargetTypical_1_1 = 2;    // Observed Hive block frequency in Hive 1.1 (1 out of this many blocks are observed to be Hive)
        consensus.hiveNonceMarker = 192;                    // Nonce marker for hivemined blocks

        // Cascoin: Hive 1.1-related consensus fields
        consensus.minK = 2;                                 // Minimum chainwork scale for Hive blocks (see Hive whitepaper section 5)
        consensus.maxK = 16;                                // Maximum chainwork scale for Hive blocks (see Hive whitepaper section 5)
        consensus.maxHiveDiff = 0.006;                      // Hive difficulty at which max chainwork bonus is awarded
        consensus.maxKPow = 5;                              // Maximum chainwork scale for PoW blocks
        consensus.powSplit1 = 0.005;                        // Below this Hive difficulty threshold, PoW block chainwork bonus is halved
        consensus.powSplit2 = 0.0025;                       // Below this Hive difficulty threshold, PoW block chainwork bonus is halved again
        consensus.maxConsecutiveHiveBlocks = 2;             // Maximum hive blocks that can occur consecutively before a PoW block is required
        consensus.hiveDifficultyWindow = 36;                // How many blocks the SMA averages over in hive difficulty adjust

        // Cascoin: MinotaurX+Hive1.2-related consensus fields
        consensus.lwmaAveragingWindow = 90;                 // Averaging window size for LWMA diff adjust
        consensus.powTypeLimits.emplace_back(uint256S("0x00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));   // sha256d limit
        consensus.powTypeLimits.emplace_back(uint256S("0x000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));   // MinotaurX limit

        // Cascoin: Rialto-related consensus fields
        // Note: These are *NOT* finalised.
        consensus.nickCreationAddress       = "cas1q9zy97ulpfqdk9ykn259jzhd3jvr5uyh8hqyv00";     // Nick creation address
        consensus.nickCreationCost3Char     = 1000000000000;                            // Minimum costs to register a nick
        consensus.nickCreationCost4Char     = 100000000000;
        consensus.nickCreationCostStandard  = 1000000000;
        consensus.nickCreationAntiDust      = 10000;                                    // Portion of creation cost burnt in 2nd output

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000ba12a25c1f2da751fc96");  // Cascoin: 1695238

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000000000000238fc08340331e2735a64ac2baccdc3db0984ef65c08f658b2"); // Cascoin: 1695238

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xca;
        pchMessageStart[1] = 0x5c;
        pchMessageStart[2] = 0x01;
        pchMessageStart[3] = 0xcf;
        nDefaultPort = 22222;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1744615453, 2000084999, 0x1e0ffff0, 1, 50 * COIN * COIN_SCALE);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000de2f360327f8be16db496f3b89dc456280426325597a7c1a066f7a5c915"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ba58814fafc987fae4478c855a706fe513650d0ffda7d0138a4fc0df270e72b"));

        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.emplace_back("seed.cascoin.net");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 40);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 8);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 50);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 188);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "cas";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {0, uint256S("0x00000666c27e333cad657bd3b90968c2e45d0ffd20da5aea32568c6e5a0c9e38")}
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 0000000000000012e28998c604bbfc58bc0ae30523bca8a2f41320f4db2655d1 (height 2511842).
            1744615453, // * UNIX timestamp of last known number of transactions
            1,   // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.0      // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 840000;
        consensus.BIP16Height = 1; // always enforce BIP16
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0x00000025140b1236292bc21b2afa9f3bd5c3d4a8cc1d0e3d1ba0ba7fdefc92eb"); // Block hash at block 48
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15; // Require 75% of last 20 blocks to activate rulechanges
        consensus.nMinerConfirmationWindow = 20;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1707828286;                  // Feb 13, 2024
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1707828286 + 31536000;         // Start + 1 year

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1707828286;               // Feb 13, 2024
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1707828286 + 31536000;      // Start + 1 year

        // Cascoin: Hive: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].bit = 7;
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].nStartTime = 1707828286;                 // Feb 13, 2024
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE].nTimeout = 1707828286 + 31536000;        // Start + 1 year

        // Cascoin: Hive 1.1: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].nStartTime = 1707828695;             // Feb 13, 2024 (but later than the above 3)
        consensus.vDeployments[Consensus::DEPLOYMENT_HIVE_1_1].nTimeout = 1707828695 + 31536000;    // Start + 1 year

        // Cascoin: MinotaurX+Hive1.2: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].bit = 7;
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].nStartTime = 1707829366;            // Feb 13, 2024 (but later than the above 4!)
        consensus.vDeployments[Consensus::DEPLOYMENT_MINOTAURX].nTimeout = 1707829366 + 31536000;   // Start + 1 year

        // Cascoin: Rialto: Deployment
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].nStartTime = 1707923363;               // Feb 14, 2024
        consensus.vDeployments[Consensus::DEPLOYMENT_RIALTO].nTimeout = 1707923363 + 31536000;      // Start + 1 year

        // Cascoin fields
        consensus.powForkTime = 1707828195;                 // Time of PoW hash method change (block 50)
        consensus.lastScryptBlock = 10;                     // Height of last scrypt block
        consensus.powLimitSHA = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");   // Initial hash target at fork
        consensus.slowStartBlocks = 40;                     // Scale post-fork block reward up over this many blocks
        consensus.premineAmount = 0;                   // Premine amount (less than 1% of issued currency at fork time)
        std::vector<unsigned char> vch = ParseHex("76a91424af51d38b740a6dc2868dfd70fc16d76901e1e088ac");
        consensus.premineOutputScript = CScript(vch.begin(), vch.end());	// Output script for premine block (tAGaQ7rk3NE7etu3bU6yAJF3KSKeDhTHry)
        consensus.totalMoneySupplyHeight = 6215968;         // Height at which TMS is reached, do not issue rewards past this point (Note, not accurate value for testnet)

        // Cascoin: Hive: Consensus Fields
        consensus.minBeeCost = 10000;                       // Minimum cost of a bee, used when no more block rewards
        consensus.beeCostFactor = 2500;                     // Bee cost is block_reward/beeCostFactor
        consensus.beeCreationAddress = "tEstNetCreateCASWorkerBeeXXXYq6T3r";        // Unspendable address for bee creation
        consensus.hiveCommunityAddress = "tCY5JWV4LYe64ivrAE2rD6P3bYxYtcoTsz";      // Community fund address
        consensus.communityContribFactor = 10;              // Optionally, donate bct_value/maxCommunityContribFactor to community fund
        consensus.beeGestationBlocks = 40;                  // The number of blocks for a new bee to mature
        consensus.beeLifespanBlocks = 48*24*14;             // The number of blocks a bee lives for after maturation
        consensus.powLimitHive = uint256S("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");  // Highest (easiest) bee hash target
        consensus.minHiveCheckBlock = 50;                   // Don't bother checking below this height for Hive blocks (not used for consensus/validation checks, just efficiency when looking for potential BCTs)
        consensus.hiveTargetAdjustAggression = 30;          // Snap speed for bee hash target adjustment EMA
        consensus.hiveBlockSpacingTarget = 2;               // Target Hive block frequency (1 out of this many blocks should be Hivemined)
        consensus.hiveBlockSpacingTargetTypical = 3;        // Observed Hive block frequency (1 out of this many blocks are observed to be Hive)
        consensus.hiveBlockSpacingTargetTypical_1_1 = 2;    // Observed Hive block frequency in Hive 1.1 (1 out of this many blocks are observed to be Hive)
        consensus.hiveNonceMarker = 192;                    // Nonce marker for hivemined blocks

        // Cascoin: Hive 1.1-related consensus fields
        consensus.minK = 2;                                 // Minimum chainwork scale for Hive blocks (see Hive whitepaper section 5)
        consensus.maxK = 10;                                // Maximum chainwork scale for Hive blocks (see Hive whitepaper section 5)
        consensus.maxHiveDiff = 0.002;                      // Hive difficulty at which max chainwork bonus is awarded
        consensus.maxKPow = 5;                              // Maximum chainwork scale for PoW blocks
        consensus.powSplit1 = 0.001;                        // Below this Hive difficulty threshold, PoW block chainwork bonus is halved
        consensus.powSplit2 = 0.0005;                       // Below this Hive difficulty threshold, PoW block chainwork bonus is halved again
        consensus.maxConsecutiveHiveBlocks = 2;             // Maximum hive blocks that can occur consecutively before a PoW block is required
        consensus.hiveDifficultyWindow = 36;                // How many blocks the SMA averages over in hive difficulty adjust

        // Cascoin: MinotaurX+Hive1.2-related consensus fields
        consensus.lwmaAveragingWindow = 90;                 // Averaging window size for LWMA diff adjust
        consensus.powTypeLimits.emplace_back(uint256S("0x000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));   // sha256d limit
        consensus.powTypeLimits.emplace_back(uint256S("0x000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));   // MinotaurX limit

        // Cascoin: Rialto-related consensus fields
        consensus.nickCreationAddress = "tKJjaPcSS3nXYBN4QmmYnSanr9oUhSXAZB";        // Nick creation address
        consensus.nickCreationCost3Char     = 100000000000; // Minimum costs to register a nick
        consensus.nickCreationCost4Char     = 5000000000;
        consensus.nickCreationCostStandard  = 100000000;
        consensus.nickCreationAntiDust      = 10000;        // Portion of creation cost burnt in 2nd output
        //consensus.firstRialtoBlock = uint256S("0xb602f3f5093626bea32c5b9cf499de562e7a364dbe55ee3d34211e7f15502a7e");   // Block 500: First block to consider for Rialto registrations (only required if launching without a UASF)

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000000058c519899a");  // Block 412

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x56d2eddb8cff67769e5c01eb30baa4897cc90c6c00f579a890adc8adfd608614"); // Block 412

        pchMessageStart[0] = 0xca;
        pchMessageStart[1] = 0x5c;
        pchMessageStart[2] = 0x01;
        pchMessageStart[3] = 0xcf;
        nDefaultPort = 22223;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1744615453, 587529, 0x1e0ffff0, 1, 50 * COIN * COIN_SCALE);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000af9cfc0bc5e04afbd67a6e908f0b4be45f347ad055fb0874f282a4bf7ee"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ba58814fafc987fae4478c855a706fe513650d0ffda7d0138a4fc0df270e72b"));

        vFixedSeeds.clear();
        vSeeds.emplace_back("testseed.cascoin.net");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 40);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 8);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 50);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 188);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tcas";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            {
                {0, uint256S("0x00000b5096d31f88b91d4589ce5c10e40771c506e763242ff4e12c0595bc0f0a")}
            }
        };

        chainTxData = ChainTxData{  // As at block 412
            1707835909,
            415,
            0.001
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 1; // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 1; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // Cascoin fields
        consensus.powForkTime = 1543765622;                 // Time of PoW hash method change (block 100)
        consensus.lastScryptBlock = 200;                    // Height of last scrypt block
        consensus.powLimitSHA = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");   // Initial hash target at fork
        consensus.slowStartBlocks = 40;                     // Scale post-fork block reward up over this many blocks
        consensus.premineAmount = 000000;                   // Premine amount (less than 1% of issued currency at fork time)
        std::vector<unsigned char> vch = ParseHex("76a91424af51d38b740a6dc2868dfd70fc16d76901e1e088ac");
        consensus.premineOutputScript = CScript(vch.begin(), vch.end());	// Output script for premine block (tAGaQ7rk3NE7etu3bU6yAJF3KSKeDhTHry)
        consensus.totalMoneySupplyHeight = 6215968;         // Height at which TMS is reached, do not issue rewards past this point (Note, not accurate value for testnet)
        consensus.hiveNonceMarker = 192;                    // Nonce marker for hivemined blocks

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xca;
        pchMessageStart[1] = 0x5c;
        pchMessageStart[2] = 0x01;
        pchMessageStart[3] = 0xcf;
        nDefaultPort = 22224;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1744615453, 0, 0x1e0ffff0, 1, 50 * COIN * COIN_SCALE);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x1386cda6706b15d776b4439500766906f15176c63f5033272be2604658fdc118"));
        assert(genesis.hashMerkleRoot == uint256S("0x2ba58814fafc987fae4478c855a706fe513650d0ffda7d0138a4fc0df270e72b"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {
            {
                {0, uint256S("0xc72b13e3a223fdb212be39837f9b997f367eac7b076af3a5ce883db067fcf0d8")}
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 40);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 8);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1, 50);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 188);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "rcas";
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}
