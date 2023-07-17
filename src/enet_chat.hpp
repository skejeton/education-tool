//
// Created by skeje on 7/17/2023.
//

#ifndef HPP_CATEDU_ENET_CHAT
#define HPP_CATEDU_ENET_CHAT

#include <enet/include/enet/enet.h>
#include <vector>
#include <string>

#define ENET_CHAT_PORT 6502

struct EnetChat {
    /**
     * Is still connected.
     */
    bool connected;

    /**
     * Host for the server and client.
     */
    ENetHost *enet_host;

    /**
     * Peer for the client.
     */
    ENetPeer *enet_peer;

    /**
     * @brief Chat log.
     */
    std::vector<std::string> log;

    /**
     * @param[out] out_chat instance as server hosted on the default port.
     * @return Whether the hosting was successful.
     */
    [[nodiscard]]
    static bool host(EnetChat *out_chat);

    /**
     * @param[out] out_chat Chat instance connected to address via the default port.
     * @param[in] address Address to connect to.
     * @return Whether the connection was successful.
     */
    [[nodiscard]]
    static bool connect(EnetChat *out_chat, ENetAddress address);

    /**
     * Services the server, must be called every tick.
     */
    void service();

    /**
     * Broadcasts a message to everyone.
     * @param message the message.
     */
    void broadcast(const char *raw_message, bool distribute = false);
};

#endif //HPP_CATEDU_ENET_CHAT
