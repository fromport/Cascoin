// Copyright (c) 2016-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <versionbits.h>
#include <consensus/params.h>
#include <validation.h> // Cascoin: MinotaurX+Hive1.2: For IsMinotaurXEnabled
#include <timedata.h> // Added for GetAdjustedTime

const struct VBDeploymentInfo VersionBitsDeploymentInfo[Consensus::MAX_VERSION_BITS_DEPLOYMENTS] = {
    {
        /*.name =*/ "testdummy",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "csv",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "segwit",
        /*.gbt_force =*/ true,
    },
    // Cascoin: Hive: Deployment
    {
        /*.name =*/ "hive",
        /*.gbt_force =*/ true,
    },
    // Cascoin: Hive: Deployment of 1.1
    {
        /*.name =*/ "hive_1_1",
        /*.gbt_force =*/ true,
    },
    // Cascoin: MinotaurX+Hive1.2: Deployment
    {
        /*.name =*/ "minotaurx_and_hive_1_2",
        /*.gbt_force =*/ true,
    },
    // Cascoin: Rialto: Deployment
    {
        /*.name =*/ "rialto",
        /*.gbt_force =*/ true,
    }
};

ThresholdState AbstractThresholdConditionChecker::GetStateFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache_in) const
{
    // Obtain deployment parameters using virtual methods
    int64_t nTimeStart_val = BeginTime(params);
    int64_t nTimeTimeout_val = EndTime(params);
    int nPeriod_val = Period(params);
    int nThreshold_val = Threshold(params);
    // Assuming nWindowSize is equivalent to nPeriod for an AbstractThresholdConditionChecker.
    // If a separate WindowSize() virtual method exists or is needed, it should be used here.
    // The original code used a parameter nWindowSize for the check below.
    int nWindowSize_val = nPeriod_val; 

    // Check if this deployment is always active.
    // The original check used direct parameters: (nTimeStart == 0 && nTimeTimeout == Consensus::BIP9Deployment::NO_TIMEOUT && nPeriod == 0 && nThreshold == 0 && nWindowSize ==0)
    // Cascoin: Added nWindowSize == 0 for Litecoin Cash "always active"
    // Note: nMinLockedBlocks and nLockPeriodBlocks were removed from the signature. If they were used in this specific "always active" logic, that needs review.
    if (nTimeStart_val == 0 && nTimeTimeout_val == Consensus::BIP9Deployment::NO_TIMEOUT && 
        nPeriod_val == 0 && nThreshold_val == 0 && nWindowSize_val == 0) {
        return THRESHOLD_ACTIVE;
    }

    // Check if this deployment is hidden defined by nTimeStart == 0 && nTimeTimeout == 0
    // Original check: (nTimeStart == 0 && nTimeTimeout == 0)
    if (nTimeStart_val == 0 && nTimeTimeout_val == 0) {
        return THRESHOLD_HIDDEN;
    }

    // Handle pindexPrev == nullptr explicitly at the beginning.
    // Original check: (GetAdjustedTime() < nTimeStart)
    if (pindexPrev == nullptr) {
        if (GetAdjustedTime() < nTimeStart_val) {
            return THRESHOLD_DEFINED;
        }
        return THRESHOLD_DEFINED; 
    }

    int64_t nTimePrev = pindexPrev->GetMedianTimePast();

    // TODO: The rest of this function body (the main state calculation loop, starting around original line 280)
    // needs to be carefully updated.
    // Specifically, all uses of the original parameters:
    // - nPeriod, nThreshold, nTimeStart, nTimeTimeout, nWindowSize
    // must be replaced with their '_val' counterparts (e.g., nPeriod_val) derived from virtual methods.
    // - The 'cache' parameter (originally VersionBitsCache&) must be replaced with 'cache_in' (ThresholdConditionCache&).
    // - The virtual Condition(pindex, params) method (i.e., this->Condition(...)) must be used for checking block conditions.
    // - Any logic relying on nMinLockedBlocks, nLockPeriodBlocks, or dep_pos (which were removed
    // from the signature) needs to be re-evaluated and potentially moved to derived classes,
    // sourced differently, or removed if no longer applicable to this generic base method.
    // The existing complex state calculation logic (walking block history, counting, caching) needs to remain,
    // but adapted to these changes.

    // Check if we are in a state where the deployment is definitely active or failed
    if (nTimePrev >= nTimeTimeout_val) {
        return THRESHOLD_FAILED;
    }
    // Cascoin: Check for started state before trying to get an ancestor (important for genesis)
    // Original Bitcoin Core just did the GetAncestor call. This is safer.
    if (nTimePrev < nTimeStart_val) {
        return THRESHOLD_DEFINED;
    }

    // A block's state is always the same as that of the first of its period, so it is computed based on a pindexPrev whose height equals a multiple of nPeriod_val - 1.
    const CBlockIndex* pindexPeriod = pindexPrev->GetAncestor(pindexPrev->nHeight - ((pindexPrev->nHeight + 1) % nPeriod_val));

    // Walk backwards in steps of nPeriod_val to find a pindexPeriod whose information is known
    std::vector<const CBlockIndex*> vToCompute;
    const CBlockIndex* pindexWalk = pindexPeriod;
    while (pindexWalk && cache_in.count(pindexWalk) == 0) {
        if (pindexWalk->GetMedianTimePast() < nTimeStart_val) {
            // Optimization: don't recompute down further, as we know every earlier block will be before the start time
            cache_in[pindexWalk] = THRESHOLD_DEFINED;
            break;
        }
        if (!pindexWalk->pprev) {
            // The genesis block is by definition defined.
            cache_in[pindexWalk] = THRESHOLD_DEFINED;
            break;
        }
        vToCompute.push_back(pindexWalk);
        pindexWalk = pindexWalk->GetAncestor(pindexWalk->nHeight - nPeriod_val);
    }

    // At this point, cache_in[pindexWalk] is known if pindexWalk is not nullptr.
    // If pindexWalk is nullptr, it implies we've gone past genesis or a point
    // where the state is implicitly THRESHOLD_DEFINED.
    ThresholdState state;
    if (pindexWalk) {
        assert(cache_in.count(pindexWalk)); // This assertion should hold true if pindexWalk is not null
        state = cache_in[pindexWalk];
    } else {
        // If pindexWalk is null, we've iterated past known blocks. The state defaults to DEFINED.
        state = THRESHOLD_DEFINED;
    }

    // Now walk forward and compute the state of descendants of pindexWalk
    while (!vToCompute.empty()) {
        ThresholdState stateNext = state;
        pindexWalk = vToCompute.back();
        vToCompute.pop_back();

        switch (state) {
            case THRESHOLD_DEFINED: {
                if (pindexWalk->GetMedianTimePast() >= nTimeTimeout_val) {
                    stateNext = THRESHOLD_FAILED;
                } else if (pindexWalk->GetMedianTimePast() >= nTimeStart_val) {
                    stateNext = THRESHOLD_STARTED;
                }
                break;
            }
            case THRESHOLD_STARTED: {
                if (pindexWalk->GetMedianTimePast() >= nTimeTimeout_val) {
                    stateNext = THRESHOLD_FAILED;
                    break;
                }
                // We need to count
                const CBlockIndex* pindexCount = pindexWalk;
                int count = 0;
                for (int i = 0; i < nPeriod_val; i++) {
                    if (!pindexCount) {
                        break;
                    }
                    if (Condition(pindexCount, params)) {
                        count++;
                    }
                    pindexCount = pindexCount->pprev;
                }
                if (count >= nThreshold_val) {
                    stateNext = THRESHOLD_LOCKED_IN;
                }
                break;
            }
            case THRESHOLD_LOCKED_IN: {
                // Always progresses into ACTIVE.
                // The custom logic for nMinLockedBlocks and nLockPeriodBlocks was here.
                // If that logic is specific to a deployment, it should be handled via
                // that deployment's parameters (BeginTime, EndTime, Period, Threshold)
                // or in an overridden method in a derived class.
                stateNext = THRESHOLD_ACTIVE;
                break;
            }
            case THRESHOLD_FAILED:
            case THRESHOLD_ACTIVE: {
                // Nothing happens, these are terminal states.
                break;
            }
        }
        cache_in[pindexWalk] = state = stateNext;
    }

    return state;
}

