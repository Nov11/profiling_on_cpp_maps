#include <assert.h>
#include "AVL.h"

namespace db {
	AVL::AVL()
	{
	}

	AVL::~AVL()
	{
	}
	void AVL::insert(const OrderPtr & order)
	{
		keyMap[order->front_session_ref] = order;
		pkgMap.insert(std::make_pair(order->package_leg_id, order));
	}
	AVL::OrderPtr AVL::get(const OrderId & orderId)
	{
		if (keyMap.count(orderId)) {
			return keyMap[orderId];
		}
		return OrderPtr();
	}
	std::vector<AVL::OrderPtr> AVL::get(const PkgId & pkgId)
	{
		auto range = pkgMap.equal_range(pkgId);
		std::vector<OrderPtr> result;
		for (auto iter = range.first; iter != range.second; iter++) {
			result.push_back(iter->second);
		}
		return result;
	}
	void AVL::remove(const OrderId & orderId)
	{
		//remove from both containers
		//1.
		if (!keyMap.count(orderId)) {
			return;
		}
		OrderPtr removeThisOrder = keyMap[orderId];
		keyMap.erase(orderId);

		//2.remove from pkg container
		auto range = pkgMap.equal_range(removeThisOrder->package_leg_id);
		auto iter = range.first;
		for (; iter != range.second; iter++) {
			assert(ns::equalPkgId(iter->first, removeThisOrder->package_leg_id));
			OrderPtr tmp = iter->second;
			if (ns::equalOrderId(tmp->front_session_ref, removeThisOrder->front_session_ref)) {
				break;
			}
		}
		if (iter != range.second) {
			//remove from pkgMap
			pkgMap.erase(iter);
		}
	}
}