project(Main)

set(SOURCES 
    src/hello_vcs.cpp
	src/GameBoard.cpp
	src/Common.h
    )

set(HEADERS 
	include/GameBoard.h
	)

cmrc_add_resource_library(rs-main
	NAMESPACE main
	resources/dice_cubes_sd_bit.png
	resources/UnicaOne-Regular.ttf
	)

add_executable(main ${SOURCES} ${HEADERS})
set_target_properties(main PROPERTIES 
	PUBLIC_HEADER ${HEADERS}
	)

target_link_libraries(main PRIVATE core sfml-graphics sfml-main sfml-system sfml-window rs-main)

target_include_directories(main 
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${Core_SOURCE_DIR}/include
    )
