cmake_minimum_required(VERSION 3.8)
project(unshieldv3)

set(CMAKE_CXX_STANDARD 17)

add_executable(unshieldv3
	main.cpp
	blast/blast.c
	installshieldarchivev3.cpp
)
install(TARGETS unshieldv3)
