#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void parsingArguments(int argc, char *argv[], std::vector<std::vector<std::string>> &pinsList,
                    std::vector<std::vector<std::string>> &writeStatusList, std::vector<std::vector<std::string>> &readStatusList,
                    std::string &filenameBSD) {
    std::vector<std::string> pins;
    std::vector<std::string> writeStatus;
    std::vector<std::string> readStatus;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--pins" && i + 1 < argc) {
            if (!pins.empty()) {
                pinsList.push_back(pins);
                writeStatusList.push_back(writeStatus);
                readStatusList.push_back(readStatus);
                pins.clear();
                writeStatus.clear();
                readStatus.clear();
            }
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
            filenameBSD = argv[++i];
        }
    }
    // Добавляем последний набор
    if (!pins.empty()) {
        pinsList.push_back(pins);
        writeStatusList.push_back(writeStatus);
        readStatusList.push_back(readStatus);
    }
}

// Функция создания и записи в файл
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Общая структура: --filename \"name_file_BSD.bsd\" --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    } else if (argc < 4) {
        std::cerr << "Структура пинов: --pins \"pin_name_1, ..., pin_name_n\" --write \"pin_status_1, ..., pin_status_n\" --read \"pin_status_1, ..., pin_status_n\"" << std::endl;
        return 1;
    }

    // Инициализация переменных
    std::vector<std::vector<std::string>> pinsList;
    std::vector<std::vector<std::string>> writeStatusList;
    std::vector<std::vector<std::string>> readStatusList;
    std::string filenameBSD;

    // Парсинг аргументов
    parsingArguments(argc, argv, pinsList, writeStatusList, readStatusList, filenameBSD);

    // Создание имени файла json
    std::string filename = replaceExtension(filenameBSD, ".bsd", "_test.json");

    // Создание json объекта
    json jsonArray = json::array();

    for (size_t i = 0; i < pinsList.size(); ++i) {
        json jsonObject;
        jsonObject["pins"] = pinsList[i];
        jsonObject["write"] = writeStatusList[i];
        jsonObject["read"] = readStatusList[i];
        jsonArray.push_back(jsonObject);
    }

    // Вывод json в консоль
    // std::cout << jsonArray.dump(4) << std::endl;

    // Запись json в файл name_file_BSD_test.json
    if (writeJsonToFile(filename, jsonArray)) {
        std::cout << "Создан файл: " << filename << std::endl;
    }

    return 0;
}
