#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

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

    std::vector<std::string> parse_arguments(int argc, char *argv[]);

private:
    // Приватные методы
    static StatePin string_to_statepin(const std::string& value);
    static std::string statepin_to_string(StatePin state);
    bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count);
    bool has_extension(const std::string& filename, const std::string& ext);
    // std::vector<std::string> parse_arguments(int argc, char *argv[]);
    PinJson::StatePin cell_value(const std::string& value);
    std::vector<PinJson> process_json(const json& jfile, std::vector<size_t>& pin_counts);
    json read_json_file(const std::string& filename);
    void print_pins(const std::vector<PinJson>& pins);
    void print_usage();
    
    // Член-данные
    std::string filename_bsdl = "NO FILE";
    std::string filename_json = "NO FILE";
};
