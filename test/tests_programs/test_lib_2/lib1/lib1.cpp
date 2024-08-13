#include "lib1.hpp"
#include <string>

namespace lib1 {

    // Реализация функции add, которая возвращает сумму двух чисел
    std::string add(char *argv[]) {
        std::string a = argv[1];
        std::string b = argv[2];

        return a + b;
    }

}
