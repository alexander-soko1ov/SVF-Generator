#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <sstream>
#include <vector>
#include <cctype>

// Перечисление для состояний конечного автомата
enum class State {
    Normal,
    Comment,
    Literal,
    Whitespace
};


struct Lexer {
    std::string comments;
    std::string literals;

    // Функция для сбора всех комментариев
    void collectComments(const std::string& code) {
        State state = State::Normal;
        std::stringstream commentBuffer;

        for (size_t i = 0; i < code.size(); ++i) {
            char c = code[i];
            switch (state) {
                case State::Normal:
                    if (c == '-' && i + 1 < code.size() && code[i + 1] == '-') {
                        state = State::Comment;
                        ++i; // Пропустить следующий '-'
                    }
                    break;
                case State::Comment:
                    if (c == '\n') {
                        state = State::Normal;
                        commentBuffer << '\n';
                    } else {
                        commentBuffer << c;
                    }
                    break;
                default:
                    break;
            }
        }
        comments = commentBuffer.str();
    }

    // Функция для сбора всех составных литералов
    void collectLiterals(const std::string& code) {
        State state = State::Normal;
        std::stringstream literalBuffer;
        bool firstLiteral = true;

        for (size_t i = 0; i < code.size(); ++i) {
            char c = code[i];
            switch (state) {
                case State::Normal:
                    if (c == '"') {
                        state = State::Literal;
                        if (!firstLiteral) {
                            literalBuffer << " & ";
                        }
                        firstLiteral = false;
                    }
                    break;
                case State::Literal:
                    if (c == '"') {
                        state = State::Normal;
                    } else {
                        literalBuffer << c;
                    }
                    break;
                default:
                    break;
            }
        }
        literals = literalBuffer.str();
    }

    // Функция для удаления пустых строк и незначащих строк
    std::string removeEmptyLines(const std::string& code) {
        std::istringstream stream(code);
        std::string line;
        std::stringstream cleanedCode;

        while (std::getline(stream, line)) {
            // Проверка, содержит ли строка только пробелы или пустая
            if (!line.empty() && !std::all_of(line.begin(), line.end(), isspace)) {
                cleanedCode << line << '\n';
            }
        }
        return cleanedCode.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd" << std::endl;
        return 1;
    }
   
    std::string filename = argv[1];
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    Lexer lexer;
    lexer.collectComments(code);
    lexer.collectLiterals(code);
    std::string cleanedCode = lexer.removeEmptyLines(code);

    // std::cout << "Comments:\n" << lexer.comments << "\n";
    std::cout << "Composite Literals:\n" << lexer.literals << "\n";
    // std::cout << "Cleaned Code:\n" << cleanedCode << "\n";

    return 0;
}
