#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;


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

int main() {

    string input =
    "--This text is pretty long, but will be concatenated into just a single string.\n"
    " attribute PIN_MAP of STM32F1_Low_density_QFPN36 : entity is PHYSICAL_PIN_MAP;\n"
    "boot0: 35;\n"
    "PA0  : 7, &\n"
    "PA   : 8, &\n";
    
    istringstream my_stream(input);

    string token;

    while (my_stream >> token) {
		cout << token;
	}

    // for (char str : input) {
    //     // cout << input << endl;
    //     cout << isAlpha(str);
    // }

    cout << endl;

    return 0;
}
