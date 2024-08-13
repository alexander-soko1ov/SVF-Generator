cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем поддиректории для библиотек
add_subdirectory(lib1)
add_subdirectory(lib2)

# Основной исполняемый файл
add_executable(main_exec main.cpp)

# Связываем исполняемый файл с библиотеками
target_link_libraries(main_exec PRIVATE lib1 lib2)
