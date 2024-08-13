# Создаем библиотеку lib2
add_library(lib2 STATIC lib2.cpp)

# Указываем зависимость от lib1
target_link_libraries(lib2 PRIVATE lib1)

# Указываем, что заголовочные файлы находятся в этой директории
target_include_directories(lib2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
