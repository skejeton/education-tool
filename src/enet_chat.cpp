//
// Created by skeje on 7/17/2023.
//

#include "enet_chat.hpp"

#define DEFAULT_CLIENT_CAP 32
#define DEFAULT_CHAN_CAP   2

static bool is_server(EnetChat *chat)
{
    return chat->enet_peer == nullptr;
}

bool EnetChat::host(EnetChat *out_chat)
{
    EnetChat chat = {};

    ENetAddress address;
    address.port = ENET_CHAT_PORT;
    address.host = ENET_HOST_ANY;
    enet_address_set_host(&address, "127.0.0.1");

    chat.enet_host = enet_host_create(&address, DEFAULT_CLIENT_CAP, DEFAULT_CHAN_CAP, 0, 0);
    if (chat.enet_host == nullptr) {
        return false;
    }

    chat.connected = true;
    *out_chat = chat;

    return true;
}

bool EnetChat::connect(EnetChat *out_chat, ENetAddress address)
{
    EnetChat chat = {};

    chat.enet_host = enet_host_create(nullptr, 1, DEFAULT_CHAN_CAP, 0, 0);
    if (chat.enet_host == nullptr) {
        return false;
    }

    chat.enet_peer = enet_host_connect(chat.enet_host, &address, DEFAULT_CHAN_CAP, 0);
    if (chat.enet_peer == nullptr) {
        return false;
    }

    chat.connected = true;
    *out_chat = chat;

    return true;
}

void EnetChat::service()
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
                if (is_server(this)) {
                    auto str = std::string((char*)event.packet->data);
                    broadcast(str.c_str(), true);
                } else {
                    auto str = std::string((char*)event.packet->data);
                    this->log.push_back(str);
                }
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void EnetChat::broadcast(const char *raw_message, bool distribute)
{
    if (*raw_message == 0) {
        // Cant send empty message.
        return;
    }

    auto message = std::string(raw_message);
    if (!distribute) {
        if (is_server(this)) {
            message = "[server] "+message;
        } else {
            char conn_str[32];
            snprintf(conn_str, 32, "[client%d] ", enet_peer->connectID);
            message = conn_str+message;
        }
    }

    ENetPacket *packet = enet_packet_create(message.c_str(), message.size()+1, ENET_PACKET_FLAG_RELIABLE);
    if (is_server(this)) {
        enet_host_broadcast(enet_host, 0, packet);
        this->log.push_back(message);
    } else {
        enet_peer_send(enet_peer, 0, packet);
    }
}

