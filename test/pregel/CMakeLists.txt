PROJECT(tests_message_id CXX)

file(GLOB ${PROJECT_NAME}_HEADERS
        ./*.hpp
        ./*.h
)

file(GLOB ${PROJECT_NAME}_SOURCES
        ./*.cpp
)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG        v1.14.0 # Specify a stable tag
)
FetchContent_MakeAvailable(googletest)

message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")

add_executable(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME}
        PUBLIC actor-zeta
        PUBLIC CONAN_PKG::catch2
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
