#include <assert.h>
#include "Order.h"

namespace ns {
	size_t hashOrderId(const OrderId& oid) {
		return std::hash<int>()(std::get<0>(oid)) 
			^ std::hash<int>()(std::get<1>(oid))
			^ std::hash<int>()(std::get<2>(oid));
	}
	bool equalOrderId(const OrderId& o1, const OrderId& o2) {
		return o1 == o2;
	}
	bool lessThenOrderId(const OrderId & o1, const OrderId & o2)
	{
		//tuple 0 1 2  
		//most significant -> least significant
		long long v1 = std::get<0>(o1);
		v1 <<= 32;
		v1|= std::get<1>(o1);
		long long v2 = std::get<0>(o2);
		v2 <<= 32;
		v2 |= std::get<1>(o2);
		if (v1 < v2) { return true; }
		else if (v1 > v2) { return false; }
		return std::get<2>(o1) < std::get<2>(o2);
	}
	size_t hashPkgId(const PkgId& id) {
		return std::hash<std::string>()(id.first) ^ std::hash<int>()(id.second);
	}
	bool equalPkgId(const PkgId& p1, const PkgId& p2) {
		return (p1.first.compare(p2.first) == 0)
			&& (p2.second == p2.second);
	}
	bool lessThanPkgId(const PkgId & p1, const PkgId & p2)
	{
		int ret = p1.first.compare(p2.first);
		if (ret == 0) {
			return p1.second < p2.second;
		}
		return ret < 0;
	}
}