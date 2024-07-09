#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>


// Структура для хранения информации о пине
struct PinInfo {
    int cellNumber;
    std::string cellType;
    std::string portName;
    std::string function;
    std::string safeState;
    std::string controlCellNumber;
    std::string disableValue;
    std::string disabledDriverState;
    int pinNumber;
    std::string direction;
};

// Функция для чтения файла и возврата его содержимого в виде строки
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Функция для парсинга строки с описанием пина
PinInfo parsePinInfo(const std::string& line) {
    std::regex pinRegex(R"(\s*(\d+)\s*\(BC_(\d+),\s*(\S*),\s*(\S+),?\s*(\S*),?\s*(\S*),?\s*(\S*),?\s*(\S*)\))");
    std::smatch match;

    PinInfo pinInfo;
    if (std::regex_search(line, match, pinRegex)) {
        pinInfo.cellNumber = std::stoi(match[1].str());
        pinInfo.cellType = "BC_" + match[2].str();
        pinInfo.portName = match[3].str();
        pinInfo.function = match[4].str();
        pinInfo.safeState = match[5].str();
        pinInfo.controlCellNumber = match[6].str();
        pinInfo.disableValue = match[7].str();
        pinInfo.disabledDriverState = match[8].str();
    }

    return pinInfo;
}

// Функция для парсинга строк с номерами ячеек
std::unordered_map<std::string, int> parsePinMap(const std::string& content) {
    std::unordered_map<std::string, int> pinMap;
    std::regex pinEntryRegex(R"(\s*(\w+)\s*:\s*(\d+),)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), pinEntryRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        std::string portName = match[1].str();
        int pinNumber = std::stoi(match[2].str());
        pinMap[portName] = pinNumber;
    }

    return pinMap;
}

// Функция для парсинга строк с типами пинов
std::unordered_map<std::string, std::string> parsePinTypes(const std::string& content) {
    std::unordered_map<std::string, std::string> pinTypes;
    std::regex pinTypeRegex(R"(\s*(\w+)\s*:\s*(in|out|inout)\s+bit;)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), pinTypeRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        std::string portName = match[1].str();
        std::string direction = match[2].str();
        pinTypes[portName] = direction;
    }

    return pinTypes;
}

// Функция для парсинга файла .bsd и извлечения информации о пинах
std::vector<PinInfo> parseBSDFile(const std::string& content) {
    std::vector<PinInfo> pins;
    std::regex lineRegex(R"((\d+\s*\(BC_\d+,.*\)))");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), lineRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        PinInfo pin = parsePinInfo(match.str());
        pins.push_back(pin);
    }

    return pins;
}

// Основная функция
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргумента необходимый файл bsd" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    try {
        std::string content = readFile(filename);
        std::vector<PinInfo> pins = parseBSDFile(content);
        std::unordered_map<std::string, int> pinMap = parsePinMap(content);
        std::unordered_map<std::string, std::string> pinTypes = parsePinTypes(content);

        // Устанавливаем связь номеров пинов и типов с пинами
        for (auto& pin : pins) {
            if (pinMap.find(pin.portName) != pinMap.end()) {
                pin.pinNumber = pinMap[pin.portName];
            } else {
                pin.pinNumber = -1;  // Используем -1 для ячеек, к которым не привязаны пины (да, это номера пинов, всё сложно) 
            }

            if (pinTypes.find(pin.portName) != pinTypes.end()) {
                pin.direction = pinTypes[pin.portName];
            } else {
                pin.direction = "unknown";  // Используем "unknown" для неизвестных типов
            }
        }

        // Вывод информации о пинах
        for (const auto& pin : pins) {
            std::cout << "Pin Number: " << pin.pinNumber 
                      << ", Cell Number: " << pin.cellNumber
                      << ", Cell Type: " << pin.cellType 
                      << ", Port Name: " << (pin.portName.empty() ? "*" : pin.portName)
                      << ", Direction: " << pin.direction
                      << ", Function: " << pin.function 
                      << ", Safe State: " << (pin.safeState.empty() ? "N/A" : pin.safeState)
                      << ", Control Cell Number: " << (pin.controlCellNumber.empty() ? "N/A" : pin.controlCellNumber)
                      << ", Disable Value: " << (pin.disableValue.empty() ? "N/A" : pin.disableValue)
                      << ", Disabled Driver State: " << (pin.disabledDriverState.empty() ? "N/A" : pin.disabledDriverState)
                      << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}