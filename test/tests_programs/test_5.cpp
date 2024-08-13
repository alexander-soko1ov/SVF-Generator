#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    STRING,
    KEYWORD,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& filename) {
        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file");
        }
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        char ch;
        while (file.get(ch)) {
            if (isspace(ch)) continue;
            if (ch == '-' && file.peek() == '-') {
                // Skip the comment
                while (file.get(ch) && ch != '\n');
            } else if (isalpha(ch)) {
                std::string identifier;
                do {
                    identifier += ch;
                } while (file.get(ch) && (isalnum(ch) || ch == '_'));
                file.unget();
                tokens.push_back({TokenType::IDENTIFIER, identifier});
            } else if (isdigit(ch)) {
                std::string number;
                do {
                    number += ch;
                } while (file.get(ch) && isdigit(ch));
                file.unget();
                tokens.push_back({TokenType::NUMBER, number});
            } else if (ch == '"') {
                std::string str;
                while (file.get(ch) && ch != '"') {
                    str += ch;
                }
                tokens.push_back({TokenType::STRING, str});
            } else if (ispunct(ch)) {
                tokens.push_back({TokenType::SYMBOL, std::string(1, ch)});
            } else {
                tokens.push_back({TokenType::UNKNOWN, std::string(1, ch)});
            }
        }
        tokens.push_back({TokenType::END_OF_FILE, ""});
        return tokens;
    }

private:
    std::ifstream file;
};

#include <iostream>
#include <vector>
#include <string>

enum class SemanticType {
    PIN_NAME,
    TYPE_PIN,
    TYPE_DATA,
    PIN_NUMBER,
    NUMBER_CELL,
    TYPE_CELL,
    PORT_NAME,
    CELL_FUNC,
    SAFE_STATE,
    NUMBER_CONTROL_CELL,
    DIS_VALUE,
    OUTPUT_DIS_DRIVE,
    UNKNOWN
};

struct SemanticToken {
    SemanticType type;
    std::string value;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), currentIndex(0) {}

    std::vector<SemanticToken> parse() {
        std::vector<SemanticToken> semanticTokens;
        while (currentToken().type != TokenType::END_OF_FILE) {
            if (isPinDefinition()) {
                semanticTokens.push_back(parsePinDefinition());
            } else if (isPinAssignment()) {
                auto assignments = parsePinAssignments();
                semanticTokens.insert(semanticTokens.end(), assignments.begin(), assignments.end());
            } else if (isCellDefinition()) {
                auto cells = parseCellDefinitions();
                semanticTokens.insert(semanticTokens.end(), cells.begin(), cells.end());
            } else {
                advance();
            }
        }
        return semanticTokens;
    }

private:
    const std::vector<Token>& tokens;
    size_t currentIndex;

    const Token& currentToken() const {
        return tokens[currentIndex];
    }

    void advance() {
        if (currentIndex < tokens.size() - 1) {
            ++currentIndex;
        }
    }

    bool isPinDefinition() {
        return currentToken().type == TokenType::IDENTIFIER && peek(1).value == ":"
            && (peek(2).value == "in" || peek(2).value == "out" || peek(2).value == "inout");
    }

    SemanticToken parsePinDefinition() {
        std::string pinName = currentToken().value;
        advance(); // Skip pin name
        advance(); // Skip ':'
        std::string typePin = currentToken().value;
        advance();
        std::string typeData = currentToken().value;
        advance(); // Skip type data
        if (currentToken().value == ";") {
            advance(); // Skip ';'
        }

        return {SemanticType::PIN_NAME, pinName + ":" + typePin + ":" + typeData};
    }

    bool isPinAssignment() {
        return currentToken().type == TokenType::STRING && currentToken().value.find(":") != std::string::npos;
    }

    std::vector<SemanticToken> parsePinAssignments() {
        std::vector<SemanticToken> assignments;
        while (currentToken().type == TokenType::STRING) {
            std::string assignment = currentToken().value;
            size_t pos = assignment.find(":");
            if (pos != std::string::npos) {
                std::string pinName = assignment.substr(0, pos);
                std::string pinNumber = assignment.substr(pos + 1);
                pinNumber.erase(pinNumber.find_last_not_of(", ") + 1);
                assignments.push_back({SemanticType::PIN_NAME, pinName});
                assignments.push_back({SemanticType::PIN_NUMBER, pinNumber});
            }
            advance(); // Skip the string
            if (currentToken().value == "&") {
                advance(); // Skip the '&'
            }
        }
        return assignments;
    }

    bool isCellDefinition() {
        return currentToken().type == TokenType::STRING && currentToken().value.find("(") != std::string::npos;
    }

    std::vector<SemanticToken> parseCellDefinitions() {
        std::vector<SemanticToken> cells;
        while (currentToken().type == TokenType::STRING) {
            std::string cellDef = currentToken().value;
            size_t start = cellDef.find("(");
            size_t end = cellDef.find(")");
            if (start != std::string::npos && end != std::string::npos) {
                std::string def = cellDef.substr(start + 1, end - start - 1);
                std::vector<std::string> parts = split(def, ',');

                if (parts.size() >= 4) {
                    cells.push_back({SemanticType::NUMBER_CELL, trim(parts[0])});
                    cells.push_back({SemanticType::TYPE_CELL, trim(parts[1])});
                    cells.push_back({SemanticType::PORT_NAME, trim(parts[2])});
                    cells.push_back({SemanticType::CELL_FUNC, trim(parts[3])});
                    if (parts.size() > 4) {
                        cells.push_back({SemanticType::SAFE_STATE, trim(parts[4])});
                    }
                    if (parts.size() > 5) {
                        cells.push_back({SemanticType::NUMBER_CONTROL_CELL, trim(parts[5])});
                    }
                    if (parts.size() > 6) {
                        cells.push_back({SemanticType::DIS_VALUE, trim(parts[6])});
                    }
                    if (parts.size() > 7) {
                        cells.push_back({SemanticType::OUTPUT_DIS_DRIVE, trim(parts[7])});
                    }
                }
            }
            advance(); // Skip the string
            if (currentToken().value == "&") {
                advance(); // Skip the '&'
            }
        }
        return cells;
    }

    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        for (char ch : str) {
            if (ch == delimiter) {
                tokens.push_back(token);
                token.clear();
            } else {
                token += ch;
            }
        }
        tokens.push_back(token);
        return tokens;
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    const Token& peek(size_t offset) const {
        if (currentIndex + offset < tokens.size()) {
            return tokens[currentIndex + offset];
        }
        static Token eofToken = {TokenType::END_OF_FILE, ""};
        return eofToken;
    }
};

int main() {
    try {
        Lexer lexer("STM32F1_Low_density_QFPN36.bsd");
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::vector<SemanticToken> semanticTokens = parser.parse();

        for (const auto& token : semanticTokens) {
            std::cout << "Type: " << static_cast<int>(token.type) << ", Value: " << token.value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
