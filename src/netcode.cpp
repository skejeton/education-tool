#include "netcode.hpp"
#include "netcode_callbacks.inl"
#include "main_menu.hpp"
#include "project.hpp"
#include "net_table.hpp"

#define DEFAULT_PORT 5621


static void load_env_project(Environment *env, Project *project)
{
    env->scene = std::move(project->scene);
    env->flashbacks = std::move(project->flashbacks);
}


static void load_project_env(Environment *env, Project *project)
{
    project->scene = std::move(env->scene);
    project->flashbacks = std::move(env->flashbacks);
}


static void write_open_save(Environment *env, OpenProject open_project)
{
    Project project = {};
    load_project_env(env, &project);
    auto project_file = ProjectFile::init_from_path(open_project.path.c_str());
    project.save(project_file);
    project_file.deinit();
}


static void load_open_save(Environment *env, OpenProject open_project)
{
    Project project = {};
    auto project_file = ProjectFile::init_from_path(open_project.path.c_str());
    project = Project::load(project_file);
    load_env_project(env, &project);
    project_file.deinit();
}


Netcode Netcode::connect(Environment *env, OpenProject open_project)
{
    Rpc rpc = {};
    Netcode nc = { rpc, env };

    ENetAddress addr;
    addr.port = DEFAULT_PORT;

    if (enet_address_set_host(&addr, open_project.host_name.c_str()) < 0) {
        // Invalid address
        assert(false);
    }


    if (open_project.hosting_type == HostingType::JOIN) {
        assert(nc.rpc.connect(&nc.rpc, addr));
    }
    else {
        load_open_save(env, open_project);
        assert(nc.rpc.host(&nc.rpc, addr));
    }

    nc.register_all();

    return nc;
}


void Netcode::disconnect()
{
    rpc.disconnect();
}


void Netcode::service()
{
    rpc.service();
}


void Netcode::register_all()
{
    rpc.register_function("net_broadcast_ping", env, nc_broadcast_ping);
    rpc.register_function("net_set_playing_mode", env, nc_set_playing_mode);
    // rpc.register_function("net_set_player_state", env, nc_set_player_state);

    rpc.register_function("_connect", env, nc_connect);
    rpc.register_function("nc_init_connection", env, nc_init_connection);
    rpc.register_function("_disconnect", env, nc_disconnect);
    rpc.register_function("nc_deinit_connection", env, nc_deinit_connection);
}


void Netcode::broadcast_ping()
{
    (void)rpc.broadcast("net_broadcast_ping", 0, nullptr);
}


void Netcode::set_playing_mode(PlayingMode playing_mode)
{
    NcSetPlayingModePacket packet = { playing_mode };

    (void)rpc.broadcast("net_set_playing_mode", sizeof(packet), &packet);
}


void Netcode::set_player_state(Player player)
{
    FileBuffer buf = net_table_init_write<Player>(&this->env->player_pool.players, ncfmt_player);

    (void)rpc.broadcast("nc_set_player_state", buf.size, buf.data);

    buf.deinit();
}
