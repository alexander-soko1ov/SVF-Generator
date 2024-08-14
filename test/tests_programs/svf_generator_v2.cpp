#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>
#include <string>
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

    bool svfGen(int argc, char *argv[]){
        
        PinJson pin_json;

        std::vector<std::string> filename = parse_arguments(argc, argv);

        std::string filename_bsdl = filename[0];
        std::string filename_json = filename[1]; 
        
        // Чтение JSON из файла
        json jfile = pin_json.read_json_file(filename_json);

        // Вектор для хранения количества пинов
        std::vector<size_t> pin_counts;

        // Обработка JSON и получение данных
        std::vector<PinJson> pins = pin_json.process_json(jfile, pin_counts);

        // Вывод результата для проверки
        pin_json.print_pins(pins);

        // Вывод количества пинов для каждого объекта JSON
        std::cout << "\nКоличество пинов в каждом объекте JSON:" << std::endl;
        for (size_t count : pin_counts) {
            std::cout << count << std::endl;
        }

        return 0;
    }

private:
    // Приватные методы управляемые методом svfGen
    // Функция для преобразования строки в значение StatePin
    static StatePin string_to_statepin(const std::string& value) {
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
    static std::string statepin_to_string(StatePin state) {
        switch (state) {
            case StatePin::high: return "high";
            case StatePin::low: return "low";
            case StatePin::z: return "z";
            case StatePin::x: return "x";
            default: throw std::invalid_argument("Неизвестное значение StatePin");
        }
    }


    // Методы и переменные для работы с getopt
    std::string filename_bsdl = "NO FILE";
    std::string filename_json = "NO FILE";

    // Функция для печати справки
    void print_usage() {
        std::cout << "Usage: program [options]\n"
                << "Options:\n"
                << "  -b, --bsdl    Add a BSDL-file\n"
                << "  -j, --json    Add a JSON-file\n"
                << "  -h, --help    Show this help message\n";
    }

    // Функция для проверки допустимости состояния
    bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
        for (size_t i = 0; i < count; ++i) {
            if (state == valid_states[i]) {
                return true;
            }
        }
        return false;
    }

    // Функция для проверки расширения файла
    bool has_extension(const std::string& filename, const std::string& ext) {
        return filename.size() >= ext.size() &&
            filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
    }

    // Функция для обработки аргументов командной строки
    std::vector<std::string> parse_arguments(int argc, char *argv[]) {
        // Состояния по умолчанию
        // std::string filename_bsdl = "NO FILE";
        // std::string filename_json = "NO FILE";
        
        int option_index = 0;
        int c;

        static struct option long_options[] = {
            {"bsdl", required_argument, 0, 'b'},
            {"json", required_argument, 0, 'j'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };

        while ((c = getopt_long(argc, argv, "b:j:h", long_options, &option_index)) != -1) {
            switch (c) {
                case 'b':
                    filename_bsdl = optarg;
                    if (!has_extension(filename_bsdl, ".bsdl") && !has_extension(filename_bsdl, ".bsd")) {
                        std::cerr << "Неверное расширение BSDL-файла. Доступные расширения: bsdl или bsd\n";
                        abort();
                    }
                    break;
                case 'j':
                    filename_json = optarg;
                    if (!has_extension(filename_json, ".json")) {
                        std::cerr << "Неверное расширение JSON-файла. Доступные расширения: json\n";
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

        // Вывод записанных аргументов
        if((filename_bsdl == "NO FILE") && (filename_json == "NO FILE")){
            std::cout << "Необходимо указать имя BSDL-файла и JSON-файла" << std::endl;
            abort();
        } else if (filename_bsdl == "NO FILE"){
            std::cout << "Ещё необходимо указать имя BSDL-файла" << std::endl;
            abort();
        } else if (filename_json == "NO FILE"){
            std::cout << "Ещё необходимо указать имя JSON-файла" << std::endl;
            abort();
        }
        
        std::cout << "\nBSDL-file: " << filename_bsdl << "\n";
        std::cout << "JSON-file: " << filename_json << "\n\n";
        
        std::vector<std::string> filename = {filename_bsdl, filename_json};

        return filename;
    }


    // Функции для работы с JSON
    // Функция для преобразования строки в StatePin
    PinJson::StatePin cell_value(const std::string& value) {
        return PinJson::string_to_statepin(value);
    }

    // Функция для чтения и обработки данных из JSON
    std::vector<PinJson> process_json(const json& jfile, std::vector<size_t>& pin_counts) {
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
    json read_json_file(const std::string& filename) {
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
    void print_pins(const std::vector<PinJson>& pins) {
        for (const auto& pin : pins) {
            std::cout << "Pin: " << pin.pin_name 
                    << ", Read: " << PinJson::statepin_to_string(pin.cell_read)
                    << ", Write: " << PinJson::statepin_to_string(pin.cell_write)
                    << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {

    PinJson PinJson;

    PinJson.svfGen(argc, argv);

}
