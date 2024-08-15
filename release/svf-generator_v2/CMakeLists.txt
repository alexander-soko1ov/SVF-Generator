# Устанавливаем минимальную версию CMake, необходимую для сборки проекта
cmake_minimum_required(VERSION 3.17)

# Поиск программы clang-tidy в системе. Если clang-tidy найден, он будет использоваться для статического анализа кода.
find_program(CLANG_TIDY clang-tidy clang-tidy-14 clang-tidy-15)
if(CLANG_TIDY)
    # Если clang-tidy был найден, указываем CMake использовать clang-tidy для C и C++ файлов
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY})
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
endif()

# Устанавливаем имя проекта и указываем, что проект использует C++ (по умолчанию это CXX)
project(pininfoProject CXX)

# Добавляем опции компилятора:
# - -Wall: Включает все предупреждения компилятора.
# - -Wextra: Включает дополнительные предупреждения компилятора.
# - -std=c++20: Устанавливает стандарт C++20 для компиляции.
add_compile_options(-Wall -Wextra -std=c++20)

# Добавляем директорию с исходниками
include_directories(${CMAKE_SOURCE_DIR})

# Создаем библиотеку svf-generator_v2 из исходных файлов
add_library(svf-generator_v2 STATIC
    svf-generator_v2.cpp
    svf-generator_v2.hpp
)

# Создаем библиотеку pininfo из исходных файлов
add_library(pininfo STATIC
    pininfo.cpp
    pininfo.hpp
)

# Создаем исполняемый файл с именем main из исходного файла main.cpp
add_executable(main
    main.cpp
)

# Связываем исполняемый файл main с библиотеками svf-generator_v2 и pininfo
target_link_libraries(main PRIVATE svf-generator_v2 pininfo gmp gmpxx)

# Устанавливаем опции компилятора для конкретного исполняемого файла:
# - -g3: Включает отладочную информацию с максимальным уровнем детализации (уровень 3)
target_compile_options(main PRIVATE -g3)
