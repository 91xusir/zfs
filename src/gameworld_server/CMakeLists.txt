SET(LS_SRC_NET ../../engine/src/net/g_cmdpacket.cpp ../../engine/src/net/g_netpacket.cpp ../../engine/src/net/g_netsocket.cpp ../../engine/src/net/g_tcplistener.cpp
	../../engine/src/net/g_tcpsession.cpp ../../engine/src/net/g_udpsession.cpp)

SET(LS_SRC_PACK ../../engine/src/filepack/ansi_file.cpp ../../engine/src/filepack/game_pack.cpp
	../../engine/src/filepack/normal_pack.cpp ../../engine/src/filepack/pack_factory.cpp
	../../engine/src/filepack/pack_file.cpp ../../engine/src/filepack/pack_manager.cpp
	../../engine/src/filepack/unpack_file.cpp ../../engine/src/filepack/blow_fish.cpp)

#SET(LS_SRC_CORE ../../engine/src/core/rt_importlib.cpp)

SET(LS_SRC_SHARE ../shared/auction_base.cpp ../shared/gwt.cpp ../shared/linux_util.cpp ../shared/region_client.cpp ../shared/pet_base.cpp ../shared/rt1tort2.cpp ../shared/server_config.cpp ../shared/string_mapping.cpp ../shared/union_base.cpp ../shared/item_base.cpp ../shared/herolist_base.cpp ../shared/team_data.cpp ../shared/castlewar_base.cpp ../shared/business.cpp ../shared/game_string_assistant.cpp ../shared/GloableBuffer.cpp)

#include_directories ()

add_executable (gameworld_server gw_chattransmittal.cpp gw_clientchannel.cpp gw_database.cpp gw_dungeon.cpp gw_gift.cpp gw_gmchannel.cpp gw_logdb.cpp gw_loginchannel.cpp gw_loginsession.cpp gw_mailmanager.cpp gw_main.cpp gw_nwar.cpp gw_prentice.cpp gw_regionchannel.cpp gw_server.cpp gw_sessionmanager.cpp gw_cmd_union.cpp gw_union.cpp gw_item.cpp gw_cmd_feod.cpp gw_feod.cpp gw_herolist.cpp gw_unionlist.cpp gw_team.cpp gw_castlewar.cpp gw_cmd_castlewar.cpp auction.cpp cardtrade.cpp gw_cmd_auction.cpp gw_master_list.cpp LogicExt.cpp gw_LineMan.cpp gw_LineGroup.cpp gw_cmd_LineSys.cpp GW_ActivityManager.cpp GW_ActDB.cpp ${LS_SRC_NET} ${LS_SRC_PACK} ${LS_SRC_SHARE})

# Link the executable to the Hello library.
target_link_libraries (gameworld_server pthread curl mysqlclient core_${CMAKE_BUILD_TYPE})
