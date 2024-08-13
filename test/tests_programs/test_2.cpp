#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// Перечисление для определения различных типов токенов
enum class TokenType {
    KEYWORD,            // ключевое слово
    IDENTIFIER,         // идентификатор
    INTEGER_LITERAL,    // целочисленный литерал
    FLOAT_LITERAL,      // литерал с плавающей точкой
    OPERATOR,           // оператор
    PUNCTUATOR,         // знак препинания
    UNKNOWN             // неизвестный тип
};

// Структура для представления токена с его типом и значением
struct Token {
    TokenType type;     // тип токена
    string value;       // значение токена

    Token(TokenType t, const string& v)
        : type(t), value(v)
    {
    }
};

// Класс, реализующий лексический анализатор
class LexicalAnalyzer {
private:
    string input;                           // входная строка
    size_t position;                        // текущая позиция в строке
    unordered_map<string, TokenType> keywords;  // хэш-таблица для ключевых слов

    // Функция для инициализации хэш-таблицы с ключевыми словами
    void initKeywords()
    {
        keywords["int"] = TokenType::KEYWORD;
        keywords["float"] = TokenType::KEYWORD;
        keywords["if"] = TokenType::KEYWORD;
        keywords["else"] = TokenType::KEYWORD;
        keywords["while"] = TokenType::KEYWORD;
        keywords["return"] = TokenType::KEYWORD;
    }

    // Функция для проверки, является ли символ пробелом
    bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n'
               || c == '\r';
    }

    // Функция для проверки, является ли символ буквой
    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z')
               || (c >= 'A' && c <= 'Z');
    }

    // Функция для проверки, является ли символ цифрой
    bool isDigit(char c) { return c >= '0' && c <= '9'; }

    // Функция для проверки, является ли символ буквенно-цифровым
    bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    // Функция для получения следующего слова (идентификатора или ключевого слова)
    // из входной строки
    string getNextWord()
    {
        size_t start = position;
        while (position < input.length()
               && isAlphaNumeric(input[position])) {
            position++;
        }
        return input.substr(start, position - start);
    }

    // Функция для получения следующего числа (целого или с плавающей точкой)
    // из входной строки
    string getNextNumber()
    {
        size_t start = position;
        bool hasDecimal = false;
        while (position < input.length()
               && (isDigit(input[position])
                   || input[position] == '.')) {
            if (input[position] == '.') {
                if (hasDecimal)
                    break;
                hasDecimal = true;
            }
            position++;
        }
        return input.substr(start, position - start);
    }

public:
    // Конструктор для LexicalAnalyzer
    LexicalAnalyzer(const string& source)
        : input(source), position(0)
    {
        initKeywords();
    }

    // Функция для токенизации входной строки
    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while (position < input.length()) {
            char currentChar = input[position];

            // Пропускаем пробелы
            if (isWhitespace(currentChar)) {
                position++;
                continue;
            }

            // Определяем ключевые слова или идентификаторы
            if (isAlpha(currentChar)) {
                string word = getNextWord();
                if (keywords.find(word) != keywords.end()) {
                    tokens.emplace_back(TokenType::KEYWORD,
                                        word);
                }
                else {
                    tokens.emplace_back(
                        TokenType::IDENTIFIER, word);
                }
            }
            // Определяем целые или числа с плавающей точкой
            else if (isDigit(currentChar)) {
                string number = getNextNumber();
                if (number.find('.') != string::npos) {
                    tokens.emplace_back(
                        TokenType::FLOAT_LITERAL, number);
                }
                else {
                    tokens.emplace_back(
                        TokenType::INTEGER_LITERAL, number);
                }
            }
            // Определяем операторы
            else if (currentChar == '+'
                     || currentChar == '-'
                     || currentChar == '*'
                     || currentChar == '='
                     || currentChar == '/') {
                tokens.emplace_back(TokenType::OPERATOR,
                                    string(1, currentChar));
                position++;
            }
            // Определяем знаки препинания
            else if (currentChar == '('
                     || currentChar == ')'
                     || currentChar == '{'
                     || currentChar == '}'
                     || currentChar == ';') {
                tokens.emplace_back(TokenType::PUNCTUATOR,
                                    string(1, currentChar));
                position++;
            }
            // Обрабатываем неизвестные символы
            else {
                tokens.emplace_back(TokenType::UNKNOWN,
                                    string(1, currentChar));
                position++;
            }
        }

        return tokens;
    }
};

// Функция для преобразования TokenType в строку для вывода
string getTokenTypeName(TokenType type)
{
    switch (type) {
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::INTEGER_LITERAL:
        return "INTEGER_LITERAL";
    case TokenType::FLOAT_LITERAL:
        return "FLOAT_LITERAL";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::PUNCTUATOR:
        return "PUNCTUATOR";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNDEFINED";
    }
}

// Функция для вывода всех токенов
void printTokens(const vector<Token>& tokens)
{
    for (const auto& token : tokens) {
        cout << "Тип: " << getTokenTypeName(token.type)
             << ", Значение: " << token.value << endl;
    }
}

// Основная программа
int main()
{
    // Пример исходного кода для анализа
    string sourceCode
        = "int main() { float x = 3.14; float y=3.15; "
          "float z=x+y; return 0; }";

    // Создаем объект LexicalAnalyzer
    LexicalAnalyzer lexer(sourceCode);

    // Токенизируем исходный код
    vector<Token> tokens = lexer.tokenize();

    // Выводим исходный код
    cout << "Исходный код: " << sourceCode << endl << endl;

    // Выводим все найденные токены
    cout << "Разложение на лексемы:" << endl;
    printTokens(tokens);

    return 0;
}
