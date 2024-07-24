#include <iostream>
#include <string>
#include <fstream>
#include <getopt.h>

// Функция замены расширения файла
std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt) {
    size_t pos = filename.rfind(oldExt);
    if (pos != std::string::npos && pos == filename.length() - oldExt.length()) {
        return filename.substr(0, pos) + newExt;
    } else {
        std::cerr << "Некорретное раcширение файла: " << filename << std::endl;
        exit(1);
    }
}

// Функция создающая файл и заполняющая его в соотвествии с json
void createFile(std::string filename_json, char *argv[]){
    // Создание имени файла с расширением svf
    std::string filename = replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    std::ofstream svfFile(filename);

    // Проверка, что файл успешно открыт
    if (!svfFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
    }
    

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    svfFile << "! Начать программу тестирования\n"
                "TRST " << statusTRST << ";\n"; // statusTRST = OFF (по умолчанию)

    svfFile << "ENDIR " << statusENDIR << ";\n";
    
    svfFile << "ENDDR " << statusENDDR << ";\n";
    
    svfFile << "SIR 8 TDI (41);\n";
    
    svfFile << "SDR 32 TDI (ABCD1234) TDO (11112222);\n";
    


    // Закрытие файла
    svfFile.close();

    // Успешное завершение программы
    std::cout << "Файл " << filename << " успешно создан." << std::endl;
} 

// Функция вывода help
void print_usage() {
    std::cout << "Usage: program [options]\n"
              << "Options:\n"
              << "  -t, --trst   Set the TRST state (ON, OFF, z, ABSENT)\n"
              << "  -i, --endir   Set the ENDIR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
              << "  -d, --enddr   Set the ENDDR state (IRPAUSE, DRPAUSE, RESET, IDLE)\n"
              << "  -h, --help    Show this help message\n";
}

// Функция проверки корректности введённого id
bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (state == valid_states[i]) {
            return true;
        }
    }
    return false;
}

// Функция парсинга аргументов и вывода записанных аргументов в консоль
void parse_arguments(int argc, char *argv[]){

    // Инициализация вспомагательных переменных
    int option_index = 0;
    int c;

    // Установка состояний по умолчанию
    std::string trst_state = "OFF"; 
    std::string endir_state = "IDLE";  
    std::string enddr_state = "IDLE";  

    // Инициализация доступных аргументов
    static struct option long_options[] = {
        {"trst", required_argument, 0, 't'},
        {"endir", required_argument, 0, 'i'},
        {"enddr", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    // Инициализация доступных состояний аргументов
    const std::string trst_states[] = {"ON", "OFF", "z", "ABSENT"};
    const std::string endir_enddr_states[] = {"IRPAUSE", "DRPAUSE", "RESET", "IDLE"};

    while ((c = getopt_long(argc, argv, "t:i:d:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 't':
                trst_state = optarg;
                if (!is_valid_state(trst_state, trst_states, 4)) {
                    std::cerr << "Неверное состояние for --trst. Возможные состояния ON, OFF, z, or ABSENT.\n";
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
                    std::cerr << "Неверное состояние for --enddr. Возможные состояния IRPAUSE, DRPAUSE, RESET, or IDLE.\n";
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

    // Вывод записанных аргументов
    std::cout << "TRST state: " << trst_state << "\n";
    std::cout << "ENDIR state: " << endir_state << "\n";
    std::cout << "ENDDR state: " << enddr_state << "\n";
} 

int main(int argc, char *argv[]) {

    // Парсим аргументы из командной строки
    parse_arguments(argc, argv);     

    // Создание SVF-файла
    createFile(filename_json, argv); 

    return 0;
}

// int main(int argc, char* argv[]) {
//     if (argc < 3) {
//         std::cerr << "Usage: " << argv[0] << " <bsd-file> <json-file> [<statusTRST>]  " << std::endl;
//         return 1;
//     }

//     // Получаем имя файла bsd
//     std::string filename_bsdl = argv[1];

//     // Получаем имя файла json
//     std::string filename_json = argv[2];
    
//     // Получаем статус линии TRST
//     // std::string statusTRST;

//     // Создание файла svf
//     createFile(filename_json, argv); 
    
//     return 0;
// }