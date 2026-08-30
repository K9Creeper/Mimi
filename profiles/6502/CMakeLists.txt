add_library(${TARGET_PROFILE}
    src/cpu/6502.c
    src/cpu/opcodes.c

    src/bus-dev/mem.c
    src/bus-dev/rom.c
)

target_include_directories(${TARGET_PROFILE}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        include
)
