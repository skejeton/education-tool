#ifndef HPP_NETCODE_CATEDU
#define HPP_NETCODE_CATEDU

#include "environment.hpp"
#include "rpc.hpp"
#include "main_menu.hpp"

struct Netcode {
    Rpc rpc;
    Environment *env;
    TableId player_id;

    /**
     * Map between client ID's and player table ID's.
     */
    std::unordered_map<uint32_t, TableId> player_ids;

    void connect(Environment *env, OpenProject open_project);
    void disconnect();

    void service();
    void broadcast_ping();
    void set_playing_mode(PlayingMode playing_mode);

    void set_player_state(Player player);

    void summon_entity(Entity entity);
    void remove_entity(TableId entity_id);

private:
    void register_all();
};

#endif
