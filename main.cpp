#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

// Структура для хранения данных о портах
struct Port {
    std::string name;
    std::string direction;
    std::string type;
};

// Функция для чтения файла .bsd и извлечения данных о портах
std::vector<Port> readBsdFile(const std::string& filename) {
    std::vector<Port> ports;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return ports;
    }
    
    std::string line;
    std::regex portRegex(R"(\s*(\w+)\s*:\s*(\w+)\s+(\w+);)");
    std::smatch matches;
    bool inPortSection = false;
    
    while (std::getline(file, line)) {
        if (line.find("port (") != std::string::npos) {
            inPortSection = true;
            continue;
        }
        if (inPortSection && line.find(");") != std::string::npos) {
            break;
        }
        if (inPortSection && std::regex_search(line, matches, portRegex)) {
            Port port;
            port.name = matches[1].str();
            port.direction = matches[2].str();
            port.type = matches[3].str();
            ports.push_back(port);
        }
    }

    file.close();
    return ports;
}

// Функция для вывода информации о портах
void printPorts(const std::vector<Port>& ports) {
    for (const auto& port : ports) {
        std::cout << "Port Name: " << port.name
                  << ", Direction: " << port.direction
                  << ", Type: " << port.type << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Передайте программе файл формата .bsd" << std::endl;
        return 1;
    }
    
    std::string bsdFilename = argv[1];
    std::string outputFilename = "ports_info.txt";
    
    // Чтение файла .bsd
    std::vector<Port> ports = readBsdFile(bsdFilename);
    if (ports.empty()) {
        std::cerr << "Ошибка при чтении BSD файла или файл пуст" << std::endl;
        return 1;
    }
    
    // Вывод информации о портах
    printPorts(ports);
   
    return 0;
}
