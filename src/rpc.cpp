//
// Created by skeje on 7/17/2023.
//

#include <cstdio>
#include <cstdint>
#include "rpc.hpp"
#include <cassert>

#define DEFAULT_CLIENT_CAP 32
#define DEFAULT_CHAN_CAP   2


struct RpcCallPacket {
    const char *function_name;
    bool broadcast;
    RpcClient client;
    size_t data_size;
    const void *data;
};


struct RpcRawPacket {
    void *data;
    size_t size;
};


static RpcCallPacket read_rpc_call_packet(void *data)
{
    size_t offset = 0;
    size_t function_name_len = strlen((const char*)data);
    offset += function_name_len+1;

    bool broadcast = *(bool*)(((uint8_t*)data)+offset);
    offset += sizeof(bool);

    RpcClient client = *(RpcClient*)(((uint8_t*)data)+offset);
    offset += sizeof(RpcClient);

    size_t data_size = *(size_t*)(((uint8_t*)data)+offset);
    offset += sizeof(size_t);

    void *data_value = (((uint8_t*)data)+offset);

    return {
        (const char *)data,
        broadcast,
        client,
        data_size,
        data_value
    };
}


static RpcRawPacket write_rpc_call_packet(RpcCallPacket data)
{
    size_t function_name_len = strlen((const char*)data.function_name)+1;
    auto *raw_data = (uint8_t*)malloc(function_name_len+data.data_size+sizeof(size_t)+sizeof(bool)+sizeof(RpcClient));
    assert(raw_data);

    size_t offset = 0;

    // TODO: User BufferFormat
    memcpy(raw_data+offset, data.function_name, function_name_len);
    offset += function_name_len;

    memcpy(raw_data+offset, &data.broadcast, sizeof(bool));
    offset += sizeof(bool);

    memcpy(raw_data+offset, &data.client, sizeof(RpcClient));
    offset += sizeof(RpcClient);

    memcpy(raw_data+offset, &data.data_size, sizeof(size_t));
    offset += sizeof(size_t);

    memcpy(raw_data+offset, data.data, data.data_size);
    offset += data.data_size;

    return {raw_data, offset};
}


static bool is_function_registered(Rpc* rpc, const char* name)
{
    return rpc->functions.count(name) != 0;
}


// NOTE: Assumes function exists before call.
static void call_rpc_function(Rpc *rpc, RpcCallPacket packet)
{
    RpcCallbackContext context = rpc->functions[std::string(packet.function_name)];

    context.callback(rpc, packet.client, context.userdata, packet.data_size, packet.data);
}


bool Rpc::host(Rpc *out_rpc, ENetAddress address)
{
    Rpc rpc = {};

    rpc.enet_host = enet_host_create(&address, DEFAULT_CLIENT_CAP, DEFAULT_CHAN_CAP, 0, 0);
    if (rpc.enet_host == nullptr) {
        return false;
    }

    rpc.just_connected = true;
    rpc.connected = true;
    *out_rpc = rpc;

    return true;
}


bool Rpc::connect(Rpc* out_rpc, ENetAddress address)
{
    Rpc rpc = {};

    rpc.enet_host = enet_host_create(nullptr, 1, DEFAULT_CHAN_CAP, 0, 0);
    if (rpc.enet_host == nullptr) {
        return false;
    }

    rpc.enet_peer = enet_host_connect(rpc.enet_host, &address, DEFAULT_CHAN_CAP, 0);
    if (rpc.enet_peer == nullptr) {
        enet_host_destroy(rpc.enet_host);
        return false;
    }
    /*
    ENetEvent event;
    if (!(enet_host_service(rpc.enet_host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)) {
        enet_peer_reset(rpc.enet_peer);
        enet_host_destroy(rpc.enet_host);
        return false;
    }
    */

    rpc.just_connected = true;
    rpc.connected = true;
    *out_rpc = rpc;

    return true;
}


