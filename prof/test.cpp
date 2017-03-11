#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <set>
#include <thread>
#include <iterator>
#include "Order.h"
#include "Ops.h"
#include "Imdb.h"
#include "AVL.h"
#include "Hybrid.h"
#include "H2.h"

using namespace std;
using ns::Order;
using db::Imdb;
using db::AVL;
using db::Ops;
using db::Hybrid;
using db::H2;
using SYS = chrono::high_resolution_clock ;

void print(tuple<int, int, int>& param) {
	cout << std::get<0>(param) << ", " << std::get<1>(param) << " ," << std::get<2>(param) << endl;
}

decltype(SYS::now()) pt(decltype(SYS::now())& beg, const std::string& msg){
    auto end = SYS::now();
    cout << "test of " << msg << " :\t\t" << chrono::duration_cast<chrono::microseconds>(end - beg).count() << "us" <<endl;
    return SYS::now();
}
void test(Ops& db1, const vector<std::shared_ptr<Order>>& testCase, const std::string& msg) {
	cout << "start test of " << msg << endl;
	auto beg = SYS::now();
	for (auto& iter : testCase) {
		db1.insert(iter);
	}
    auto next = pt(beg, "insert");
	for (auto& iter : testCase) {
		auto ret = db1.get(iter->front_session_ref);
		assert(ret == iter);
	}
    next = pt(next, "get");
	for (auto& iter : testCase) {
		auto ret = db1.get(iter->package_leg_id);
		assert(ret.size() > 0);
#ifndef NDEBUG
		for (auto& inner : ret) {
			assert(ns::equalPkgId(iter->package_leg_id, inner->package_leg_id));
		}
#endif
	}
    next = pt(next, "range get");

	for (auto& iter : testCase) {
		db1.remove(iter->front_session_ref);
		assert(iter.unique());
	}
	pt(next, "remove");
	auto end = SYS::now();
	cout << "test of " << msg << " takes:\t" << chrono::duration_cast<chrono::microseconds>(end - beg).count() << "us" << endl;
}

void th(vector<std::shared_ptr<Order>> & list, int number, int idx) {
	for (int i = 0; i < number; i++) {
		shared_ptr<Order> ptr(new Order());
		std::get<2>(ptr->front_session_ref) = i;
		std::get<0>(ptr->front_session_ref) = idx;
		ptr->package_leg_id.first = to_string(i / 10);
		list.push_back(ptr);
	}
}

int main(int args, char* argv[]) {	
	if (args < 2) { cerr << "need to specify test data size" << endl; exit(1); }
	int sum = std::stoi(argv[1]);
	vector<std::shared_ptr<Order>> testCase;
	//for (int i = 0; i < sum; i++) {
	//	shared_ptr<Order> ptr(new Order());
	//	std::get<2>(ptr->front_session_ref) = i;
	//	ptr->package_leg_id.first = to_string(i / 10);
	//	testCase.push_back(ptr);
	//	cout << "prepared " << i << " data" << endl;
	//}

	vector < vector<std::shared_ptr<Order>>> v(8);

	vector<std::shared_ptr<thread> > vth(8);
	for (int i = 0; i < 8; i++) {
		vth[i] = std::make_shared<thread>(th, std::ref(v[i]), sum / 8, i);
	}
	if (sum % 8) {
		th(testCase, sum % 8, 8);
	}
	for (int i = 0; i < 8; i++) {
		vth[i]->join();
		copy(v[i].begin(), v[i].end(), std::back_inserter(testCase));
	}

	{
		decltype(v) blank;
		swap(blank, v);
	}
	cout << "data set of " << testCase.size() << " is ready." << endl;
	assert(testCase.size() == static_cast<unsigned >(sum));
	Imdb imdb;
	test(imdb, testCase, "hash");
	AVL avl;
	test(avl, testCase, "avl");
	Hybrid hybrid;
	test(hybrid, testCase, "hybrid");
	H2 h2;
	test(h2, testCase, "h2");
	//cout << sizeof(Order) << endl;
}