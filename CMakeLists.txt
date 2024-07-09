cmake_minimum_required(VERSION 3.17)

find_program(CLANG_TIDY clang-tidy clang-tidy-14 clang-tidy-15)
if(CLANG_TIDY)
set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY})
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
endif()

project (parctic CXX)

add_compile_options(-Wall -Wextra -std=c++20 )

add_executable(main main.cpp)
target_compile_options(main PRIVATE -g3)
