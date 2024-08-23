#include <iostream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>
#include <unordered_set>
#include <gmpxx.h>
#include <cstring>

#include "svf-generator.hpp"
#include "pininfo.hpp"

using json = nlohmann::json;

bool PinJson::svfGen(std::string& filename_json){
    
    PinJson pin_json;
    
    // Чтение JSON из файла
    json jfile = pin_json.read_json_file(filename_json);

    // Обработка JSON и получение данных
    pins_svf = pin_json.process_json(jfile, pin_counts);

    // Вывод результата для проверки
    // pin_json.print_pins(pins_svf, pin_counts);

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
        case StatePin::high: return "1";
        case StatePin::low: return "0";
        case StatePin::z: return "z";
        case StatePin::x: return "x";
        default: throw std::invalid_argument("Неизвестное значение StatePin");
    }
}

// Функция вывода help
void PinJson::print_usage() {
    std::cout << "Usage: program [options]\n"
            << "Options:\n"
            << "  -b, --bsdl     Add a BSDL-file\n"
            << "  -j, --json     Add a JSON-file\n"
            << "  -t, --trst     Set the TRST state (ON, OFF, z, ABSENT)\n"
            << "  -i, --endir    Set the ENDIR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
            << "  -d, --enddr    Set the ENDDR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
            << "  -r, --runtest  Number of clock cycles (def 100 TCK)\n"
            << "  -h, --help     Show this help message\n";
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
 std::vector<std::string>  PinJson::parse_arguments(int argc, char *argv[]){

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
        {"runtest", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "Z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    // Цикл проверки всех переданных аргументов
    while ((c = getopt_long(argc, argv, "b:j:t:i:d:r:h", long_options, &option_index)) != -1) {
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
                    std::cerr << "Неверное состояние --trst. Возможные состояния ON, OFF, Z, or ABSENT.\n";
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
            case 'r':
                runtest_state = optarg;
                // if (!is_valid_state(runtest_state, endir_enddr_states, 4)) {
                //     std::cerr << "Неверное значение --runtest. Возможное значение от 20 до .\n";
                //     abort();
                // }
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

    std::vector<std::string> filename = {filename_bsdl, filename_json};

    return filename;
}

// Тестовый вывод пинов из json-файла
void PinJson::print_json(){
    // Вывод записанных аргументов
    std::cout << "\n";
    std::cout << "BSDL-file:    " << green << filename_bsdl << reset << "\n";
    std::cout << "JSON-file:    " << green << filename_json << reset << "\n";
    std::cout << "TRST state:   " << green << trst_state << reset << "\n";
    std::cout << "ENDIR state:  " << green << endir_state << reset << "\n";
    std::cout << "ENDDR state:  " << green << enddr_state << reset << "\n";
    std::cout << "RUNTEST:      " << green << runtest_state << reset << "\n";
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
void PinJson::print_pins() {
    std::cout << "\nДанные записанные в JSON-файле:\n";

    size_t index = 0;

    for(auto& pin_count : pin_counts){
        for(size_t i = 0; i < pin_count; ++i){
            
            const auto& pin = pins_svf[index++];

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
// Функция для заполнения строки единицами (ТЕСТОВЫЙ РЕЖИМ)
void PinJson::fill_binary_string(mpz_class& bitmask, size_t& register_length_bsdl) {
    for (size_t i = 0; i < register_length_bsdl; ++i) {
        bitmask |= (mpz_class(1) << i);
    }
}

// Функция генерирующая маску для чтения битов в ячейки BS
void PinJson::genPinTdi(mpz_class& bitmask, size_t& register_length_bsdl,
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index){

    /* В структуре bsd файла при описании ячейки control имеется значение при котором драйвер отключается, нам 
    необходимо заменить данное значение на инверсное и записать по индексу ячейки control
    Таким образом драйвер будет включен и ячейка output будет активна и мы записываем в неё значение из json-файла
    */

    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){

                // std::cout << "найдено совпадение!" << "  cells:  " << cells[i].cell << "    " << cells[i].label << "     pins_svf " << pins_svf[temp_index].pin_name << std::endl;
                
                if(((cells[i].function == "OUTPUT") || (cells[i].function == "OUTPUT2") || (cells[i].function == "OUTPUT3")) && (statepin_to_string(pins_svf[temp_index].cell_write) != "z")){

                    // std::cout << "найдено совпадение!" << "  cells:  " << cells[i].cell << "    " << cells[i].label << "     " << cells[i].function << "   " << statepin_to_string(pins_svf[temp_index].cell_write) << std::endl;
                    bitmask |= (mpz_class(!cells[i].turnOff) << cells[i].Config); // запись в битовую маску для включённого драйвера 

                    if(statepin_to_string(pins_svf[temp_index].cell_write) == "1"){
                        
                        // std::cout  << "  cells:  " << cells[i].cell << "    " << cells[i].label << "  cell_write  " << statepin_to_string(pins_svf[temp_index].cell_write) << "   " << statepin_to_string(pins_svf[temp_index].cell_write) << std::endl;
                        bitmask |= (mpz_class(1) << cells[i].Out); // если write = 1, то пишем 1 в маску
                    } else if(statepin_to_string(pins_svf[temp_index].cell_write) == "0"){

                        // std::cout << "  cells:  " << cells[i].cell << "    " << cells[i].label << "  cell_write  " << statepin_to_string(pins_svf[temp_index].cell_write) << "   " << statepin_to_string(pins_svf[temp_index].cell_write) << std::endl;
                        bitmask |= (mpz_class(0) << cells[i].Out); // если write = 0, то пишем 0 в маску (для варианта z ниже)
                    }
                    
                    // std::cout << "значение драйвера "  << !cells[i].turnOff << "   отключения " << cells[i].turnOff << " " << cells[i].label << std::endl;
                } else if(((cells[i].function == "OUTPUT") || (cells[i].function == "OUTPUT2") || (cells[i].function == "OUTPUT3")) && (statepin_to_string(pins_svf[temp_index].cell_write) == "z")){
                    
                    // std::cout << "найдено совпадение!" << "  cells:  " << cells[i].cell << "    " << cells[i].label << "     " << cells[i].function << "   " << statepin_to_string(pins_svf[temp_index].cell_write) << std::endl;
                    bitmask |= (mpz_class(cells[i].turnOff) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                } 
            } 
        }
        temp_index++;    
    }
}

// Функция генерирующая маску для установки значений, при чтении битов ячеек BS
void PinJson::genPinTdo(mpz_class& bitmask, size_t& register_length_bsdl,
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index){
    
    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {
        
        // std::cout << "temp_index " << temp_index << "   index " << index << std::endl;

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){

                // std::cout << "найдено совпадение!" << "  cells:  " << cells[i].cell << "    " << cells[i].label << "     pins_svf " << pins_svf[temp_index].pin_name << std::endl;

                if((cells[i].function == "INPUT") && (statepin_to_string(pins_svf[temp_index].cell_read) != "x")){
                    
                    // std::cout << "найдено совпадение!" << "  cells:  " << cells[i].cell << "    " << cells[i].label << "     pins_svf " << pins_svf[temp_index].pin_name << std::endl;
                    
                    if(statepin_to_string(pins_svf[temp_index].cell_read) == "1"){

                        // std::cout << "read  " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
                        bitmask |= (mpz_class(1) << cells[i].In); // если write = 1, то пишем 1 в маску

                    } else if(statepin_to_string(pins_svf[temp_index].cell_read) == "0"){
                        
                        // std::cout << "read  " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
                        bitmask |= (mpz_class(0) << cells[i].In); // если write = 0, то пишем 0 в маску (для варианта z ниже)
                    }
                    
                } 
                // else if ((cells[i].function == "INPUT") && (statepin_to_string(pins_svf[temp_index].cell_read) == "x")){
                //     std::cout << red << "Я понятия не имею, как записать в двоичную ячейку значение " << bold 
                //                 << statepin_to_string(pins_svf[temp_index].cell_read) << "  " << pins_svf[temp_index].pin_name << reset << std::endl;
                // }
            }
        }
        temp_index++;    
    }
}

// Функция генерирующая общую маску для ячеек BS
void PinJson::genPinMask(mpz_class& bitmask, size_t& register_length_bsdl, 
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index){   

    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){
               
                if((cells[i].function == "INPUT") && (statepin_to_string(pins_svf[temp_index].cell_read) != "x")){

                    // std::cout << "найдено совпадение!"  << "  config: " << cells[i].Config << "     " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
                    bitmask |= (mpz_class(1) << cells[i].cell);
                } 
                // else if ((cells[i].function == "OUTPUT3") && (statepin_to_string(pins_svf[temp_index].cell_read) == "x")){
                    
                //     std::cout << "найдено совпадение!"  << "  config: " << cells[i].Config << "     " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
                //     // bitmask |= (mpz_class(1) << cells[i].Config);
                // }
            } 
        }
        temp_index++;    
    }
}

