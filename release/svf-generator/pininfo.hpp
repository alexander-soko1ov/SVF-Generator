#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <string_view> // Подключаем string_view

// Структура для хранения информации о порте и ячейках
class BsdlPins{
public:
    
    class PinInfo {
    public:
        enum class StatePin {
            HIGH, 
            LOW,
            Z,
            X,
            PULL0,  // A weak “0” external pull down
            PULL1,  // A weak “1” external pull up
            WEAK0,  // A weak “0” internal pull down
            WEAK1,  // A weak “1” internal pull up
            KEEPER, // “Сохраненная” память состояния последнего сильно управляемого логического состояния
        };

        std::string pin;       // номер физического пина, 0 для не выведенных пинов
        unsigned int cell;      // номер ячейки BS
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

    // Метод для получения вектора ячеек
    const std::vector<PinInfo>& getCells() const {
        return cells;
    }

    // Метод для парсинга данных о длине регистра BSDL
    size_t boundaryLength(const std::string& filename);

    // Метод для парсинга данных о длине регистра инструкций
    size_t instructionLength(const std::string& filename);

    // Метод для парсинга данных битовой маски для запуска EXTEST
    std::string opcodeEXTEST(const std::string& filename, const size_t& register_length_instr);

    void loadBsdl(std::string filename){
    
        // Читаем данные из файла и записываем в переменную content
        std::string content = readFile(filename);

        // Получаем даныне о пинах и заносим данные в переменную ping (vector)
        cells = parseBSDFile(content);

        // Получаем данные об имени пина и его номере
        std::unordered_map<std::string, std::string> pinMap = parsePinMap(content);
        
        // Получаем данные об имени пина и его типе
        std::unordered_map<std::string, std::string> pinTypes = parsePinTypes(content);

        // Устанавливаем связь номеров пинов и их типов
        mapPinNumbersAndTypes(cells, pinMap, pinTypes);

        // Удаляем дублирующиеся пины
        pins = removeDuplicatePins(cells);
    }
    // Функция для вывода информации о пинах
    // void printPinInfo(std::ostream &os=std::cout);
    void printPinInfo(const std::vector<BsdlPins::PinInfo>& pins) const;

    // Функция для приведения строк к нижнему регистру
    static std::string toLowerCase(const std::string& input);

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
    std::vector<PinInfo> cells;

public:
    // Переменные для определения ANSI escape-кодов
    // Сброс всех атрибутов
    const std::string reset = "\033[0m";

    // Цвета текста
    const std::string red = "\033[31m";
    const std::string magenta = "\033[35m";
    const std::string green = "\033[32m";
    
    // Стиль текста
    const std::string bold = "\033[1m";
};
