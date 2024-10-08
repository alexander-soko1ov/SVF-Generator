#include <iostream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>
#include <unordered_set>
#include <gmpxx.h>
#include <cstring>
#include <vector>

#include "svf-gen_lib.hpp"
#include "pininfo_lib.hpp"
#include "json_pars_lib.hpp"

// Приватные методы управляемые методом print_param_cli
// Функция для преобразования строки в значение StatePin
PinJson::PinsJsonInfo::StatePin PinJson::PinsJsonInfo::string_to_statepin(const std::string& value) {

    static const std::unordered_map<std::string, PinsJsonInfo::StatePin> state_map = {
        {"1", PinsJsonInfo::StatePin::HIGH},
        {"high", PinsJsonInfo::StatePin::HIGH},
        {"0", PinsJsonInfo::StatePin::LOW},
        {"low", PinsJsonInfo::StatePin::LOW},
        {"z", PinsJsonInfo::StatePin::Z},
        {"x", PinsJsonInfo::StatePin::X}
    };

    auto iter = state_map.find(value);
    
    if (iter != state_map.end()) {
        return iter->second;  // Возвращаем найденное значение
    } else {
        throw std::invalid_argument("Неизвестное значение для StatePin: " + value);
    }
}

// Функция для преобразования StatePin в строку
std::string PinJson::PinsJsonInfo::statepin_to_string(PinsJsonInfo::StatePin state) {
    switch (state) {
        case PinsJsonInfo::StatePin::HIGH: return "1";
        case PinsJsonInfo::StatePin::LOW: return "0";
        case PinsJsonInfo::StatePin::Z: return "z";
        case PinsJsonInfo::StatePin::X: return "x";
        default: throw std::invalid_argument("Неизвестное значение StatePin");
    }
}

// Главный метод чтения JSON и генерации внутренного представления 
bool PinJson::read_proc_Json(std::string& filename_json){
    
    if(filename_json == ""){
        std::cerr << red << "Введите имя JSON-файла!" << reset << std::endl;
        abort();
    }

    // Чтение JSON из файла
    json jfile = read_json_file(filename_json);

    // Обработка JSON и получение данных

    process_json(jfile);

    return 0;
}

// Функция для чтения JSON из файла
PinJson::json PinJson::read_json_file(const std::string& filename) {
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

// Метод для чтения и обработки данных из Json
void PinJson::process_json(const json& jfile) {
    // std::vector<std::vector<PinJson>> pins;

    // std::vector<std::vector<PinJson>> pin_svf; // Инициализация вектора

    size_t num_items = jfile.size();

    // Заранее задаем размер вектора
    pins_svf.resize(num_items);

    PinsJsonInfo pin;

    size_t index = 0;

    for (const auto& item : jfile) {

        if (!item.contains("pins") || !item.contains("read") || !item.contains("write") || 
            !item["pins"].is_array() || !item["read"].is_array() || !item["write"].is_array()) {

            throw std::runtime_error("Отсутствуют или некорректны данные в JSON!");
            
            return; // Возвращаем пустой вектор
        }

        // std::cerr << magenta << "item   " << item << reset << std::endl;

        const auto& pin_names = item["pins"];
        const auto& read_values = item["read"];
        const auto& write_values = item["write"]; 

        for (size_t i = 0; i < pin_names.size(); ++i) {

            pin.pin_name = pin_names[i];
            
            if (PinsJsonInfo::string_to_statepin(read_values[i]) == PinsJsonInfo::StatePin::HIGH ||
                PinsJsonInfo::string_to_statepin(read_values[i]) == PinsJsonInfo::StatePin::LOW ||
                PinsJsonInfo::string_to_statepin(read_values[i]) == PinsJsonInfo::StatePin::X){
                
                pin.cell_read = PinsJsonInfo::string_to_statepin(read_values[i]);
            } else {
                std::cerr << red << "Неверное состояние pin read! Допустимые: 1, 0, x" << reset << std::endl;
                abort();
            }

            if (PinsJsonInfo::string_to_statepin(write_values[i]) == PinsJsonInfo::StatePin::HIGH ||
                PinsJsonInfo::string_to_statepin(write_values[i]) == PinsJsonInfo::StatePin::LOW ||
                PinsJsonInfo::string_to_statepin(write_values[i]) == PinsJsonInfo::StatePin::Z){
                
                pin.cell_write = PinsJsonInfo::string_to_statepin(write_values[i]);
            } else {
                std::cerr << red << "Неверное состояние pin write! Допустимые: 1, 0, z" << reset << std::endl;
                abort();
            }

            // std::cerr << "pin_name:     " << pin.pin_name << std::endl;
            // std::cerr << "cell_read:    " << PinsJsonInfo::statepin_to_string(pin.cell_read) << std::endl;
            // std::cerr << "cell_write:   " << PinsJsonInfo::statepin_to_string(pin.cell_write) << std::endl << std::endl; 

            pins_svf[index].push_back(pin);
        } index++;
    }
}

// Функция для вывода данных пинов записанных в Json
void PinJson::print_pins() {
    std::cout << "\nДанные записанные в JSON-файле:" << std::endl;

    for (size_t i = 0; i < pins_svf.size(); ++i) {
        for (const auto& pin_out : pins_svf[i]) {
            std::cout << "Pin: " << green << pin_out.pin_name << reset 
                << ", Read: " << green << PinsJsonInfo::statepin_to_string(pin_out.cell_read) << reset
                << ", Write: " << green << PinsJsonInfo::statepin_to_string(pin_out.cell_write) << reset
                << std::endl; 
        }
        std::cout << "\n";
    }
}
