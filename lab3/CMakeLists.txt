cmake_minimum_required(VERSION 3.20)
project(lab3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lab3 hanoi.cpp board.cpp board.h canvas.h disk.h node.h queue.h rod.h stack.h
        unique_ptr.h
        unique_ptr_impl.h
        stack_impl.h
        queue_impl.h
        rod.cpp
)

include(FetchContent)
FetchContent_Declare(
        googletest
        # 指定googletest的仓库URL
        GIT_REPOSITORY https://github.com/google/googletest.git
        # 你可以指定一个特定的提交、分支或标签
        GIT_TAG v1.14.x
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

add_executable(
        lab3_test
        lab3_test.cpp
)
target_link_libraries(
        lab3_test
        GTest::gtest_main
)

gtest_discover_tests(lab3_test)