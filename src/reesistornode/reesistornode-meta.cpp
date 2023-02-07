// Copyright (c) 2014-2019 The Dash Core developers
// Copyright (c) 2020-2022 The Raptoreum developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <reesistornode/reesistornode-meta.h>

#include <timedata.h>

CReesistornodeMetaMan mmetaman;

const std::string CReesistornodeMetaMan::SERIALIZATION_VERSION_STRING = "CReesistornodeMetaMan-Version-2";

UniValue CReesistornodeMetaInfo::ToJson() const
{
    UniValue ret(UniValue::VOBJ);

    auto now = GetAdjustedTime();

    ret.push_back(Pair("lastDSQ", nLastDsq));
    ret.push_back(Pair("mixingTxCount", nMixingTxCount));
    ret.push_back(Pair("lastOutboundAttempt", lastOutboundAttempt));
    ret.push_back(Pair("lastOutboundAttemptElapsed", now - lastOutboundAttempt));
    ret.push_back(Pair("lastOutboundSuccess", lastOutboundSuccess));
    ret.push_back(Pair("lastOutboundSuccessElapsed", now - lastOutboundSuccess));

    return ret;
}

void CReesistornodeMetaInfo::AddGovernanceVote(const uint256& nGovernanceObjectHash)
{
    LOCK(cs);
    // Insert a zero value, or not. Then increment the value regardless. This
    // ensures the value is in the map.
    const auto& pair = mapGovernanceObjectsVotedOn.emplace(nGovernanceObjectHash, 0);
    pair.first->second++;
}

void CReesistornodeMetaInfo::RemoveGovernanceObject(const uint256& nGovernanceObjectHash)
{
    LOCK(cs);
    // Whether or not the govobj hash exists in the map first is irrelevant.
    mapGovernanceObjectsVotedOn.erase(nGovernanceObjectHash);
}

CReesistornodeMetaInfoPtr CReesistornodeMetaMan::GetMetaInfo(const uint256& proTxHash, bool fCreate)
{
    LOCK(cs);
    auto it = metaInfos.find(proTxHash);
    if (it != metaInfos.end()) {
        return it->second;
    }
    if (!fCreate) {
        return nullptr;
    }
    it = metaInfos.emplace(proTxHash, std::make_shared<CReesistornodeMetaInfo>(proTxHash)).first;
    return it->second;
}

// We keep track of dsq (mixing queues) count to avoid using same reesistornodes for mixing too often.
// This threshold is calculated as the last dsq count this specific reesistornode was used in a mixing
// session plus a margin of 20% of reesistornode count. In other words we expect at least 20% of unique
// reesistornodes before we ever see a reesistornode that we know already mixed someone's funds ealier.
int64_t CReesistornodeMetaMan::GetDsqThreshold(const uint256& proTxHash, int nMnCount)
{
    LOCK(cs);
    auto metaInfo = GetMetaInfo(proTxHash);
    if (metaInfo == nullptr) {
        // return a threshold which is slightly above nDsqCount i.e. a no-go
        return nDsqCount + 1;
    }
    return metaInfo->GetLastDsq() + nMnCount / 5;
}

void CReesistornodeMetaMan::AllowMixing(const uint256& proTxHash)
{
    LOCK(cs);
    auto mm = GetMetaInfo(proTxHash);
    nDsqCount++;
    LOCK(mm->cs);
    mm->nLastDsq = nDsqCount;
    mm->nMixingTxCount = 0;
}

void CReesistornodeMetaMan::DisallowMixing(const uint256& proTxHash)
{
    LOCK(cs);
    auto mm = GetMetaInfo(proTxHash);

    LOCK(mm->cs);
    mm->nMixingTxCount++;
}

bool CReesistornodeMetaMan::AddGovernanceVote(const uint256& proTxHash, const uint256& nGovernanceObjectHash)
{
    LOCK(cs);
    auto mm = GetMetaInfo(proTxHash);
    mm->AddGovernanceVote(nGovernanceObjectHash);
    return true;
}

void CReesistornodeMetaMan::RemoveGovernanceObject(const uint256& nGovernanceObjectHash)
{
    LOCK(cs);
    for(auto& p : metaInfos) {
        p.second->RemoveGovernanceObject(nGovernanceObjectHash);
    }
}

std::vector<uint256> CReesistornodeMetaMan::GetAndClearDirtyGovernanceObjectHashes()
{
    LOCK(cs);
    std::vector<uint256> vecTmp = std::move(vecDirtyGovernanceObjectHashes);
    vecDirtyGovernanceObjectHashes.clear();
    return vecTmp;
}

void CReesistornodeMetaMan::Clear()
{
    LOCK(cs);
    metaInfos.clear();
    vecDirtyGovernanceObjectHashes.clear();
}

void CReesistornodeMetaMan::CheckAndRemove()
{

}

std::string CReesistornodeMetaMan::ToString() const
{
    std::ostringstream info;

    info << "Reesistornodes: meta infos object count: " << (int)metaInfos.size() <<
         ", nDsqCount: " << (int)nDsqCount;
    return info.str();
}
