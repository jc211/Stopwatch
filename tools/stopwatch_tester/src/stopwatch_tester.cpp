#include <iostream>
#include <thread>
#include <chrono>
#include "Stopwatch.h"

int main(int argc, char* argv) {
	using namespace std::chrono_literals;

	Stopwatch::getInstance().setCustomSignature(32434);
	std::cout << "Stopwatch Tester" << std::endl;

	std::thread t([]() {
		while (true) {
			TICK("High Frequency");
			std::this_thread::sleep_for(10ms);
			TOCK("High Frequency");
		}
	
	});
	while (true) {
		TICK("Low Frequency");
		std::cout << "Tick" << std::endl;
		std::this_thread::sleep_for(1000ms);
		TOCK("Low Frequency");
		Stopwatch::getInstance().sendAll();
	}
	t.join();

	return EXIT_SUCCESS;
}