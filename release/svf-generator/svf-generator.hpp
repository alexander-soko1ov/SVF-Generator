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
    enum class StatePin {
        high, 
        low,
        z,
        x
    };

    std::string pin_name;
    StatePin cell_read;
    StatePin cell_write;

    // Главный метод библиотеки
    bool svfGen(std::string& filename_json);

    // Метод работающий с getopt и парсящий аргументы
    std::vector<std::string> parse_arguments(int argc, char *argv[]);

    // Метод создающий svf файл и заполняющий его данными
    void createFile(std::string& filename_json, size_t& register_length_bsdl, size_t& register_length_instr, 
                    const std::vector<BsdlPins::PinInfo>& cells, const bool out);

    // Метод для вывода пинов записываемых в svf-файл
    void print_pins();

    // Вывод пинов, записанных в json-файл
    void print_json();

private:
    // Приватные методы
    static StatePin string_to_statepin(const std::string& value);
    static std::string statepin_to_string(StatePin state);
    PinJson::StatePin cell_value(const std::string& value);
    std::vector<PinJson> process_json(const json& jfile, std::vector<size_t>& pin_counts);
    json read_json_file(const std::string& filename);

    // Метод замены расширений файлов
    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);

    // Методы для работы с getopt
    void print_usage();
    bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions);
    bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count);

    // Методы для генерации битовых масок для чтения и записи битов BS
    void genPinTdi(mpz_class& binary_string, size_t& length,  
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index);
    void genPinTdo(mpz_class& binary_string, size_t& length, 
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index);
    void genPinMask(mpz_class& binary_string, size_t& length,  
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, size_t index);

    // Функция для заполнения строки чередующимися 1 и 0 (ТЕСТОВЫЙ РЕЖИМ)
    void fill_binary_string(mpz_class& binary_string, size_t& length);

    // Члены класса управления SVF и передачи нужных файлов
    std::string filename_bsdl = "NO FILE";
    std::string filename_json = "NO FILE";
    std::string trst_state = "OFF";
    std::string endir_state = "IDLE";
    std::string enddr_state = "IDLE";
    std::string runtest_state = "100";

    std::string EXTEST = "00000";  // TODO: заглушка, пока что не парсил из файла (поставить расширенную подсветку синтаксиса)
    
    // Вектор для хранения количества пинов
    std::vector<size_t> pin_counts;

    // Вектор для хранения данных о пинах, передаваемых как аргументы для svf-файла
    std::vector<PinJson> pins_svf; 

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
