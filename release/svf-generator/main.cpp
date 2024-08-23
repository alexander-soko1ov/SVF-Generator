#include <iostream>
#include <vector>

#include "svf-generator.hpp"
#include "pininfo.hpp"


int main(int argc, char *argv[]) {

    // Создаём экземляры класса
    PinJson PinJson;
    BsdlPins BsdlPins;

    // Создаём vector для хранения и передачи имён файлов
    const std::vector<std::string>& filename = PinJson.parse_arguments(argc, argv);

    // Записываем имена файлов в соответсвующие им переменные
    std::string filename_bsdl = filename[0];
    std::string filename_json = filename[1];

    // Загружаем BSDL-файл, парсим его и записываем данные в переменные
    BsdlPins.loadBsdl(filename_bsdl);

    // Получаем данные о длине регистра BSDL
    size_t register_length_bsdl = BsdlPins.boundaryLength(filename_bsdl);
    // std::cout << "Длина регистра BSDL: " << register_length_bsdl << "\n" << std::endl;

    // Получаем данные о длине регистра BSDL
    size_t register_length_instr = BsdlPins.instructionLength(filename_bsdl);
    // std::cout << "Длина регистра инструкций: " << register_length_instr << "\n" << std::endl;

    // Получаем данные о коде для запуска EXTEST
    std::string opcode_extest = BsdlPins.opcodeEXTEST(filename_bsdl);
    // std::cout << "Код для запуска EXTEST: " << opcode_extest << "\n" << std::endl;
    if(register_length_instr != opcode_extest.length()) {
        std::cerr << PinJson.red << "Несоответсвие длины регистра инструкций и команды EXTEST" << PinJson.reset << std::endl;
        abort();
    }

    // Получаем вектор пинов
    const std::vector<BsdlPins::PinInfo>& cells = BsdlPins.getCells();
    const std::vector<BsdlPins::PinInfo>& pins = BsdlPins.getPins();

    // Выводим информацию о ячейках и пинах
    // std::cout << "\nВывод ячеек:\n";
    // BsdlPins.printPinInfo(cells);
    // std::cout << "\nВывод пинов:\n";
    // BsdlPins.printPinInfo(pins);

    // Читаем данные из JSON и записываем их в переменные
    PinJson.svfGen(filename_json);

    // Тестовый вывод пинов из json-файла
    PinJson.print_json();

    // Выводим пины, записанные в JSON-файле
    PinJson.print_pins();

    // Создаём SVF-файл 
    PinJson.createFile(filename_json, register_length_bsdl, register_length_instr, opcode_extest, cells, 1);

    return 0;
}