// Функция создающая файл и заполняющая его в соотвествии с json
void PinJson::createFile(std::string& filename_json, size_t& register_length_bsdl, size_t& register_length_instr, 
                        const std::vector<BsdlPins::PinInfo>& cells, const bool out) {
    // Создание имени файла с расширением svf
    std::string filename_svf = PinJson::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    std::ofstream svfFile(filename_svf);

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    if(trst_state == "OFF"){
        svfFile << "! Начать программу тестирования\n\n"
                "TRST " << trst_state << ";\n";
    } else{
        
        std::cerr << red << "Окончание программы тестирования так как вывод TRST находится не в состоянии OFF" << reset << std::endl;
        abort();
    }
    
    svfFile << "ENDIR "<< endir_state << ";\n";
    
    svfFile << "ENDDR "<< enddr_state << ";\n\n";
    
    // Инициализация переменной index для работы с вектором pins_svf
    size_t index = 0;

    for(size_t count_out = 0; count_out < pin_counts.size(); count_out++){
        svfFile << "TIR " << register_length_instr << " TDI (" << EXTEST << ")\n"; 

        // Инициализация переменной pinTdi
        mpz_class pin_tdi(0);
        mpz_class pin_tdo(0);
        mpz_class pin_mask(0);
        
        // Заполнение строки двоичными данными
        genPinTdi(pin_tdi, register_length_bsdl, cells, count_out, index);
        genPinTdo(pin_tdo, register_length_bsdl, cells, count_out, index);    
        genPinMask(pin_mask, register_length_bsdl, cells, count_out, index);
        
        index += pin_counts[count_out];
        
        if(out == 1){
            // тестовый вывод битовых полей
            std::cout << "Тестовый вывод битовых полей для блока номер " << count_out << ":   " << std::endl;
            std::cout << "Поле TDI:     " << green << pin_tdi.get_str(16) << reset << std::endl;
            std::cout << "Поле TDO:     " << green << pin_tdo.get_str(16) << reset << std::endl;
            std::cout << "Поле MASK:    " << green << pin_mask.get_str(16)<< reset  << std::endl;
            std::cout << "\n";
        }

        // Запись строки битовой маски в файл
        svfFile << "SDR " << register_length_bsdl << " TDI (" << pin_tdi.get_str(16) <<  ") TDO (" 
                << pin_tdo.get_str(16) <<  ") MASK ("  << pin_mask.get_str(16) << ");\n";
        
        // Запись строки в файл
        svfFile << "RUNTEST " << runtest_state << " TCK ENDSTATE IDLE;\n\n";
    }

    // Окончание программы тестирования
    svfFile << "! Программа тестирования окончена\n";

    // Закрытие файла
    svfFile.close();

    if(out == 1){
        // Успешное завершение программы
        std::cout << "\nФайл " << magenta << filename_svf << reset << " успешно создан." << std::endl;
    }
}
