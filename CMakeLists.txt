cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

include(GetGitRevisionDescription)
git_describe(GIT_VERSION --tags)

#parse the version information into pieces.
string(REGEX REPLACE "^V([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION}")
string(REGEX REPLACE "^V[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION}")
string(REGEX REPLACE "^V[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION}")
string(REGEX REPLACE "^V[0-9]+\\.[0-9]+\\.[0-9]+-(.*)" "\\1" VERSION_BUILD "${GIT_VERSION}")
set(CMAKE_PROJECT_BUILD VERSION_BUILD)

set(GIT_VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message(STATUS ${CMAKE_BUILD_TYPE})
if(NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "Build type not specified, using DEBUG as default")
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(ipl
    LANGUAGES C
        VERSION ${GIT_VERSION_SHORT}
        )

set(CMAKE_C_STANDARD 99)

include(CheckCCompilerFlag)

set(CMAKE_C_FLAGS "")

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"))
  # using Clang

  check_c_compiler_flag("-O0" FLAG_OPTIMIZATION_O0)
    if (${FLAG_OPTIMIZATION_O0})
        message(STATUS "Compiler -O0 optimization flag is working!")
    else()
        message(WARNING "Compiler -O0 optimization flag is not working!")
    endif()

    check_c_compiler_flag("-Ofast" FLAG_OPTIMIZATION_OFAST)
    if (${FLAG_OPTIMIZATION_OFAST})
        message(STATUS "Compiler -Ofast optimization flag is working!")
    else()
        message(WARNING "Compiler -Ofast optimization flag is not working!")
    endif(
    )
    check_c_compiler_flag("-Og" FLAG_OPTIMIZATION_OG)
    if (${FLAG_OPTIMIZATION_OG})
        message(STATUS "Compiler -Og optimization flag is working!")
    else()
        message(WARNING "Compiler -Og optimization flag is not working!")
    endif()

    check_c_compiler_flag("-Oz" FLAG_OPTIMIZATION_OZ)
    if (${FLAG_OPTIMIZATION_OZ})
        message(STATUS "Compiler -Oz optimization flag is working!")
    else()
        message(WARNING "Compiler -Oz optimization flag is not working!")
    endif()

    check_c_compiler_flag("-g" FLAG_DEBUG_G)
    if (${FLAG_DEBUG_G})
        message(STATUS "Compiler -g debugging flag is working!")
    else()
        message(WARNING "Compiler -g debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g0" FLAG_DEBUG_G0)
    if (${FLAG_DEBUG_G0})
        message(STATUS "Compiler -g0 debugging flag is working!")
    else()
        message(WARNING "Compiler -g0 debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g1" FLAG_DEBUG_G1)
    if (${FLAG_DEBUG_G1})
        message(STATUS "Compiler -g1 debugging flag is working!")
    else()
        message(WARNING "Compiler -g1 debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g3" FLAG_DEBUG_G3)
    if (${FLAG_DEBUG_G3})
        message(STATUS "Compiler -g3 debugging flag is working!")
    else()
        message(WARNING "Compiler -g3 debugging flag is not working!")
    endif()

    check_c_compiler_flag("--coverage" FLAG_PROFILING_COVERAGE)
    if (${FLAG_PROFILING_ARCS})
        message(STATUS "Compiler --coverage profiling flag is working!")
    else()
        message(WARNING "Compiler --coverage  profiling flag is not working!")
    endif()

    check_c_compiler_flag("-Weverything" FLAG_DIAGNOSTIC_EVERYTHING)
    if (${FLAG_DIAGNOSTIC_EVERYTHING})
        message(STATUS "Compiler -Weverything diagnostic flag is working!")
    else()
        message(WARNING "Compiler -Weverything diagnostic flag is not working!")
    endif()

    check_c_compiler_flag("-pedantic" FLAG_DIAGNOSTIC_PEDANTIC)
    if (${FLAG_DIAGNOSTIC_PEDANTIC})
        message(STATUS "Compiler -pedantic diagnostic flag is working!")
    else()
        message(WARNING "Compiler -pedantic diagnostic flag is not working!")
    endif()

    set(CMAKE_C_FLAGS_DEBUG
        "-std=gnu99 -Weverything -pedantic -O0 -g -g3 --coverage")
    set(CMAKE_C_FLAGS_RELEASE
        "-std=gnu99 -Weverything -Ofast -g -g0")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO
        "-std=gnu99 -Weverything -Og -g -g1")
    set(CMAKE_C_FLAGS_MINSIZEREL
        "-std=gnu99 -Weverything -Oz -g0")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # using GCC

    check_c_compiler_flag("-O0" FLAG_OPTIMIZATION_O0)
    if (${FLAG_OPTIMIZATION_O0})
        message(STATUS "Compiler -O0 optimization flag is working!")
    else()
        message(WARNING "Compiler -O0 optimization flag is not working!")
    endif()

    check_c_compiler_flag("-Ofast" FLAG_OPTIMIZATION_OFAST)
    if (${FLAG_OPTIMIZATION_OFAST})
        message(STATUS "Compiler -Ofast optimization flag is working!")
    else()
        message(WARNING "Compiler -Ofast optimization flag is not working!")
    endif()

        check_c_compiler_flag("-Og" FLAG_OPTIMIZATION_OG)
    if (${FLAG_OPTIMIZATION_OG})
        message(STATUS "Compiler -Og optimization flag is working!")
    else()
        message(WARNING "Compiler -Og optimization flag is not working!")
    endif()

    check_c_compiler_flag("-Os" FLAG_OPTIMIZATION_OS)
    if (${FLAG_OPTIMIZATION_OS})
        message(STATUS "Compiler -Os optimization flag is working!")
    else()
        message(WARNING "Compiler -Os optimization flag is not working!")
    endif()

    check_c_compiler_flag("-g" FLAG_DEBUG_G)
    if (${FLAG_DEBUG_G})
        message(STATUS "Compiler -g debugging flag is working!")
    else()
        message(WARNING "Compiler -g debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g0" FLAG_DEBUG_G0)
    if (${FLAG_DEBUG_G0})
        message(STATUS "Compiler -g0 debugging flag is working!")
    else()
        message(WARNING "Compiler -g0 debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g1" FLAG_DEBUG_G1)
    if (${FLAG_DEBUG_G1})
        message(STATUS "Compiler -g1 debugging flag is working!")
    else()
        message(WARNING "Compiler -g1 debugging flag is not working!")
    endif()

    check_c_compiler_flag("-g3" FLAG_DEBUG_G3)
    if (${FLAG_DEBUG_G3})
        message(STATUS "Compiler -g3 debugging flag is working!")
    else()
        message(WARNING "Compiler -g3 debugging flag is not working!")
    endif()

    check_c_compiler_flag("--coverage" FLAG_PROFILING_COVERAGE)
    if (${FLAG_PROFILING_ARCS})
        message(STATUS "Compiler --coverage profiling flag is working!")
    else()
        message(WARNING "Compiler --coverage  profiling flag is not working!")
    endif()

    check_c_compiler_flag("-Wall" FLAG_DIAGNOSTIC_EVERYTHING)
    if (${FLAG_DIAGNOSTIC_EVERYTHING})
        message(STATUS "Compiler -Wall diagnostic flag is working!")
    else()
        message(WARNING "Compiler -Wall diagnostic flag is not working!")
    endif()

    check_c_compiler_flag("-Wpedantic" FLAG_DIAGNOSTIC_PEDANTIC)
    if (${FLAG_DIAGNOSTIC_PEDANTIC})
        message(STATUS "Compiler -Wpedantic diagnostic flag is working!")
    else()
        message(WARNING "Compiler -Wpedantic diagnostic flag is not working!")
    endif()

    check_c_compiler_flag("--std=gnu99" FLAG_STANDARD_99)
    if (${FLAG_STANDARD_99})
        message(STATUS "Compiler --std=gnu99 flag is working!")
    else()
        message(WARNING "Compiler --std=gnu99 flag is not working!")
    endif()

        set(CMAKE_C_FLAGS_DEBUG 
        "--std=gnu99 -Wall -Wpedantic -O0 -g3 -fexceptions --coverage"
    )
    set(CMAKE_C_FLAGS_RELEASE
        "--std=gnu99 -Wall -Ofast -g0 -DNDEBUG"
    )
    set(CMAKE_C_FLAGS_RELWITHDEBINFO
        "--std=gnu99 -Wall -O2 -g -DNDEBUG"
    )
    set(CMAKE_C_FLAGS_MINSIZEREL
        "--std=gnu99 -Wall -Os -DNDEBUG"
    )
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
  message(FATAL "Intel compiler not supported yet!")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++

    check_c_compiler_flag("/Od" FLAG_OPTIMIZATION_OD)
    if (${FLAG_OPTIMIZATION_OD})
        message(STATUS "Compiler /Od optimization flag is working!")
    else()
        message(WARNING "Compiler /Od optimization flag is not working!")
    endif()

    check_c_compiler_flag("/Ox" FLAG_OPTIMIZATION_OX)
    if (${FLAG_OPTIMIZATION_OX})
        message(STATUS "Compiler /Ox optimization flag is working!")
    else()
        message(WARNING "Compiler /Ox optimization flag is not working!")
    endif(
    )
    check_c_compiler_flag("/O1" FLAG_OPTIMIZATION_O1)
    if (${FLAG_OPTIMIZATION_O1})
        message(STATUS "Compiler /O1 optimization flag is working!")
    else()
        message(WARNING "Compiler /O1 optimization flag is not working!")
    endif()

    check_c_compiler_flag("/Os /Og" FLAG_OPTIMIZATION_OS)
    if (${FLAG_OPTIMIZATION_OS})
        message(STATUS "Compiler /Os optimization flag is working!")
    else()
        message(WARNING "Compiler /Os optimization flag is not working!")
    endif()

    check_c_compiler_flag("/DEBUG:FULL" FLAG_DEBUG)
    if (${FLAG_DEBUG})
        message(STATUS "Compiler /DEBUG debugging flag is working!")
    else()
        message(WARNING "Compiler /DEBUG debugging flag is not working!")
    endif()

    check_c_compiler_flag("/Z7" FLAG_OPTIMIZATION_Z7)
    if (${FLAG_OPTIMIZATION_Z7})
        message(STATUS "Compiler /Z7 debugging flag is working!")
    else()
        message(WARNING "Compiler /Z7 debugging flag is not working!")
    endif()

    check_c_compiler_flag("/Zi" FLAG_OPTIMIZATION_ZI)
    if (${FLAG_OPTIMIZATION_ZI})
        message(STATUS "Compiler /ZI debugging flag is working!")
    else()
        message(WARNING "Compiler /ZI debugging flag is not working!")
    endif()

    check_c_compiler_flag("/PROFILE" FLAG_OPTIMIZATION_ZI)
    if (${FLAG_OPTIMIZATION_ZI})
        message(STATUS "Compiler /PROFILE debugging flag is working!")
    else()
        message(WARNING "Compiler /PROFILE debugging flag is not working!")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG
        "/Wall /Od /DEBUG:FULL /ZI /PROFILE"
    )
    set(CMAKE_CXX_FLAGS_RELEASE
        "/Wall /Ox"
    )
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
        "/Wall /O1 /g /DEBUG:FASTLINK"
    )
    set(CMAKE_CXX_FLAGS_MINSIZEREL
        "/Wall /Os /Og"
    )
