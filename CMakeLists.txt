cmake_minimum_required (VERSION 3.5)

project (unshieldv3 LANGUAGES CXX C VERSION "0.1.0")

if (NOT ${CMAKE_C_BYTE_ORDER} STREQUAL "LITTLE_ENDIAN")
	message (FATAL_ERROR "Incompatible target. Only LITTLE_ENDIAN byte order is supported.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
)

add_executable (unshieldv3
	main.cpp
	installshieldarchivev3.cpp
	blast/blast.c
)
target_link_libraries(unshieldv3 stdc++fs)
install(TARGETS unshieldv3)
