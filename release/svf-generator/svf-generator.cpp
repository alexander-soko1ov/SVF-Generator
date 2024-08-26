#include <iostream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>
#include <unordered_set>
#include <gmpxx.h>
#include <cstring>
#include <vector>

#include "svf-generator.hpp"
#include "pininfo.hpp"

using json = nlohmann::json;

bool PinJson::svfGen(std::string& filename_json){
    
    if(filename_json == "NO FILE"){
        std::cerr << red << "Введите имя JSON-файла!" << reset << std::endl;
        abort();
    }

    PinJson pin_json;
    
    // Чтение JSON из файла
    json jfile = pin_json.read_json_file(filename_json);

    // Обработка JSON и получение данных
    pins_svf = pin_json.process_json(jfile, pin_counts);

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

// Тестовый вывод пинов из json-файла
void PinJson::print_json(std::string filename_bsdl, std::string  filename_json, std::string filename_svf,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state){
    // Вывод записанных аргументов
    std::string filename_svf_json = PinJson::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    if(filename_svf == "NO FILE"){
        filename_svf = filename_svf_json;
    }

    std::cout << "\n";
    std::cout << "BSDL-file:    " << green << filename_bsdl << reset << "\n";
    std::cout << "JSON-file:    " << green << filename_json << reset << "\n";
    std::cout << "SVF-file:     " << green << filename_svf << reset << "\n";
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
        std::cerr << "Не удалось открыть файл: " << red << filename  << reset << std::endl;
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

// Функция для заполнения строки единицами
void PinJson::genSingleMask(mpz_class& bitmask, const size_t& register_length_bsdl) {
    for (size_t i = 0; i < register_length_bsdl; ++i) {
        bitmask |= (mpz_class(1) << i);
    }
}

// Функция генерирующая маску для записи значений в ячейки BS
void PinJson::genPinTdi(mpz_class& bitmask, const size_t& register_length_bsdl,
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){

    /* В структуре bsd файла при описании ячейки control имеется значение при котором драйвер отключается, нам 
    необходимо заменить данное значение на инверсное и записать по индексу ячейки control
    Таким образом драйвер будет включен и ячейка output будет активна и мы записываем в неё значение из json-файла
    */

    // Тестовое заполнение маски единицами
    // genSingleMask(bitmask, register_length_bsdl);
    // return;

    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){ 
                
                if((cells[i].function == "OUTPUT") || (cells[i].function == "OUTPUT2") || (cells[i].function == "OUTPUT3") || (cells[i].function == "BIDIR") ){

                    if (statepin_to_string(pins_svf[temp_index].cell_write) != "z"){
                        // std::cout << magenta << "найдено совпадение!" << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl; 

                        if(statepin_to_string(pins_svf[temp_index].cell_write) == "1"){
                        
                            bitmask |= (mpz_class(1) << cells[i].Out); // если write = 1, то пишем 1 в маску
                        } else if(statepin_to_string(pins_svf[temp_index].cell_write) == "0"){
 
                            bitmask &= ~(mpz_class(1) << cells[i].Out); // если write = 0, то пишем 0 в маску
                        }

                    } else {
                        if(cells[i].turnOff == 1){
                            // std::cout << red << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                            bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                        } else {
                            // std::cout << green << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                            bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                        }
                        
                    }
                } 
            }                
        } temp_index++;
    }
}


void PinJson::genPinTdo(mpz_class& bitmask, const size_t& register_length_bsdl,
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){
    
    // Тестовое заполнение маски единицами
    // genSingleMask(bitmask, register_length_bsdl);
    // return;

    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {
        
        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){

                if((cells[i].function == "INPUT") || (cells[i].function == "BIDIR")){
                    
                    if(statepin_to_string(pins_svf[temp_index].cell_read) == "1"){

                        // std::cout << red << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                        bitmask |= (mpz_class(1) << cells[i].In); // если write = 1, то пишем 1 в маску
                    } else if(statepin_to_string(pins_svf[temp_index].cell_read) == "0"){
                        
                        // std::cout << magenta << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                        bitmask &= ~(mpz_class(1) << cells[i].In); 
                    } else if(statepin_to_string(pins_svf[temp_index].cell_read) == "x"){
                        
                        // std::cout << magenta << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                        return;
                    } else {
                        
                        std::cerr << red << "Неверное состояние read!" << reset << std::endl;
                        abort();
                    }   
                } 
            }
        } temp_index++;    
    }
}

