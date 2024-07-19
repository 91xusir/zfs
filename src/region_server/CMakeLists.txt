#include_directories (../../../engine/src/scene)
include_directories (/usr/include/python2.5)

SET(RS_SRC_NET ../../engine/src/net/g_cmdpacket.cpp ../../engine/src/net/g_netpacket.cpp
	../../engine/src/net/g_netsocket.cpp ../../engine/src/net/g_tcplistener.cpp
	../../engine/src/net/g_tcpsession.cpp ../../engine/src/net/g_udpsession.cpp)

SET(RS_SRC_PACK ../../engine/src/filepack/ansi_file.cpp ../../engine/src/filepack/game_pack.cpp
	../../engine/src/filepack/normal_pack.cpp ../../engine/src/filepack/pack_factory.cpp
	../../engine/src/filepack/pack_file.cpp ../../engine/src/filepack/pack_manager.cpp
	../../engine/src/filepack/unpack_file.cpp ../../engine/src/filepack/profile.cpp
	../../engine/src/filepack/profile_result.cpp ../../engine/src/filepack/blow_fish.cpp)

#SET(RS_SRC_CORE ../../engine/src/core/rt_importlib.cpp)

SET(RS_SRC_SCENE ../../engine/head/scene/rts_path_find.cpp ../../engine/head/scene/rts_scene_area.cpp
	../../engine/head/scene/rts_scene_doc.cpp ../../engine/head/scene/rts_scene_terrain.cpp)

SET(RS_SRC_SHARE ../shared/auction_base.cpp ../shared/gwt.cpp ../shared/linux_util.cpp ../shared/rt1tort2.cpp ../shared/string_mapping.cpp ../shared/union_base.cpp ../shared/item_base.cpp ../shared/herolist_base.cpp ../shared/team_data.cpp ../shared/castlewar_base.cpp ../shared/business.cpp ../shared/game_string_assistant.cpp ../shared/item_bank.cpp ../shared/item_npc_shop.cpp ../shared/item_pshop.cpp ../shared/item_trade.cpp ../shared/pk.cpp ../shared/region_client.cpp ../shared/server_config.cpp ../shared/skill_base.cpp ../shared/skill_manage.cpp ../shared/pet_base.cpp ../shared/GloableBuffer.cpp)

SET(RS_SRC_SCENE ../../engine/head/scene/rts_scene_area.cpp ../../engine/head/scene/rts_path_find.cpp
	../../engine/head/scene/rts_scene_terrain.cpp ../../engine/head/scene/rts_scene_doc.cpp ../../engine/head/scene/rts_path_point.cpp)

add_executable (region_server PlayerLogic.cpp region.cpp region_ai.cpp region_anti_robot.cpp region_auction.cpp region_castlewar.cpp region_creature.cpp region_friend.cpp region_gift.cpp region_guarder.cpp region_gwsession.cpp region_item.cpp region_mail.cpp region_npc_shop.cpp region_object.cpp region_prentice.cpp region_scene.cpp region_server.cpp region_spawn.cpp region_state.cpp region_state_snapshot.cpp region_team.cpp region_union.cpp region_util.cpp cmd.cpp cmd_auction.cpp cmd_castlewar.cpp cmd_channel.cpp cmd_feod.cpp cmd_item.cpp cmd_union.cpp region_skill.cpp rt_randomimage.cpp rs_dungeon.cpp rs_auction.cpp rs_card_trade.cpp region_master_list.cpp rs_national_war.cpp jhwg_session.cpp nes.cpp region_MonCastle.cpp region_deduct.cpp rs_war_zhulu.cpp region_object_py.cpp region_server_py.cpp region_creature_py.cpp region_scene_py.cpp region_item_py.cpp script_engine_py.cpp rs_LineMan.cpp rs_LineGroup.cpp rs_cmd_LineSys.cpp RegionActManager.cpp region_activity_py.cpp ${RS_SRC_NET} ${RS_SRC_PACK} ${RS_SRC_SHARE} ${RS_SRC_SCENE})

add_definitions (-DRT_SCENE_ONLY_LOAD_TERRAIN -DREGION_SERVER)

# Link the executable to the Hello library.
target_link_libraries (region_server pthread curl mysqlclient core_${CMAKE_BUILD_TYPE} python2.5)
