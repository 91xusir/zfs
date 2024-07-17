SET(LS_SRC_NET  ../../engine/src/net/g_cmdpacket.cpp ../../engine/src/net/g_netpacket.cpp ../../engine/src/net/g_netsocket.cpp
                ../../engine/src/net/g_tcplistener.cpp ../../engine/src/net/g_tcpsession.cpp ../../engine/src/net/g_udpsession.cpp)

SET(LS_SRC_WSDL wsdl/CBlowFishSimple.cpp wsdl/WsdlWrapper.cpp wsdl/schemaparser/ComplexType.cpp
                wsdl/schemaparser/ContentModel.cpp wsdl/schemaparser/Group.cpp wsdl/schemaparser/SchemaDebug.cpp wsdl/schemaparser/SchemaParser.cpp
                wsdl/schemaparser/SchemaValidator.cpp wsdl/schemaparser/SimpleType.cpp wsdl/schemaparser/TypeContainer.cpp
                wsdl/schemaparser/TypesTable.cpp wsdl/wsdlparser/Message.cpp wsdl/wsdlparser/Soap.cpp wsdl/wsdlparser/WsdlDebug.cpp
                wsdl/wsdlparser/WsdlElement.cpp wsdl/wsdlparser/WsdlInvoker.cpp wsdl/wsdlparser/WsdlParser.cpp
                wsdl/xmlpull/ConfigFile.cpp wsdl/xmlpull/XmlPullParser.cpp wsdl/xmlpull/XmlSerializer.cpp
                wsdl/xmlpull/XmlUtils.cpp)

SET(LS_SRC_SHARE ../shared/license_check.cpp ../shared/rt1tort2.cpp ../shared/linux_util.cpp ../shared/server_config.cpp)

include_directories (wsdl)
include_directories (../../engine/head)
include_directories (../shared)

add_executable (login_server charge_session.cpp gameworldstub.cpp gamigo_adb_session.cpp guide_server.cpp lgsthread.cpp
                                loginserver.cpp main.cpp  dbmysql.cpp LoginLogicExt.cpp ${LS_SRC_NET} ${LS_SRC_WSDL} ${LS_SRC_SHARE})

# Link the executable to the Hello library.
target_link_libraries (login_server pthread curl mysqlclient core_${CMAKE_BUILD_TYPE})

