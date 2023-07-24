#include <assert.h>
#include "save.hpp"

#define RPC_HANDLER(name) static void name(Rpc *rpc, RpcClient source, void *userdata, size_t data_size, const void *data)

void ncfmt_player(BinaryFormat *fmt, Player *p) {
    fmt->pass_value(&p);
}

RPC_HANDLER(nc_broadcast_ping) {
    auto env = (Environment*)userdata;

    env->dialogs.push("Ping!", DialogType::Info);
}


struct NcSetPlayingModePacket {
    PlayingMode new_mode;
};


RPC_HANDLER(nc_set_playing_mode) {
    auto env = (Environment*)userdata;
    auto p = (NcSetPlayingModePacket*)data;

    env->playing_mode = p->new_mode;

    switch (env->playing_mode) {
    case PLAYING_MODE_PLAY:
        env->flashbacks.backlog = {};
        // FIXME: env->camera_velocity = {0, 0, 0};
        break;
    }
}


RPC_HANDLER(nc_connect) {
    printf("someone connected\n");

    auto env = (Environment*)userdata;

    if (rpc->is_server()) {
        // Send connection initialization data.

        FileBuffer buf;
        {
            FileBuffer player_buf = net_table_init_write<Player>(&env->player_pool.players, ncfmt_player);

            BinaryFormat fmt = BinaryFormat::begin_write();
            fmt.pass_value(&env->playing_mode);
            fmt.pass_pointer((void**)&buf.data, buf.size);
            player_buf.deinit();
            buf = fmt.leak_file_buffer();
        }

        (void)rpc->send(source, "nc_init_connection", buf.size, buf.data);
    }


    
    env->player_pool.players.allocate({});
}

RPC_HANDLER(nc_init_connection) {
    printf("init connection");

    auto env = (Environment*)userdata;

    {
        BinaryFormat fmt = BinaryFormat::begin_read(data, data_size);
        fmt.pass_value(&env->playing_mode);
        net_table_init_apply<Player>(fmt.get_rest(), &env->player_pool.players, ncfmt_player);
    }
}


RPC_HANDLER(nc_disconnect) {
    printf("someone disconnected\n");

    auto env = (Environment*)userdata;

    if (rpc->is_server()) {
        FileBuffer remove_buf = net_table_remove_write<Player>(&env->player_pool.players);
        rpc->broadcast("nc_deinit_connection", remove_buf.size, remove_buf.data);
    }
}


RPC_HANDLER(nc_deinit_connection) {
    printf("deinit connection");

    auto env = (Environment*)userdata;

    {
        BinaryFormat fmt = BinaryFormat::begin_read(data, data_size);
        fmt.pass_value(&env->playing_mode);
        net_table_remove_apply<Player>(fmt.get_rest(), &env->player_pool.players);
    }
}
