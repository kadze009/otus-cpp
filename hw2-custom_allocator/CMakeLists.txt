cmake_minimum_required(VERSION 3.2)

project(custom_allocator VERSION 0.0.1$ENV{TRAVIS_BUILD_NUMBER})

find_package(GTest REQUIRED)

add_executable(custom_allocator main.cpp)
add_executable(gtest_custom_allocator
	test/test_main.cpp
	test/test_custom_alloc.cpp
	test/test_custom_list.cpp
)

set_target_properties(custom_allocator gtest_custom_allocator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(custom_allocator
    PRIVATE "${CMAKE_SOURCE_DIR}"
)

target_include_directories(gtest_custom_allocator
    PRIVATE "${GTEST_INCLUDE_DIRS}"
    PRIVATE "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(gtest_custom_allocator
    GTest::GTest
)

if (MSVC)
    target_compile_options(custom_allocator PRIVATE
        /W4
    )
    target_compile_options(gtest_custom_allocator PRIVATE
        /W4
    )
else ()
    target_compile_options(custom_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest_custom_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS custom_allocator RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT zhaldak.anton@ya.ru)
include(CPack)

enable_testing()

add_test(NAME custom_allocator_test COMMAND gtest_custom_allocator)

