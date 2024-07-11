#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Структура для токенов и данных
struct Token {
    enum class TokenType {
    IDENTIFIER,
    NUMBER,
    COMMENT,
    END_OF_FILE
    };

    TokenType type;
    std::string lexeme;
};

class Lexer {
public:
    // Конструктор, принимающий поток ввода
    Lexer(std::istream& input) : input(input) {}

    // Функция для получения следующего токена из входного потока
    Token getNextToken() {
        Token token;
        char currentChar = getNextChar();

        if (currentChar == '-') {
            char nextChar = getNextChar();
            if (nextChar == '-') {
                skipSingleLineComment(); // Однострочный комментарий
                return getNextToken(); // Рекурсивно получаем следующий токен после комментария
            } else {
                putBackChar(nextChar); // Возвращаем символ обратно в поток
                token.lexeme += currentChar;
                token.type = Token::TokenType::IDENTIFIER; 
            }
        } else if (currentChar == EOF) {
            token.type = Token::TokenType::END_OF_FILE;
        } else {
            token.lexeme += currentChar;
            token.type = Token::TokenType::IDENTIFIER;
        }

        return token;
    }

private:
    std::istream& input;
    std::stringstream buffer;

    // Функция для получения следующего символа из потока
    char getNextChar() {
        char c;
        if (buffer.get(c)) {
            return c;
        } else {
            return input.get();
        }
    }

    // Функция для возврата символа в поток
    void putBackChar(char c) {
        buffer.putback(c);
    }

    // Функция для пропуска однострочного комментария
    void skipSingleLineComment() {
        char c;
        while ((c = getNextChar()) != '\n' && c != EOF) {
            // Пропускаем символы до конца строки или конца файла
        }
        if (c == '\n') {
            putBackChar(c); // Возвращаем символ новой строки в поток
        }
    }
};

// Функция для чтения содержимого файла и передачи его в std::istringstream
std::istringstream getFileContentAsStream(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::ostringstream oss;
    oss << file.rdbuf(); // Считываем содержимое файла в строковый поток
    std::string fileContent = oss.str();

    return std::istringstream(fileContent); // Возвращаем std::istringstream с содержимым файла
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd" << std::endl;
        return 1;
    }
   
    std::string filename = argv[1];
    
    // Использование функции для получения std::istringstream из файла
    std::istringstream inputCode = getFileContentAsStream(filename);

    // Создаем лексер с переданным в него потоком данных
    Lexer lexer(inputCode);
    Token token;
    do {
        token = lexer.getNextToken();
        if (token.type != Token::TokenType::END_OF_FILE) {
            std::cout << token.lexeme;
        }
    } while (token.type != Token::TokenType::END_OF_FILE);

    std::cout << std::endl;

    return 0;
}
