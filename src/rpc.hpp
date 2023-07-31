//
// Created by skeje on 7/17/2023.
//

#ifndef HPP_CATEDU_RPC
#define HPP_CATEDU_RPC

#include <string>
#include <unordered_map>
#include "enet/enet.h"

struct RpcClient {
    uint32_t id;

    inline bool is_server() {
        return id == 0;
    }
};

struct Rpc;


typedef void (*RpcCallback)(Rpc *rpc, RpcClient sender, void *userdata, size_t data_size, const void *data);


struct RpcCallbackContext {
    void *userdata;
    RpcCallback callback;
};


struct PeerMap {
    std::unordered_map<uint32_t, ENetPeer*> peers;
};

struct Rpc {
    /**
     * Called if just connected before any serving operations.
     */
    bool just_connected;

    /**
     * Map between peer ID's and ENet Peers.
     */
    PeerMap map;

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
     * @brief      Sends a packet to a specific client.
     *
     * @param[in]  fn_name    The function name to call.
     * @param[in]  data_size  Size of data in bytes.
     * @param[in]  data       Function parameter data.
     * @param[in]  poser      Allows server to pose as some client, for example
     *                        to inform everyone about disconnection. Parameter
     *                        ignored on clients.
     *
     * @return     True if function is registered and called.
     */
    [[nodiscard]] bool broadcast(const char *fn_name, const size_t data_size, const void *data, uint32_t poser = 0);

    /**
     * @brief      Sends a packet to a specific client.
     *
     * @param[in]  client     The recepient client.
     * @param[in]  fn_name    The function name to call.
     * @param[in]  data_size  Size of data in bytes.
     * @param[in]  data       Function parameter data.
     * @param[in]  poser      Allows server to pose as some client, for example
     *                        to inform everyone about disconnection. Parameter
     *                        ignored on clients.
     *
     * @return     True if function is registered and called.
     */
    [[nodiscard]] bool send(RpcClient client, const char* fn_name, const size_t data_size, const void* data, uint32_t poser = 0);

    /**
     * @brief      Disconnects from the server, or stops hosting.
     */
    void disconnect();

    bool is_server();

    uint32_t own_id();
};

#endif //HPP_CATEDU_RPC
