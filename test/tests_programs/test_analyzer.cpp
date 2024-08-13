// Программа реализующая парсинг и разбор лексем из файла bsd используя метод конечных автоматов

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

class PinParser{

public:
    
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
    };

    // Инициализация вектора для записи структуры PinInfo
    std::vector<PinInfo> pins;

    void parserFile(int argc, char* argv[]){
        
        PinParser PinParser;        

        std::string input = PinParser.readFile(argc, argv);

        PinParser.checkString(input);
    }

private:

    enum class TokenType : unsigned char {
        Normal,
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
    char tokenDash(){
        
        if(PrevTokenType == TokenType::Dash){
            std::cout << "комментарий" << std::endl;
        }

        PrevTokenType = TokenType::Dash;
        
        return 0;
    }

    // Основная функция которая реализует автомат
    void checkString(const std::string& str) {
        
        for (size_t count = 0, length = str.length(); count < length; ++count) {
            switch (classifyCharacter(str[count])) {
                case TokenType::Dash:
                    tokenDash();
                    break;
                
                default:
                    break;
            }
        }
    }


    // Функция для чтения файла и возврата его содержимого в виде строки
    std::string readFile(int argc, char* argv[]) {
        
        if (argc < 2) {
            std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd и флаг all или non" << std::endl;
            std::abort();
        }

        std::string filename = argv[1];

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

protected:
    // Иниципализация переменной, содержащей предыдущий токен
    TokenType PrevTokenType = TokenType::Normal;
};



int main(int argc, char* argv[]) {    

    PinParser PinParser;

    PinParser.parserFile(argc, argv);

    return 0;
}