#include <iostream>
#include <getopt.h>

#include "pininfo.hpp"
#include "svf-generator.hpp"

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
void print_usage() {
    std::cout << "Usage: svf-gen [паметры] source destination\n"
            << "Основные параметры:\n"
            << "  -b, --bsdl     Указание имени BSDL-файла. Допустимые расширения: .bsd, .bsdl\n"
            << "  -j, --json     Указание имени JSON-файла. Допустимое расширение: .json\n"
            << "\nДополнительные параметры:\n"
            << "  -s, --svf      Указание имени SVF-файла. Допустимое расширение: .svf\n"
            << "  -t, --trst     Состояние TRST. Допустимые состояния: ON, OFF, z, ABSENT\n"
            << "  -i, --endir    Состояние ENDIR. Допустимые состояния: IRPAUSE, DRPAUSE, RESET, IDLE\n"
            << "  -d, --enddr    Состояние ENDDR. Допустимые состояния: IRPAUSE, DRPAUSE, RESET, IDLE\n"
            << "  -r, --runtest  Указание количества тиков ожидания. По умолчанию 100 TCK\n"
            << "  -h, --help     Отображение этого справочного сообщения\n\n";
}

// Функция проверки корректности id введённого в CLI аргументов
bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (state == valid_states[i]) {
            return true;
        }
    }
    return false;
}

// Функция проверки расширения файла
bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos && pos != filename.length() - 1) {
        std::string fileExt = filename.substr(pos + 1);
        return validExtensions.find(fileExt) != validExtensions.end();
    }
    return false;
}

// Функция для обработки аргументов командной строки
void parse_arguments(int argc, char *argv[], std::string& filename_bsdl, 
                    std::string&  filename_json, std::string& filename_svf,
                    std::string& trst_state, std::string& endir_state, 
                    std::string& enddr_state, std::string& runtest_state){

    // Инициализация вспомагательных переменных
    int option_index = 0;
    int c;

    PinJson PinJson;

    // Инициализация доступных аргументов
    static struct option long_options[] = {
        {"bsdl", required_argument, 0, 'b'},
        {"json", required_argument, 0, 'j'},
        {"svf", required_argument, 0, 's'},
        {"trst", required_argument, 0, 't'},
        {"endir", required_argument, 0, 'i'},
        {"enddr", required_argument, 0, 'd'},
        {"runtest", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "Z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    // Цикл проверки всех переданных аргументов
    while ((c = getopt_long(argc, argv, "b:j:s:t:i:d:r:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b':
                filename_bsdl = optarg;
                if (!has_extension(filename_bsdl,{"bsd","bsdl"})) {
                    std::cerr << red << "Неверное расширение BSDL-файла (.bsd)" << reset << std::endl;
                    abort();
                }
                break;
            case 'j':
                filename_json = optarg;
                if (!has_extension(filename_json, {"json"})) {
                    std::cerr << red << "Неверное расширение JSON-файла (.json)" << reset << std::endl;
                    abort();
                }
                break;
            case 's':
                filename_svf = optarg;
                if (!has_extension(filename_svf, {"svf"})) {
                    std::cerr << red << "Неверное расширение SVF-файла (.svf)" << reset << std::endl;
                    abort();
                }
                break;
            case 't':
                trst_state = optarg;
                if (!is_valid_state(trst_state, trst_states, 4)) {
                    std::cerr << red << "Неверное состояние --trst. Возможные состояния ON, OFF, Z, or ABSENT." << reset << std::endl;
                    abort();
                }
                break;
            case 'i':
                endir_state = optarg;
                if (!is_valid_state(endir_state, endir_enddr_states, 4)) {
                    std::cerr << red << "Неверное состояние --endir. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE." << reset << std::endl;
                    abort();
                }
                break;
            case 'd':
                enddr_state = optarg;
                if (!is_valid_state(enddr_state, endir_enddr_states, 4)) {
                    std::cerr << red << "Неверное состояние --enddr. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE." << reset << std::endl;
                    abort();
                }
                break;
            case 'r':
                runtest_state = optarg;
                break;
            case 'h':
                print_usage();
                exit(0);
            default:
                abort();
        }
    }
}


int main(int argc, char *argv[]) {
    
    // Создаём экземляры класса
    PinJson PinJson;
    BsdlPins BsdlPins;

    // Создаём vector для хранения и передачи имён файлов
    parse_arguments(argc, argv, PinJson.filename_bsdl, PinJson.filename_json, PinJson.filename_svf,
                    PinJson.trst_state, PinJson.endir_state, PinJson.enddr_state, PinJson.runtest_state);

    // Загружаем BSDL-файл, парсим его и записываем данные в переменные
    BsdlPins.loadBsdl(PinJson.filename_bsdl);

    // Получаем данные о длине регистра BSDL
    size_t register_length_bsdl = BsdlPins.boundaryLength(PinJson.filename_bsdl);

    // Получаем данные о длине регистра BSDL
    size_t register_length_instr = BsdlPins.instructionLength(PinJson.filename_bsdl);

    // Получаем данные о коде для запуска EXTEST
    std::string opcode_extest = BsdlPins.opcodeEXTEST(PinJson.filename_bsdl, register_length_instr);    

    // Получаем вектор пинов
    const std::vector<BsdlPins::PinInfo>& cells = BsdlPins.getCells();
    const std::vector<BsdlPins::PinInfo>& pins = BsdlPins.getPins(); // Пока что отключено за ненадобностью

    // Выводим информацию о ячейках и пинах
    // std::cout << "\nВывод ячеек:\n";
    // BsdlPins.printPinInfo(cells);
    // 
    // std::cout << "\nВывод пинов:\n";
    BsdlPins.printPinInfo(pins);

    // Читаем данные из JSON и записываем их в переменные
    PinJson.svfGen(PinJson.filename_json);

    // Тестовый вывод пинов из json-файла
    PinJson.print_json(PinJson.filename_bsdl, PinJson.filename_json, PinJson.filename_svf, 
                        PinJson.trst_state, PinJson.endir_state, PinJson.enddr_state, PinJson.runtest_state);

    // Выводим пины, записанные в JSON-файле
    PinJson.print_pins();

    // Создаём SVF-файл 
    PinJson.createFile(PinJson.filename_json, register_length_bsdl, PinJson.filename_svf, register_length_instr, opcode_extest, 
                        cells, PinJson.trst_state, PinJson.endir_state, PinJson.enddr_state, PinJson.runtest_state, 2);

    return 0;
}
