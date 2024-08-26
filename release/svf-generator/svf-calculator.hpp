#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonForm {
public:
    // Переменные записываемые в JSON-файл
    std::vector<std::vector<std::string>> pinsList;
    std::vector<std::vector<std::string>> writeStatusList;
    std::vector<std::vector<std::string>> readStatusList;
    std::string filename = "NO FILE";

    // Главный метод формирующий JSON-файл
    void fileForm(std::vector<std::vector<std::string>> &pinsList, std::vector<std::vector<std::string>> &writeStatusList, 
                    std::vector<std::vector<std::string>> &readStatusList, std::string &filename);

private:

    bool writeJsonToFile(const std::string& filename, const nlohmann::json& jsonObject);

    bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions);

    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);

    json createJsonObject(const std::vector<std::vector<std::string>>& pinsList,
                          const std::vector<std::vector<std::string>>& writeStatusList,
                          const std::vector<std::vector<std::string>>& readStatusList);

protected:
    // Переменные для определения ANSI escape-кодов
    // Сброс всех атрибутов
    const std::string reset = "\033[0m";

    // Цвета текста
    const std::string red = "\033[31m";
    const std::string magenta = "\033[35m";
    const std::string green = "\033[32m";
    
    // Стиль текста
    const std::string bold = "\033[1m";

};
