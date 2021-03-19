set(USE_COVERAGE
    false
    CACHE
    BOOL
    "Использование отчета покрытия тестами. Опции: 1, 0"
)

if (USE_COVERAGE) 
	message(STATUS "Использование покрытия тестами")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

