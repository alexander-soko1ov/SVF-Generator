#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <nlohmann/json.hpp>

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
    void createFile(std::string& filename_json, unsigned int& register_length_bsdl, unsigned int& register_length_instr, 
                    const std::vector<BsdlPins::PinInfo>& pins, const std::vector<BsdlPins::PinInfo>& cells);

    // Метод для вывода пинов записываемых в svf-файл
    void print_pins();

private:
    // Приватные методы

    static StatePin string_to_statepin(const std::string& value);
    static std::string statepin_to_string(StatePin state);
    PinJson::StatePin cell_value(const std::string& value);
    std::vector<PinJson> process_json(const json& jfile, std::vector<size_t>& pin_counts);
    json read_json_file(const std::string& filename);
    // void print_pins(const std::vector<PinJson>& pins, std::vector<size_t> pin_counts);
    // void print_usage();

    // Метод замены расширений файлов
    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);

    // Методы для работы с getopt
    void print_usage();
    bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions);
    bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count);

    // Методы для заполнения SVF-файла
    void to_upper_case(char* str);
    char* convert_binary_to_hex(const char* binary_string);
    void print_conversion(const char* binary_string, char* hex_string);
    // void fill_binary_string(char* binary_string, size_t length);

    // Методы для генерации битовых масок для чтения и записи битов BS
    void genPinTdi(char* binary_string, size_t length, const std::vector<BsdlPins::PinInfo>& pins, 
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out);
    void genPinTdo(char* binary_string, size_t length, const std::vector<BsdlPins::PinInfo>& pins, 
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out);
    void genPinMask(char* binary_string, size_t length, const std::vector<BsdlPins::PinInfo>& pins, 
                    const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out);

    // Функция для заполнения строки чередующимися 1 и 0 (ТЕСТОВЫЙ РЕЖИМ)
    void fill_binary_string(char* binary_string, size_t length);

    // Члены класса управления SVF и передачи нужных файлов
    std::string filename_bsdl = "NO FILE";
    std::string filename_json = "NO FILE";
    std::string trst_state = "OFF";
    std::string endir_state = "IDLE";
    std::string enddr_state = "IDLE";
    std::string EXTEST = "00000";  // заглушка, пока что не парсил из файла
    
    // Вектор для хранения количества пинов
    std::vector<size_t> pin_counts;

    // Вектор для хранения данных о пинах, передаваемых как аргументы для svf-файла
    std::vector<PinJson> pins_svf; 
};
