// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2020-2022 The Raptoreum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <reesistornode/activereesistornode.h>
#include <evo/deterministicmns.h>
#include <reesistornode/reesistornode-sync.h>
#include <netbase.h>
#include <protocol.h>
#include <validation.h>
#include <warnings.h>

// Keep track of the active Reesistornode
CActiveReesistornodeInfo activeReesistornodeInfo;
CActiveReesistornodeManager* activeReesistornodeManager;

std::string CActiveReesistornodeManager::GetStateString() const
{
    switch (state) {
    case SMARTNODE_WAITING_FOR_PROTX:
        return "WAITING_FOR_PROTX";
    case SMARTNODE_POSE_BANNED:
        return "POSE_BANNED";
    case SMARTNODE_REMOVED:
        return "REMOVED";
    case SMARTNODE_OPERATOR_KEY_CHANGED:
        return "OPERATOR_KEY_CHANGED";
    case SMARTNODE_PROTX_IP_CHANGED:
        return "PROTX_IP_CHANGED";
    case SMARTNODE_READY:
        return "READY";
    case SMARTNODE_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

std::string CActiveReesistornodeManager::GetStatus() const
{
    switch (state) {
    case SMARTNODE_WAITING_FOR_PROTX:
        return "Waiting for ProTx to appear on-chain";
    case SMARTNODE_POSE_BANNED:
        return "Reesistornode was PoSe banned";
    case SMARTNODE_REMOVED:
        return "Reesistornode removed from list";
    case SMARTNODE_OPERATOR_KEY_CHANGED:
        return "Operator key changed or revoked";
    case SMARTNODE_PROTX_IP_CHANGED:
        return "IP address specified in ProTx changed";
    case SMARTNODE_READY:
        return "Ready";
    case SMARTNODE_ERROR:
        return "Error. " + strError;
    default:
        return "Unknown";
    }
}

void CActiveReesistornodeManager::Init(const CBlockIndex* pindex)
{
    LOCK(cs_main);

    if (!fReesistornodeMode) return;

    if (!deterministicMNManager->IsDIP3Enforced(pindex->nHeight)) return;

    // Check that our local network configuration is correct
    if (!fListen && Params().RequireRoutableExternalIP()) {
        // listen option is probably overwritten by something else, no good
        state = SMARTNODE_ERROR;
        strError = "Reesistornode must accept connections from outside. Make sure listen configuration option is not overwritten by some another parameter.";
        LogPrintf("CActiveReesistornodeManager::Init -- ERROR: %s\n", strError);
        return;
    }

    if (!GetLocalAddress(activeReesistornodeInfo.service)) {
        state = SMARTNODE_ERROR;
        return;
    }

    CDeterministicMNList mnList = deterministicMNManager->GetListForBlock(pindex);

    CDeterministicMNCPtr dmn = mnList.GetMNByOperatorKey(*activeReesistornodeInfo.blsPubKeyOperator);
    if (!dmn) {
        // MN not appeared on the chain yet
        return;
    }

    if (!mnList.IsMNValid(dmn->proTxHash)) {
        if (mnList.IsMNPoSeBanned(dmn->proTxHash)) {
            state = SMARTNODE_POSE_BANNED;
        } else {
            state = SMARTNODE_REMOVED;
        }
        return;
    }

    LogPrintf("CActiveReesistornodeManager::Init -- proTxHash=%s, proTx=%s\n", dmn->proTxHash.ToString(), dmn->ToString());

    if (activeReesistornodeInfo.service != dmn->pdmnState->addr) {
        state = SMARTNODE_ERROR;
        strError = "Local address does not match the address from ProTx";
        LogPrintf("CActiveReesistornodeManager::Init -- ERROR: %s\n", strError);
        return;
    }

    // Check socket connectivity
    LogPrintf("CActiveReesistornodeManager::Init -- Checking inbound connection to '%s'\n", activeReesistornodeInfo.service.ToString());
    SOCKET hSocket = CreateSocket(activeReesistornodeInfo.service);
    if (hSocket == INVALID_SOCKET) {
        state = SMARTNODE_ERROR;
        strError = "Could not create socket to connect to " + activeReesistornodeInfo.service.ToString();
        LogPrintf("CActiveReesistornodeManager::Init -- ERROR: %s\n", strError);
        return;
    }
    bool fConnected = ConnectSocketDirectly(activeReesistornodeInfo.service, hSocket, nConnectTimeout, true) && IsSelectableSocket(hSocket);
    CloseSocket(hSocket);

    if (!fConnected && Params().RequireRoutableExternalIP()) {
        state = SMARTNODE_ERROR;
        strError = "Could not connect to " + activeReesistornodeInfo.service.ToString();
        LogPrintf("CActiveReesistornodeManager::Init -- ERROR: %s\n", strError);
        return;
    }

    activeReesistornodeInfo.proTxHash = dmn->proTxHash;
    activeReesistornodeInfo.outpoint = dmn->collateralOutpoint;
    state = SMARTNODE_READY;
}

void CActiveReesistornodeManager::UpdatedBlockTip(const CBlockIndex* pindexNew, const CBlockIndex* pindexFork, bool fInitialDownload)
{
    LOCK(cs_main);

    if (!fReesistornodeMode) return;

    if (!deterministicMNManager->IsDIP3Enforced(pindexNew->nHeight)) return;

    if (state == SMARTNODE_READY) {
        auto oldMNList = deterministicMNManager->GetListForBlock(pindexNew->pprev);
        auto newMNList = deterministicMNManager->GetListForBlock(pindexNew);
        if (!newMNList.IsMNValid(activeReesistornodeInfo.proTxHash)) {
            // MN disappeared from MN list
            state = SMARTNODE_REMOVED;
            activeReesistornodeInfo.proTxHash = uint256();
            activeReesistornodeInfo.outpoint.SetNull();
            // MN might have reappeared in same block with a new ProTx
            Init(pindexNew);
            return;
        }

        auto oldDmn = oldMNList.GetMN(activeReesistornodeInfo.proTxHash);
        auto newDmn = newMNList.GetMN(activeReesistornodeInfo.proTxHash);
        if (newDmn->pdmnState->pubKeyOperator != oldDmn->pdmnState->pubKeyOperator) {
            // MN operator key changed or revoked
            state = SMARTNODE_OPERATOR_KEY_CHANGED;
            activeReesistornodeInfo.proTxHash = uint256();
            activeReesistornodeInfo.outpoint.SetNull();
            // MN might have reappeared in same block with a new ProTx
            Init(pindexNew);
            return;
        }

        if (newDmn->pdmnState->addr != oldDmn->pdmnState->addr) {
            // MN IP changed
            state = SMARTNODE_PROTX_IP_CHANGED;
            activeReesistornodeInfo.proTxHash = uint256();
            activeReesistornodeInfo.outpoint.SetNull();
            Init(pindexNew);
            return;
        }
    } else {
        // MN might have (re)appeared with a new ProTx or we've found some peers
        // and figured out our local address
        Init(pindexNew);
    }
}

bool CActiveReesistornodeManager::GetLocalAddress(CService& addrRet)
{
    // First try to find whatever our own local address is known internally.
    // Addresses could be specified via externalip or bind option, discovered via UPnP
    // or added by TorController. Use some random dummy IPv4 peer to prefer the one
    // reachable via IPv4.
    CNetAddr addrDummyPeer;
    bool fFoundLocal{false};
    if (LookupHost("8.8.8.8", addrDummyPeer, false)) {
        fFoundLocal = GetLocal(addrRet, &addrDummyPeer) && IsValidNetAddr(addrRet);
    }
    if (!fFoundLocal && !Params().RequireRoutableExternalIP()) {
        if (Lookup("127.0.0.1", addrRet, GetListenPort(), false)) {
            fFoundLocal = true;
        }
    }
    if (!fFoundLocal) {
        bool empty = true;
        // If we have some peers, let's try to find our local address from one of them
        g_connman->ForEachNodeContinueIf(CConnman::AllNodes, [&fFoundLocal, &empty](CNode* pnode) {
            empty = false;
            if (pnode->addr.IsIPv4())
                fFoundLocal = GetLocal(activeReesistornodeInfo.service, &pnode->addr) && IsValidNetAddr(activeReesistornodeInfo.service);
            return !fFoundLocal;
        });
        // nothing and no live connections, can't do anything for now
        if (empty) {
            strError = "Can't detect valid external address. Please consider using the externalip configuration option if problem persists. Make sure to use IPv4 address only.";
            LogPrintf("CActiveReesistornodeManager::GetLocalAddress -- ERROR: %s\n", strError);
            return false;
        }
    }
    return true;
}

bool CActiveReesistornodeManager::IsValidNetAddr(CService addrIn)
{
    // TODO: regtest is fine with any addresses for now,
    // should probably be a bit smarter if one day we start to implement tests for this
    return !Params().RequireRoutableExternalIP() ||
           (addrIn.IsIPv4() && IsReachable(addrIn) && addrIn.IsRoutable());
}
