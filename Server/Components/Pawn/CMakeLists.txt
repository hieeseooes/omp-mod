get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_server_component(${ProjectId})

target_sources(${ProjectId} PRIVATE
	vendor/enet/enet_impl.c
)

target_include_directories(${ProjectId} PRIVATE
	vendor/enet
)

target_link_libraries(${ProjectId} PRIVATE
	pawn-runtime
	CONAN_PKG::ghc-filesystem
	ws2_32
	winmm
)

target_compile_definitions(${ProjectId} PRIVATE
	-DPAWN_CELL_SIZE=32
)

set_property(TARGET ${ProjectId} PROPERTY CXX_EXTENSIONS ON)

include_directories(${CMAKE_SOURCE_DIR}/lib)
