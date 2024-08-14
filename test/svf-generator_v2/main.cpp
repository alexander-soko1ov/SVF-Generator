#include <iostream>
#include <vector>
#include "svf-generator_v2.hpp"
#include "pininfo.hpp"


int main(int argc, char *argv[]) {

    PinJson PinJson;
    BsdlPins BsdlPins;

    std::vector<std::string> filename = PinJson.parse_arguments(argc, argv);

    std::string filename_bsdl = filename[0];
    std::string filename_json = filename[1];

    BsdlPins.loadBsdl(filename_bsdl);

    // Получаем вектор пинов
    const std::vector<BsdlPins::PinInfo>& pins = BsdlPins.getPins();

    // Выводим информацию о пинах
    BsdlPins.printPinInfo(pins);

    PinJson.svfGen(filename_json);

    return 0;
}
