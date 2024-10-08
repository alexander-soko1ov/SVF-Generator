#include <iostream>
#include <getopt.h>

#include "pininfo_lib.hpp"
#include "json_pars_lib.hpp"
#include "svf-gen_lib.hpp"

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
            << "  -v, --verbose  Подробный вывод информации о данных принятых ПО и выведенных в файл\n"
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
// bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
//     size_t pos = filename.rfind('.');
//     if (pos != std::string::npos && pos != filename.length() - 1) {
//         std::string fileExt = filename.substr(pos + 1);
//         return validExtensions.find(fileExt) != validExtensions.end();
//     }
//     return false;
// }

bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos || pos == filename.length() - 1) {
        return false; // Нет расширения
    }
    std::string fileExt = filename.substr(pos + 1);
    return validExtensions.find(fileExt) != validExtensions.end();
}

// Функция для обработки аргументов командной строки
void parse_arguments(int argc, char *argv[], std::string& filename_bsdl, 
                    std::string&  filename_json, std::string& filename_svf,
                    std::string& trst_state, std::string& endir_state, 
                    std::string& enddr_state, bool& verbose, std::string& runtest_state){

    // Инициализация вспомагательных переменных
    int option_index = 0;
    int c;

    PinSVF PinSVF;

    // Инициализация доступных аргументов
    static struct option long_options[] = {
        {"bsdl", required_argument, 0, 'b'},
        {"json", required_argument, 0, 'j'},
        {"svf", required_argument, 0, 's'},
        {"trst", required_argument, 0, 't'},
        {"endir", required_argument, 0, 'i'},
        {"enddr", required_argument, 0, 'd'},
        {"runtest", required_argument, 0, 'r'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "Z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    // Цикл проверки всех переданных аргументов
    while ((c = getopt_long(argc, argv, "b:j:s:t:i:d:r:vh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b':
            if(optarg){
                filename_bsdl = optarg;
                if (!has_extension(filename_bsdl,{"bsd","bsdl"})) {
                    std::cerr << red << "Неверное расширение BSDL-файла (.bsd)" << reset << std::endl;
                    abort();
                }
                break;
            }
            case 'j':
                if(optarg){
                    filename_json = optarg;
                    if (!has_extension(filename_json, {"json"})) {
                        std::cerr << red << "Неверное расширение JSON-файла (.json)" << reset << std::endl;
                        abort();
                    }
                    break;
                }
            case 's':
                if(optarg){
                    filename_svf = optarg;
                    if (!has_extension(filename_svf, {"svf"})) {
                        std::cerr << red << "Неверное расширение SVF-файла (.svf)" << reset << std::endl;
                        abort();
                    }
                    break;
                }
            case 't':
                if(optarg){
                    trst_state = optarg;
                    if (!is_valid_state(trst_state, trst_states, 4)) {
                        std::cerr << red << "Неверное состояние --trst. Возможные состояния ON, OFF, Z, or ABSENT." << reset << std::endl;
                        abort();
                    }
                    break;
                }
            case 'i':
                if(optarg){
                    endir_state = optarg;
                    if (!is_valid_state(endir_state, endir_enddr_states, 4)) {
                        std::cerr << red << "Неверное состояние --endir. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE." << reset << std::endl;
                        abort();
                    }
                    break;
                }
            case 'd':
                if(optarg){
                    enddr_state = optarg;
                    if (!is_valid_state(enddr_state, endir_enddr_states, 4)) {
                        std::cerr << red << "Неверное состояние --enddr. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE." << reset << std::endl;
                        abort();
                    }
                    break;
                }
            case 'r':
                if(optarg){
                    runtest_state = optarg;
                    break;
                }
            case 'v':
                verbose = true;
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
    PinSVF PinSVF;
    PinJson PinJson;
    BsdlPins BsdlPins;
    

    // Создаём vector для хранения и передачи имён файлов
    parse_arguments(argc, argv, PinSVF.filename_bsdl, PinSVF.filename_json, PinSVF.filename_svf,
                    PinSVF.trst_state, PinSVF.endir_state, PinSVF.enddr_state, PinSVF.verbose, PinSVF.runtest_state);

    try {
        // Загружаем BSDL-файл, парсим его и записываем данные в переменные
        BsdlPins.loadBsdl(PinSVF.filename_bsdl);

        // Получаем данные о длине регистра BSDL
        size_t register_length_bsdl = BsdlPins.boundaryLength(PinSVF.filename_bsdl);

        // Получаем данные о длине регистра BSDL
        size_t register_length_instr = BsdlPins.instructionLength(PinSVF.filename_bsdl);

        // Получаем данные о коде для запуска EXTEST
        std::string opcode_extest = BsdlPins.opcodeEXTEST(PinSVF.filename_bsdl, register_length_instr);

        // Получаем вектор пинов
        const std::vector<BsdlPins::PinInfo>& cells = BsdlPins.getCells();
        if (cells.empty()) {
            throw std::runtime_error("Вектор ячеек пуст");
        }

        // Читаем данные из JSON и записываем их в переменные
        PinJson.read_proc_Json(PinSVF.filename_json);

        const std::vector<std::vector<PinJson::PinsJsonInfo>>& pins_svf = PinJson.getPins();
        if (pins_svf.empty()) {
            throw std::runtime_error("Вектор пинов в файле JSON пуст");
        }

        // Подробный вывод информации о данных принятых ПО и выведенных в файл
        if(PinSVF.verbose == 1){

            // Тестовый вывод пинов из json-файла
            PinSVF.print_param_cli(PinSVF.filename_bsdl, PinSVF.filename_json, PinSVF.filename_svf, 
                            PinSVF.trst_state, PinSVF.endir_state, PinSVF.enddr_state, PinSVF.runtest_state);
            
            // Выводим пины, записанные в JSON-файле
            PinJson.print_pins();  
        } 

        // Создаём SVF-файл 
        PinSVF.createFile(pins_svf, PinSVF.filename_json, register_length_bsdl, PinSVF.filename_svf, register_length_instr, opcode_extest, 
                            cells, PinSVF.trst_state, PinSVF.endir_state, PinSVF.enddr_state, PinSVF.runtest_state, PinSVF.verbose, 2);

    } catch (const std::exception& e) {
        
        std::cerr << red << "Ошибка при обработке: " << e.what() << reset << std::endl;
        return 1; // Возвращаем код ошибки
    }

    return 0;
}
