#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <algorithm> // Для std::transform
#include <cctype>    // Для std::tolower
#include <string_view> // Подключаем string_view
#include <unordered_map>

#include "pininfo.hpp"

// Функция для преобразования str в bool 
bool BsdlPins::stringToBool(const std::string& str) {
    int value = std::stoi(str);
    return value != 0;
}

// Функция для парсинга данных о длине регистра BSDL
size_t BsdlPins::boundaryLength(const std::string& filename) {
    std::string content = readFile(filename);
    size_t register_length_bsdl = 0;

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
size_t BsdlPins::instructionLength(const std::string& filename) {
    std::string content = readFile(filename);
    size_t register_length_instr = 0;

    std::regex reglenRegex(R"(INSTRUCTION_LENGTH.*entity\s+is\s+(\d+);)");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), reglenRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        register_length_instr = std::stoi(match[1].str());
    }

    // std::cout << magenta << "register_length_instr: " << register_length_instr << reset << std::endl;

    return register_length_instr;
}

// Функция для парсинга данных битовой маски для запуска EXTEST
std::string BsdlPins::opcodeEXTEST(const std::string& filename, const size_t& register_length_instr) {
    std::string content = readFile(filename);
    std::string opcode_extest;

    std::regex reglenRegex(R"(EXTEST\s+\((\w+)\))");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), reglenRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        opcode_extest = match[1].str();
    }

    if(register_length_instr != opcode_extest.length()) {
        std::cerr << red << "Несоответствие длины регистра инструкций и команды EXTEST" << reset << std::endl;
        abort();
    }

    // std::cout << magenta << "opcode_extest: " << opcode_extest << reset << std::endl;

    return opcode_extest;
}

// Функция для чтения файла и возврата его содержимого в виде строки
std::string BsdlPins::readFile(const std::string& filename_bsdl) {

    BsdlPins BsdlPins;

    if(filename_bsdl == "NO FILE"){
        std::cerr << BsdlPins.red << "Не подключен файл BSDL" << BsdlPins.reset << std::endl;
        abort();
    }

    std::ifstream file(filename_bsdl);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Функция для преобразования строки в StatePin
BsdlPins::PinInfo::StatePin BsdlPins::PinInfo::stringToStatePin(const std::string& stateStr) {
    if (toLowerCase(stateStr) == "1") {
        return StatePin::HIGH;
    } else if (toLowerCase(stateStr) == "0") {
        return StatePin::LOW;
    } else if (toLowerCase(stateStr) == "z") {
        return StatePin::Z;
    } else if (toLowerCase(stateStr) == "x") {
        return StatePin::X;
    } else if (toLowerCase(stateStr) == "weak0") {
        return StatePin::WEAK0;
    } else if (toLowerCase(stateStr) == "weak1") {
        return StatePin::WEAK1;
    } else if (toLowerCase(stateStr) == "pull0") {
        return StatePin::PULL0;
    }  else if (toLowerCase(stateStr) == "pull1") {
        return StatePin::PULL1;
    }  else if (toLowerCase(stateStr) == "keeper") {
        return StatePin::KEEPER;
    }
    std::cerr << "Ошибка: недопустимое значение строки для StatePin: " << stateStr << std::endl;
    throw std::invalid_argument("Invalid state pin string: " + stateStr);
}

// Функция для преобразования StatePin в строку
std::string BsdlPins::PinInfo::statePinToString(StatePin state) {
    switch (state) {
        case StatePin::HIGH:
            return "1";
        case StatePin::LOW:
            return "0";
        case StatePin::Z:
            return "z";
        case StatePin::X:
            return "x";
        case StatePin::PULL0:
            return "pull0";
        case StatePin::PULL1:
            return "pull1";
        case StatePin::WEAK0:
            return "weak0";
        case StatePin::WEAK1:
            return "weak1";
        case StatePin::KEEPER:
            return "keeper";
        default:
            return "unknown";
    }
}

// Функция для приведения строк к нижнему регистру
std::string BsdlPins::toLowerCase(const std::string& input) {
    std::string result = input; // Создаем копию исходной строки
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
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

        if (toLowerCase(match[4].str()) == "input") {
            pinInfo.In = std::stoi(match[1].str());
            match[6].str().empty() ?  pinInfo.Config = 0 : pinInfo.Config = std::stoi(match[6].str());
            match[7].str().empty() ?  pinInfo.turnOff = 0 : pinInfo.turnOff = std::stoi(match[7].str());
            pinInfo.Out = 0;
        } else if (toLowerCase(match[4].str()) == "output3") {
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
    std::vector<PinInfo> cells;
    std::regex lineRegex(R"((\d+\s*\(BC_\w+,.*\)))");
    auto lines_begin = std::sregex_iterator(content.begin(), content.end(), lineRegex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; ++i) {
        std::smatch match = *i;
        PinInfo pin = parsePinInfo(match.str());
        cells.push_back(pin);
    }

    return cells;
}

// Функция для удаления дублирующихся пинов и переноса значения Cell In из дубликата в первый пин
std::vector<BsdlPins::PinInfo> BsdlPins::removeDuplicatePins(const std::vector<PinInfo>& pins) {
    std::unordered_map<std::string, PinInfo> pinMap;
    for (const auto& pin : pins) {
        if (pinMap.find(pin.pin) == pinMap.end()) {
            pinMap[pin.pin] = pin;
        } else {
            if (toLowerCase(pin.function) == "input" && toLowerCase(pinMap[pin.pin].function) == "output3") {
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
