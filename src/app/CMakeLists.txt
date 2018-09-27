cmake_minimum_required(VERSION 3.11)

set(IPL_APP_SOURCES
	main.c
	hw_raspi.c
	)

add_executable(IPL_APP
		${IPL_APP_SOURCES}
		)

target_link_libraries(IPL_APP
		INIC_PROGRAMMING_LIBRARY
		)

set_target_properties(IPL_APP
		PROPERTIES
		LINKER_LANGUAGE C
		VERSION         ${${PROJECT_NAME}_VERSION}
		)

# Install libs and headers.
install(TARGETS IPL_APP
		EXPORT Ipl:App
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
