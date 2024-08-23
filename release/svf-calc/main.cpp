#include <iostream>
#include "svf-calc.hpp"

using json = nlohmann::json;

// Основная функция
int main(int argc, char *argv[]) {

    JsonForm JsonForm;

    JsonForm.fileForm(argc, argv);

    return 0;
}
