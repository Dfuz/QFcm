set(USE_SANITIZER
    ""
    CACHE
    STRING
    "Использование санитайзеров. Опции: Addr, Mem, Undef, Thread, Leak"
)

if(USE_SANITIZER)
    if(USE_SANITIZER MATCHES "[Aa]ddr")
	message(STATUS "Используем address санитайзер")
	append("-fsanitize=address" CMAKE_CXX_FLAGS)
    elseif(USE_SANITIZER MATCHES "[Mm]em")
	message(STATUS "Используем memory санитайзер")
	append("-fsanitize=memory" CMAKE_CXX_FLAGS)
    elseif(USE_SANITIZER MATCHES "[Uu]ndef")
	message(STATUS "Используем undefiend beh санитайзер")
	append("-fsanitize=undefined" CMAKE_CXX_FLAGS)
    elseif(USE_SANITIZER MATCHES "[Tt]hread")
	message(STATUS "Используем thread санитайзер")
	append("-fsanitize=thread" CMAKE_CXX_FLAGS)
    elseif(USE_SANITIZER MATCHES "[Ll]eak")
	message(STATUS "Используем leak санитайзер")
	append("-fsanitize=leak" CMAKE_CXX_FLAGS)
    else()
	    message(FATAL_ERROR "Данная опция не поддерживается: ${USE_SANITIZER}")
    endif()
endif()
