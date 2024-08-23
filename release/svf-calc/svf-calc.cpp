#include <iostream>
#include <fstream>
#include <getopt.h>  

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

// Функция вывода help
void JsonForm::print_usage() {
    std::cout << "Usage: program [options]\n"
            << "Options:\n"
            << "  -f, --filename    Add a BSDL-file\n"
            << "  -p, --pins        The name of the required pins\n"
            << "  -w, --write       Input (1, 0, z)\n"
            << "  -r, --read        Output (1, 0, x)\n"
            << "  -h, --help        Show this help message\n";
}

void JsonForm::parsingArguments(int argc, char *argv[], std::vector<std::vector<std::string>> &pinsList,
                    std::vector<std::vector<std::string>> &writeStatusList, std::vector<std::vector<std::string>> &readStatusList,
                    std::string &filenameBSD) {

    // std::vector<std::string> pins;
    // std::vector<std::string> writeStatus;
    // std::vector<std::string> readStatus;

    // Опции для getopt_long
    const struct option long_options[] = {
        {"pins", required_argument, 0, 'p'},
        {"write", required_argument, 0, 'w'},
        {"read", required_argument, 0, 'r'},
        {"filename", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}  // Завершающая запись
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:w:r:f:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p': {
                if (!pins.empty()) {
                    pinsList.push_back(pins);
                    writeStatusList.push_back(writeStatus);
                    readStatusList.push_back(readStatus);
                    pins.clear();
                    writeStatus.clear();
                    readStatus.clear();
                }
                std::string pinsStr = optarg;
                size_t pos = 0;
                while ((pos = pinsStr.find(',')) != std::string::npos) {
                    pins.push_back(pinsStr.substr(0, pos));
                    pinsStr.erase(0, pos + 1);
                }
                pins.push_back(pinsStr);
                break;
            }
            case 'w': {
                std::string writeStr = optarg;
                size_t pos = 0;
                while ((pos = writeStr.find(',')) != std::string::npos) {
                    writeStatus.push_back(writeStr.substr(0, pos));
                    writeStr.erase(0, pos + 1);
                }
                writeStatus.push_back(writeStr);
                break;
            }
            case 'r': {
                std::string readStr = optarg;
                size_t pos = 0;
                while ((pos = readStr.find(',')) != std::string::npos) {
                    readStatus.push_back(readStr.substr(0, pos));
                    readStr.erase(0, pos + 1);
                }
                readStatus.push_back(readStr);
                break;
            }
            case 'f':
                filenameBSD = optarg;
                break;
            case 'h':
                print_usage();
                break;
            default:
                std::cerr << "Неизвестный аргумент: " << opt << std::endl;
                exit(EXIT_FAILURE);
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
