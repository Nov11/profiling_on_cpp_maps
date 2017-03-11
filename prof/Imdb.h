#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <assert.h>
#include "Order.h"
#include "Ops.h"
#include "Noncopyable.h"
namespace db {
	class Imdb : Noncopyable, public Ops {
	public:
		void insert(const OrderPtr& order) override;
		OrderPtr get(const OrderId& orderId) override;
		std::vector<OrderPtr> get(const PkgId& pkgId) override;
		void remove(const OrderId& orderId) override;
	private:
		//init bucket size is arbitarily chosen
		using KM = std::unordered_map<OrderId, OrderPtr, decltype(&ns::hashOrderId), decltype(&ns::equalOrderId)>;
		KM keyMap{ 1000000, &ns::hashOrderId, &ns::equalOrderId };
		using PM = std::unordered_multimap<PkgId, OrderPtr, decltype(&ns::hashPkgId), decltype(&ns::equalPkgId)>;
		PM pkgMap{ 1000000, &ns::hashPkgId, &ns::equalPkgId };
		//possibly need a mutex for concurrent access, just leave it here.
	};
}
