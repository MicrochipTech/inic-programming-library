cmake_minimum_required(VERSION 3.11)

set(IPL_EXAMPLE_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	)
	
set(IPL_EXAMPLE_WIN_AARDVARK_STATIC_LIB
	${CMAKE_CURRENT_SOURCE_DIR}/aardvark.dll
	)

add_subdirectory(src)
