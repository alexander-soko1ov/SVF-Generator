#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

# include "pininfo.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <bsd-file> " << std::endl;
        return 1;
    }
    
    // Получаем имя файла
    std::string filename = argv[1];

    BsdlPins BsdlPins;

    // Вызываем методы загрузки и обработки bsd
    BsdlPins.loadBsdl(filename);

    // Получаем вектор пинов
    const std::vector<BsdlPins::PinInfo>& pins = BsdlPins.getPins();

    // Выводим информацию о пинах
    BsdlPins.printPinInfo(pins);

    return 0;
}
