// Copyright (c) 2014-2019 The Dash Core developers
// Copyright (c) 2020-2022 The Raptoreum developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SMARTNODE_PAYMENTS_H
#define SMARTNODE_PAYMENTS_H

#include <util.h>
#include <core_io.h>
#include <key.h>
#include <net_processing.h>
#include <utilstrencodings.h>

#include <evo/deterministicmns.h>

class CReesistornodePayments;

/// TODO: all 4 functions do not belong here really, they should be refactored/moved somewhere (main.cpp ?)
bool IsBlockValueValid(const CBlock& block, int nBlockHeight, CAmount blockReward, std::string& strErrorRet);
bool IsBlockPayeeValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward, CAmount specialTxFees);
void FillBlockPayments(CMutableTransaction& txNew, int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutReesistornodePaymentsRet, std::vector<CTxOut>& voutSuperblockPaymentsRet, CAmount specialTxFees = 0);
std::map<int, std::string> GetRequiredPaymentsStrings(int nStartHeight, int nEndHeight);

extern CReesistornodePayments mnpayments;

//
// Reesistornode Payments Class
// Keeps track of who should get paid for which blocks
//

class CReesistornodePayments
{
public:
    static bool GetBlockTxOuts(int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutReesistornodePaymentsRet, CAmount specialTxFee);
    static bool IsTransactionValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward, CAmount specialTxFee);

    static bool GetReesistornodeTxOuts(int nBlockHeight, CAmount blockReward, std::vector<CTxOut>& voutReesistornodePaymentsRet, CAmount specialTxFee);
};

#endif
