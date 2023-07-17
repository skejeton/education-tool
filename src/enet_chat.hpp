//
// Created by skeje on 7/17/2023.
//

#ifndef HPP_CATEDU_ENET_CHAT
#define HPP_CATEDU_ENET_CHAT

#include <vector>
#include <string>
#include "rpc.hpp"

#define ENET_CHAT_PORT 6502

struct EnetChat {
    /**
     * List of messages.
     */
    std::vector<std::string> messages;

    /**
     * Connect to an RPC system.
     * @param rpc RPC system.
     */
    void connect(Rpc *rpc);

    /**
     * Broadcast message on the RPC system.
     * @param rpc RPC system.
     * @param string Message.
     */
    void broadcast(Rpc *rpc, const char *string);
};

#endif //HPP_CATEDU_ENET_CHAT
