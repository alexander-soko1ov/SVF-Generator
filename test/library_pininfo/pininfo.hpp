#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// Структура для хранения информации о порте и ячейках

class BsdlPins{
public:
    
    class PinInfo {
    public:
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
        
        StatePin stateOff;  // состояние выходного отключенного драйвера z, 1 (high), 0 (low)
        StatePin safeState; // безопасное значение ячейки X, 1 (high), 0 (low)

        // Метод для преобразования строки в StatePin
        static StatePin stringToStatePin(const std::string& str);

        // Метод для преобразования StatePin в строку
        static std::string statePinToString(StatePin state);
    };

    // Метод для получения вектора пинов
    const std::vector<PinInfo>& getPins() const {
        return pins;
    }
    
    void loadBsdl(std::string filename){
    
        // Читаем данные из файла и записываем в переменную content
        std::string content = readFile(filename);

        // Получаем даныне о пинах и заносим данные в переменную ping (vector)
        pins = parseBSDFile(content);

        // Получаем данные об имени пина и его номере
        std::unordered_map<std::string, std::string> pinMap = parsePinMap(content);
        
        // Получаем данные об имени пина и его типе
        std::unordered_map<std::string, std::string> pinTypes = parsePinTypes(content);

        // Устанавливаем связь номеров пинов и их типов
        mapPinNumbersAndTypes(pins, pinMap, pinTypes);

        // Удаляем дублирующиеся пины
        pins = removeDuplicatePins(pins);

    }
    // Функция для вывода информации о пинах
    // void printPinInfo(std::ostream &os=std::cout);
    void printPinInfo(const std::vector<BsdlPins::PinInfo>& pins) const;

protected:
    // Защищенные методы для выполнения операций

    // Функция для чтения файла и возврата его содержимого в виде строки
    static std::string readFile(const std::string& filename);

    // Функция для преобразования str в bool
    static bool stringToBool(const std::string& str);

    // Функция для парсинга файла .bsd и извлечения информации о пинах
    static std::vector<PinInfo> parseBSDFile(const std::string& content);

    // Функция для парсинга строки с описанием пина
    static PinInfo parsePinInfo(const std::string& line);

    // Функция для парсинга строк с номерами пинов
    static std::unordered_map<std::string, std::string> parsePinMap(const std::string& content);

    // Функция для парсинга строк с типами пинов
    static std::unordered_map<std::string, std::string> parsePinTypes(const std::string& content);

    // Функция для удаления дублирующихся пинов и переноса значения Cell In из дубликата в первый пин
    static std::vector<PinInfo> removeDuplicatePins(const std::vector<PinInfo>& pins);

    // Функция для установки связи номеров пинов и их типов
    static void mapPinNumbersAndTypes(std::vector<PinInfo>& pins, const std::unordered_map<std::string, 
        std::string>& pinMap, const std::unordered_map<std::string, std::string>& pinTypes);

private:
    std::vector<PinInfo> pins;
};
