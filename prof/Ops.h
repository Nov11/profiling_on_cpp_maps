#pragma once
#include<vector>
#include<tuple>
#include <memory>
#include "Order.h"

namespace db {
	class Ops {
	public:
		using OrderId = std::tuple<int, int, int>;
		using PkgId = std::pair<std::string, int>;
		using OrderPtr = std::shared_ptr<ns::Order>;

		virtual void insert(const OrderPtr& order) = 0;
		virtual OrderPtr get(const OrderId& orderId) = 0;
		virtual std::vector<OrderPtr> get(const PkgId& pkgId) = 0;
		virtual void remove(const OrderId& orderId) = 0;
	};
}