// Функция генерирующая общую маску для ячеек BS
void PinJson::genPinMask(mpz_class& bitmask, const size_t& register_length_bsdl, 
                        const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){   
    
    // Тестовое заполнение маски единицами
    // genSingleMask(bitmask, register_length_bsdl);
    // return;

    size_t temp_index = index;

    for (size_t j = 0; j < pin_counts[count_out]; ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i) {
            
            if(cells[i].label == pins_svf[temp_index].pin_name) {
               
                if((cells[i].function == "INPUT") || (cells[i].function == "BIDIR")) {

                    if(statepin_to_string(pins_svf[temp_index].cell_read) == "x") {
                        // std::cout << "найдено совпадение!   "  << cells[i].label << "     " << cells[i].cell << "    " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
                        bitmask &= ~(mpz_class(1) << cells[i].cell);
                    } else {
                        bitmask |= (mpz_class(1) << cells[i].cell);
                    }
                }
            } 
        } temp_index++;    
    }
}

// Заполнение переменной pin_tdi безопасными значениями 
void PinJson::safeValues(mpz_class& bitmask, const size_t& register_length_bsdl, 
                        const std::vector<BsdlPins::PinInfo>& cells, const bool& development){
    
    if(development == 1){
        // Тестовое заполнение маски единицами
        genSingleMask(bitmask, register_length_bsdl);

    } else {
       for(size_t i = 0; i < register_length_bsdl; ++i){
        
            if((cells[i].function == "OUTPUT") || (cells[i].function == "OUTPUT2") || (cells[i].function == "OUTPUT3") 
                || (cells[i].function == "BIDIR")){
                
                if(BsdlPins::PinInfo::statePinToString(cells[i].safeState) == "1"){
                    bitmask |= (mpz_class(1) << cells[i].cell);   
                } else if (BsdlPins::PinInfo::statePinToString(cells[i].safeState) == "0"){
                    bitmask |= (mpz_class(0) << cells[i].cell); 
                } else if (BsdlPins::PinInfo::statePinToString(cells[i].safeState) == "x"){
                    // std::cout << cells[i].label  << "   " << cells[i].function << "  " << " Control Pin: " << cells[i].Config << std::endl;
                    bitmask |= (mpz_class(cells[i].turnOff) << cells[i].Config); 
                } else {
                    std::cerr << red << "Неверное безопасное состояние!" << reset << std::endl;
                }  
            }
        } 
    }
} 

// Проверим длину маски и добавим ведущий ноль, если необходимо
std::string PinJson::output_str(mpz_class bitmask, const size_t& register_length_bsdl) {
    std::string bitmask_str = bitmask.get_str(2);
    if (bitmask_str.length() < register_length_bsdl) {
        bitmask_str = std::string(register_length_bsdl - bitmask_str.length(), '0') + bitmask_str;
    }

    // return bitmask_str;
    return bitmask_str;
}   

// Функция создающая файл и заполняющая его в соотвествии с json
void PinJson::createFile(std::string& filename_json, size_t& register_length_bsdl, std::string filename_svf, 
                        size_t& register_length_instr, std::string& opcode_extest, const std::vector<BsdlPins::PinInfo>& cells,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state, 
                        const size_t& out_format) {
    
    if(filename_svf == "NO FILE"){
        filename_svf = replaceExtension(filename_json, ".json", ".svf");
    }

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
        svfFile << "TIR " << register_length_instr << " TDI (" << opcode_extest << ")\n"; 

        // Инициализация переменной pinTdi
        mpz_class pin_tdi(0);
        mpz_class pin_tdo(0);
        mpz_class pin_mask(0);

        /* Если нужно сначала залить маску, то сделать переменную равнной 1, а далее нужный 
        бит оставить единицей или сделать нулём (это сделано исходя из той рекомендации от разрабочиков, 
        вдруг пригдится, решил добавить) управляется из библиотеки так как вряд-ли пригодится*/ 
        const bool development = 0;

        // Запонение переменной pin_tdi безопасными значениями
        safeValues(pin_tdi, register_length_bsdl, cells, development);
        
        // Заполнение строки двоичными данными
        genPinTdi(pin_tdi, register_length_bsdl, cells, count_out, index);
        genPinTdo(pin_tdo, register_length_bsdl, cells, count_out, index);
        genPinMask(pin_mask, register_length_bsdl, cells, count_out, index);  
        
        index += pin_counts[count_out];

        // std::cout << red << index << reset << std::endl;
        
        // Тестовый вывод битовых полей в 2-ой или 16-ричной системе исчисления
        if(out_format == 2){
            std::cout << "Тестовый вывод битовых полей для блока номер " << count_out << ":   " << std::endl;
            std::cout << "Поле TDI:     " << green << output_str(pin_tdi, register_length_bsdl) << reset << std::endl;
            std::cout << "Поле TDO:     " << green << output_str(pin_tdo, register_length_bsdl) << reset << std::endl;
            std::cout << "Поле MASK:    " << green << output_str(pin_mask, register_length_bsdl) << reset  << std::endl;
            std::cout << "\n";

        } else if(out_format == 16){        
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

    if((out_format == 2) || (out_format == 16)){
        // Успешное завершение программы
        std::cout << "\nФайл " << magenta << filename_svf << reset << " успешно создан." << std::endl;
    }
}
