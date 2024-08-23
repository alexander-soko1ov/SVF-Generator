#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonForm {
public:
    void fileForm(int argc, char *argv[]);

private:
    std::vector<std::vector<std::string>> pinsList;
    std::vector<std::vector<std::string>> writeStatusList;
    std::vector<std::vector<std::string>> readStatusList;
    std::string filenameBSD;

    void print_usage();

    void parsingArguments(int argc, char *argv[], std::vector<std::vector<std::string>> &pinsList,
                          std::vector<std::vector<std::string>> &writeStatusList, std::vector<std::vector<std::string>> &readStatusList,
                          std::string &filenameBSD);

    bool writeJsonToFile(const std::string& filename, const nlohmann::json& jsonObject);

    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);

    json createJsonObject(const std::vector<std::vector<std::string>>& pinsList,
                          const std::vector<std::vector<std::string>>& writeStatusList,
                          const std::vector<std::vector<std::string>>& readStatusList);

protected:
    std::vector<std::string> pins;
    std::vector<std::string> writeStatus;
    std::vector<std::string> readStatus;
};
