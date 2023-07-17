//
// Created by skeje on 7/17/2023.
//

#ifndef HPP_CATEDU_RPC
#define HPP_CATEDU_RPC

#include <string>
#include <unordered_map>
#include "enet/enet.h"

typedef void (*RpcCallback)(void *userdata, size_t data_size, const void *data);

struct RpcCallbackContext {
    void *userdata;
    RpcCallback callback;
};

struct Rpc {
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
     * Map of all function names to functions.
     */
    std::unordered_map<std::string, RpcCallbackContext> functions;

    /**
     * @param[out] out_rpc instance as server hosted on the default port.
     * @param[in] address Address to host.
     * @return Whether the hosting was successful.
     */
    [[nodiscard]]
    static bool host(Rpc *out_rpc, ENetAddress address);

    /**
     * @param[out] out_rpc Rpc instance connected to address via the default port.
     * @param[in] address Address to connect to.
     * @return Whether the connection was successful.
     */
    [[nodiscard]]
    static bool connect(Rpc *out_rpc, ENetAddress address);

    /**
     * Services the server and the client, must be called every tick.
     */
    void service();

    /**
     * Register a function in the RPC system.
     * @param fn_name Function name.
     * @param userdata Custom userdata/context for each call.
     * @param callback The callback to the RPC function.
     */
    void register_function(const char *fn_name, void *userdata, RpcCallback callback);

    /**
     * Broadcasts a call to everyone.
     * @param fn_name function name to call.
     */
    void broadcast(const char *fn_name, const size_t data_size, const void *data);
};

#endif //HPP_CATEDU_RPC
