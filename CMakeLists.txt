cmake_minimum_required(VERSION 3.14)

project(QFcm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Sql DBus REQUIRED)

set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(errors.cmake)
include(sanitizers.cmake)
include(coverage.cmake)

list(APPEND QT_MODULES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::DBus
    )

list(APPEND SOURCES
    ${TOP_DIR}/src/fcmanager.cpp
    ${TOP_DIR}/src/fcmanager_adapter.cpp
    ${TOP_DIR}/src/threads/fcmWorker.cpp)

list(APPEND HEADERS
    ${TOP_DIR}/src/fcmanager.h
    ${TOP_DIR}/src/fcmanager_adapter.h
    ${TOP_DIR}/src/agents/agentsinfo.h
    ${TOP_DIR}/src/agents/agentdata.h
    ${TOP_DIR}/src/threads/fcmWorker.h
    ${TOP_DIR}/src/common/utils.h
    ${TOP_DIR}/src/common/querybuilder.h
    ${TOP_DIR}/src/common/messagebuilder.h
    ${TOP_DIR}/src/common/database_query.h
    ${TOP_DIR}/src/common/message_spec/messagetypes.h
    ${TOP_DIR}/src/common/message_spec/messagesendable.h
    ${TOP_DIR}/src/common/message_spec/messagereadable.h
    ${TOP_DIR}/src/common/message_spec/messagereadable_handshake.h
    ${TOP_DIR}/src/common/message_spec/messagereadable_nomessage.h
    ${TOP_DIR}/src/common/message_spec/messagereadable_service.h
    ${TOP_DIR}/src/common/message_spec/messagereadable_test.h
    ${TOP_DIR}/src/common/message_spec/messagereadable_data.h
    ${TOP_DIR}/src/common/service_name_and_property.h
    )

add_subdirectory(src)
add_subdirectory(test)
