set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(QFcm
    main.cpp
    ${SOURCES}
    ${HEADERS})

target_link_libraries(QFcm
    ${QT_MODULES})