endif()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)

# add a target to generate API documentation with Doxygen
find_package(Doxygen REQUIRED)

CONFIGURE_FILE(
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_BINARY_DIR}/README.txt
        COPYONLY
    )

#Set cpack general settings
set(CPACK_PACKAGE_NAME     "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION  "${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR   "Microchip Inc.")
set(CPACK_PACKAGE_CONTACT  "Roland Tissl <roland.tissl@microchip.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}"
   )

#set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/icon.png")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_BINARY_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION
    ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}
   )
set(CPACK_PACKAGE_FILE_NAME
    ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}
   )

message(STATUS "CPACK_PACKAGE_FILE_NAME: " ${CPACK_PACKAGE_FILE_NAME})

#Debian package options
set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)

set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_RPM_PACKAGE_SUMMARY ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_RPM_PACKAGE_ARCHITECTURE})
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3+")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_CONTACT})
set(CPACK_RPM_PACKAGE_DESCRIPTION "${CMAKE_CURRENT_BINARY_DIR}/README.txt")

include(CPack)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ipl/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ipl/cfg)
set(IPL_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/ipl/inc
   )
set(IPL_CONFIGURATION_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/ipl/cfg
 )
 
option(IPL_EXAMPLE_RPI "Build raspberry pi example" ON)
option(IPL_EXAMPLE_WIN "Build windows aardvarg usb example" OFF)

if(${IPL_EXAMPLE_RPI} AND ${IPL_EXAMPLE_WIN})
	message(FATAL "Only one example can be build not both")
endif()

add_subdirectory(ipl)
add_subdirectory(example)

