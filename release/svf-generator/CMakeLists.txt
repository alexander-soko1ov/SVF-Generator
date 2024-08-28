# Устанавливаем минимальную версию CMake, необходимую для сборки проекта
cmake_minimum_required(VERSION 3.17)

# Устанавливаем уровень оптимизации для всего проекта
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

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

# Создаем библиотеку svf-generator из исходных файлов
add_library(svf-generator STATIC
    svf-generator.cpp
    svf-generator.hpp
)

# Создаем библиотеку pininfo из исходных файлов
add_library(pininfo STATIC
    pininfo.cpp
    pininfo.hpp
)

# Создаем библиотеку svf-calculator из исходных файлов
add_library(svf-calculator STATIC
    svf-calculator.cpp
    svf-calculator.hpp
)

# Создаем исполняемый файл с именем svf-gen из исходного файла svf-gen.cpp
add_executable(svf-gen
    svf-gen.cpp
)

# Связываем исполняемый файл svf-gen с библиотеками svf-generator и pininfo, а также с библиотеками gmp и gmpxx
target_link_libraries(svf-gen PRIVATE svf-generator pininfo gmp gmpxx)

# Устанавливаем опции компилятора для исполняемого файла svf-gen:
# - -g3: Включает отладочную информацию с максимальным уровнем детализации (уровень 3)
target_compile_options(svf-gen PRIVATE -g3 -O3)

# Создаем исполняемый файл с именем svf-calc из исходного файла svf-calc.cpp
add_executable(svf-calc
    svf-calc.cpp
)

# Связываем исполняемый файл svf-calc с библиотекой svf-calculator
target_link_libraries(svf-calc PRIVATE svf-calculator)

# Устанавливаем опции компилятора для исполняемого файла svf-calc:
# - -g3: Включает отладочную информацию с максимальным уровнем детализации (уровень 3)
target_compile_options(svf-calc PRIVATE -g3 -O2)
