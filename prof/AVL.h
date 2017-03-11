#pragma once
#include <memory>
#include <vector>
#include <map>
#include "Order.h"
#include "Ops.h"
#include "Noncopyable.h"
namespace db {
	class AVL :Noncopyable,public Ops
	{
	public:
		AVL();
		~AVL();
		void insert(const OrderPtr& order) override;
		OrderPtr get(const OrderId& orderId) override;
		std::vector<OrderPtr> get(const PkgId& pkgId) override;
		void remove(const OrderId& orderId)override;
	private:
		using KM = std::map<OrderId, OrderPtr, decltype(&ns::lessThenOrderId)>;
		KM keyMap{ &ns::lessThenOrderId };
		using PM = std::multimap<PkgId, OrderPtr, decltype(&ns::lessThanPkgId)>;
		PM pkgMap{ &ns::lessThanPkgId };
		//possibly need a mutex for concurrent access, just leave it here.
	};
}


