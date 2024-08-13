#pragma once

class Lib {
public:
    // Метод для вывода результата
    static void print(int result); 

protected:
    // Защищенные методы для выполнения операций
    int sum(int a, int b);
    int multiply(int a, int b);
};

class DopLib : public Lib {
public:
    // Публичный метод для вывода результатов
    void printResult(char *argv[]);
};
