#include "netcode.hpp"
#include "main_menu.hpp"
#include "net_table.hpp"
#include "netcode_callbacks.inl"
#include "project.hpp"

#define DEFAULT_PORT 5621

static void
load_env_project(Environment* env, Project* project)
{
    env->scene = std::move(project->scene);
    env->flashbacks = std::move(project->flashbacks);
}

static void
load_project_env(Environment* env, Project* project)
{
    project->scene = std::move(env->scene);
    project->flashbacks = std::move(env->flashbacks);
}

static void
write_open_save(Environment* env, OpenProject open_project)
{
    Project project = {};
    load_project_env(env, &project);
    auto project_file = ProjectFile::init_from_path(open_project.path.c_str());
    project.save(project_file);
    project_file.deinit();
}

static void
load_open_save(Environment* env, OpenProject open_project)
{
    Project project = {};
    auto project_file = ProjectFile::init_from_path(open_project.path.c_str());
    project = Project::load(project_file);
    load_env_project(env, &project);
    project_file.deinit();
}

void
Netcode::connect(Environment* env, OpenProject open_project)
{
    ENetAddress addr;
    addr.port = DEFAULT_PORT;

    this->env = env;

    if (enet_address_set_host(&addr, open_project.host_name.c_str()) < 0) {
        // Invalid address
        assert(false);
    }

    if (open_project.hosting_type == HostingType::JOIN) {
        assert(rpc.connect(&rpc, addr));
    } else {
        load_open_save(env, open_project);
        assert(rpc.host(&rpc, addr));
    }

    register_all();
}

void
Netcode::disconnect()
{
    rpc.disconnect();
}

void
Netcode::service()
{
    rpc.service();
}

void
Netcode::register_all()
{
    rpc.register_function("net_broadcast_ping", this, nc_broadcast_ping);
    rpc.register_function("net_set_playing_mode", this, nc_set_playing_mode);
    rpc.register_function("net_set_player_state", this, nc_set_player_state);
    rpc.register_function("net_summon_entity", this, nc_summon_entity);
    rpc.register_function("net_remove_entity", this, nc_remove_entity);
    rpc.register_function("net_set_entity", this, nc_set_entity);
    rpc.register_function("net_add_dialog", this, nc_add_dialog);
    rpc.register_function("net_remove_dialog", this, nc_remove_dialog);
    rpc.register_function("net_set_dialog", this, nc_set_dialog);

    rpc.register_function("_connect", this, nc_connect);
    rpc.register_function("nc_init_connection", this, nc_init_connection);
    rpc.register_function("nc_add_connection", this, nc_add_connection);
    rpc.register_function("_disconnect", this, nc_disconnect);
    rpc.register_function("nc_remove_connection", this, nc_remove_connection);
}

void
Netcode::broadcast_ping()
{
    (void)rpc.broadcast("net_broadcast_ping", 0, nullptr);
}

void
Netcode::set_playing_mode(PlayingMode playing_mode)
{
    NcSetPlayingModePacket packet = { playing_mode };

    (void)rpc.broadcast("net_set_playing_mode", sizeof(packet), &packet);
}

void
Netcode::set_player_state(Player player)
{
    FileBuffer buf =
      net_table_set_write<Player>(this->player_id, player, ncfmt_player);

    (void)rpc.broadcast("net_set_player_state", buf.size, buf.data);

    buf.deinit();
}

void
Netcode::summon_entity(Entity entity)
{
    BinaryFormat format = BinaryFormat::begin_write();
    net_table_alloc<Entity>(
      &format, &env->scene.entities, &entity, ncfmt_entity);

    (void)rpc.broadcast("net_summon_entity", format.size(), format.origin);

    format.leak_file_buffer().deinit();
}

void
Netcode::remove_entity(TableId entity_id)
{
    FileBuffer buf = net_table_remove_write<Entity>(entity_id);

    (void)rpc.broadcast("net_remove_entity", buf.size, buf.data);

    buf.deinit();
}

void
Netcode::set_entity(TableId id, Entity entity)
{
    FileBuffer buf = net_table_set_write<Entity>(id, entity, ncfmt_entity);

    (void)rpc.broadcast("net_set_entity", buf.size, buf.data);

    buf.deinit();
}

TableId
Netcode::add_dialog(FlashbacksDialog dialog)
{
    TableId next_id = env->flashbacks.dialogs.find_next_id();

    BinaryFormat format = BinaryFormat::begin_write();
    net_table_alloc<FlashbacksDialog>(
      &format, &env->flashbacks.dialogs, &dialog, ncfmt_dialog);

    (void)rpc.broadcast("net_add_dialog", format.size(), format.origin);

    format.leak_file_buffer().deinit();

    return next_id;
}

void
Netcode::remove_dialog(TableId id)
{
    FileBuffer buf = net_table_remove_write<FlashbacksDialog>(id);

    (void)rpc.broadcast("net_remove_dialog", buf.size, buf.data);

    buf.deinit();
}

void
Netcode::set_dialog(TableId id, FlashbacksDialog dialog)
{
    FileBuffer buf =
      net_table_set_write<FlashbacksDialog>(id, dialog, ncfmt_dialog);

    (void)rpc.broadcast("net_set_dialog", buf.size, buf.data);

    buf.deinit();
}
