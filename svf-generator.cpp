#include <iostream>
#include <string>
#include <fstream>

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
    
    // Инициализация переменной statusTRST
    std::string statusTRST;

    // Открытие файла для записи
    std::ofstream svfFile(filename);

    // Проверка, что файл успешно открыт
    if (!svfFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
    }
    
    if(argv[3] != NULL ){
        std::string temp_statusTRST = argv[3];
        if (temp_statusTRST == "OFF" || temp_statusTRST == "ON" || temp_statusTRST == "Z" || temp_statusTRST == "ABSENT"){
            statusTRST = argv[3];
        } else{
            std::cerr << "Не верный статус statusTRST" << std::endl;
            return;
        }
    } else{
        statusTRST = "OFF";
    }

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    svfFile << "! Начать программу тестирования\n"
                "TRST " << statusTRST << ";\n"; // statusTRST = OFF (по умолчанию)

    svfFile << "ENDIR IDLE;\n";
    
    svfFile << "ENDDR IDLE;\n";
    
    // svfFile << "HIR 8 TDI (00);\n";
    
    svfFile << "HDR 16 TDI (FFFF) TDO (FFFF) MASK (FFFF);\n";
    
    svfFile << "TIR 16 TDI (0000);\n";
    
    svfFile << "TDR 8 TDI (12);\n";
    
    svfFile << "SIR 8 TDI (41);\n";
    
    svfFile << "SDR 32 TDI (ABCD1234) TDO (11112222);\n";
    
    svfFile << "STATE DRPAUSE;\n";   
    
    svfFile << "RUNTEST 100 TCK ENDSTATE IRPAUSE;\n";

    // Закрытие файла
    svfFile.close();

    // Успешное завершение программы
    std::cout << "Файл " << filename << " успешно создан." << std::endl;
} 

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <bsd-file> <json-file> [<statusTRST>]  " << std::endl;
        return 1;
    }

    // Получаем имя файла bsd
    std::string filename_bsdl = argv[1];

    // Получаем имя файла json
    std::string filename_json = argv[2];
    
    // Получаем статус линии TRST
    // std::string statusTRST;

    // Создание файла svf
    createFile(filename_json, argv); 
    
    return 0;
}
