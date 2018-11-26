cmake_minimum_required(VERSION 3.11)

if(${IPL_EXAMPLE_RPI})
	message(STATUS "Building raspberry pi example.")
	set(IPL_EXAMPLE_SOURCES
			hw_raspi.c
			)
elseif(${IPL_EXAMPLE_WIN})
	message(STATUS "Building windows aardvark example.")
	set(IPL_EXAMPLE_PUBLIC_HEADERS
		${IPL_EXAMPLE_INCLUDE_DIR}/aardvark.h
	)

	set(IPL_EXAMPLE_PRIVATE_HEADERS)
	
	set(IPL_EXAMPLE_HEADERS
		${IPL_EXAMPLE_PUBLIC_HEADERS}
		${IPL_EXAMPLE_PRIVATE_HEADERS}
	)
	set(IPL_EXAMPLE_SOURCES
			hw_win.c
			aardvark.c
			)
else()
	message(ERROR "No example will be build")
endif()

add_executable(IPL_EXAMPLE
		main.c
        ${IPL_EXAMPLE_SOURCES}
	)

target_link_libraries(IPL_EXAMPLE
        INIC_PROGRAMMING_LIBRARY
    )
	
if (${IPL_EXAMPLE_WIN})
	target_include_directories(IPL_EXAMPLE
		PUBLIC ${IPL_EXAMPLE_INCLUDE_DIR}
	)
	
	target_link_libraries(IPL_EXAMPLE
		Winmm
		${IPL_EXAMPLE_WIN_AARDVARK_STATIC_LIB}
	)
endif()

set_target_properties(IPL_EXAMPLE
        PROPERTIES
        OUTPUT_NAME "ipl_rpi_example"
        LINKER_LANGUAGE C
        VERSION         ${${PROJECT_NAME}_VERSION}
    )

# Install libs and headers.
install(TARGETS IPL_EXAMPLE
        EXPORT Ipl:Example
                LIBRARY DESTINATION
                lib
                COMPONENT libraries
                ARCHIVE DESTINATION
                lib
                COMPONENT libraries
                RUNTIME DESTINATION
                bin
                COMPONENT libraries # Windows DLLs
                FRAMEWORK DESTINATION
                lib
                COMPONENT libraries
                )
