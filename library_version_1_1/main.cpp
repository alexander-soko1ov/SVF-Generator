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

    // Можем использовать дополнительный флаг для включения/выключения печати доп. данных о пинах (частично вырезан)
    // std::string flag = (argc > 2) ? argv[2] : "non";


    // private (закинуть в один метод)
        // Читаем данные из файла и записываем в переменную content
        std::string content = readFile(filename);

        // Получаем даныне о пинах и заносим данные в переменную ping (vector)
        std::vector<PinInfo> pins = parseBSDFile(content);

        // Получаем данные об имени пина и его номере
        std::unordered_map<std::string, std::string> pinMap = parsePinMap(content);
        
        // Получаем данные об имени пина и его типе
        std::unordered_map<std::string, std::string> pinTypes = parsePinTypes(content);

        // Устанавливаем связь номеров пинов и их типов
        mapPinNumbersAndTypes(pins, pinMap, pinTypes);

        // Удаляем дублирующиеся пины
        pins = removeDuplicatePins(pins);

    // Выводим информацию о пинах
    printPinInfo(pins);

    return 0;
}
