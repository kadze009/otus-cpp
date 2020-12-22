cmake_minimum_required(VERSION 3.2)

project(bulk VERSION 0.0.1$ENV{TRAVIS_BUILD_NUMBER})

find_package(GTest REQUIRED)

add_executable(bulk
        main.cpp
        StdinCommandHandler.cpp
        StdoutBulkHandler.cpp
        FileBulkHandler.cpp
    )
add_executable(gtest_bulk
		test/test_main.cpp
		StdinCommandHandler.cpp
		StdoutBulkHandler.cpp
		FileBulkHandler.cpp
	)

set_target_properties(bulk gtest_bulk PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bulk
    PRIVATE "${CMAKE_SOURCE_DIR}"
    PRIVATE "${CMAKE_SOURCE_DIR}/common"
)

target_include_directories(gtest_bulk
    PRIVATE "${GTEST_INCLUDE_DIRS}"
    PRIVATE "${CMAKE_SOURCE_DIR}"
    PRIVATE "${CMAKE_SOURCE_DIR}/common"
)

target_link_libraries(gtest_bulk
    GTest::GTest
)

if (MSVC)
    target_compile_options(bulk PRIVATE
        /W4
    )
    target_compile_options(gtest_bulk PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest_bulk PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS bulk RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT zhaldak.anton@ya.ru)
include(CPack)

enable_testing()

add_test(NAME bulk_test COMMAND gtest_bulk)

