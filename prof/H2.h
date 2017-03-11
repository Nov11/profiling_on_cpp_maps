//
// Created by c6s on 17-3-11.
//

#ifndef ORDER_H2_H
#define ORDER_H2_H
#include <map>
#include <unordered_map>
#include "Order.h"
#include "Noncopyable.h"
#include "Ops.h"

namespace db{
    class H2 :Noncopyable , public Ops{
    public:
        void insert(const OrderPtr& order) override;
        OrderPtr get(const OrderId& orderId) override;
        std::vector<OrderPtr> get(const PkgId& pkgId) override;
        void remove(const OrderId& orderId) override;
    private:
        using KM = std::map<OrderId, OrderPtr, decltype(&ns::lessThenOrderId)>;
        KM keyMap{ &ns::lessThenOrderId };
        using PM = std::unordered_multimap<PkgId, OrderPtr, decltype(&ns::hashPkgId), decltype(&ns::equalPkgId)>;
        PM pkgMap{ 1000000, &ns::hashPkgId, &ns::equalPkgId };
    };

}


#endif //ORDER_H2_H
