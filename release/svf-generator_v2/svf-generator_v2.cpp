#include <iostream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>
#include <unordered_set>
#include <gmpxx.h>
#include <cstring>

#include "svf-generator_v2.hpp"
#include "pininfo.hpp"

using json = nlohmann::json;

bool PinJson::svfGen(std::string& filename_json){
    
    PinJson pin_json;
    
    // Чтение JSON из файла
    json jfile = pin_json.read_json_file(filename_json);

    // Обработка JSON и получение данных
    std::vector<PinJson> pins = pin_json.process_json(jfile, pin_counts);

    // Вывод результата для проверки
    pin_json.print_pins(pins, pin_counts);

    // Вывод количества пинов для каждого объекта JSON
    // std::cout << "\nКоличество пинов в каждом объекте JSON:" << std::endl;
    // for (size_t count : pin_counts) {
    //     std::cout << count << std::endl;
    // }

    return 0;
}

// Приватные методы управляемые методом svfGen
// Функция для преобразования строки в значение StatePin
PinJson::StatePin PinJson::string_to_statepin(const std::string& value) {
    if (value == "1" || value == "high") {
        return StatePin::high;
    } else if (value == "0" || value == "low") {
        return StatePin::low;
    } else if (value == "z") {
        return StatePin::z;
    } else if (value == "x") {
        return StatePin::x;
    } else {
        throw std::invalid_argument("Неизвестное значение для StatePin: " + value);
    }
}

// Функция для преобразования StatePin в строку
std::string PinJson::statepin_to_string(StatePin state) {
    switch (state) {
        case StatePin::high: return "high";
        case StatePin::low: return "low";
        case StatePin::z: return "z";
        case StatePin::x: return "x";
        default: throw std::invalid_argument("Неизвестное значение StatePin");
    }
}

// Функция вывода help
void PinJson::print_usage() {
    std::cout << "Usage: program [options]\n"
            << "Options:\n"
            << "  -b, --bsdl    Add a BSDL-file\n"
            << "  -j, --json   Add a JSON-file\n"
            << "  -t, --trst   Set the TRST state (ON, OFF, z, ABSENT)\n"
            << "  -i, --endir  Set the ENDIR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
            << "  -d, --enddr  Set the ENDDR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
            << "  -h, --help   Show this help message\n";
}

// Функция проверки корректности id введённого в CLI аргументов
bool PinJson::is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (state == valid_states[i]) {
            return true;
        }
    }
    return false;
}

// Функция проверки расширения файла
bool PinJson::has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos && pos != filename.length() - 1) {
        std::string fileExt = filename.substr(pos + 1);
        return validExtensions.find(fileExt) != validExtensions.end();
    }
    return false;
}

