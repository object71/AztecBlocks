project(Core)

set(SOURCES 
	src/GameLoop.cpp
	)
set(HEADERS
	include/GameLoop.h
	include/Interfaces.h
	)

add_library(core ${SOURCES} ${HEADERS})
target_link_libraries(core PRIVATE sfml-graphics sfml-main sfml-system sfml-window)

target_include_directories(core 
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)