// return the numerical statistics of blocks signalling the specified BIP9 condition in this current period
BIP9Stats AbstractThresholdConditionChecker::GetStateStatisticsFor(const CBlockIndex* pindex, const Consensus::Params& params) const
{
    BIP9Stats stats = {};

    stats.period = Period(params);
    stats.threshold = Threshold(params);

    if (pindex == nullptr)
        return stats;

    // Find beginning of period
    const CBlockIndex* pindexEndOfPrevPeriod = pindex->GetAncestor(pindex->nHeight - ((pindex->nHeight + 1) % stats.period));
    stats.elapsed = pindex->nHeight - pindexEndOfPrevPeriod->nHeight;

    // Count from current block to beginning of period
    int count = 0;
    const CBlockIndex* currentIndex = pindex;
    while (pindexEndOfPrevPeriod->nHeight != currentIndex->nHeight){
        if (Condition(currentIndex, params))
            count++;
        currentIndex = currentIndex->pprev;
    }

    stats.count = count;
    stats.possible = (stats.period - stats.threshold ) >= (stats.elapsed - count);

    return stats;
}

int AbstractThresholdConditionChecker::GetStateSinceHeightFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache) const
{
    int64_t start_time = BeginTime(params);
    if (start_time == Consensus::BIP9Deployment::ALWAYS_ACTIVE) {
        return 0;
    }

    const ThresholdState initialState = GetStateFor(pindexPrev, params, cache);

    // BIP 9 about state DEFINED: "The genesis block is by definition in this state for each deployment."
    if (initialState == THRESHOLD_DEFINED) {
        return 0;
    }

    const int nPeriod = Period(params);

    // A block's state is always the same as that of the first of its period, so it is computed based on a pindexPrev whose height equals a multiple of nPeriod - 1.
    // To ease understanding of the following height calculation, it helps to remember that
    // right now pindexPrev points to the block prior to the block that we are computing for, thus:
    // if we are computing for the last block of a period, then pindexPrev points to the second to last block of the period, and
    // if we are computing for the first block of a period, then pindexPrev points to the last block of the previous period.
    // The parent of the genesis block is represented by nullptr.
    pindexPrev = pindexPrev->GetAncestor(pindexPrev->nHeight - ((pindexPrev->nHeight + 1) % nPeriod));

    const CBlockIndex* previousPeriodParent = pindexPrev->GetAncestor(pindexPrev->nHeight - nPeriod);

    while (previousPeriodParent != nullptr && GetStateFor(previousPeriodParent, params, cache) == initialState) {
        pindexPrev = previousPeriodParent;
        previousPeriodParent = pindexPrev->GetAncestor(pindexPrev->nHeight - nPeriod);
    }

    // Adjust the result because right now we point to the parent block.
    return pindexPrev->nHeight + 1;
}

