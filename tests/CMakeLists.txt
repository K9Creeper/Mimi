add_executable(MimiTests
    test_mimi.cpp
)

target_link_libraries(MimiTests
    PRIVATE
        Mimi
        cpu_${TARGET_PROFILE} 
        bus_dev
)

add_test(
    NAME MimiTests
    COMMAND MimiTests
)
