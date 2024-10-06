#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <gmpxx.h>

#include "pininfo_lib.hpp"
// #include "svf-gen_lib.hpp"


class PinJson{
public:

    class PinsJsonInfo{
    public:
        enum class StatePin {
            HIGH, 
            LOW,
            Z,
            X
        };
        
        std::string pin_name;
        StatePin cell_read;
        StatePin cell_write;


        static StatePin string_to_statepin(const std::string& value);
        static std::string statepin_to_string(StatePin state);  
    };
    

    // Подключаем пространство имен nlohmann/json
    using json = nlohmann::json;

    // Главный метод библиотеки
    bool read_proc_Json(std::string& filename_json);

    // Метод для вывода данных записанных в Json
    void print_pins();

    // Метод для получения вектора пинов
    const std::vector<std::vector<PinJson::PinsJsonInfo>>& getPins() const {
        return pins_svf;
    }

private:
    // Вектор для хранения данных о пинах, передаваемых как аргументы для svf-файла
    std::vector<std::vector<PinJson::PinsJsonInfo>> pins_svf; 

protected:
    // Методы читающие и обрабатывающие JSON
    json read_json_file(const std::string& filename);
    void process_json(const json& jfile);

    // Вектор для хранения данных о пинах, передаваемых как аргументы для svf-файла
    // std::vector<std::vector<PinsJsonInfo>> pins_svf; 

    // static StatePin string_to_statepin(const std::string& value);
    // static std::string statepin_to_string(StatePin state);

    // Переменные для определения ANSI escape-кодов
    // Сброс всех атрибутов
    const std::string reset = "\033[0m";

    // Цвета текста
    const std::string red = "\033[31m";
    const std::string magenta = "\033[35m";
    const std::string green = "\033[32m";
    
    // Стиль текста
    const std::string bold = "\033[1m";
};
