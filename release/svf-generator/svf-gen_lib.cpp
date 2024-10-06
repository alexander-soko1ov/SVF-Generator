#include <iostream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>
#include <unordered_set>
#include <gmpxx.h>
#include <cstring>
#include <vector>

#include "json_pars_lib.hpp"
#include "svf-gen_lib.hpp"
#include "pininfo_lib.hpp"


// // Приватные методы управляемые методом print_param_cli
// // Функция для преобразования строки в значение StatePin
// PinSVF::StatePin PinSVF::string_to_statepin(const std::string& value) {

//     static const std::unordered_map<std::string, StatePin> state_map = {
//         {"1", StatePin::HIGH},
//         {"high", StatePin::HIGH},
//         {"0", StatePin::LOW},
//         {"low", StatePin::LOW},
//         {"z", StatePin::Z},
//         {"x", StatePin::X}
//     };

//     auto iter = state_map.find(value);
    
//     if (iter != state_map.end()) {
//         return iter->second;  // Возвращаем найденное значение
//     } else {
//         throw std::invalid_argument("Неизвестное значение для StatePin: " + value);
//     }
// }

// // Функция для преобразования StatePin в строку
// std::string PinSVF::statepin_to_string(StatePin state) {
//     switch (state) {
//         case StatePin::HIGH: return "1";
//         case StatePin::LOW: return "0";
//         case StatePin::Z: return "z";
//         case StatePin::X: return "x";
//         default: throw std::invalid_argument("Неизвестное значение StatePin");
//     }
// }

// Тестовый вывод пинов из json-файла
void PinSVF::print_param_cli(std::string filename_bsdl, std::string  filename_json, std::string filename_svf,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state){
    // Вывод записанных аргументов
    std::string filename_svf_json = PinSVF::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    if(filename_svf == ""){
        filename_svf = filename_svf_json;
    }
    BsdlPins color;
  
    std::cout << "\n";
    std::cout << "BSDL-file:    " << color.green << filename_bsdl << color.reset << "\n";
    std::cout << "JSON-file:    " << color.green << filename_json << color.reset << "\n";
    std::cout << "SVF-file:     " << color.green << filename_svf << color.reset << "\n";
    std::cout << "TRST state:   " << color.green << trst_state << color.reset << "\n";
    std::cout << "ENDIR state:  " << color.green << endir_state << color.reset << "\n";
    std::cout << "ENDDR state:  " << color.green << enddr_state << color.reset << "\n";
    std::cout << "RUNTEST:      " << color.green << runtest_state << color.reset << "\n";
}

// Функция замены расширения файла
std::string PinSVF::replaceExtension(const std::string& filename, const std::string& oldExt, const std::string& newExt) {
    size_t pos = filename.rfind(oldExt);
    if (pos != std::string::npos && pos == filename.length() - oldExt.length()) {
        return filename.substr(0, pos) + newExt;
    } else {
        throw std::runtime_error("Некорректное расширение файла: " + filename);
    }
}

// Методы для заполнения SVF-файла данными
// Метод для заполнения строки единицами
void PinSVF::genSingleMask(mpz_class& bitmask, const size_t& register_length_bsdl) {
    for (size_t i = 0; i < register_length_bsdl; ++i) {
        bitmask |= (mpz_class(1) << i);
    }
}

// // Метод генерирующий маску для записи значений в ячейки BS
// void PinSVF::genPinTdi(mpz_class& bitmask, const size_t& register_length_bsdl,
//                         const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){

//     /* В структуре bsd файла при описании ячейки control имеется значение при котором драйвер отключается, нам 
//     необходимо заменить данное значение на инверсное и записать по индексу ячейки control
//     Таким образом драйвер будет включен и ячейка output будет активна и мы записываем в неё значение из json-файла
//     */

//     // Тестовое заполнение маски единицами
//     // genSingleMask(bitmask, register_length_bsdl);
//     // return;

//     size_t temp_index = index;

//     for (size_t j = 0; j < pin_counts[count_out]; ++j) {

//         if (temp_index >= pins_svf.size()) {
//             std::cerr << red << "Индекс temp_index выходит за пределы массива pins_svf!" << reset << std::endl;
//             abort();
//         }

//         for(size_t i = 0; i < register_length_bsdl; ++i){
            
//             if(cells[i].label == pins_svf[temp_index].pin_name){ 

//                 if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
//                     || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir") ){
                    
//                     // std::cout << magenta << "найдено совпадение!" << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl; 
                    
//                     if (pins_svf[temp_index].cell_write != string_to_statepin("z")){
                        

//                         if(pins_svf[temp_index].cell_write == string_to_statepin("1")){
                        
//                             bitmask |= (mpz_class(1) << cells[i].Out); // если write = 1, то пишем 1 в маску

//                             if(cells[i].turnOff == 0){
                                
//                                 bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                             } else {
                                
