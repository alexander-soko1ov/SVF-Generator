#include <iostream>
#include <fstream>
#include "svf-calc.hpp"

void JsonForm::fileForm(int argc, char *argv[]){
        
    // Парсинг аргументов
    parsingArguments(argc, argv, pinsList, writeStatusList, readStatusList, filenameBSD);

    // Создание имени файла json
    std::string filename = replaceExtension(filenameBSD, ".bsd", "_test.json");

    // Создание json объекта
    json jsonArray = createJsonObject(pinsList, writeStatusList, readStatusList);

    // Запись json в файл
    if (writeJsonToFile(filename, jsonArray)) {
        std::cout << "Создан файл: " << filename << std::endl;
    }
    
    return;
}

// Функция для парсинга аргументов командной строки
void JsonForm::parsingArguments(int argc, char *argv[], std::vector<std::vector<std::string>> &pinsList,
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
                pins.push_back(pinsStr.substr(0, pos));
                pinsStr.erase(0, pos + 1);
            }
            pins.push_back(pinsStr); 
        } else if (std::string(argv[i]) == "--write" && i + 1 < argc) {
            std::string writeStr = argv[++i];
            size_t pos = 0;
            while ((pos = writeStr.find(',')) != std::string::npos) {
                writeStatus.push_back(writeStr.substr(0, pos));
                writeStr.erase(0, pos + 1);
            }
            writeStatus.push_back(writeStr); 
        } else if (std::string(argv[i]) == "--read" && i + 1 < argc) {
            std::string readStr = argv[++i];
            size_t pos = 0;
            while ((pos = readStr.find(',')) != std::string::npos) {
                readStatus.push_back(readStr.substr(0, pos));
                readStr.erase(0, pos + 1);
            }
            readStatus.push_back(readStr);
        } else if (std::string(argv[i]) == "--filename" && i + 1 < argc) {
            filenameBSD = argv[++i];
        }
    }
    if (!pins.empty()) {
        pinsList.push_back(pins);
        writeStatusList.push_back(writeStatus);
        readStatusList.push_back(readStatus);
    }
}

// Функция создания и записи в файл
bool JsonForm::writeJsonToFile(const std::string& filename, const nlohmann::json& jsonObject) {
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
std::string JsonForm::replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt) {
    size_t pos = filename.rfind(oldExt);
    if (pos != std::string::npos && pos == filename.length() - oldExt.length()) {
        return filename.substr(0, pos) + newExt;
    } else {
        std::cerr << "Некорректное расширение файла: " << filename << std::endl;
        exit(1);
    }
}

// Функция создания json объекта
json JsonForm::createJsonObject(const std::vector<std::vector<std::string>>& pinsList,
                    const std::vector<std::vector<std::string>>& writeStatusList,
                    const std::vector<std::vector<std::string>>& readStatusList) {
    json jsonArray = json::array();
    for (size_t i = 0; i < pinsList.size(); ++i) {
        json jsonObject;
        jsonObject["pins"] = pinsList[i];
        jsonObject["write"] = writeStatusList[i];
        jsonObject["read"] = readStatusList[i];
        jsonArray.push_back(jsonObject);
    }
    return jsonArray;
}
