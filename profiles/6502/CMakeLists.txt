add_library(cpu_${TARGET_PROFILE}
    src/6502.c
    src/opcodes.c
)

target_include_directories(cpu_${TARGET_PROFILE}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        include
)
