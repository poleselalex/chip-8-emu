cmake_minimum_required(VERSION 3.8)
project("chip-8-emulator")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB SRCs
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB ROMs
    "${PROJECT_SOURCE_DIR}/roms/*"
)

find_package(SDL2 REQUIRED)
add_executable(chip-8-emulator ${SRCs} ${ROMs})
target_link_libraries(chip-8-emulator SDL2::SDL2) 
