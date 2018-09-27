cmake_minimum_required(VERSION 3.11)

set(IPL_PUBLIC_HEADERS
	${IPL_INCLUDE_DIR}/ipl_pb.h
	)

set(IPL_PRIVATE_HEADERS
	${IPL_INCLUDE_DIR}/ipf.h
	${IPL_INCLUDE_DIR}/ipl_81118.h
	${IPL_INCLUDE_DIR}/ipl_81119.h
	${IPL_INCLUDE_DIR}/ipl_81210.h
	${IPL_INCLUDE_DIR}/ipl_81212.h
	${IPL_INCLUDE_DIR}/ipl_81214.h
	${IPL_INCLUDE_DIR}/ipl_cfg.h
	${IPL_INCLUDE_DIR}/ipl.h
	)

set(IPL_HEADERS
	${IPL_PRIVATE_HEADERS}
	${IPL_PUBLIC_HEADERS}
	)

set(IPL_SOURCES
	ipf.c
	ipl_81118.c
	ipl_81119.c
	ipl_81210.c
	ipl_81212.c
	ipl_81214.c
	ipl.c
	)
add_library(INIC_PROGRAMMING_LIBRARY STATIC
        ${IPL_SOURCES}
        ${IPL_HEADERS}
        )

set_target_properties(INIC_PROGRAMMING_LIBRARY
        PROPERTIES
        LINKER_LANGUAGE C
        VERSION         ${${PROJECT_NAME}_VERSION}
        )
set_target_properties(INIC_PROGRAMMING_LIBRARY
        PROPERTIES
        OUTPUT_NAME     "Ipl"
        FRAMEWORK       ON
        PUBLIC_HEADER   "${IPL_PUBLIC_HEADERS}"
        PRIVATE_HEADER  "${IPL_PRIVATE_HEADERS}"
        )

install(TARGETS INIC_PROGRAMMING_LIBRARY
        EXPORT Ipl:Lib
        LIBRARY DESTINATION
            lib
            COMPONENT libraries
        ARCHIVE DESTINATION
            lib
            COMPONENT libraries
        RUNTIME DESTINATION
            bin
            COMPONENT libraries # Windows DLLs
        PUBLIC_HEADER DESTINATION
            include
            COMPONENT dev
	PRIVATE_HEADER DESTINATION
	    share/ipl/include
	    COMPONENT dev
        FRAMEWORK DESTINATION
            lib
            COMPONENT libraries
        )
