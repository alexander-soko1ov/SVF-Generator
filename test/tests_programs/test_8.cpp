#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

// Подключаем пространство имен для удобства
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
};

// Функция для преобразования строки в StatePin
PinJson::StatePin cell_value(const std::string& value) {
    return PinJson::string_to_statepin(value);
}

// Функция для чтения и обработки данных из JSON
std::vector<PinJson> process_json(const json& j, std::vector<size_t>& pin_counts) {
    std::vector<PinJson> pins;

    for (const auto& item : j) {
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

    json j;
    input_file >> j;
    return j;
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

int main() {
    try {
        // Чтение JSON из файла
        json j = read_json_file("STM32F1_Low_density_QFPN36_test.json");

        // Вектор для хранения количества пинов
        std::vector<size_t> pin_counts;

        // Обработка JSON и получение данных
        std::vector<PinJson> pins = process_json(j, pin_counts);

        // Вывод результата для проверки
        print_pins(pins);

        // Вывод количества пинов для каждого объекта JSON
        std::cout << "/nКоличество пинов в каждом объекте JSON:" << std::endl;
        for (size_t count : pin_counts) {
            std::cout << count << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
