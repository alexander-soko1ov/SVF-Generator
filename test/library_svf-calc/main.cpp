#include <iostream>
#include "svf-calc.hpp"

using json = nlohmann::json;

// Основная функция
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Общая структура: --filename \"name_file_BSD.bsd\" --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    } else if (argc < 4) {
        std::cerr << "Структура пинов: --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    }

    JsonForm JsonForm;

    JsonForm.fileForm(argc, argv);

    return 0;
}
