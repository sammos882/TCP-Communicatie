#include <iostream>
#include <cstring>
#include <stdexcept>
#include <thread>
#include "log.hpp"
#include "socket.hpp"
#include <string>
// Instellingen voor de verbinding met de server.
#define PORT 8080
#define BUF_SIZE 1024
#define IP_ADDRESS "127.0.0.1"  // Het loopback-adres verwijst naar deze computer.

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // Toon informatie over de client en de build.
    std::cout << "TCP Client!\n";
    std::cout << __DATE__ << " " << __TIME__ << std::endl; // log date and time of compilation, not runtime
    LogTargetOperatingSystem();
    LogTargetCompiler();

#ifdef _WIN32
    // Initialiseer Winsock voordat sockets op Windows worden gebruikt.
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // Maak een IPv4-socket die TCP gebruikt.
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Cannot create socket\n";
        throw std::runtime_error("Cannot create socket");
        return 1;
    }

    // Stel het IPv4-adres en de poort van de server in.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT); // Zet de poort om naar netwerk-bytevolgorde.
    int result = inet_pton(AF_INET, IP_ADDRESS, &addr.sin_addr);
    if (result <= 0) {
        std::cerr << "Invalid address\n";
        throw std::runtime_error("Invalid address");
        return 1;
    }

    // Maak verbinding met de server op het ingestelde adres en de ingestelde poort.
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("connect failed");
        std::cerr << "connect failed\n"; 
        return 1;
    }
    // Deze thread blijft ontvangen terwijl de hoofdthread invoer leest en verstuurt.
    std::thread receiverThread([sock]() {
        char buffer[BUF_SIZE];
        int bytes = 0;

        while ((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            std::cout << "\nOntvangen van server: ";
            std::cout.write(buffer, bytes);
            std::cout << '\n';
        }

        if (bytes == SOCKET_ERROR) {
            std::cerr << "Fout bij ontvangen van de server.\n";
        }
    });

    // De hoofdthread leest berichten van de gebruiker en verstuurt ze.
    std::string outgoingMessage;
    while (true) {
        std::cout << "Bericht (typ stop om te stoppen):\n";
        if (!std::getline(std::cin, outgoingMessage) || outgoingMessage == "stop") {
            break;
        }

        if (send(sock, outgoingMessage.c_str(), static_cast<int>(outgoingMessage.size()), 0) == SOCKET_ERROR) {
            std::cerr << "Fout bij versturen naar de server.\n";
            break;
        }
    }

    // Stop beide socketrichtingen zodat recv() in de receiver-thread wordt vrijgegeven.
#ifdef _WIN32
    (void)shutdown(sock, SD_BOTH);
#else
    (void)shutdown(sock, SHUT_RDWR);
#endif
    receiverThread.join();

    // Sluit de socket nadat de communicatie klaar is.
    closeSocket(sock);
    std::cout << "Socket gesloten.\n";

#ifdef _WIN32
    // Ruim Winsock op wanneer het programma op Windows draait.
    (void)WSACleanup();
#endif

    // Geef aan dat het programma succesvol is afgerond.
    return 0;
}