//                                 bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                             }

//                         } else if(pins_svf[temp_index].cell_write == string_to_statepin("0")){
 
//                             bitmask &= ~(mpz_class(1) << cells[i].Out); // если write = 0, то пишем 0 в маску

//                             if(cells[i].turnOff == 0){

//                                 bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                             } else {

//                                 bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                             }
//                         }

//                     } else if(pins_svf[temp_index].cell_write == string_to_statepin("z")){
//                         if(cells[i].turnOff == 1){
                           
//                             bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                         } else {
//                             // std::cout << green << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
//                             bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
//                         }      
//                     }
//                 } 
//             }                
//         } temp_index++;
//     }
// }

// Метод, генерирующий маску для регистров BS, отвечающих за приём значений 
// void PinSVF::genPinTdo(mpz_class& bitmask, const size_t& register_length_bsdl,
//                         const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){
    
//     // Тестовое заполнение маски единицами
//     // genSingleMask(bitmask, register_length_bsdl);
//     // return;

//     size_t temp_index = index;

//     for (size_t j = 0; j < pin_counts[count_out]; ++j) {
        
//         for(size_t i = 0; i < register_length_bsdl; ++i){
            
//             if(cells[i].label == pins_svf[temp_index].pin_name){

//                 if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                    
//                     if(pins_svf[temp_index].cell_read == string_to_statepin("1")){

//                         // std::cout << red << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
//                         bitmask |= (mpz_class(1) << cells[i].In); // если write = 1, то пишем 1 в маску
//                     } else if(pins_svf[temp_index].cell_read == string_to_statepin("0")){
                        
//                         // std::cout << magenta << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
//                         bitmask &= ~(mpz_class(1) << cells[i].In); 
//                     } else if(pins_svf[temp_index].cell_read == string_to_statepin("x")){
                        
//                         // std::cout << magenta << "  cells:  " << cells[i].cell << "  " << cells[i].In << "     pins_svf " << pins_svf[temp_index].pin_name << reset << std::endl;
//                         return;
//                     } else {
                        
//                         std::cerr << red << "Неверное состояние read!" << reset << std::endl;
//                         abort();
//                     }   
//                 } 
//             }
//         } temp_index++;    
//     }
// }

// // Функция генерирующая общую маску для ячеек BS
// void PinSVF::genPinMask(mpz_class& bitmask, const size_t& register_length_bsdl, 
//                         const std::vector<BsdlPins::PinInfo>& cells, size_t& count_out, const size_t& index){   
    
//     // Тестовое заполнение маски единицами
//     // genSingleMask(bitmask, register_length_bsdl);
//     // return;

//     size_t temp_index = index;

//     for (size_t j = 0; j < pin_counts[count_out]; ++j) {

//         for(size_t i = 0; i < register_length_bsdl; ++i) {
            
//             if(cells[i].label == pins_svf[temp_index].pin_name) {
               
//                 if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")) {

//                     if(pins_svf[temp_index].cell_read == string_to_statepin("x")) {
//                         // std::cout << "найдено совпадение!   "  << cells[i].label << "     " << cells[i].cell << "    " << statepin_to_string(pins_svf[temp_index].cell_read) << std::endl;
//                         bitmask &= ~(mpz_class(1) << cells[i].cell);
//                     } else {
//                         bitmask |= (mpz_class(1) << cells[i].cell);
//                     }
//                 }
//             } 
//         } temp_index++;    
//     }
// }

// Заполнение переменной pin_tdi безопасными значениями 
// void PinSVF::safeValues(mpz_class& bitmask, const size_t& register_length_bsdl, 
//                         const std::vector<BsdlPins::PinInfo>& cells, const bool& development){
    
//     if(development == 1){
//         // Тестовое заполнение маски единицами
//         genSingleMask(bitmask, register_length_bsdl);

//     } else {
//        for(size_t i = 0; i < register_length_bsdl; ++i){
        
//             if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
//                 || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                
//                 // std::cout << cells[i].label  << "   " << BsdlPins::toLowerCase(cells[i].function) << "  " << " Safe State: " << BsdlPins::PinInfo::statePinToString(cells[i].safeState) << std::endl;

//                 if(cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("1")){
                    
//                     bitmask |= (mpz_class(1) << cells[i].cell);   
//                 } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("0")){
                    
//                     bitmask |= (mpz_class(0) << cells[i].cell); 
//                 } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("X")){
                    
                   
//                     bitmask |= (mpz_class(cells[i].turnOff) << cells[i].Config); 
//                 } else {
                    
//                     std::cerr << red << "Неверное безопасное состояние!" << reset << std::endl;
//                 }  
//             }
//         } 
//     }
// } 

