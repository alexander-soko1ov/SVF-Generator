#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Функиця парсинга аргументов
void parsingArguments(int argc, char *argv[], std::vector<std::string> &pins, 
                    std::vector<std::string> &writeStatus, std::vector<std::string> &readStatus, 
                    const std::string &filename_bsd){
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--pins" && i + 1 < argc) { 
            std::string pinsStr = argv[++i];
            size_t pos = 0;
            while ((pos = pinsStr.find(',')) != std::string::npos) {
                std::string pin = pinsStr.substr(0, pos);
                pins.push_back(pin);
                pinsStr.erase(0, pos + 1);
            }
            pins.push_back(pinsStr);
        } else if (std::string(argv[i]) == "--write" && i + 1 < argc) {
            std::string writeStr = argv[++i];
            size_t pos = 0;
            while ((pos = writeStr.find(',')) != std::string::npos) {
                std::string status = writeStr.substr(0, pos);
                writeStatus.push_back(status);
                writeStr.erase(0, pos + 1);
            }
            writeStatus.push_back(writeStr); 
        } else if (std::string(argv[i]) == "--read" && i + 1 < argc) {
            std::string readStr = argv[++i];
            size_t pos = 0;
            while ((pos = readStr.find(',')) != std::string::npos) {
                std::string status = readStr.substr(0, pos);
                readStatus.push_back(status);
                readStr.erase(0, pos + 1);
            }
            readStatus.push_back(readStr);
        } else if (std::string(argv[i]) == "--filename" && i + 1 < argc) {
            filename_bsd = argv[++i];
        }
    }
};

// Функиця записи данных в json файл 
bool writeJsonToFile(const std::string& filename, const nlohmann::json& jsonObject) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Файл: " << filename << " не может быть открыт" << std::endl;
        return false;
    }
    file << jsonObject.dump(4); // параметр 4 задает отступы для красивого форматирования
    file.close();
    return true;
}

// Функция создания названия файла (замены filename_bsd.bsd на filename_bsd_test.json)
std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt) {
    size_t pos = filename.rfind(oldExt);
    if (pos != std::string::npos && pos == filename.length() - oldExt.length()) {
        return filename.substr(0, pos) + newExt;
    } else {
        return filename; // Возвращаем исходное имя файла, если расширение не найдено или не совпадает
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Общая структура: name_file_BSD.bsd --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    } else if (argc < 4) {
        std::cerr << "Структура пинов: --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    }

    // Инициализация переменных
    std::vector<std::string> pins;
    std::vector<std::string> writeStatus;
    std::vector<std::string> readStatus;
    std::string filename_bsd;

    // Парсинг аргументов
    parsingArguments(argc, argv, pins, writeStatus, readStatus, filename_bsd);

    // Создание имени файла json
    std::string filename = replaceExtension(filename_bsd, ".bsd", "_test.json");

    // Создание json объекта
    json jsonObject;
    jsonObject["pins"] = pins;
    jsonObject["write"] = writeStatus;
    jsonObject["read"] = readStatus;

    // Вывод json в консоль
    // std::cout << jsonObject.dump(4) << std::endl;

    // Запись json в файл name_file_BSD_test.json
    if (writeJsonToFile(filename, jsonObject)) {
        std::cout << "Создан файл: " << filename << std::endl;
    }

    return 0;
}
