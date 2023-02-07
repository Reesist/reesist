/*
 * Copyright (c) 2020-2022 The Raptoreum developers
 * Distributed under the MIT/X11 software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 *      Author: tri
 */

#include <reesistornode/reesistornode-collaterals.h>
#include <limits.h>
#include <iostream>

ReesistornodeCollaterals::ReesistornodeCollaterals(vector<Collateral> collaterals, vector<RewardPercentage> rewardPercentages) {
	this->collaterals = collaterals;
	this->rewardPercentages = rewardPercentages;
	for (auto& it : this->collaterals) {
		collateralsHeightMap.insert(make_pair(it.amount, it.height));
	}

}

CAmount ReesistornodeCollaterals::getCollateral(int height) const {
	for (auto& it : this->collaterals) {
		if(it.height == INT_MAX || height <= it.height) {
			return it.amount;
		}
	}
	return 0;
}

int ReesistornodeCollaterals::getRewardPercentage(int height) const {
	for (auto& it : this->rewardPercentages) {
		if(it.height == INT_MAX || height <= it.height) {
			return it.percentage;
		}
	}
	return 0;
}

ReesistornodeCollaterals::~ReesistornodeCollaterals() {
	this->collaterals.clear();
}

bool ReesistornodeCollaterals::isValidCollateral(CAmount collateralAmount) const {
	auto it = collateralsHeightMap.find(collateralAmount);
	return it != collateralsHeightMap.end();
}

bool ReesistornodeCollaterals::isPayableCollateral(int height, CAmount collateralAmount) const {
	if(!this->isValidCollateral(collateralAmount)) {
		return false;
	}
	int collateralEndHeight = this->collateralsHeightMap.at(collateralAmount);
	return collateralEndHeight == INT_MAX || height <= collateralEndHeight;
}

void ReesistornodeCollaterals::printCollateral() const {
	{
	    for (auto const &pair: collateralsHeightMap) {
	        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
	    }

	}
}
