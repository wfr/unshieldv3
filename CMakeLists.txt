cmake_minimum_required (VERSION 3.5)

project (unshieldv3 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable (unshieldv3
	main.cpp
	installshieldarchivev3.cpp
	blast/blast.c
)
target_link_libraries(unshieldv3 stdc++fs)
