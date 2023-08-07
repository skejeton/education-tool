#include <assert.h>
#include "player.hpp"
#include "save.hpp"
#include "rpc.hpp"
#include "environment.hpp"
#include "net_table.hpp"
#include "netcode.hpp"

#define RPC_HANDLER(name) static void name(Rpc *rpc, RpcClient source, void *userdata, size_t data_size, const void *data)

void ncfmt_player(BinaryFormat *fmt, Player *p)
{
    fmt->pass_value(p);
}


void ncfmt_entity(BinaryFormat *fmt, Entity *e)
{
    fmt->pass_value(e);
}


void ncfmt_dialog(BinaryFormat *fmt, FlashbacksDialog *d)
{
    fmt->pass_c_string((char**)&d->answer);
    fmt->pass_c_string((char**)&d->text);
    fmt->pass_value(&d->numeric);
    fmt->pass_value(&d->prev);
    fmt->pass_value(&d->next);
}


RPC_HANDLER(nc_broadcast_ping)
{
    auto nc = (Netcode*)userdata;

    nc->env->dialogs.push("Ping!", DialogType::Info);
}


struct NcSetPlayingModePacket {
    PlayingMode new_mode;
};


RPC_HANDLER(nc_set_playing_mode)
{
    auto nc = (Netcode*)userdata;
    auto p = (NcSetPlayingModePacket*)data;

    nc->env->playing_mode = p->new_mode;

    switch (nc->env->playing_mode) {
    case PLAYING_MODE_PLAY:
        nc->env->flashbacks.backlog = {};
        // FIXME: env->camera_velocity = {0, 0, 0};
        break;
    }
}

RPC_HANDLER(nc_set_player_state)
{
    auto nc = (Netcode*)userdata;

    TableId id;
    BinaryFormat format = BinaryFormat::begin_read(data, data_size);
    format.pass_value(&id);

    if (nc->player_id != id) {
        net_table_set_apply<Player>(FileBuffer{(uint8_t*)data, data_size}, &nc->env->player_pool.players, ncfmt_player);
    }
}

RPC_HANDLER(nc_summon_entity)
{
    auto nc = (Netcode*)userdata;

    BinaryFormat format = BinaryFormat::begin_read(data, data_size);
    Entity entity;
    net_table_alloc<Entity>(&format, &nc->env->scene.entities, &entity, ncfmt_entity);
}


RPC_HANDLER(nc_remove_entity)
{
    auto nc = (Netcode*)userdata;

    net_table_remove_apply<Entity>({(uint8_t*)data, data_size}, &nc->env->scene.entities);
}


RPC_HANDLER(nc_set_entity)
{
    auto nc = (Netcode*)userdata;

    net_table_set_apply<Entity>({(uint8_t*)data, data_size}, &nc->env->scene.entities, ncfmt_entity);
}


RPC_HANDLER(nc_add_dialog)
{
    auto nc = (Netcode*)userdata;

    BinaryFormat format = BinaryFormat::begin_read(data, data_size);
    FlashbacksDialog dialog;
    net_table_alloc<FlashbacksDialog>(&format, &nc->env->flashbacks.dialogs, &dialog, ncfmt_dialog);
}


RPC_HANDLER(nc_remove_dialog)
{
    auto nc = (Netcode*)userdata;

    net_table_remove_apply<FlashbacksDialog>({(uint8_t*)data, data_size}, &nc->env->flashbacks.dialogs);
}


RPC_HANDLER(nc_set_dialog)
{
    auto nc = (Netcode*)userdata;

    net_table_set_apply<FlashbacksDialog>({(uint8_t*)data, data_size}, &nc->env->flashbacks.dialogs, ncfmt_dialog);
}


RPC_HANDLER(nc_connect)
{
    printf("someone connected\n");

    auto nc = (Netcode*)userdata;

    if (rpc->is_server()) {
        // Send connection initialization data.

        {
            BinaryFormat format = BinaryFormat::begin_write();
            format.pass_value(&nc->env->playing_mode);
            net_table_init<Player>(&format, &nc->env->player_pool.players, ncfmt_player);
            net_table_init<Entity>(&format, &nc->env->scene.entities, ncfmt_entity);
            net_table_init<FlashbacksDialog>(&format, &nc->env->flashbacks.dialogs, ncfmt_dialog);
            FileBuffer buf = format.leak_file_buffer();

            (void)rpc->send(source, "nc_init_connection", buf.size, buf.data);
            buf.deinit();
        }

        {
            Player p = {};
            p.camera.fov_deg = 90;
            BinaryFormat format = BinaryFormat::begin_write();
            format.pass_value(&source.id);
            net_table_alloc<Player>(&format, &nc->env->player_pool.players, &p, ncfmt_player);
            FileBuffer buf = format.leak_file_buffer();

            (void)rpc->broadcast("nc_add_connection", buf.size, buf.data);
            buf.deinit();
        }
    }
}


RPC_HANDLER(nc_init_connection)
{
    printf("init connection\n");

    auto nc = (Netcode*)userdata;

    BinaryFormat fmt = BinaryFormat::begin_read(data, data_size);
    fmt.pass_value(&nc->env->playing_mode);
    net_table_init<Player>(&fmt, &nc->env->player_pool.players, ncfmt_player);
    net_table_init<Entity>(&fmt, &nc->env->scene.entities, ncfmt_entity);
    net_table_init<FlashbacksDialog>(&fmt, &nc->env->flashbacks.dialogs, ncfmt_dialog);

    printf("init connection: success, playing mode: %d\n", nc->env->playing_mode);
}


RPC_HANDLER(nc_add_connection)
{
    auto nc = (Netcode*)userdata;

    BinaryFormat format = BinaryFormat::begin_read(data, data_size);
    format.pass_value(&source.id);

    Player p;
    TableId player_id;
    player_id = nc->player_ids[source.id] = net_table_alloc<Player>(&format, &nc->env->player_pool.players, &p, ncfmt_player);

    if (source.id == rpc->own_id()) {
        nc->player_id = player_id;
    }
}

RPC_HANDLER(nc_disconnect)
{
    auto nc = (Netcode*)userdata;
    printf("client %u(%zu) disconnected\n", source.id, nc->player_ids[source.id].id);

    if (source.id == 0) {
        nc->env->playing_mode = PLAYING_MODE_MENU;
    }

    nc->env->player_pool.players.remove(nc->player_ids[source.id]);
    nc->player_ids.erase(source.id);
}


RPC_HANDLER(nc_remove_connection)
{
    printf("deinit connection");

    auto nc = (Netcode*)userdata;

    BinaryFormat fmt = BinaryFormat::begin_read(data, data_size);
    fmt.pass_value(&nc->env->playing_mode);
    net_table_remove_apply<Player>(fmt.get_rest(), &nc->env->player_pool.players);
}
