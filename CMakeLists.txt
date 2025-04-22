cmake_minimum_required(VERSION 3.28.3)

project(mango)

include(GoogleTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/155b337c938a2953e5675f9dc18c99f05f4c85d0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(mango mango.cc)
target_compile_options(mango PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(mango gtest_main)
gtest_discover_tests(mango)
# add_test(NAME mango_test COMMAND mango)

