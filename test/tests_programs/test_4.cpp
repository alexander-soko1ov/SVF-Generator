#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <variant>

// Определение состояний конечного автомата
enum class State {
    Start,
    Keyword,
    Comment,
    PinName,
    PinNumber,
    CellName,
    CellNumber,
    End
};

// Лексический анализатор
class Lexer {
public:
    Lexer(const std::string& input) : input(input), state(State::Start), pos(0) {}

    // Метод для распределения лексем по токенам
    void tokenize() {
        while (pos < input.size()) {
            char currentChar = input[pos];

            switch (state) {
                case State::Start:
                    if (currentChar == '-' && pos + 1 < input.size() && input[pos + 1] == '-') {
                        state = State::Comment;
                        pos += 2;
                        currentComment = "--";
                    } else {
                        // Добавить обработки других лексем
                        ++pos;
                    }
                    break;

                case State::Comment:
                    if (currentChar == '\n' || pos == input.size() - 1) {
                        if (pos == input.size() - 1) {
                            currentComment += currentChar;
                        }
                        comments.push_back(currentComment);
                        state = State::Start;
                    } else {
                        currentComment += currentChar;
                    }
                    ++pos;
                    break;

                default:
                    ++pos;
                    break;
            }
        }
    }

    const std::vector<std::string>& getComments() const {
        return comments;
    }

private:
    std::string input;
    State state;
    size_t pos;
    std::string currentComment;
    std::vector<std::string> comments;
};

// Функция для чтения файла
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка чтения файла: " + filename);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Добавьте в качестве аргументов необходимый файл .bsd" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string input;

    input = readFile(filename);

    Lexer lexer(input);
    lexer.tokenize();

    const auto& comments = lexer.getComments();
    for (const auto& comment : comments) {
        std::cout << "Comment: " << comment << std::endl;
    }

    return 0;
}
