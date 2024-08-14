#include <iostream>
#include <vector>
#include "svf-generator_v2.hpp"
#include "pininfo.hpp"


int main(int argc, char *argv[]) {

    // Создаём экземляры класса
    PinJson PinJson;
    BsdlPins BsdlPins;

    // Создаём vector для хранения и передачи имён файлов
    std::vector<std::string> pars_arg = PinJson.parse_arguments(argc, argv);

    // Записываем имена файлов в соответсвующие им переменные
    std::string filename_bsdl = pars_arg[0];
    std::string filename_json = pars_arg[1];

    // Загружаем BSDL-файл, парсим его и записываем данные в переменные
    BsdlPins.loadBsdl(filename_bsdl);

    // Получаем вектор пинов
    const std::vector<BsdlPins::PinInfo>& pins = BsdlPins.getPins();

    // Выводим информацию о пинах
    BsdlPins.printPinInfo(pins);

    // Читаем данные из JSON и записываем их в переменные
    PinJson.svfGen(filename_json);

    // Создаём SVF-файл 
    PinJson.createFile(filename_json);

    return 0;
}
