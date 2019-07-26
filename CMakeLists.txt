cmake_minimum_required(VERSION 3.8)

include(CMakeRC.cmake)

find_package(SFML 2.5 COMPONENTS system main window graphics audio REQUIRED)

if(WIN32)
	add_compile_definitions(WINDOWS=1)
endif (WIN32)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_compile_definitions(DEBUG=1)
else()
	add_compile_definitions(DEBUG=0)
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

add_subdirectory(core)
add_subdirectory(main)