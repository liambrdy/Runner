cmake_minimum_required(VERSION 3.14)

project(Runner
    VERSION 0.1.0
    LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(CompilerWarnings)

add_library(project_warnings INTERFACE)
get_compiler_warnings(project_warnings)

file(GLOB GAME_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB GAME_INCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h")

if(WIN32)
    file(GLOB PLATFORM_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/win32/*.cpp")
    file(GLOB PLATFORM_INCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/win32/*.h")
elseif(UNIX)
    file(GLOB PLATFORM_SRCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/linux/*.cpp")
    file(GLOB PLATFORM_INCS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/linux/*.h")
endif()

add_executable(runner WIN32
    ${PLATFORM_SRCS} ${PLATFORM_INCS}
    ${GAME_SRCS} ${GAME_INCS}
)

target_include_directories(runner
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    PUBLIC ${CMAKE_SOURCE_DIR}/src/ext
)

target_link_libraries(runner
    PUBLIC project_warnings

    PUBLIC opengl32.lib
    PUBLIC winmm.lib
)