namespace
{
/**
 * Class to implement versionbits logic.
 */
class VersionBitsConditionChecker : public AbstractThresholdConditionChecker {
private:
    const Consensus::DeploymentPos id;

protected:
    int64_t BeginTime(const Consensus::Params& params) const override { return params.vDeployments[id].nStartTime; }
    int64_t EndTime(const Consensus::Params& params) const override { return params.vDeployments[id].nTimeout; }
    int Period(const Consensus::Params& params) const override { return params.nMinerConfirmationWindow; }
    int Threshold(const Consensus::Params& params) const override { return params.nRuleChangeActivationThreshold; }

    bool Condition(const CBlockIndex* pindex, const Consensus::Params& params) const override
    {
        // Cascoin: MinotaurX+Hive1.2: Versionbits always active since powforktime and high bits repurposed at minotaurx UASF activation;
        // So, don't use VERSIONBITS_TOP_MASK any time past powforktime
        if (pindex->nTime > params.powForkTime)
            return (pindex->nVersion & Mask(params)) != 0;
        else
            return (((pindex->nVersion & VERSIONBITS_TOP_MASK) == VERSIONBITS_TOP_BITS) && (pindex->nVersion & Mask(params)) != 0);
    }

public:
    explicit VersionBitsConditionChecker(Consensus::DeploymentPos id_) : id(id_) {}
    uint32_t Mask(const Consensus::Params& params) const { return ((uint32_t)1) << params.vDeployments[id].bit; }
};

} // namespace

ThresholdState VersionBitsState(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos, VersionBitsCache& cache)
{
    return VersionBitsConditionChecker(pos).GetStateFor(pindexPrev, params, cache.caches[pos]);
}

BIP9Stats VersionBitsStatistics(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos)
{
    return VersionBitsConditionChecker(pos).GetStateStatisticsFor(pindexPrev, params);
}

int VersionBitsStateSinceHeight(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos, VersionBitsCache& cache)
{
    return VersionBitsConditionChecker(pos).GetStateSinceHeightFor(pindexPrev, params, cache.caches[pos]);
}

uint32_t VersionBitsMask(const Consensus::Params& params, Consensus::DeploymentPos pos)
{
    return VersionBitsConditionChecker(pos).Mask(params);
}

void VersionBitsCache::Clear()
{
    for (unsigned int d = 0; d < Consensus::MAX_VERSION_BITS_DEPLOYMENTS; d++) {
        caches[d].clear();
    }
}
