#include <iostream>

#include "svf-generator.hpp"
#include "pininfo.hpp"

int main(int argc, char* argv[]) {

    // Получаем имя файла
    std::string filename = argv[1];

    BsdlPins BsdlPins;

    // Вызываем методы загрузки и обработки bsd
    BsdlPins.loadBsdl(filename);

    StateArg StateArg;

    // Парсим аргументы введённые при запуске и создаём SVF-файл 
    StateArg.formation_svf(argc, argv);
    
    return 0;
}
