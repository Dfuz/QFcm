set(USE_SANITIZER
    ""
    CACHE
    STRING
    "Использование санитайзеров. Опции: Addr, Mem, Undef, Thread, Leak"
)

if(USE_SANITIZER)
    if(USE_SANITIZER MATCHES "[Aa]ddr")
	message(STATUS "Используем address санитайзер")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    elseif(USE_SANITIZER MATCHES "[Mm]em")
	message(STATUS "Используем memory санитайзер")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
    elseif(USE_SANITIZER MATCHES "[Uu]ndef")
	message(STATUS "Используем undefiend beh санитайзер")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    elseif(USE_SANITIZER MATCHES "[Tt]hread")
	message(STATUS "Используем thread санитайзер")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    elseif(USE_SANITIZER MATCHES "[Ll]eak")
	message(STATUS "Используем leak санитайзер")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
    else()
	    message(FATAL_ERROR "Данная опция не поддерживается: ${USE_SANITIZER}")
    endif()
endif()
