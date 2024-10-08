# Устанавливаем минимальную версию CMake, необходимую для сборки проекта
cmake_minimum_required(VERSION 3.20)

# Устанавливаем имя проекта и указываем, что проект использует C++
project(pininfoProject CXX)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опции компиляции для различных конфигураций
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3 -g3")

# Включаем покрытие кода для профилирования
option(ENABLE_TEST_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_TEST_COVERAGE)
    message(STATUS "Test coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
endif()

# Не меняем компиляторы для scan-build, используем его отдельно
find_program(SCAN_BUILD scan-build)
if(SCAN_BUILD)
    message(STATUS "Found scan-build, but it should be used externally to CMake.")
endif()

# Поиск и запуск Cppcheck
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    set(CPPCHECK_COMMAND
        ${CPPCHECK}
        --enable=all
        --inconclusive
        --language=c++
        --std=c++17
        --suppress=missingIncludeSystem
        --quiet
        ${PROJECT_SOURCE_DIR}
    )

    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_COMMAND}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running Cppcheck"
    )
endif()

# Добавляем директорию с исходниками
include_directories(${CMAKE_SOURCE_DIR})

# Создаем библиотеку svf-gen_lib из исходных файлов
add_library(svf-gen_lib STATIC
    svf-gen_lib.cpp
    svf-gen_lib.hpp
)

# Создаем библиотеку pininfo_lib из исходных файлов
add_library(pininfo_lib STATIC
    pininfo_lib.cpp
    pininfo_lib.hpp
)

# Создаем библиотеку svf-calc_lib из исходных файлов
add_library(svf-calc_lib STATIC
    svf-calc_lib.cpp
    svf-calc_lib.hpp
)

# Создаем библиотеку json_pars_lib из исходных файлов
add_library(json_pars_lib STATIC
    json_pars_lib.cpp
    json_pars_lib.hpp
)

# Связываем библиотеки json_pars_lib и svf-gen_lib
# target_link_libraries(json_pars_lib PUBLIC svf-gen_lib)
target_link_libraries(svf-gen_lib PUBLIC json_pars_lib)

# Работа с файлом svf-gen
# Создаем исполняемый файл svf-gen
add_executable(svf-gen
    svf-gen.cpp
)

# Связываем библиотеки svf-gen_lib pininfo_lib json_pars_lib с исполняемым файлом svf-gen
target_link_libraries(svf-gen PRIVATE pininfo_lib svf-gen_lib gmp gmpxx)

# Устанавливаем опции компилятора для svf-gen
target_compile_options(svf-gen PRIVATE -g3 -O3)


# Работа с файлом svf-calc
# Создаем исполняемый файл svf-calc
add_executable(svf-calc
    svf-calc.cpp
)

# Связываем библиотеки с исполняемым файлом svf-calc
target_link_libraries(svf-calc PRIVATE svf-calc_lib)
