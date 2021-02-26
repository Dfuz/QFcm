cmake_minimum_required(VERSION 3.14)

project(QFcm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND QT_MODULES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network)

list(APPEND SOURCES
    ${TOP_DIR}/src/fcmanager.cpp
    ${TOP_DIR}/src/fcm_thread.cpp
    ${TOP_DIR}/src/fcmthreadhandshake.cpp
    ${TOP_DIR}/src/fcmthreadpoller.cpp)

list(APPEND HEADERS
    ${TOP_DIR}/src/fcmanager.h
    ${TOP_DIR}/src/fcm_thread.h
    ${TOP_DIR}/src/utils.h
    ${TOP_DIR}/src/fcmthreadhandshake.h
    ${TOP_DIR}/src/fcmthreadpoller.h)

add_subdirectory(src)
add_subdirectory(test)
