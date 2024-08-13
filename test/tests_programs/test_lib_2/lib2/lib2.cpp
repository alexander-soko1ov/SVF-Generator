#include "lib2.hpp"
#include <iostream>

namespace lib2 {

    // Реализация функции squareAndPrint, которая выводит квадрат числа
    void squareAndPrint(std::string_view& number) {
        std::string result = number * number;
        std::cout << "Square of " << number << " is " << result << std::endl;
    }

}
