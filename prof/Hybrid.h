//
// Created by c6s on 17-3-11.
//

#ifndef ORDER_HYBRID_H
#define ORDER_HYBRID_H
#include <unordered_map>
#include <map>
#include "Noncopyable.h"
#include "Ops.h"

namespace db{
    class Hybrid :Noncopyable , public Ops{
    public:
        void insert(const OrderPtr& order) override;
        OrderPtr get(const OrderId& orderId) override;
        std::vector<OrderPtr> get(const PkgId& pkgId) override;
        void remove(const OrderId& orderId) override;
    private:
        using KM = std::unordered_map<OrderId, OrderPtr, decltype(&ns::hashOrderId), decltype(&ns::equalOrderId)>;
        KM keyMap{ 1000000, &ns::hashOrderId, &ns::equalOrderId };
        using PM = std::multimap<PkgId, OrderPtr, decltype(&ns::lessThanPkgId)>;
        PM pkgMap{ &ns::lessThanPkgId };
    };
}



#endif //ORDER_HYBRID_H
