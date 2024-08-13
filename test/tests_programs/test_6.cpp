#include <iostream>
#include <string>
#include <fstream>

class PinInfo {
    public:
        // enum class StatePin {
        //     high, 
        //     low,
        //     z,
        //     x
        // };

        std::string pin_name;       // номер физического пина, 0 для не выведенных пинов
        std::string pin_label;      // название физического пина
        std::string pin_type;       // тип ячейки in, out, inout
        unsigned int cell_in;       // номер ячейки ввода
        unsigned int cell_out;      // номер ячейки вывода
        unsigned int cell_config;   // ячейка управления
        std::string cell_function;  // <function> ячейки BS
        bool turnOff;               // при каком значении в ячейки происходит отключение драйвера 1 или 0
        std::string stateOff;       // состояние выходного отключенного драйвера z, 1 (high), 0 (low)
        std::string safeState;      // безопасное значение ячейки X, 1 (high), 0 (low)
        std::string comment;        // комментарий
};

enum class TokenType : unsigned char {
    Start,
    Dash,
    Letter,
    Number,
    Space,
    Tab,
    Colon,
    Semicolon,
    Ampersand,
    NewLine,
    OpenBracket,
    CloseBracket,
    OpenQuotes,
    CloseQuotes,
    Comma,
    LowerDash,
    Star,
    End,
    Other
};

TokenType classifyCharacter(char symbol) {
    if (symbol >= '0' && symbol <= '9') {
        return TokenType::Number;
    } else if ((symbol >= 'a' && symbol <= 'z')||(symbol >= 'A' && symbol <= 'Z')) {
        return TokenType::Letter;
    } else if (symbol == '-') {
        return TokenType::Dash;
    } else if (symbol == '\n') {
        return TokenType::NewLine;
    } else if (symbol == ' ') {
        return TokenType::Space;
    } else if (symbol == '\t') {
        return TokenType::Tab;
    } else if (symbol == '\0') {
        return TokenType::End;
    } else if (symbol == '&') {
        return TokenType::Ampersand;
    } else {
        return TokenType::Other;
    }
}

// Функция реализующая работу с комментариями
tokenDash(std::string symbol){
    
}

// Основная функция которая реализует автомат
void checkString(const std::string& str) {
    for (char symbol : str) {
        switch (classifyCharacter(symbol)) {
            case TokenType::Dash:
                tokenDash(symbol);
                break;
            default:
                break;
        }
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd и флаг all или non" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::string input = readFile(filename);

    checkString(input);
    
    return 0;
}
