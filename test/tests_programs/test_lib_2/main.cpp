#include "lib1.hpp"
#include "lib2.hpp"
#include <string>
#include <string_view>

int main(int argc, char *argv[]) {
    
    // Используем lib1 для вычисления суммы
    std::string res = lib1::func(argv);
    
    // Передаем результат в lib2 для возведения в квадрат и вывода
    lib2::squareAndPrint(std::string_view res);

    return 0;
}