// Функция для обработки аргументов командной строки
std::vector<std::string> PinJson::parse_arguments(int argc, char *argv[]){

    // Инициализация вспомагательных переменных
    int option_index = 0;
    int c;

    // Инициализация доступных аргументов
    static struct option long_options[] = {
        {"bsdl", required_argument, 0, 'b'},
        {"json", required_argument, 0, 'j'},
        {"trst", required_argument, 0, 't'},
        {"endir", required_argument, 0, 'i'},
        {"enddr", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    // Цикл проверки всех переданных аргументов
    while ((c = getopt_long(argc, argv, "b:j:t:i:d:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b':
                filename_bsdl = optarg;
                if (!has_extension(filename_bsdl,{"bsd","bsdl"})) {
                    std::cerr << "Неверное расширение BSDL-файла (.bsd)\n";
                    abort();
                }
                break;
            case 'j':
                filename_json = optarg;
                if (!has_extension(filename_json, {"json"})) {
                    std::cerr << "Неверное расширение JSON-файла (.json)\n";
                    abort();
                }
                break;
            case 't':
                trst_state = optarg;
                if (!is_valid_state(trst_state, trst_states, 4)) {
                    std::cerr << "Неверное состояние --trst. Возможные состояния ON, OFF, z, or ABSENT.\n";
                    abort();
                }
                break;
            case 'i':
                endir_state = optarg;
                if (!is_valid_state(endir_state, endir_enddr_states, 4)) {
                    std::cerr << "Неверное состояние --endir. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE.\n";
                    abort();
                }
                break;
            case 'd':
                enddr_state = optarg;
                if (!is_valid_state(enddr_state, endir_enddr_states, 4)) {
                    std::cerr << "Неверное состояние --enddr. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE.\n";
                    abort();
                }
                break;
            case 'h':
                print_usage();
                break;
            default:
                abort();
        }
    }

    // Проверка на наличие BSD-файла и JSON-файла
    if((filename_bsdl == "NO FILE")||(filename_json == "NO FILE")){
        std::cout << "Необходимо указать имя BSDL-файла и JSON-файла" << std::endl;
        abort();
    }

    // Вывод записанных аргументов
    std::cout << "\nBSDL-file: " << filename_bsdl << "\n";
    std::cout << "JSON-file: " << filename_json << "\n";
    std::cout << "TRST state: " << trst_state << "\n";
    std::cout << "ENDIR state: " << endir_state << "\n";
    std::cout << "ENDDR state: " << enddr_state << "\n\n";

    std::vector<std::string> filename = {filename_bsdl, filename_json};

    return filename;
}

// Функции для работы с JSON
// Функция для преобразования строки в StatePin
PinJson::StatePin PinJson::cell_value(const std::string& value) {
    return string_to_statepin(value);
}

// Функция для чтения и обработки данных из JSON
std::vector<PinJson> PinJson::process_json(const json& jfile, std::vector<size_t>& pin_counts) {
    std::vector<PinJson> pins;

    for (const auto& item : jfile) {
        const auto& pin_names = item["pins"];
        const auto& read_values = item["read"];
        const auto& write_values = item["write"];

        // Подсчет количества элементов в массиве
        size_t num_pins = pin_names.size();
        pin_counts.push_back(num_pins); // Сохраняем количество пинов

        size_t num_reads = read_values.size();
        size_t num_writes = write_values.size();

        // Вывод количества элементов
        // std::cout << "Количество пинов: " << num_pins << std::endl;
        // std::cout << "Количество значений read: " << num_reads << std::endl;
        // std::cout << "Количество значений write: " << num_writes << std::endl;

        if (num_pins != num_reads || num_pins != num_writes) {
            std::cerr << "Несоответствие размеров в JSON!" << std::endl;
            throw std::runtime_error("Несоответствие размеров в JSON!");
        }

        for (size_t i = 0; i < num_pins; ++i) {
            PinJson pin;
            pin.pin_name = pin_names[i];
            pin.cell_read = cell_value(read_values[i]);
            pin.cell_write = cell_value(write_values[i]);

            pins.push_back(pin);
        }
    }

    return pins;
}

// Функция для чтения JSON из файла
json PinJson::read_json_file(const std::string& filename) {
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
        throw std::runtime_error("Не удалось открыть файл!");
    }

    json jfile;
    input_file >> jfile;
    return jfile;
}

// Функция для вывода данных пинов
void PinJson::print_pins(const std::vector<PinJson>& pins, std::vector<size_t> pin_counts) {
    std::cout << "\nДанные записанные в JSON-файле:\n";

    for (size_t i = 0; i < pin_counts.size(); i++) {
        for(size_t count = 0; count < pin_counts[i]; count ++){
            const auto& pin = pins[count];
            std::cout << "Pin: " << pin.pin_name 
                << ", Read: " << statepin_to_string(pin.cell_read)
                << ", Write: " << statepin_to_string(pin.cell_write)
                << std::endl; 
        }
        std::cout << std::endl; 
    }
}

// Функция замены расширения файла
std::string PinJson::replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt) {
    size_t pos = filename.rfind(oldExt);
    if (pos != std::string::npos && pos == filename.length() - oldExt.length()) {
        return filename.substr(0, pos) + newExt;
    } else {
        std::cerr << "Некорретное раcширение файла: " << filename << std::endl;
        exit(1);
    }
}

// Методы для заполнения SVF-файла данными
// Преобразует строку в верхний регистр
void PinJson::to_upper_case(char* str) {
    while (*str) {
        *str = std::toupper(*str);
        ++str;
    }
}

// Функция для преобразования двоичной строки в 16-ричное представление
char* PinJson::convert_binary_to_hex(const char* binary_string) {
    // Инициализация переменной для хранения большого числа
    mpz_t big_number;
    mpz_init(big_number);

    // Устанавливаем значение числа в двоичной системе
    if (mpz_set_str(big_number, binary_string, 2) != 0) {
        std::cerr << "Error: Invalid binary string." << std::endl;
        mpz_clear(big_number);
        return nullptr;
    }

    // Инициализация строки для хранения 16-ричного результата
    char *hex_string = mpz_get_str(nullptr, 16, big_number);

    // Преобразование строки в верхний регистр
    to_upper_case(hex_string);

    // Освобождение ресурсов
    mpz_clear(big_number);

    return hex_string;
}

// Функция для печати двоичной и 16-ричной строк
void PinJson::print_conversion(const char* binary_string, char* hex_string) {
    // Печать входного бинарного числа и выходного 16-ричного  
    std::cout << "In:   " << binary_string << std::endl;
    std::cout << "Hex:  " << hex_string << std::endl;
    
    // Печать длины строки до её освобождения
    std::cout << "Length of hex string: " << std::strlen(hex_string) << std::endl;

    // Освобождение памяти, выделенной для 16-ричной строки
    free(hex_string);
}

// Функция для заполнения строки чередующимися 1 и 0
void PinJson::fill_binary_string(char* binary_string, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        binary_string[i] = '1';
    }
    binary_string[length] = '\0';
}

// Функция создающая файл и заполняющая его в соотвествии с json
void PinJson::createFile(std::string& filename_json, unsigned int& register_length_bsdl, unsigned int& register_length_instr, const std::vector<BsdlPins::PinInfo>& pins) {
    // Создание имени файла с расширением svf
    std::string filename_svf = PinJson::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    std::ofstream svfFile(filename_svf);

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    svfFile << "! Начать программу тестирования\n"
                "TRST " << trst_state << ";\n";

    svfFile << "ENDIR "<< endir_state << ";\n";
    
    svfFile << "ENDDR "<< enddr_state << ";\n";

    std::string EXTEST = "00000";  // заглушка, пока что не парсил из файла
    
    for(unsigned int i = 0; i < pin_counts.size(); i++){
        svfFile << "TIR " << register_length_instr << " TDI (" << EXTEST << ")\n";

        // char* pin_tdi = genPinTdi();

        // char* pin_tdo = genPinTdo();

        // svfFile << "SDR " << register_length_bsdl << " TDI (" << pin_tdi << ") TDO (" << pin_tdo << ") MASK (" << pin_mask << ");\n";
        
        svfFile << "RUNTEST 100 TCK ENDSTATE IDLE;\n\n"; 
    }

    // Закрытие файла
    svfFile.close();

    // Успешное завершение программы
    std::cout << "\nФайл " << filename_svf << " успешно создан." << std::endl;
}
