#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <gmpxx.h>

#include "pininfo.hpp"

// Подключаем пространство имен nlohmann/json
using json = nlohmann::json;

class PinJson {
public:

    // Объявляем переменные получаемые при запуске кода
    std::string filename_bsdl = "";
    std::string filename_json = "";
    std::string filename_svf = "";
    std::string trst_state = "OFF";
    std::string endir_state = "IDLE";
    std::string enddr_state = "IDLE";
    std::string runtest_state = "100";
    bool verbose = false;

    enum class StatePin {
        HIGH, 
        LOW,
        Z,
        X
    };

    std::string pin_name;
    StatePin cell_read;
    StatePin cell_write;

    // Главный метод библиотеки
    bool svfGen(std::string& filename_json);

    // Метод создающий svf файл и заполняющий его данными
    void createFile(std::string& filename_json, size_t& register_length_bsdl, std::string filename_svf, 
                    size_t& register_length_instr, std::string& opcode_extest, const std::vector<BsdlPins::PinInfo>& cells,
                    std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state, 
                    const bool verbose, const size_t& out);

    // Метод для вывода пинов записываемых в svf-файл
    void print_pins();

    // Вывод пинов, записанных в json-файл
    void print_json(std::string filename_bsdl, std::string  filename_json, std::string filename_svf,
                    std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state);

private:
    // Приватные методы
    static StatePin string_to_statepin(const std::string& value);
    static std::string statepin_to_string(StatePin state);
    PinJson::StatePin cell_value(const std::string& value);
    std::vector<PinJson> process_json(const json& jfile, std::vector<size_t>& pin_counts);
    json read_json_file(const std::string& filename);

    // Метод замены расширений файлов
    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);
    
    // Методы для генерации битовых масок для чтения и записи битов BS
    void safeValues( mpz_class& bitmask, const size_t& register_length_bsdl,
                    const std::vector<BsdlPins::PinInfo>& cells, const bool& development);
    void genPinTdi(mpz_class& bitmask, const size_t& register_length_bsdl,  
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index);
    void genPinTdo(mpz_class& bitmask, const size_t& register_length_bsdl, 
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index);
    void genPinMask(mpz_class& bitmask, const size_t& register_length_bsdl,  
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index);

    // Функция для заполнения строки единицами 
    void genSingleMask(mpz_class& binary_string, const size_t& length);
    
    // Метод вывода битовых масок с проверкой длины и дополнения незначащими нулями вначале
    std::string output_str(mpz_class bitmask, const size_t& register_length_bsdl);

    // Вектор для хранения количества пинов
    std::vector<size_t> pin_counts;

    // Вектор для хранения данных о пинах, передаваемых как аргументы для svf-файла
    std::vector<PinJson> pins_svf; 

protected:
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
