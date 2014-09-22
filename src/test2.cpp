#include <iostream>
#include <chrono>
#include <vector>

int main(int argc, char const *argv[])
{
	auto start = std::chrono::system_clock::now();
	
	std::vector<int> v(1000000, 42);

	auto end = std::chrono::system_clock::now();

	auto elapsed = end - start;

	std::cout << (end < start) << std::endl;

	return 0;
}