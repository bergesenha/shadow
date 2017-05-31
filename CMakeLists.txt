cmake_minimum_required(VERSION 2.8)


project(shadow CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHADOW_BUILD_TESTS "Build unit tests" ON)

add_executable(main src/main.cpp)
target_include_directories(main
    PRIVATE include/
    )
set_target_properties(main PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_STANDARD 14
    )

if(SHADOW_BUILD_TESTS)
    set(SHADOW_TEST_SRC
        tests/test_main.cpp
        tests/test_any.cpp
        tests/test_free_function_binding.cpp
        tests/test_member_function_binding.cpp
        tests/test_member_variable_binding.cpp
        )

    add_executable(unit_tests ${SHADOW_TEST_SRC})
    target_include_directories(unit_tests
        PRIVATE external/Catch/include/
        PRIVATE include/
        )
    set_target_properties(unit_tests PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_STANDARD 14
        )
endif(SHADOW_BUILD_TESTS)
