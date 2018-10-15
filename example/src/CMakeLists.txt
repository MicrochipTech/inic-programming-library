cmake_minimum_required(VERSION 3.11)

set(IPL_EXAMPLE_SOURCES
        main.c
        hw_raspi.c
        )

add_executable(IPL_EXAMPLE
        ${IPL_EXAMPLE_SOURCES}
                )

        target_link_libraries(IPL_EXAMPLE
                INIC_PROGRAMMING_LIBRARY
                )

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
