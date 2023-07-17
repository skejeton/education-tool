//
// Created by skeje on 7/17/2023.
//

#include "enet_chat.hpp"

void send_callback(void *userdata, size_t data_size, const void *data)
{
    auto chat = (EnetChat*)userdata;

    chat->messages.emplace_back((const char *)data);
}

void EnetChat::connect(Rpc *rpc)
{
    rpc->register_function("send", this, send_callback);
}


void EnetChat::broadcast(Rpc *rpc, const char *string)
{
    rpc->broadcast("send", strlen(string)+1, string);
}
