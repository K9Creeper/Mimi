add_subdirectory(mem)
add_subdirectory(rom)

add_library(bus_dev
    $<TARGET_OBJECTS:mem>
    $<TARGET_OBJECTS:rom>
)

target_include_directories(bus_dev
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)