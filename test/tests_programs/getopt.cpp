#include <iostream>
#include <getopt.h>
#include <string>
#include <algorithm>

// Функция для печати справки
void print_usage() {
    std::cout << "Usage: program [options]\n"
              << "Options:\n"
              << "  -b, --bsdl    Add a BSDL-file\n"
              << "  -j, --json    Add a JSON-file\n"
              << "  -o, --out     Out state (CELL, PIN)\n"
              << "  -h, --help    Show this help message\n";
}

// Функция для проверки допустимости состояния
bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (state == valid_states[i]) {
            return true;
        }
    }
    return false;
}

// Функция для проверки расширения файла
bool has_extension(const std::string& filename, const std::string& ext) {
    return filename.size() >= ext.size() &&
           filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

// Функция для обработки аргументов командной строки
bool parse_arguments(int argc, char *argv[]) {
    // Состояния по умолчанию
    std::string filename_bsd = "NO FILE";
    std::string filename_json = "NO FILE";
    std::string out_state = "PIN";
    
    int option_index = 0;
    int c;

    static struct option long_options[] = {
        {"bsdl", required_argument, 0, 'b'},
        {"json", required_argument, 0, 'j'},
        {"out", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    const std::string out_states[] = {"CELL", "PIN"};

    while ((c = getopt_long(argc, argv, "b:j:o:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'b':
                filename_bsd = optarg;
                if (!has_extension(filename_bsd, ".bsdl") && !has_extension(filename_bsd, ".bsd")) {
                    std::cerr << "Неверное расширение BSDL-файла. Доступные расширения: bsdl или bsd\n";
                    return 1;
                }
                break;
            case 'j':
                filename_json = optarg;
                if (!has_extension(filename_json, ".json")) {
                    std::cerr << "Неверное расширение JSON-файла. Доступные расширения: json\n";
                    return 1;
                }
                break;
            case 'o':
                out_state = optarg;
                if (!is_valid_state(out_state, out_states, 2)) {
                    std::cerr << "Invalid OUT state. Available states: PIN, CELL.\n";
                    return 1;
                }
                break;
            case 'h':
                print_usage();
                return 0;
            case '?':
                return 1;
            default:
                abort();
        }
    }

    // Вывод записанных аргументов
    if((filename_bsd == "NO FILE") && (filename_json == "NO FILE")){
        std::cout << "Необходимо указать имя BSDL-файла и JSON-файла" << std::endl;
        abort();
    } else if (filename_bsd == "NO FILE"){
        std::cout << "Ещё необходимо указать имя BSDL-файла" << std::endl;
        abort();
    } else if (filename_json == "NO FILE"){
        std::cout << "Ещё необходимо указать имя JSON-файла" << std::endl;
        abort();
    }
     
    std::cout << "BSDL-file: " << filename_bsd << "\n";
    std::cout << "OUT state: " << out_state << "\n";
    std::cout << "JSON-file: " << filename_json << "\n";

    return 0;
}

int main(int argc, char *argv[]) {

    parse_arguments(argc, argv);

}
