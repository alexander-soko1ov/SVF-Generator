#include <iostream>
#include <fstream>
#include <getopt.h>  

#include "svf-calc_lib.hpp"

void JsonForm::fileForm(std::vector<std::vector<std::string>> &pinsList, std::vector<std::vector<std::string>> &writeStatusList, 
                        std::vector<std::vector<std::string>> &readStatusList, std::string &filename){

    // Локальная переменная имени JSON-файла
    std::string filename_json;

    // Проверка наличия подключённого файла
    if(filename == "NO FILE"){
        std::cerr << red << "Введите желаемое имя файла" << reset << std::endl;
        abort();
    }

    // Подмена расширения если имя файла создаётся ил BSDL-файла
    if (has_extension(filename, {"bsd"})) {
        filename_json = replaceExtension(filename, ".bsd", "_test.json");
    } else if (has_extension(filename, {"bsdl"})) {
        filename_json = replaceExtension(filename, ".bsdl", "_test.json");
    } else if (has_extension(filename, {"json"})){
        filename_json = filename;
    } else {
        std::cerr << red << "Неверное расширение (bsd, bsdl, json)" << reset << std::endl;
        abort();
    }
    
    // Создание json объекта
    json jsonArray = createJsonObject(pinsList, writeStatusList, readStatusList);

    // Запись json в файл
    if (writeJsonToFile(filename_json, jsonArray)) {
        std::cout << "\n"  << "Создан файл: " << magenta << filename_json << reset << std::endl;
    }
    
    return;
}

// Функция проверки расширения файла
bool JsonForm::has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos && pos != filename.length() - 1) {
        std::string fileExt = filename.substr(pos + 1);
        return validExtensions.find(fileExt) != validExtensions.end();
    }
    return false;
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
