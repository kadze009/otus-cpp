cmake_minimum_required(VERSION 3.2)

project(ip_filter VERSION 0.0.1$ENV{TRAVIS_BUILD_NUMBER})

find_package(GTest REQUIRED)

add_executable(ip_filter main.cpp utils.cpp IpV4_c.cpp)
add_executable(gtest_ip_filter test/test_main.cpp utils.cpp IpV4_c.cpp)

set_target_properties(ip_filter gtest_ip_filter PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(ip_filter
    PRIVATE "${CMAKE_SOURCE_DIR}"
)

target_include_directories(gtest_ip_filter
    PRIVATE "${GTEST_INCLUDE_DIRS}"
    PRIVATE "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(gtest_ip_filter
    GTest::GTest
)

if (MSVC)
    target_compile_options(ip_filter PRIVATE
        /W4
    )
    target_compile_options(gtest_ip_filter PRIVATE
        /W4
    )
else ()
    target_compile_options(ip_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest_ip_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS ip_filter RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT zhaldak.anton@ya.ru)
include(CPack)

enable_testing()

add_test(NAME IpV4_and_utils COMMAND gtest_ip_filter)

