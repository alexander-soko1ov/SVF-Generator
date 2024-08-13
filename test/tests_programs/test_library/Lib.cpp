#include <iostream>
#include <string>
#include "Lib.hpp"

int Lib::sum(int a, int b) {
    return a + b;
}

int Lib::multiply(int a, int b) {
    return a * b;
}

void Lib::print(int result) {
    std::cout << "Результат: " << result << std::endl;
    
}

void DopLib::printResult(char *argv[]) {
    
    int a = std::stoi(argv[1]);
    int b = std::stoi(argv[2]);

    int sumResult = sum(a, b);
    int multiplyResult = multiply(a, b);

    print(sumResult);
    print(multiplyResult);
}