void Rpc::service()
{
    if (!this->connected) {
        return;
    }

    if (this->just_connected && this->is_server()) {
        (void)broadcast("_connect", 0, nullptr);
    }

    this->just_connected = false;


    ENetEvent event;


    while (enet_host_service(this->enet_host, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            printf("Connect %u\n", event.peer->connectID);
            this->map.peers[event.peer->connectID] = event.peer;
            if (is_server()) {
                (void)broadcast("_connect", 0, nullptr, event.peer->connectID);
            }
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Disconnect %u\n", event.peer->connectID);
            this->map.peers.erase(event.peer->connectID);
            if (!is_server()) {
                call_rpc_function(this, { "_disconnect", false, 0, 0, nullptr});
                this->connected = false;
            }
            else {
                (void)broadcast("_disconnect", 0, nullptr, event.peer->connectID);
            }
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("Receive %d\n", is_server());
            {
                RpcCallPacket packet = read_rpc_call_packet(event.packet->data);

                if (is_server()) {
                    if (packet.broadcast) {
                        (void)broadcast(packet.function_name, packet.data_size, packet.data);
                    }
                    else {
                        (void)send(packet.client, packet.function_name, packet.data_size, packet.data);
                    }
                } else {
                    assert(is_function_registered(this, packet.function_name));
                    call_rpc_function(this, packet);
                }
            }
            break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}


bool Rpc::broadcast(const char *fn_name, const size_t data_size, const void *data, uint32_t poser)
{
    if (!connected || !is_function_registered(this, fn_name)) {
        return false;
    }

    RpcCallPacket call_packet = { fn_name, true, is_server() ? poser : this->enet_peer->connectID, data_size, data };
    RpcRawPacket raw_packet = write_rpc_call_packet(call_packet);

    ENetPacket *packet = enet_packet_create(raw_packet.data, raw_packet.size, ENET_PACKET_FLAG_RELIABLE);
    free(raw_packet.data);

    if (is_server()) {
        call_rpc_function(this, call_packet);
        enet_host_broadcast(enet_host, 0, packet);
    } else {
        enet_peer_send(enet_peer, 0, packet);
    }

    return true;
}


bool Rpc::send(RpcClient client, const char *fn_name, const size_t data_size, const void *data, uint32_t poser)
{
    if (!connected || !is_function_registered(this, fn_name)) {
        return false;
    }

    RpcCallPacket call_packet = { fn_name, false, is_server() ? poser : this->enet_peer->connectID, data_size, data };
    RpcRawPacket raw_packet = write_rpc_call_packet(call_packet);

    ENetPacket *packet = enet_packet_create(raw_packet.data, raw_packet.size, ENET_PACKET_FLAG_RELIABLE);
    free(raw_packet.data);

    if (is_server()) {
        if (client.id == 0) {
            // - Send to server itself.
            call_rpc_function(this, call_packet);
        } else {
            enet_peer_send(this->map.peers.at(client.id), 0, packet);
        }
    } else {
        enet_peer_send(enet_peer, 0, packet);
    }

    return true;
}


void Rpc::register_function(const char *fn_name, void *userdata, RpcCallback callback)
{
    this->functions[fn_name] = {userdata, callback};
}


void Rpc::disconnect()
{
    if (!this->connected) {
        return;
    }

    this->connected = false;
    if (is_server()) {
        for (auto peer : this->map.peers) {
            enet_peer_disconnect(std::get<1>(peer), 0);
        }
        ENetEvent event;
        enet_host_service(enet_host, &event, 0);
    }
    else {
        enet_peer_disconnect(enet_peer, 0);
        ENetEvent event;
        enet_host_service(enet_host, &event, 0);
    }
    enet_host_destroy(enet_host);
}


bool Rpc::is_server()
{
    return this->enet_peer == nullptr;
}

uint32_t Rpc::own_id()
{
    return is_server() ? 0 : this->enet_peer->connectID;
}
