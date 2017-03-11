#include "Imdb.h"

using namespace db;

void Imdb::insert(const OrderPtr & order)
{
	//insert into both maps
	//1.
	keyMap[order->front_session_ref] = order;
	//2.
	pkgMap.insert(std::make_pair(order->package_leg_id, order));
}

Imdb::OrderPtr Imdb::get(const OrderId & orderId)
{
	if (keyMap.count(orderId)) {
		return keyMap[orderId];
	}
	return OrderPtr();
}

std::vector<Imdb::OrderPtr> Imdb::get(const PkgId & pkgId)
{
	auto range = pkgMap.equal_range(pkgId);
	std::vector<Imdb::OrderPtr> result;
	for (auto iter = range.first; iter != range.second; iter++) {
		result.push_back(iter->second);
	}
	return result;
}

void Imdb::remove(const OrderId & orderId)
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
