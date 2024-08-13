# Создаем библиотеку lib1
add_library(lib1 STATIC lib1.cpp)

# Указываем, что заголовочные файлы находятся в этой директории
target_include_directories(lib1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
