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
    
    if(filename_json == ""){
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
        return StatePin::HIGH;
    } else if (value == "0" || value == "low") {
        return StatePin::LOW;
    } else if (value == "z") {
        return StatePin::Z;
    } else if (value == "x") {
        return StatePin::X;
    } else {
        throw std::invalid_argument("Неизвестное значение для StatePin: " + value);
    }
}

// Функция для преобразования StatePin в строку
std::string PinJson::statepin_to_string(StatePin state) {
    switch (state) {
        case StatePin::HIGH: return "1";
        case StatePin::LOW: return "0";
        case StatePin::Z: return "z";
        case StatePin::X: return "x";
        default: throw std::invalid_argument("Неизвестное значение StatePin");
    }
}

// Тестовый вывод пинов из json-файла
void PinJson::print_json(std::string filename_bsdl, std::string  filename_json, std::string filename_svf,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state){
    // Вывод записанных аргументов
    std::string filename_svf_json = PinJson::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    if(filename_svf == ""){
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

        if (!item.contains("pins") || !item.contains("read") || !item.contains("write") || 
            !item["pins"].is_array() || !item["read"].is_array() || !item["write"].is_array()) {
            
            std::cerr << "Отсутствуют или некорректны данные в JSON!" << std::endl;
            return {}; // Возвращаем пустой вектор
        }

        const auto& pin_names = item["pins"];
        const auto& read_values = item["read"];
        const auto& write_values = item["write"];

        // Подсчет количества элементов в массиве
        size_t num_pins = pin_names.size();
        pin_counts.push_back(num_pins); // Сохраняем количество пинов

        // size_t num_reads = read_values.size();
        // size_t num_writes = write_values.size();

        // Вывод количества элементов
        // std::cout << "Количество пинов: " << num_pins << std::endl;
        // std::cout << "Количество значений read: " << num_reads << std::endl;
        // std::cout << "Количество значений write: " << num_writes << std::endl;

        if (!item.contains("pins") || !item.contains("read") || !item.contains("write") || 
            !item["pins"].is_array() || !item["read"].is_array() || !item["write"].is_array()) {
            
            throw std::runtime_error("Отсутствуют или некорректны данные в JSON!");
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
    try {
        input_file >> jfile;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Ошибка парсинга JSON: " + std::string(e.what()));
    }
    return jfile;
}

// Функция для вывода данных пинов
void PinJson::print_pins() {
    std::cout << "\nДанные записанные в JSON-файле:\n";

    size_t index = 0;

    for(auto& pin_count : pin_counts){
        for(size_t i = 0; i < pin_count; ++i){
            
            if (index >= pins_svf.size()) {
                std::cerr << "Ошибка: индекс выходит за рамки массива pins_svf!" << std::endl;
                abort();
            }

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
        throw std::runtime_error("Некорректное расширение файла: " + filename);
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

        if (temp_index >= pins_svf.size()) {
            std::cerr << red << "Индекс temp_index выходит за пределы массива pins_svf!" << reset << std::endl;
            abort();
        }

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[temp_index].pin_name){ 

                if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
                    || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir") ){
                    
                    // std::cout << magenta << "найдено совпадение!" << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl; 
                    
                    if (pins_svf[temp_index].cell_write != string_to_statepin("z")){
                        

                        if(pins_svf[temp_index].cell_write == string_to_statepin("1")){
                        
                            bitmask |= (mpz_class(1) << cells[i].Out); // если write = 1, то пишем 1 в маску

                            if(cells[i].turnOff == 0){
                                
                                bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            } else {
                                
                                bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            }

                        } else if(pins_svf[temp_index].cell_write == string_to_statepin("0")){
 
                            bitmask &= ~(mpz_class(1) << cells[i].Out); // если write = 0, то пишем 0 в маску

                            if(cells[i].turnOff == 0){

                                bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            } else {

                                bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            }
                        }

                    } else if(pins_svf[temp_index].cell_write == string_to_statepin("z")){
                        if(cells[i].turnOff == 1){
                           
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

                if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                    
                    if(pins_svf[temp_index].cell_read == string_to_statepin("1")){

                        // std::cout << red << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                        bitmask |= (mpz_class(1) << cells[i].In); // если write = 1, то пишем 1 в маску
                    } else if(pins_svf[temp_index].cell_read == string_to_statepin("0")){
                        
                        // std::cout << magenta << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
                        bitmask &= ~(mpz_class(1) << cells[i].In); 
                    } else if(pins_svf[temp_index].cell_read == string_to_statepin("x")){
                        
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
               
                if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")) {

                    if(pins_svf[temp_index].cell_read == string_to_statepin("x")) {
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
        
            if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
                || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                
                // std::cout << cells[i].label  << "   " << BsdlPins::toLowerCase(cells[i].function) << "  " << " Safe State: " << BsdlPins::PinInfo::statePinToString(cells[i].safeState) << std::endl;

                if(cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("1")){
                    
                    bitmask |= (mpz_class(1) << cells[i].cell);   
                } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("0")){
                    
                    bitmask |= (mpz_class(0) << cells[i].cell); 
                } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("X")){
                    
                   
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
                        const bool verbose, const size_t& out_format) {
    
    if(filename_svf == ""){
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

    mpz_class opcode_extest_hex(opcode_extest);
    // std::cout << red << opcode_extest << reset << std::endl;

    for(size_t count_out = 0; count_out < pin_counts.size(); count_out++){
        // Инициализация переменной pinTdi
        mpz_class pin_tdi(0);
        mpz_class pin_tdo(0);
        mpz_class pin_mask(0);
         
        svfFile << "SIR " << register_length_instr << " TDI (" << opcode_extest_hex.get_str(16) << ")\n"; 

        /* Если нужно сначала залить маску, то сделать переменную равнной 1, а далее нужный 
        бит оставить единицей или сделать нулём (это сделано исходя из той рекомендации от разрабочиков, 
        вдруг пригдится, решил добавить) управляется из библиотеки так как вряд-ли пригодится*/ 
        const bool development = 0;

        // Запонение переменной pin_tdi безопасными значениями
        // safeValues(pin_tdi, register_length_bsdl, cells, development);
        
        // Заполнение строки двоичными данными
        genPinTdi(pin_tdi, register_length_bsdl, cells, count_out, index);
        genPinTdo(pin_tdo, register_length_bsdl, cells, count_out, index);
        genPinMask(pin_mask, register_length_bsdl, cells, count_out, index);  
        
        index += pin_counts[count_out];
        // std::cout << red << index << reset << std::endl;
        
        // Тестовый вывод битовых полей в 2-ой или 16-ричной системе исчисления
        if(verbose == 1){
            
            std::cout << "Тестовый вывод битовых полей в " << out_format << " формате для блока номер " << count_out << ":   " << std::endl;
            
            if(out_format == 2){
                std::cout << "Поле TDI:     " << green << output_str(pin_tdi, register_length_bsdl) << reset << std::endl;
                std::cout << "Поле TDO:     " << green << output_str(pin_tdo, register_length_bsdl) << reset << std::endl;
                std::cout << "Поле MASK:    " << green << output_str(pin_mask, register_length_bsdl) << reset  << std::endl;
                std::cout << "\n";

            } else {
                std::cout << "Поле TDI:     " << green << pin_tdi.get_str(out_format) << reset << std::endl;
                std::cout << "Поле TDO:     " << green << pin_tdo.get_str(out_format) << reset << std::endl;
                std::cout << "Поле MASK:    " << green << pin_mask.get_str(out_format)<< reset  << std::endl;
                std::cout << "\n";
            }  
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

    if(verbose == 1){
        // Успешное завершение программы
        std::cout << "\nФайл " << magenta << filename_svf << reset << " успешно создан." << std::endl;
    }
}
