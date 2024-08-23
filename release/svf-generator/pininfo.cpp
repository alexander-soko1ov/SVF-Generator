#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

#include "pininfo.hpp"

// Функция для преобразования str в bool 
bool BsdlPins::stringToBool(const std::string& str) {
    int value = std::stoi(str);
    return value != 0;
}

// Функция для парсинга данных о длине регистра BSDL
unsigned int BsdlPins::boundary_length(const std::string& filename) {
    std::string content = readFile(filename);
    unsigned int register_length_bsdl;

    std::regex reglenRegex(R"(BOUNDARY_LENGTH.*entity\s+is\s+(\d+);)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), reglenRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        register_length_bsdl = std::stoi(match[1].str());
    }

    return register_length_bsdl;
}

// Функция для парсинга данных о длине регистра инструкций
unsigned int BsdlPins::instruction_length(const std::string& filename) {
    std::string content = readFile(filename);
    unsigned int register_length_instr;

    std::regex reglenRegex(R"(INSTRUCTION_LENGTH.*entity\s+is\s+(\d+);)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), reglenRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        register_length_instr = std::stoi(match[1].str());
    }

    return register_length_instr;
}

// Функция для чтения файла и возврата его содержимого в виде строки
std::string BsdlPins::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Функция для преобразования строки в StatePin
BsdlPins::PinInfo::StatePin BsdlPins::PinInfo::stringToStatePin(const std::string& stateStr) {
    if (stateStr == "1") {
        return StatePin::high;
    } else if (stateStr == "0") {
        return StatePin::low;
    } else if (stateStr == "Z") {
        return StatePin::z;
    } else if (stateStr == "X") {
        return StatePin::x;
    }
    std::cerr << "Ошибка: недопустимое значение строки для StatePin: " << stateStr << std::endl;
    throw std::invalid_argument("Invalid state pin string: " + stateStr);
}

// Функция для преобразования StatePin в строку
std::string BsdlPins::PinInfo::statePinToString(StatePin state) {
    switch (state) {
        case StatePin::high:
            return "1";
        case StatePin::low:
            return "0";
        case StatePin::z:
            return "z";
        case StatePin::x:
            return "x";
        default:
            return "unknown";
    }
}

// Функция для парсинга строки с описанием пина
BsdlPins::PinInfo BsdlPins::parsePinInfo(const std::string& line) {
    std::regex pinRegex(R"(\s*(\d+)\s*\((\w+),\s*(\S*),\s*(\w+),?\s*(\w*),?\s*(\S*),?\s*(\w*).\s*(\w*))"); 
    std::smatch match;

    PinInfo pinInfo;
    if (std::regex_search(line, match, pinRegex)) {
        pinInfo.cell = std::stoi(match[1].str());
        pinInfo.label = match[3].str();
        pinInfo.function = match[4].str();
        match[5].str().empty() ? pinInfo.safeState : pinInfo.safeState = PinInfo::stringToStatePin(match[5].str()); 
        match[7].str().empty() ? pinInfo.turnOff = 0 : pinInfo.turnOff = stringToBool(match[7].str()); 
        match[8].str().empty() ? pinInfo.stateOff : pinInfo.stateOff = PinInfo::stringToStatePin(match[8].str()); 

        if (match[4].str() == "INPUT") {
            pinInfo.In = std::stoi(match[1].str());
            match[6].str().empty() ?  pinInfo.Config = 0 : pinInfo.Config = std::stoi(match[6].str());
            match[7].str().empty() ?  pinInfo.turnOff = 0 : pinInfo.turnOff = std::stoi(match[7].str());
            pinInfo.Out = 0;
        } else if (match[4].str() == "OUTPUT3") {
            pinInfo.Out = std::stoi(match[1].str());
            match[6].str().empty() ? pinInfo.Config = 0 : pinInfo.Config = std::stoi(match[6].str());
            match[7].str().empty() ?  pinInfo.turnOff = 0 : pinInfo.turnOff = std::stoi(match[7].str());
            pinInfo.In = 0;
        } else {
            pinInfo.In = 0; pinInfo.Out = 0; pinInfo.Config = 0;
        }
    }

    return pinInfo;
}

// Функция для парсинга строк с номерами пинов
std::unordered_map<std::string, std::string> BsdlPins::parsePinMap(const std::string& content) {
    std::unordered_map<std::string, std::string> pinMap;
    std::regex pinEntryRegex(R"(\s*(\w+)\s*:\s*(\w+),)");
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
std::unordered_map<std::string, std::string> BsdlPins::parsePinTypes(const std::string& content) {
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
std::vector<BsdlPins::PinInfo> BsdlPins::parseBSDFile(const std::string& content) {
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
std::vector<BsdlPins::PinInfo> BsdlPins::removeDuplicatePins(const std::vector<PinInfo>& pins) {
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
void BsdlPins::mapPinNumbersAndTypes(std::vector<PinInfo>& pins, const std::unordered_map<std::string, 
    std::string>& pinMap, const std::unordered_map<std::string, std::string>& pinTypes) {
    for (auto& pin : pins) {
        if (pinMap.find(pin.label) != pinMap.end()) {
            pin.pin = pinMap.at(pin.label);
        } else {
            pin.pin = "*";  // Используем 0 для ячеек BS, к которым не привязаны пины 
        }

        if (pinTypes.find(pin.label) != pinTypes.end()) {
            pin.pin_type = pinTypes.at(pin.label);
        } else {
            pin.pin_type = "unknown";  // Используем "unknown" для неизвестных типов
        }
    }
}

// Функция для вывода информации о пинах
// void BsdlPins::printPinInfo(std::ostream &os) { 
void BsdlPins::printPinInfo(const std::vector<BsdlPins::PinInfo>& pins) const { 
    for (const auto& pin : pins) {
        std::cout << "Pin: " << pin.pin 
            << ", Cell: " << pin.cell
            << ", Port Name: " << pin.label // condition ? true_value : false_value (pin.label.empty() ? "*" : pin.label)
            << ", Pin type: " << pin.pin_type
            << ", Function: " << pin.function 
            << ", Cell In: " << pin.In
            << ", Cell Out: " << pin.Out
            << ", Cell Config: " << pin.Config
            << ", Disable Value: " << pin.turnOff
            << ", Safe State: " << PinInfo::statePinToString(pin.safeState)
            << ", State Off: " << PinInfo::statePinToString(pin.stateOff)
            << std::endl;
    }
}
