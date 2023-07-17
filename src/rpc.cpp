//
// Created by skeje on 7/17/2023.
//

#include <cstdio>
#include <cstdint>
#include "rpc.hpp"


#define DEFAULT_CLIENT_CAP 32
#define DEFAULT_CHAN_CAP   2

struct RpcCallPacket {
    const char *function_name;
    size_t data_size;
    const void *data;
};

struct RpcRawPacket {
    void *data;
    size_t size;
};

static RpcCallPacket read_rpc_call_packet(void *data)
{
    size_t function_name_len = strlen((const char*)data);
    size_t data_size = *(size_t*)(((uint8_t*)data)+function_name_len+1);
    void *data_value = (((uint8_t*)data)+function_name_len+1+sizeof(size_t));

    return {
        (const char *)data,
        data_size,
        data_value
    };
}

static RpcRawPacket write_rpc_call_packet(RpcCallPacket data)
{
    size_t function_name_len = strlen((const char*)data.function_name)+1;
    auto *raw_data = (uint8_t*)malloc(function_name_len+data.data_size+sizeof(size_t));

    memcpy(raw_data, data.function_name, function_name_len);
    memcpy(raw_data+function_name_len, &data.data_size, sizeof(size_t));
    memcpy(raw_data+function_name_len+sizeof(size_t), data.data, data.data_size);

    return {raw_data, function_name_len+data.data_size+sizeof(size_t) };
}

static void call_rpc_function(Rpc *rpc, const char *fn_name, size_t data_size, const void *data)
{
    RpcCallbackContext context = rpc->functions[std::string(fn_name)];

    context.callback(context.userdata, data_size, data);
}

static bool is_server(Rpc *rpc)
{
    return rpc->enet_peer == nullptr;
}

bool Rpc::host(Rpc *out_rpc, ENetAddress address)
{
    Rpc rpc = {};

    rpc.enet_host = enet_host_create(&address, DEFAULT_CLIENT_CAP, DEFAULT_CHAN_CAP, 0, 0);
    if (rpc.enet_host == nullptr) {
        return false;
    }

    rpc.connected = true;
    *out_rpc = rpc;

    return true;
}

bool Rpc::connect(Rpc *out_rpc, ENetAddress address)
{
    Rpc rpc = {};

    rpc.enet_host = enet_host_create(nullptr, 1, DEFAULT_CHAN_CAP, 0, 0);
    if (rpc.enet_host == nullptr) {
        return false;
    }

    rpc.enet_peer = enet_host_connect(rpc.enet_host, &address, DEFAULT_CHAN_CAP, 0);
    if (rpc.enet_peer == nullptr) {
        return false;
    }

    rpc.connected = true;
    *out_rpc = rpc;

    return true;
}

void Rpc::service()
{
    if (!this->connected) {
        return;
    }

    ENetEvent event;

    while (enet_host_service(this->enet_host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("Connect %d\n", is_server(this));
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Disconnect %d\n", is_server(this));
                if (!is_server(this)) {
                    this->connected = false;
                }
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                printf("Receive %d\n", is_server(this));
                {
                    RpcCallPacket packet = read_rpc_call_packet(event.packet->data);

                    if (is_server(this)) {
                        broadcast(packet.function_name, packet.data_size, packet.data);
                    } else {
                        call_rpc_function(this, packet.function_name, packet.data_size, packet.data);
                    }
                }
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void Rpc::broadcast(const char *fn_name, const size_t data_size, const void *data)
{
    RpcRawPacket raw_packet = write_rpc_call_packet({fn_name, data_size, data});

    ENetPacket *packet = enet_packet_create(raw_packet.data, raw_packet.size, ENET_PACKET_FLAG_RELIABLE);
    if (is_server(this)) {
        enet_host_broadcast(enet_host, 0, packet);
        call_rpc_function(this, fn_name, data_size, data);
    } else {
        enet_peer_send(enet_peer, 0, packet);
    }
}

void Rpc::register_function(const char *fn_name, void *userdata, RpcCallback callback) {
    this->functions[fn_name] = {userdata, callback};
}

