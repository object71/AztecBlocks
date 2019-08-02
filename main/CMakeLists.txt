project(Main)

set(SOURCES 
	src/Game.cpp
	src/GameBoard.cpp
	)

set(HEADERS 
	include/GameBoard.h
	include/Game.h
	)

cmrc_add_resource_library(rs-main
	NAMESPACE main
	resources/textureStone.png
	resources/Boxy-Bold.ttf
	resources/background.png
	resources/completionSound.wav
	resources/gameMusic.ogg
	)

if(WIN32)
	add_executable(main WIN32 ${SOURCES} ${HEADERS})
else()
	add_executable(main ${SOURCES} ${HEADERS})
endif (WIN32)

target_link_libraries(main PRIVATE core sfml-graphics sfml-main sfml-system sfml-window sfml-audio rs-main)

target_include_directories(main 
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${Core_SOURCE_DIR}/include
	)
