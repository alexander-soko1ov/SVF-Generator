#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

// Структура для хранения информации о порте и ячейках
struct pins {
    enum class StatePin { // enum class StatePin : unsigned char (явное определение типа данных)
        high, 
        low,
        z,
        x
    };

    std::string pin;       // номер физического пина, 0 для не выведенных пинов
    std::string label;     // название физического пина
    std::string pin_type;  // тип ячейки in, out, inout
    unsigned int In;       // номер ячейки ввода
    unsigned int Out;      // номер ячейки вывода
    unsigned int Config;   // ячейка управления
    std::string function;  // <function> ячейки boundary scan  
    bool turnOff;          // при каком значении в ячейки происходит отключение драйвера 1 или 0
    StatePin stateOff;     // состояние выходного отключенного драйвера z, 1 (high), 0 (low)
    StatePin stateSafe;    // безопасное значение ячейки X, 1 (high), 0 (low)
};

// Структура для хранения информации о пине
struct PinInfo {
    std::string pin;
    std::string label;
    std::string pin_type;

    unsigned int In;       // номер ячейки ввода
    unsigned int Out;      // номер ячейки вывода
    unsigned int Config; 

    // std::string cellType;
    std::string function;
    // std::string controlCellNumber;
    
    bool disableValue;
    
    std::string stateOff;
    std::string safeState;
};

// Функция для преобразования str в bool
bool stringToBool(const std::string& str) {
    int value = std::stoi(str);
    return value != 0;
}

// Функция для преобразования str в enum 
std::string stringToEnum(pins::StatePin StatePin) {
    switch(StatePin) {
        case pins::StatePin::high : return "1";
        case pins::StatePin::low : return "0";
        case pins::StatePin::z : return "z";
        case pins::StatePin::x : return "x";
        default: return "UNKNOWN";
    }
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
    // (\s*(\d+)\s*\((\w+),\s*(\S*),\s*(\w+),?\s*(\S*),?\s*(\S*),?\s*(\S*),?\s*(\S*)\))
    std::smatch match;

    PinInfo pinInfo;
    if (std::regex_search(line, match, pinRegex)) {
        // pinInfo.cellType = match[2].str();
        pinInfo.label = match[3].str();
        pinInfo.function = match[4].str();
        pinInfo.safeState = match[5].str();
        // pinInfo.controlCellNumber = match[6].str();
        match[7].str().empty() ? pinInfo.disableValue = 0 : pinInfo.disableValue = stringToBool(match[7].str()); 
        // может лучше сделать вместо bool str, чтобы можно было выводить N/A для отдельных ячеек

        pinInfo.stateOff = match[8].str();

        if (match[4].str() == "INPUT") {
            pinInfo.In = std::stoi(match[1].str());
            match[6].str().empty() ? 0 : pinInfo.Config = std::stoi(match[6].str());
            pinInfo.Out = 0;
        } else if (match[4].str() == "OUTPUT3") {
            pinInfo.Out = std::stoi(match[1].str());
            match[6].str().empty() ? 0 : pinInfo.Config = std::stoi(match[6].str());
            pinInfo.In = 0;
        } else {
            pinInfo.In = 0; pinInfo.Out = 0; pinInfo.Config = 0;
        }
    }

    return pinInfo;
}

// Функция для парсинга строк с номерами ячеек
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

// Основная функция
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd и флаг all или non" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string flag = argv[2];
       
    std::string content = readFile(filename);
    std::vector<PinInfo> pins = parseBSDFile(content);
    std::unordered_map<std::string, std::string> pinMap = parsePinMap(content);
    std::unordered_map<std::string, std::string> pinTypes = parsePinTypes(content);

    // Устанавливаем связь номеров пинов и их типов
    for (auto& pin : pins) {
        if (pinMap.find(pin.label) != pinMap.end()) {
            pin.pin = pinMap[pin.label];
        } else {
            pin.pin = "0";  // Используем 0 для ячеек BS, к которым не привязаны пины 
        }

        if (pinTypes.find(pin.label) != pinTypes.end()) {
            pin.pin_type = pinTypes[pin.label];
        } else {
            pin.pin_type = "unknown";  // Используем "unknown" для неизвестных типов
        }
    }

    // Вывод информации о пинах
    for (const auto& pin : pins) {
        if (pin.pin != "0" || flag == "all") {
            std::cout << "Pin: " << pin.pin 
                << ", Port Name: " << (pin.label.empty() ? "*" : pin.label) // condition ? true_value : false_value
                << ", Pin type: " << pin.pin_type
                << ", Function: " << pin.function 
                << ", Cell In: " << pin.In
                << ", Cell Out: " << pin.Out
                << ", Cell Config: " << pin.Config
                << ", Disable Value: " << pin.disableValue
                << ", Safe State: " << (pin.safeState.empty() ? "N/A" : pin.safeState)
                << ", State Off: " << (pin.stateOff.empty() ? "N/A" : pin.stateOff)
                << std::endl;
        }
    }
    return 0;
}