// Проверим длину маски и добавим ведущий ноль, если необходимо
std::string PinSVF::output_str(const mpz_class& bitmask, const size_t& register_length_bsdl) {
    std::string bitmask_str = bitmask.get_str(2);
    if (bitmask_str.length() < register_length_bsdl) {
        bitmask_str = std::string(register_length_bsdl - bitmask_str.length(), '0') + bitmask_str;
    }

    // return bitmask_str;
    return bitmask_str;
}   

// Функция создающая файл и заполняющая его в соотвествии с json
void PinSVF::createFile(const std::vector<std::vector<PinJson::PinsJsonInfo>>& pins_svf,
                        std::string& filename_json, size_t& register_length_bsdl, std::string filename_svf, 
                        size_t& register_length_instr, std::string& opcode_extest, const std::vector<BsdlPins::PinInfo>& cells,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state, 
                        const bool verbose, const size_t& out_format) {
    
    if(filename_svf == ""){
        filename_svf = replaceExtension(filename_json, ".json", ".svf");
    }

    std::ofstream svfFile(filename_svf);

    BsdlPins color;

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    if(trst_state == "OFF"){
        svfFile << "! Начать программу тестирования\n\n"
                "TRST " << trst_state << ";\n";
    } else{
        
        std::cerr << color.red << "Окончание программы тестирования так как вывод TRST находится не в состоянии OFF" << color.reset << std::endl;
        abort();
    }
    
    svfFile << "ENDIR "<< endir_state << ";\n";
    
    svfFile << "ENDDR "<< enddr_state << ";\n\n";

    mpz_class opcode_extest_hex(opcode_extest);

    std::cerr << color.red << pins_svf.size() << color.reset << std::endl;

    // for(size_t i = 0; i < pins_svf.size(); ++i){
    //     // Инициализация переменной pinTdi
    //     mpz_class pin_tdi(0);
    //     mpz_class pin_tdo(0);
    //     mpz_class pin_mask(0);
         
    //     svfFile << "SIR " << register_length_instr << " TDI (" << opcode_extest_hex << ")\n"; 

    //     /* Если нужно сначала залить маску, то сделать переменную равнной 1, а далее нужный 
    //     бит оставить единицей или сделать нулём (это сделано исходя из той рекомендации от разрабочиков, 
    //     вдруг пригдится, решил добавить) управляется из библиотеки так как вряд-ли пригодится*/ 
    //     const bool development = 0;

    //     // Запонение переменной pin_tdi безопасными значениями
    //     safeValues(pin_tdi, register_length_bsdl, cells, development);
        
    //     // Заполнение строки двоичными данными
    //     // genPinTdi(pin_tdi, register_length_bsdl, cells[count_out]); TODO: доделать по нормальному
    //     // genPinTdi(pin_tdi, register_length_bsdl, cells, count_out, index);
    //     // genPinTdo(pin_tdo, register_length_bsdl, cells, count_out, index);
    //     // genPinMask(pin_mask, register_length_bsdl, cells, count_out, index);  
        
    //     // Тестовый вывод битовых полей в 2-ой или 16-ричной системе исчисления
    //     std::cerr << "verbose:   " << verbose << std::endl;

        

    //     if(verbose == 1){
            
    //         std::cout << "Тестовый вывод битовых полей в " << out_format << " формате для блока номер " << i << ":   " << std::endl;
            
    //         if(out_format == 2){
    //             std::cout << "Поле TDI:     " << green << output_str(pin_tdi, register_length_bsdl) << reset << std::endl;
    //             std::cout << "Поле TDO:     " << green << output_str(pin_tdo, register_length_bsdl) << reset << std::endl;
    //             std::cout << "Поле MASK:    " << green << output_str(pin_mask, register_length_bsdl) << reset  << std::endl;
    //             std::cout << "\n";

    //         } else {
    //             std::cout << "Поле TDI:     " << green << pin_tdi.get_str(out_format) << reset << std::endl;
    //             std::cout << "Поле TDO:     " << green << pin_tdo.get_str(out_format) << reset << std::endl;
    //             std::cout << "Поле MASK:    " << green << pin_mask.get_str(out_format)<< reset  << std::endl;
    //             std::cout << "\n";
    //         }  
    //     }

    //     // Запись строки битовой маски в файл
    //     svfFile << "SDR " << register_length_bsdl << " TDI (" << pin_tdi.get_str(16) <<  ") TDO (" 
    //             << pin_tdo.get_str(16) <<  ") MASK ("  << pin_mask.get_str(16) << ");\n";
        
    //     // Запись строки в файл
    //     svfFile << "RUNTEST " << runtest_state << " TCK ENDSTATE IDLE;\n\n";
    // }

    // Окончание программы тестирования
    svfFile << "! Программа тестирования окончена\n";

    // Закрытие файла
    svfFile.close();

    if(verbose == 1){
        // Успешное завершение программы
        std::cout << "\nФайл " << color.magenta << filename_svf << color.reset << " успешно создан." << std::endl;
    }
}
