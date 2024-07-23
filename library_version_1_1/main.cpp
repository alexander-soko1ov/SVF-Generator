#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

# include "pininfo.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd и флаг all или non" << std::endl;
        return 1;
    }
    
    // Получаем имя файла
    std::string filename = argv[1];

    BsdlPins BsdlPins;

    // Вызываем методы загрузки и обработки bsd
    BsdlPins.loadBsdl(filename);

    // Выводим информацию о пинах
    BsdlPins.printPinInfo();

    return 0;
}
