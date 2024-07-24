#include <iostream>
#include <getopt.h>
#include <string>
#include <unordered_set>

// Функция вывода help
void print_usage() {
    std::cout << "Usage: program [options]\n"
              << "Options:\n"
              << "  -b, --bsdl    Add a BSDL-file\n"
              << "  -j, --json   Add a JSON-file\n"
              << "  -t, --trst   Set the TRST state (ON, OFF, z, ABSENT)\n"
              << "  -i, --endir  Set the ENDIR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
              << "  -d, --enddr  Set the ENDDR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
              << "  -h, --help   Show this help message\n";
}

// Функция проверки корректности id введённого в CLI аргументов
bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (state == valid_states[i]) {
            return true;
        }
    }
    return false;
}

// Функция проверки расширения файла
bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos && pos != filename.length() - 1) {
        std::string fileExt = filename.substr(pos + 1);
        return validExtensions.find(fileExt) != validExtensions.end();
    }
    return false;
}

// Функция парсинга аргументов и вывода записанных аргументов в консоль
void parse_arguments(int argc, char *argv[]){

    // Инициализация вспомагательных переменных
    int option_index = 0;
    int c;

    // Установка состояний по умолчанию
    std::string filename_bsd = "NO FILE"; 
    std::string filename_json = "NO FILE";  
    std::string trst_state = "OFF"; 
    std::string endir_state = "IDLE";  
    std::string enddr_state = "IDLE";  

    // Инициализация доступных аргументов
    static struct option long_options[] = {
        {"bsdl", required_argument, 0, 'b'},
        {"json", required_argument, 0, 'j'},
        {"trst", required_argument, 0, 't'},
        {"endir", required_argument, 0, 'i'},
        {"enddr", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    // Цикл проверки всех переданных аргументов
    while ((c = getopt_long(argc, argv, "b:j:t:i:d:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b':
                filename_bsd = optarg;
                if (!has_extension(filename_bsd,{"bsd","bsdl"})) {
                    std::cerr << "Неверное расширение BSDL-файла (.bsd)\n";
                    abort();
                }
                break;
            case 'j':
                filename_json = optarg;
                if (!has_extension(filename_json, {"json"})) {
                    std::cerr << "Неверное расширение JSON-файла (.json)\n";
                    abort();
                }
                break;
            case 't':
                trst_state = optarg;
                if (!is_valid_state(trst_state, trst_states, 4)) {
                    std::cerr << "Неверное состояние --trst. Возможные состояния ON, OFF, z, or ABSENT.\n";
                    abort();
                }
                break;
            case 'i':
                endir_state = optarg;
                if (!is_valid_state(endir_state, endir_enddr_states, 4)) {
                    std::cerr << "Неверное состояние --endir. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE.\n";
                    abort();
                }
                break;
            case 'd':
                enddr_state = optarg;
                if (!is_valid_state(enddr_state, endir_enddr_states, 4)) {
                    std::cerr << "Неверное состояние --enddr. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE.\n";
                    abort();
                }
                break;
            case 'h':
                print_usage();
                return;
            default:
                abort();
        }
    }

    if((filename_bsd == "NO FILE")||(filename_json == "NO FILE")){
        std::cout << "Необходимо указать имя BSDL-файла и JSON-файла" << std::endl;
        abort();
    }

    // Вывод записанных аргументов
    std::cout << "BSDL-file: " << filename_bsd << "\n";
    std::cout << "JSON-file: " << filename_json << "\n";
    std::cout << "TRST state: " << trst_state << "\n";
    std::cout << "ENDIR state: " << endir_state << "\n";
    std::cout << "ENDDR state: " << enddr_state << "\n";
} 

int main(int argc, char *argv[]) {
    
    parse_arguments(argc, argv);     

    return 0;
}