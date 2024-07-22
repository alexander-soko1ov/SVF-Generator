#ifndef PININFO_HPP
#define PININFO_HPP

#include <string>
#include <vector>
#include <unordered_map>

// Структура для хранения информации о порте и ячейках
struct PinInfo {
    enum class StatePin {
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
    std::string function;  // <function> ячейки BS
    bool turnOff;          // при каком значении в ячейки происходит отключение драйвера 1 или 0
    
    std::string stateOff;  // состояние выходного отключенного драйвера z, 1 (high), 0 (low)
    std::string safeState; // безопасное значение ячейки X, 1 (high), 0 (low)
    
    // void f () {} // метод может быть вызван для не const объекта
    // void f () const {} // метод может быть вызван только для const объекта
};

// Функция для преобразования str в bool
bool stringToBool(const std::string& str);

// Функция для чтения файла и возврата его содержимого в виде строки
std::string readFile(const std::string& filename);

// Функция для парсинга строки с описанием пина
PinInfo parsePinInfo(const std::string& line);

// Функция для парсинга строк с номерами пинов
std::unordered_map<std::string, std::string> parsePinMap(const std::string& content);

// Функция для парсинга строк с типами пинов
std::unordered_map<std::string, std::string> parsePinTypes(const std::string& content);

// Функция для парсинга файла .bsd и извлечения информации о пинах
std::vector<PinInfo> parseBSDFile(const std::string& content);

// Функция для удаления дублирующихся пинов и переноса значения Cell In из дубликата в первый пин
std::vector<PinInfo> removeDuplicatePins(const std::vector<PinInfo>& pins);

// Функция для установки связи номеров пинов и их типов
void mapPinNumbersAndTypes(std::vector<PinInfo>& pins, const std::unordered_map<std::string, 
    std::string>& pinMap, const std::unordered_map<std::string, std::string>& pinTypes);

// Функция для вывода информации о пинах
void printPinInfo(const std::vector<PinInfo>& pins);

#endif // PININFO_HPP