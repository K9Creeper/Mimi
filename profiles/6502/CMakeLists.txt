add_library(cpu_${TARGET_PROFILE}
    6502.c
)

target_include_directories(cpu_${TARGET_PROFILE}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
