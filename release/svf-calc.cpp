#include <iostream>
#include <getopt.h>  

#include "svf-calc_lib.hpp"

using json = nlohmann::json;

// Переменные для определения ANSI escape-кодов
// Сброс всех атрибутов
const std::string reset = "\033[0m";

// Цвета текста
const std::string red = "\033[31m";
const std::string magenta = "\033[35m";
const std::string green = "\033[32m";

// Стиль текста
const std::string bold = "\033[1m";

// Функция вывода help
void print_help() {
    std::cout << "Usage: svf-gen [паметры] source destination\n"
            << "Параметры:\n"
            << "  -f, --filename    Указание имени итогового JSON-файла.\n"
            << "                    Возможные расширения .json, [.bsdl, .bsd] - будут преобразованы в namefile_test.json \n"
            << "  -p, --pins        Указание имеён выбранных пинов pin_1, [pin_2, [pin_3, [...]]]\n"
            << "  -w, --write       Указание имеён выбранных пинов write_1, [write_2, [write_3, [...]]]. Допустимые состояния: 1, 0, z\n"
            << "  -r, --read        Указание имеён выбранных пинов read_1, [read_2, [read_3, [...]]]. Допустимые состояния: 1, 0, x\n"
            << "  -h, --help        Отображение этого справочного сообщения\n\n";
}

// Функция парсящая аргументы из командной строки
void parsingArguments(int argc, char *argv[], std::vector<std::vector<std::string>> &pinsList,
                    std::vector<std::vector<std::string>> &writeStatusList, 
                    std::vector<std::vector<std::string>> &readStatusList, std::string &filename) {

    // Локальные перменные
    std::vector<std::string> pins;
    std::vector<std::string> writeStatus;
    std::vector<std::string> readStatus;

    // Опции для getopt_long
    const struct option long_options[] = {
        {"pins", required_argument, 0, 'p'},
        {"write", required_argument, 0, 'w'},
        {"read", required_argument, 0, 'r'},
        {"filename", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}  // Завершающая запись
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:w:r:f:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p': {
                if (!pins.empty()) {
                    pinsList.push_back(pins);
                    writeStatusList.push_back(writeStatus);
                    readStatusList.push_back(readStatus);
                    pins.clear();
                    writeStatus.clear();
                    readStatus.clear();
                }
                std::string pinsStr = optarg;
                size_t pos = 0;
                while ((pos = pinsStr.find(',')) != std::string::npos) {
                    pins.push_back(pinsStr.substr(0, pos));
                    pinsStr.erase(0, pos + 1);
                }
                pins.push_back(pinsStr);
                break;
            }
            case 'w': {
                std::string writeStr = optarg;
                size_t pos = 0;
                while ((pos = writeStr.find(',')) != std::string::npos) {
                    writeStatus.push_back(writeStr.substr(0, pos));
                    writeStr.erase(0, pos + 1);
                }
                writeStatus.push_back(writeStr);
                break;
            }
            case 'r': {
                std::string readStr = optarg;
                size_t pos = 0;
                while ((pos = readStr.find(',')) != std::string::npos) {
                    readStatus.push_back(readStr.substr(0, pos));
                    readStr.erase(0, pos + 1);
                }
                readStatus.push_back(readStr);
                break;
            }
            case 'f':
                filename = optarg;
                break;
            case 'h':
                print_help();
                exit(0);
            default:
                std::cerr << "Неизвестный аргумент: " << opt << std::endl;
                abort();
        }
    }

    if (!pins.empty()) {
        pinsList.push_back(pins);
        writeStatusList.push_back(writeStatus);
        readStatusList.push_back(readStatus);
    }
}

// Основная функция
int main(int argc, char *argv[]) {

    JsonForm JsonForm;

    parsingArguments(argc, argv, JsonForm.pinsList, JsonForm.writeStatusList, JsonForm.readStatusList, JsonForm.filename);

    JsonForm.fileForm(JsonForm.pinsList, JsonForm.writeStatusList, JsonForm.readStatusList, JsonForm.filename);

    return 0;
}
