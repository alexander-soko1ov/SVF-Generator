#pragma once
#include <string>
#include <unordered_set>

class StateArg {
public:
    // Публичный метод для запуска формирования SVF
    void formation_svf(int argc, char *argv[]);

private:
    // Параметры для формирования SVF
    std::string filename_bsd = "NO FILE";
    std::string filename_json = "NO FILE";
    std::string trst_state = "OFF";
    std::string endir_state = "IDLE";
    std::string enddr_state = "IDLE";

    // Приватные методы
    void print_usage();
    bool is_valid_state(const std::string& state, const std::string valid_states[], size_t count);
    bool has_extension(const std::string& filename, const std::unordered_set<std::string>& validExtensions);
    void parse_arguments(int argc, char *argv[]);
    std::string replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt);
    void createFile(char *argv[]);
};
