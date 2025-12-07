#include <rtc/rtc.hpp>
#include <iostream>

using namespace rtc;

int main() {
    Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    auto pc = std::make_shared<PeerConnection>(config);

    auto dc = pc->createDataChannel("chat");

    dc->onOpen([]() { std::cout << "Data channel open!\n"; });
    dc->onMessage([](auto data) {
        if (std::holds_alternative<std::string>(data))
            std::cout << "Peer: " << std::get<std::string>(data) << std::endl;
    });

    pc->onLocalDescription([](const Description& desc) {
        std::cout << "Send this SDP to your peer via signaling server:\n";
        std::cout << desc << "\n";
    });

    pc->setLocalDescription();

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        dc->send(line);
    }
}

