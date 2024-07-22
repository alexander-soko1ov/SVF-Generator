#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

#include "pininfo.hpp"

// Функция для преобразования str в bool
bool stringToBool(const std::string& str) {
    int value = std::stoi(str);
    return value != 0;
}

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
    std::regex pinRegex(R"(\s*(\d+)\s*\((\w+),\s*(\S*),\s*(\w+),?\s*(\w*),?\s*(\S*),?\s*(\w*).\s*(\w*))"); 
    std::smatch match;

    PinInfo pinInfo;
    if (std::regex_search(line, match, pinRegex)) {
        pinInfo.label = match[3].str();
        pinInfo.function = match[4].str();
        pinInfo.safeState = match[5].str();
        match[7].str().empty() ? pinInfo.turnOff = 0 : pinInfo.turnOff = stringToBool(match[7].str()); 
        pinInfo.stateOff = match[8].str();

        if (match[4].str() == "INPUT") {
            pinInfo.In = std::stoi(match[1].str());
            match[6].str().empty() ?  pinInfo.Config = 0 : pinInfo.Config = std::stoi(match[6].str());
            pinInfo.Out = 0;
        } else if (match[4].str() == "OUTPUT3") {
            pinInfo.Out = std::stoi(match[1].str());
            match[6].str().empty() ? pinInfo.Config = 0 : pinInfo.Config = std::stoi(match[6].str());
            pinInfo.In = 0;
        } else {
            pinInfo.In = 0; pinInfo.Out = 0; pinInfo.Config = 0;
        }
    }

    return pinInfo;
}

// Функция для парсинга строк с номерами пинов
std::unordered_map<std::string, std::string> parsePinMap(const std::string& content) {
    std::unordered_map<std::string, std::string> pinMap;
    std::regex pinEntryRegex(R"(\s*(\w+)\s*:\s*(\d+),)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), pinEntryRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        std::string label = match[1].str();
        std::string pin = match[2].str();
        pinMap[label] = pin;
    }

    return pinMap;
}

// Функция для парсинга строк с типами пинов
std::unordered_map<std::string, std::string> parsePinTypes(const std::string& content) {
    std::unordered_map<std::string, std::string> pinTypes;
    std::regex pinTypeRegex(R"(\s*(\w+)\s*:\s*(\w+)\s*(\w*);)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), pinTypeRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        std::string label = match[1].str();
        std::string pin_type = match[2].str();
        pinTypes[label] = pin_type;
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

// Функция для удаления дублирующихся пинов и переноса значения Cell In из дубликата в первый пин
std::vector<PinInfo> removeDuplicatePins(const std::vector<PinInfo>& pins) {
    std::unordered_map<std::string, PinInfo> pinMap;
    for (const auto& pin : pins) {
        if (pinMap.find(pin.pin) == pinMap.end()) {
            pinMap[pin.pin] = pin;
        } else {
            if (pin.function == "INPUT" && pinMap[pin.pin].function == "OUTPUT3") {
                pinMap[pin.pin].In = pin.In;
            }
        }
    }
    
    std::vector<PinInfo> result;
    for (const auto& pair : pinMap) {
        result.push_back(pair.second);
    }

    return result;
}

// Функция для установки связи номеров пинов и их типов
void mapPinNumbersAndTypes(std::vector<PinInfo>& pins, const std::unordered_map<std::string, 
    std::string>& pinMap, const std::unordered_map<std::string, std::string>& pinTypes) {
    for (auto& pin : pins) {
        if (pinMap.find(pin.label) != pinMap.end()) {
            pin.pin = pinMap.at(pin.label);
        } else {
            pin.pin = "0";  // Используем 0 для ячеек BS, к которым не привязаны пины 
        }

        if (pinTypes.find(pin.label) != pinTypes.end()) {
            pin.pin_type = pinTypes.at(pin.label);
        } else {
            pin.pin_type = "unknown";  // Используем "unknown" для неизвестных типов
        }
    }
}

// Функция для вывода информации о пинах
void printPinInfo(const std::vector<PinInfo>& pins) {
    for (const auto& pin : pins) {
        std::cout << "Pin: " << pin.pin 
            << ", Port Name: " << (pin.label.empty() ? "*" : pin.label) // condition ? true_value : false_value
            << ", Pin type: " << pin.pin_type
            << ", Function: " << pin.function 
            << ", Cell In: " << pin.In
            << ", Cell Out: " << pin.Out
            << ", Cell Config: " << pin.Config
            << ", Disable Value: " << pin.turnOff
            << ", Safe State: " << (pin.safeState.empty() ? "N/A" : pin.safeState)
            << ", State Off: " << (pin.stateOff.empty() ? "N/A" : pin.stateOff)
            << std::endl;
}
}