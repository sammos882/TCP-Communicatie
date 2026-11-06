#include <iostream>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <atomic>
#include "log.hpp"
#include "socket.hpp"
#include <string>
// Instellingen voor de verbinding met de server.
#define PORT 8080
#define BUF_SIZE 1024
#define IP_ADDRESS "127.0.0.1"  // Het loopback-adres verwijst naar deze computer.

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    std::cout << "TCP Client!\n";
    std::cout << __DATE__ << " " << __TIME__ << std::endl;
    LogTargetOperatingSystem();
    LogTargetCompiler();

#ifdef _WIN32
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    // socket aanmaken
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Cannot create socket\n";
        throw std::runtime_error("Cannot create socket");
        return 1;
    }
    // socket adress instellen
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    int result = inet_pton(AF_INET, IP_ADDRESS, &addr.sin_addr);
    if (result <= 0) {
        std::cerr << "Invalid address\n";
        throw std::runtime_error("Invalid address");
        return 1;
    }
    // connecten met de server
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("connect failed");
        std::cerr << "connect failed\n";
        return 1;
    }

    // Tijdstip (in microseconden) waarop het laatste bericht is verstuurd.
    std::atomic<long long> sendTimeUs{ 0 };

    // Deze thread blijft ontvangen terwijl de hoofdthread invoer leest en verstuurt.
    std::thread receiverThread([sock, &sendTimeUs]() {
        char buffer[BUF_SIZE];
        int bytes = 0;

        while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            const auto receiveTime = std::chrono::steady_clock::now();
            buffer[bytes] = '\0';
            std::string message(buffer);

            std::cout << "\nserver: " << message << '\n';

            // Is dit de confirmatie van de server? Dan meten we de delay.
            if (message.rfind("Ontvangen bericht op server", 0) == 0) {
                const long long nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
                    receiveTime.time_since_epoch()).count();
                const double delayMs = (nowUs - sendTimeUs.load()) / 1000.0;
                std::cout << "Delay (heen en terug): " << delayMs << " ms\n";
            }
            if (message.rfind("Ontvangen bericht op server", 0) == 0) {
                const long long nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
                    receiveTime.time_since_epoch()).count();
                const double delayMs = (nowUs - sendTimeUs.load()) / 1000.0;
                std::cout << "Delay (heen en terug): " << delayMs << " ms\n";
            }
            else {
                // Gewoon bericht van de server: stuur een confirmatie terug.
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
    int messageId = 0;
    while (true) {
        std::cout << "Bericht (typ stop om te stoppen):\n";
        if (!std::getline(std::cin, outgoingMessage) || outgoingMessage == "stop") {
            break;
        }
        // Geef het bericht een uniek ID en laat het zien voor het versturen.
        messageId++;
        outgoingMessage = "[C-" + std::to_string(messageId) + "] " + outgoingMessage;
        std::cout << "Verstuur: " << outgoingMessage << '\n';
        // Start de klok vlak voor het versturen.
        sendTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

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