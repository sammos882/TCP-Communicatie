#include "Test.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace {
constexpr int testCount = 100;

long long nowUs() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count();
}
}

void runTest(SOCKET sock, TestState& state) {
	state.testRunning = true;
	int received = 0;
	double minMs = 1e9;
	double maxMs = 0.0;
	double sumMs = 0.0;

	for (int i = 1; i <= testCount; ++i) {
		const std::string message = "testbericht " + std::to_string(i) + '\n';
		{
			std::lock_guard lock(state.confirmationMutex);
			state.gotConfirmation = false;
		}
		state.sendTimeUs = nowUs();

		if (send(sock, message.c_str(), static_cast<int>(message.size()), 0) == SOCKET_ERROR) {
			std::cerr << "Fout bij versturen naar de server.\n";
			break;
		}

		std::unique_lock lock(state.confirmationMutex);
		if (!state.confirmationCv.wait_for(lock, std::chrono::seconds(5), [&state] {
			return state.gotConfirmation;
		})) {
			std::cout << "Bericht " << i << ": geen confirmatie ontvangen\n";
			break;
		}

		const double delayMs = state.lastDelayMs.load();
		std::cout << "Bericht " << i << ": " << std::fixed << std::setprecision(3)
			<< delayMs << " ms\n";
		++received;
		sumMs += delayMs;
		if (delayMs < minMs) minMs = delayMs;
		if (delayMs > maxMs) maxMs = delayMs;
	}

	state.testRunning = false;
	std::cout << "\n--- Resultaat ---\n"
		<< "Confirmaties ontvangen: " << received << " van " << testCount << '\n';
	if (received > 0) {
		std::cout << "Minimum:    " << minMs << " ms\n"
			<< "Gemiddeld:  " << sumMs / received << " ms\n"
			<< "Maximum:    " << maxMs << " ms\n";
	}
}
