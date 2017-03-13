#include <iostream>
#include <chrono>

using namespace std;

/*
vs上，输出的精度是1ns
linux 上也是
*/


int main()
{
	std::cout.precision(17);
	std::cout << fixed << static_cast<double>(std::chrono::high_resolution_clock::period::num)
		/ std::chrono::high_resolution_clock::period::den << "(s)" << std::endl;
	std::chrono::duration<double, std::nano> nn = std::chrono::high_resolution_clock::duration(1);
	std::cout << nn.count() << "(ns)" << std::endl;
}