#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

void recv_loop(int sock) {
    char buf[1024];
    sockaddr_in src{};
    socklen_t srclen = sizeof(src);

    while (true) {
        ssize_t len = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                               (sockaddr*)&src, &srclen);
        if (len > 0) {
            buf[len] = 0;
            std::cout << "\n[" << inet_ntoa(src.sin_addr) << ":" 
                      << ntohs(src.sin_port) << "] " << buf << "\n> " << std::flush;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << "  Host:   " << argv[0] << " host <port>\n"
                  << "  Client: " << argv[0] << " client <host_ip> <host_port>\n";
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;

    if (std::string(argv[1]) == "host") {
        int port = std::stoi(argv[2]);
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            return 1;
        }

        std::cout << "Hosting on port " << port << " ...\n";
        std::thread recv_thread(recv_loop, sock);
        recv_thread.detach();

        std::cout << "Waiting for client to connect...\n";

        sockaddr_in client{};
        socklen_t clen = sizeof(client);
        char buf[64];
        ssize_t len = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &clen);
        if (len < 0) { perror("recvfrom"); return 1; }

        std::cout << "Client connected from "
                  << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\n";

        std::string msg;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, msg);
            sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr*)&client, sizeof(client));
        }

    } else if (std::string(argv[1]) == "client") {
        if (argc < 4) {
            std::cerr << "Usage: client <host_ip> <host_port>\n";
            return 1;
        }

        std::string host_ip = argv[2];
        int port = std::stoi(argv[3]);

        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(0); // random port
        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            return 1;
        }

        sockaddr_in host{};
        host.sin_family = AF_INET;
        inet_pton(AF_INET, host_ip.c_str(), &host.sin_addr);
        host.sin_port = htons(port);

        std::string hello = "HELLO";
        sendto(sock, hello.c_str(), hello.size(), 0, (sockaddr*)&host, sizeof(host));
        std::cout << "Sent hello to " << host_ip << ":" << port << "\n";

        std::thread recv_thread(recv_loop, sock);
        recv_thread.detach();

        std::string msg;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, msg);
            sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr*)&host, sizeof(host));
        }
    } else {
        std::cerr << "Unknown mode: " << argv[1] << "\n";
        return 1;
    }

    close(sock);
    return 0;
}

