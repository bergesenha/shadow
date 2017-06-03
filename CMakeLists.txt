cmake_minimum_required(VERSION 2.8)


project(shadow CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHADOW_BUILD_TESTS "Build unit tests" ON)


add_library(shadow INTERFACE)
target_compile_features(shadow 
    INTERFACE cxx_std_14)
target_include_directories(shadow
    INTERFACE include/
    )

add_executable(main src/main.cpp)
target_link_libraries(main shadow)

# unit tests
if(SHADOW_BUILD_TESTS)
    set(SHADOW_TEST_SRC
        tests/test_main.cpp
        tests/test_any.cpp
        tests/test_free_function_binding.cpp
        tests/test_member_function_binding.cpp
        tests/test_member_variable_binding.cpp
        )

    add_executable(unit_tests ${SHADOW_TEST_SRC})
    target_link_libraries(unit_tests shadow)
    target_include_directories(unit_tests
        PRIVATE external/Catch/include/
        )
endif(SHADOW_BUILD_TESTS)
