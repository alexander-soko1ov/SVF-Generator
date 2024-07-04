#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

// Структура для хранения информации о порте
struct PortInfo {
    std::string name;
    std::string direction;
    std::string type;
    int pinNumber;
};

// Функция для чтения файла и возврата его содержимого в виде строки
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Функция для парсинга строки с описанием порта
PortInfo parsePortInfo(const std::smatch& match) {
    PortInfo portInfo;
    portInfo.name = match[1].str();
    portInfo.direction = match[2].str();
    portInfo.type = match[3].str();
    return portInfo;
}

// Функция для парсинга файла .bsd и извлечения информации о портах
std::vector<PortInfo> parseBSDFile(const std::string& content) {
    std::vector<PortInfo> ports;
    std::regex portRegex(R"(\s*(\w+)\s*:\s*(\w+)\s+(\w+);\s*)");
    auto words_begin = std::sregex_iterator(content.begin(), content.end(), portRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        ports.push_back(parsePortInfo(match));
    }

    return ports;
}

// Функция для парсинга секции PIN_MAP и создания отображения имен портов на номера пинов
std::unordered_map<std::string, int> parsePinMap(const std::string& content) {
    std::unordered_map<std::string, int> pinMap;
    std::regex pinEntryRegex(R"((\w+)\s*:\s*(\d+))");
    auto words_begin = std::sregex_iterator(content.begin(), content.end(), pinEntryRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch pinMatch = *i;
        std::string portName = pinMatch[1].str();
        int pinNumber = std::stoi(pinMatch[2].str());
        pinMap[portName] = pinNumber;
    }

    return pinMap;
}

// Основная функция
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <bsd_filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    std::string content = readFile(filename);
    std::vector<PortInfo> ports = parseBSDFile(content);
    std::unordered_map<std::string, int> pinMap = parsePinMap(content);

    // Ассоциирование номеров пинов с портами
    for (auto& port : ports) {
        if (pinMap.find(port.name) != pinMap.end()) {
            port.pinNumber = pinMap[port.name];
        } else {
            std::cerr << "Pin number for port " << port.name << " not found." << std::endl;
        }
    }

    // Вывод информации о портах
    for (const auto& port : ports) {
        std::cout << "Port Name: " << port.name 
                    << ", Direction: " << port.direction 
                    << ", Type: " << port.type 
                    << ", Pin Number: " << port.pinNumber << std::endl;
    }

    return 0;
}
