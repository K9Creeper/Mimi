add_executable(MimiTests
    test_mimi.cpp
)

target_link_libraries(MimiTests
    PRIVATE
        Mimi
        ${TARGET_PROFILE} 
)

add_test(
    NAME MimiTests
    COMMAND MimiTests
)
