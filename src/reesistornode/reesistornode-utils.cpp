// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2020-2022 The Raptoreum developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <reesistornode/reesistornode-utils.h>

#ifdef ENABLE_WALLET
#include <coinjoin/coinjoin-client.h>
#endif
#include <init.h>
#include <reesistornode/reesistornode-sync.h>
#include <validation.h>

struct CompareScoreMN
{
    bool operator()(const std::pair<arith_uint256, const CDeterministicMNCPtr&>& t1,
                    const std::pair<arith_uint256, const CDeterministicMNCPtr&>& t2) const
    {
        return (t1.first != t2.first) ? (t1.first < t2.first) : (t1.second->collateralOutpoint < t2.second->collateralOutpoint);
    }
};

void CReesistornodeUtils::ProcessReesistornodeConnections(CConnman& connman)
{
    std::vector<CDeterministicMNCPtr> vecDmns; // will be empty when no wallet
#ifdef ENABLE_WALLET
    for(const auto& pair : coinJoinClientManagers) {
        pair.second->GetMixingReesistornodesInfo(vecDmns);
    }
#endif // ENABLE_WALLET

    // Don't disconnect reesistornode connections when we have less then the desired amount of outbound nodes
    int nonReesistornodeCount = 0;
    connman.ForEachNode(CConnman::AllNodes, [&](CNode* pnode) {
        if (!pnode->fInbound && !pnode->fFeeler && !pnode->m_manual_connection && !pnode->m_reesistornode_connection && !pnode->m_reesistornode_probe_connection) {
            nonReesistornodeCount++;
        }
    });
    if (nonReesistornodeCount < connman.GetMaxOutboundNodeCount()) {
        return;
    }

    connman.ForEachNode(CConnman::AllNodes, [&](CNode* pnode) {
        // we're only disconnecting m_reesistornode_connection connections
        if (!pnode->m_reesistornode_connection) return;
        // we're only disconnecting outbound connections
        if (pnode->fInbound) return;
        // we're not disconnecting LLMQ connections
        if (connman.IsReesistornodeQuorumNode(pnode)) return;
        // we're not disconnecting reesistornode probes for at least a few seconds
        if (pnode->m_reesistornode_probe_connection && GetSystemTimeInSeconds() - pnode->nTimeConnected < 5) return;

#ifdef ENABLE_WALLET
        bool fFound = false;
        for (const auto& dmn : vecDmns) {
            if (pnode->addr == dmn->pdmnState->addr) {
                fFound = true;
                break;
            }
        }
        if (fFound) return; // do NOT disconnect mixing reesistornodes
#endif // ENABLE_WALLET
        if (fLogIPs) {
            LogPrintf("Closing Reesistornode connection: peer=%d, addr=%s\n", pnode->GetId(), pnode->addr.ToString());
        } else {
            LogPrintf("Closing Reesistornode connection: peer=%d\n", pnode->GetId());
        }
        pnode->fDisconnect = true;
    });
}

void CReesistornodeUtils::DoMaintenance(CConnman& connman)
{
    if(!reesistornodeSync.IsBlockchainSynced() || ShutdownRequested())
        return;

    static unsigned int nTick = 0;

    nTick++;

    if(nTick % 60 == 0) {
        ProcessReesistornodeConnections(connman);
    }
}
