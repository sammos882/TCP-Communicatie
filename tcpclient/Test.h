#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include "socket.hpp"

struct TestState {
	std::atomic<long long> sendTimeUs{ 0 };
	std::atomic<double> lastDelayMs{ 0.0 };
	std::atomic<bool> testRunning{ false };
	std::mutex confirmationMutex;
	std::condition_variable confirmationCv;
	bool gotConfirmation = false;
};

void runTest(SOCKET sock, TestState& state);
