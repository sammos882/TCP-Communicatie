#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include "log.hpp"
#include "socket.hpp"
// Instellingen voor de verbinding met de server.
#define PORT 8080
#define BUF_SIZE 1024
#define IP_ADDRESS "127.0.0.1"  // Het loopback-adres verwijst naar deze computer.
#define TEST_COUNT 100          // Aantal berichten bij het commando "test".

// Huidige tijd in microseconden.
static long long nowUs() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    std::cout << "TCP Client!\n";
    std::cout << __DATE__ << " " << __TIME__ << std::endl;
    //LogTargetOperatingSystem();
    //LogTargetCompiler();

#ifdef _WIN32
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Cannot create socket\n";
        throw std::runtime_error("Cannot create socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    int result = inet_pton(AF_INET, IP_ADDRESS, &addr.sin_addr);
    if (result <= 0) {
        std::cerr << "Invalid address\n";
        throw std::runtime_error("Invalid address");
        return 1;
    }

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("connect failed");
        std::cerr << "connect failed\n";
        return 1;
    }

    std::atomic<long long> sendTimeUs{ 0 };      // Tijdstip waarop het laatste bericht is verstuurd.
    std::atomic<double> lastDelayMs{ 0.0 };      // Delay van de laatste confirmatie.
    std::atomic<bool> gotConfirmation{ false };  // Wordt true zodra de confirmatie binnen is.
    std::atomic<bool> testRunning{ false };      // Tijdens de test houden we de uitvoer rustig.

    // Deze thread blijft ontvangen terwijl de hoofdthread invoer leest en verstuurt.
    std::thread receiverThread([sock, &sendTimeUs, &lastDelayMs, &gotConfirmation, &testRunning]() {
        char buffer[BUF_SIZE];
        int bytes = 0;

        while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            const long long receiveUs = nowUs();
            buffer[bytes] = '\0';
            std::string message(buffer);

            if (message.rfind("Ontvangen bericht op server", 0) == 0) {
                // Confirmatie van de server: delay meten.
                lastDelayMs = (receiveUs - sendTimeUs.load()) / 1000.0;
                gotConfirmation = true;
                if (!testRunning) {
                    std::cout << "\nserver: " << message << '\n';
                    std::cout << "Delay (heen en terug): " << lastDelayMs.load() << " ms\n";
                }
            }
            else {
                // Gewoon bericht van de server: stuur een confirmatie terug.
                std::cout << "\nserver: " << message << '\n';
                std::string confirmation = "Ontvangen bericht op client: " + message;
                send(sock, confirmation.c_str(), static_cast<int>(confirmation.size()), 0);
            }
        }

        if (bytes == SOCKET_ERROR) {
            std::cerr << "Fout bij ontvangen van de server.\n";
        }
        });

    // De hoofdthread leest berichten van de gebruiker en verstuurt ze.
    std::string outgoingMessage;
    while (true) {
        std::cout << "Bericht (typ test voor " << TEST_COUNT << " berichten, stop om te stoppen):\n";
        if (!std::getline(std::cin, outgoingMessage) || outgoingMessage == "stop") {
            break;
        }

        // Commando "test": automatisch TEST_COUNT berichten versturen.
        if (outgoingMessage == "test") {
            testRunning = true;
            int received = 0;
            double minMs = 1e9, maxMs = 0.0, sumMs = 0.0;

            for (int i = 1; i <= TEST_COUNT; i++) {
                const std::string msg = "testbericht " + std::to_string(i);

                gotConfirmation = false;
                sendTimeUs = nowUs();   // Start de klok vlak voor het versturen.
                if (send(sock, msg.c_str(), static_cast<int>(msg.size()), 0) == SOCKET_ERROR) {
                    std::cerr << "Fout bij versturen naar de server.\n";
                    break;
                }

                // Wacht op de confirmatie (maximaal 5 seconden), dan pas het volgende bericht.
                const long long deadline = nowUs() + 5000000;
                while (!gotConfirmation && nowUs() < deadline) {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }

                if (!gotConfirmation) {
                    std::cout << "Bericht " << i << ": geen confirmatie ontvangen\n";
                    continue;
                }

                const double d = lastDelayMs.load();
                std::cout << "Bericht " << i << ": " << std::fixed << std::setprecision(3) << d << " ms\n";
                received++;
                sumMs += d;
                if (d < minMs) minMs = d;
                if (d > maxMs) maxMs = d;
            }

            testRunning = false;
            std::cout << "\n--- Resultaat ---\n";
            std::cout << "Confirmaties ontvangen: " << received << " van " << TEST_COUNT << '\n';
            if (received > 0) {
                std::cout << "Minimum:    " << minMs << " ms\n";
                std::cout << "Gemiddeld:  " << sumMs / received << " ms\n";
                std::cout << "Maximum:    " << maxMs << " ms\n";
            }
            continue;
        }

        // Normaal bericht.
        sendTimeUs = nowUs();   // Start de klok vlak voor het versturen.
        if (send(sock, outgoingMessage.c_str(), static_cast<int>(outgoingMessage.size()), 0) == SOCKET_ERROR) {
            std::cerr << "Fout bij versturen naar de server.\n";
            break;
        }
    }

#ifdef _WIN32
    (void)shutdown(sock, SD_BOTH);
#else
    (void)shutdown(sock, SHUT_RDWR);
#endif
    receiverThread.join();

    closeSocket(sock);
    std::cout << "Socket gesloten.\n";

#ifdef _WIN32
    (void)WSACleanup();
#endif

    return 0;
}