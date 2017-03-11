//
// Created by c6s on 17-3-11.
//

#include "H2.h"
#include <assert.h>

void db::H2::insert(const db::Ops::OrderPtr &order) {
    keyMap[order->front_session_ref] = order;
    pkgMap.insert(std::make_pair(order->package_leg_id, order));
}

db::Ops::OrderPtr db::H2::get(const db::Ops::OrderId &orderId) {
    if (keyMap.count(orderId)) {
        return keyMap[orderId];
    }
    return OrderPtr();
}

std::vector<db::Ops::OrderPtr> db::H2::get(const db::Ops::PkgId &pkgId) {
    auto range = pkgMap.equal_range(pkgId);
    std::vector<OrderPtr> result;
    for (auto iter = range.first; iter != range.second; iter++) {
        result.push_back(iter->second);
    }
    return result;
}

void db::H2::remove(const db::Ops::OrderId &orderId) {
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
