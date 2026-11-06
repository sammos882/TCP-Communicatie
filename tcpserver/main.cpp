#include <iostream>
#include <chrono>
#include <cstring>
#include <string>
#include <atomic>
#include <thread>
#include "log.hpp"
#include "socket.hpp"

// Instellingen voor de server.
#define PORT 8080
#define BUF_SIZE 1024
constexpr int BACKLOG = 5; // Maximaal aantal verbindingen dat in de wachtrij mag staan.

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // Toon informatie over de server en het moment waarop deze is gecompileerd.
    std::cout << "TCP Server!\n";
    std::cout << __DATE__ << " " << __TIME__ << std::endl; // log date and time of compilation, not runtime
    LogTargetOperatingSystem();
    LogTargetCompiler();
#ifdef _WIN32
    // Initialiseer de Windows Sockets-bibliotheek op Windows.
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // Maak een TCP-socket aan voor IPv4.
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "Cannot create socket!\n";
        throw std::runtime_error("Cannot create socket");
        return 1;
    }

    // Stel het adres en de poort in waarop de server verbindingen accepteert.
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Koppel de socket aan het ingestelde adres en de poort.
    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closeSocket(serverSock);
        std::cerr << "Cannot bind socket\n";
        throw std::runtime_error("Cannot bind socket");
        return 1;
    }

    // Zet de socket klaar om inkomende TCP-verbindingen te ontvangen.
    std::cout << "TCP Server listening on port " << PORT << "...\n";

    if (listen(serverSock, BACKLOG) == SOCKET_ERROR) {
        closeSocket(serverSock);
        std::cerr << "listen error!\n";
        throw std::runtime_error("listen error!");
        return 1;
    }

    // Wacht op één client en maak daarvoor een aparte socket.
    SOCKET clientSock = accept(serverSock, nullptr, nullptr);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "accept error!\n";
        throw std::runtime_error("accept error!");
        return 1;
    }

    std::atomic<bool> running = true;

    // Tijdstip (in microseconden) waarop het laatste bericht naar de client is verstuurd.
    std::atomic<long long> sendTimeUs{ 0 };

    // Deze thread ontvangt berichten van de client.
    std::thread receiver([&]() {
        char buffer[BUF_SIZE];

        while (running) {
            int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
            const auto receiveTime = std::chrono::steady_clock::now();
            if (bytes <= 0) {
                std::cout << "De client heeft de verbinding gesloten.\n";
                running = false;
                break;
            }

            buffer[bytes] = '\0';
            buffer[strcspn(buffer, "\r\n")] = '\0';

            // Is dit de confirmatie van de client op een bericht van de server? Dan meten we de delay.
            if (std::string(buffer).rfind("Ontvangen bericht op client", 0) == 0) {
                const long long nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
                    receiveTime.time_since_epoch()).count();
                const double delayMs = (nowUs - sendTimeUs.load()) / 1000.0;
                std::cout << "\nBevestigd: " << buffer << "\nDelay (heen en terug): " << delayMs << " ms\n";
                continue; // een confirmatie bevestigen we niet opnieuw
            }

            std::cout << "Client: " << buffer << '\n';
            std::string comfromatiebericht = "Ontvangen bericht op server: " + std::string(buffer);
            if (send(clientSock, comfromatiebericht.c_str(), static_cast<int>(comfromatiebericht.size()), 0) == SOCKET_ERROR) {
                std::cerr << "send error!\n";
                running = false;
                break;
            }

            if (strcmp(buffer, "stop") == 0) {
                running = false;
                break;
            }
        }

#ifdef _WIN32
        shutdown(clientSock, SD_BOTH);
#else
        shutdown(clientSock, SHUT_RDWR);
#endif
        });

    // De hoofdthread leest berichten van het toetsenbord en verstuurt ze.
    std::string antwoord;
    int messageId = 0;
    while (running) {
        std::cout << "server: ";
        if (!std::getline(std::cin, antwoord) || !running) {
            running = false;
            break;
        }

        // Start de klok vlak voor het versturen.
        sendTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        // Geef het bericht een uniek ID en laat het zien voor het versturen.
        messageId++;
        std::string metId = "[S-" + std::to_string(messageId) + "] " + antwoord;
        std::cout << "Verstuur: " << metId << '\n';

        if (send(clientSock, metId.c_str(), static_cast<int>(metId.size()), 0) == SOCKET_ERROR) {
            std::cerr << "send error!\n";
            running = false;
            break;
        }

        if (antwoord == "stop") {
            running = false;
            break;
        }
    }

#ifdef _WIN32
    shutdown(clientSock, SD_BOTH);
#else
    shutdown(clientSock, SHUT_RDWR);
#endif
    receiver.join();

    // Sluit de verbinding met de client en daarna de server-socket.
    closeSocket(clientSock);
    closeSocket(serverSock);

#ifdef _WIN32
    // Ruim de Windows Sockets-bibliotheek op.
    (void)WSACleanup();
#endif

    // Geef aan dat het programma succesvol is afgerond.
    return 0;
}