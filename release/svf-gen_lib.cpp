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


// Тестовый вывод пинов из json-файла
void PinSVF::print_param_cli(std::string filename_bsdl, std::string  filename_json, std::string filename_svf,
                        std::string trst_state, std::string endir_state, std::string enddr_state, std::string runtest_state){
    // Вывод записанных аргументов
    std::string filename_svf_json = PinSVF::replaceExtension(filename_json, ".json", ".svf");

    // Открытие файла для записи
    if(filename_svf == ""){
        filename_svf = filename_svf_json;
    }

  
    std::cout << "\n";
    std::cout << "BSDL-file:    " << green << filename_bsdl << reset << "\n";
    std::cout << "JSON-file:    " << green << filename_json << reset << "\n";
    std::cout << "SVF-file:     " << green << filename_svf << reset << "\n";
    std::cout << "TRST state:   " << green << trst_state << reset << "\n";
    std::cout << "ENDIR state:  " << green << endir_state << reset << "\n";
    std::cout << "ENDDR state:  " << green << enddr_state << reset << "\n";
    std::cout << "RUNTEST:      " << green << runtest_state << reset << "\n";
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

// Метод генерирующий маску для записи значений в ячейки BS
void PinSVF::genPinTdi(mpz_class& bitmask, const size_t& register_length_bsdl,  
                        const std::vector<BsdlPins::PinInfo>& cells, 
                        const std::vector<PinJson::PinsJsonInfo>& pins_svf){

    /* В структуре bsd файла при описании ячейки control имеется значение при котором драйвер отключается, нам 
    необходимо заменить данное значение на инверсное и записать по индексу ячейки control
    Таким образом драйвер будет включен и ячейка output будет активна и мы записываем в неё значение из json-файла
    */

    for (size_t j = 0; j < pins_svf.size(); ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[j].pin_name){ 

                if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
                    || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir") ){
                    
                    // std::cerr << magenta << "найдено совпадение!" << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl; 
                    
                    if (pins_svf[j].cell_write != string_to_statepin("z")){

                        // std::cerr << magenta << "найдено совпадение!" << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;
                        
                        if(pins_svf[j].cell_write == string_to_statepin("1")){
                        
                            bitmask |= (mpz_class(1) << cells[i].Out); // если write = 1, то пишем 1 в маску

                            // std::cerr << red << "cells:" << cells[i].cell << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;

                            if(cells[i].turnOff == 0){
                                
                                bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            } else {
                                
                                bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            }

                        } else if(pins_svf[j].cell_write == string_to_statepin("0")){
 
                            bitmask &= ~(mpz_class(1) << cells[i].Out); // если write = 0, то пишем 0 в маску

                            if(cells[i].turnOff == 0){

                                bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            } else {

                                bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                            }
                        }

                    } else if(pins_svf[j].cell_write == string_to_statepin("z")){
                        if(cells[i].turnOff == 1){
                           
                            bitmask |= (mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                        } else {
                            // std::cout << green << "  cells:  " << cells[i].cell << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;
                            bitmask &= ~(mpz_class(1) << cells[i].Config); // запись в битовую маску для отключённого драйвера
                        }      
                    }
                } 
            }                
        } 
    }
}

// Метод, генерирующий маску для регистров BS, отвечающих за приём значений 
void PinSVF::genPinTdo(mpz_class& bitmask, const size_t& register_length_bsdl,  
                        const std::vector<BsdlPins::PinInfo>& cells, 
                        const std::vector<PinJson::PinsJsonInfo>& pins_svf){

    for (size_t j = 0; j < pins_svf.size(); ++j) {
        
        for(size_t i = 0; i < register_length_bsdl; ++i){
            
            if(cells[i].label == pins_svf[j].pin_name){

                if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                    
                    // std::cout << red << "  cells:  " << cells[i].cell << "  cells In:   " << cells[i].In << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;

                    if(pins_svf[j].cell_read == string_to_statepin("1")){

                        // std::cerr << red << "  cells:  " << cells[i].cell << "  1   " << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;
                        bitmask |= (mpz_class(1) << cells[i].In); // если write = 1, то пишем 1 в маску
                    } else if(pins_svf[j].cell_read == string_to_statepin("0")){
                        
                        // std::cerr << magenta << "  cells:  " << cells[i].cell << "  0   " << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;
                        bitmask &= ~(mpz_class(1) << cells[i].In); 
                    } else if(pins_svf[j].cell_read == string_to_statepin("x")){
                        
                        // std::cerr << magenta << "  cells:  " << cells[i].cell << "  x   "  << "     pins_svf " << pins_svf[j].pin_name << reset << std::endl;
                        return;
                    } else {
                        
                        // std::cerr << red << "Неверное состояние read!" << reset << std::endl;
                        abort();
                    }   
                } 
            }
        }  
    }
}

// Функция генерирующая общую маску для ячеек BS
void PinSVF::genPinMask(mpz_class& bitmask, const size_t& register_length_bsdl,  
                        const std::vector<BsdlPins::PinInfo>& cells, 
                        const std::vector<PinJson::PinsJsonInfo>& pins_svf){   

    for (size_t j = 0; j < pins_svf.size(); ++j) {

        for(size_t i = 0; i < register_length_bsdl; ++i) {
            
            if(cells[i].label == pins_svf[j].pin_name) {
               
                if((BsdlPins::toLowerCase(cells[i].function) == "input") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")) {

                    if(pins_svf[j].cell_read == string_to_statepin("x")) {
                        
                        // std::cerr << "найдено совпадение!   "  << cells[i].label << "     " << cells[i].cell << "    " << statepin_to_string(pins_svf[j].cell_read) << std::endl;
                        
                        bitmask &= ~(mpz_class(1) << cells[i].cell);
                    } else {
                        bitmask |= (mpz_class(1) << cells[i].cell);
                    }
                }
            } 
        } 
    }
}

// Заполнение переменной pin_tdi безопасными значениями 
void PinSVF::safeValues(mpz_class& bitmask, const size_t& register_length_bsdl, 
                        const std::vector<BsdlPins::PinInfo>& cells, const bool& development){
    
    if(development == 1){
        // Тестовое заполнение маски единицами
        genSingleMask(bitmask, register_length_bsdl);

    } else {
       for(size_t i = 0; i < register_length_bsdl; ++i){
        
            if((BsdlPins::toLowerCase(cells[i].function) == "output") || (BsdlPins::toLowerCase(cells[i].function) == "output2") 
                || (BsdlPins::toLowerCase(cells[i].function) == "output3") || (BsdlPins::toLowerCase(cells[i].function) == "bidir")){
                
                // std::cout << cells[i].label  << "   " << BsdlPins::toLowerCase(cells[i].function) << "  " << " Safe State: " << BsdlPins::PinInfo::statePinToString(cells[i].safeState) << std::endl;

                if(cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("1")){
                    
                    bitmask |= (mpz_class(1) << cells[i].cell);   
                } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("0")){
                    
                    bitmask |= (mpz_class(0) << cells[i].cell); 
                } else if (cells[i].safeState == BsdlPins::PinInfo::stringToStatePin("X")){
                    
                   
                    bitmask |= (mpz_class(cells[i].turnOff) << cells[i].Config); 
                } else {
                    
                    std::cerr << red << "Неверное безопасное состояние!" << reset << std::endl;
                }  
            }
        } 
    }
} 

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
    
    size_t index = 0;

    if(filename_svf == ""){
        filename_svf = replaceExtension(filename_json, ".json", ".svf");
    }

    std::ofstream svfFile(filename_svf);

    // Запись данных в файл (длина регистра 5 bit для STM32F1)
    if(trst_state == "OFF"){
        svfFile << "! Начать программу тестирования\n\n"
                "TRST " << trst_state << ";\n";
    } else{
        
        std::cerr << red << "Окончание программы тестирования так как вывод TRST находится не в состоянии OFF" << reset << std::endl;
        abort();
    }
    
    svfFile << "ENDIR "<< endir_state << ";\n";
    
    svfFile << "ENDDR "<< enddr_state << ";\n\n";

    mpz_class opcode_extest_hex;

    opcode_extest_hex.set_str(opcode_extest, 2);

    // std::cerr << red << pins_svf.size() << reset << std::endl;

    for(size_t i = 0; i < pins_svf.size(); ++i){
        // Инициализация переменной pinTdi
        mpz_class pin_tdi(0);
        mpz_class pin_tdo(0);
        mpz_class pin_mask(0);
         
        svfFile << "SIR " << register_length_instr << " TDI (" << opcode_extest_hex.get_str(16) << ")\n"; 

        /* Если нужно сначала залить маску, то сделать переменную равнной 1, а далее нужный 
        бит оставить единицей или сделать нулём (это сделано исходя из той рекомендации от разрабочиков, 
        вдруг пригдится, решил добавить) управляется из библиотеки так как вряд-ли пригодится*/ 
        const bool development = 0;

        // Запонение переменной pin_tdi безопасными значениями
        safeValues(pin_tdi, register_length_bsdl, cells, development);
       
        if(index < pins_svf.size()) {
            // Заполнение строки двоичными данными
            genPinTdi(pin_tdi, register_length_bsdl, cells, pins_svf[index]);
            genPinTdo(pin_tdo, register_length_bsdl, cells, pins_svf[index]);
            genPinMask(pin_mask, register_length_bsdl, cells, pins_svf[index]); 
        } else {
            std::cerr << "Индекс выходит за пределы вектора!" << std::endl;
        }

        index++;

        if(verbose == 1){
            
            std::cout << "Тестовый вывод битовых полей в " << out_format << " формате для блока номер " << i << ":   " << std::endl;
            
            if(out_format == 2){
                std::cout << "Поле TDI:     " << green << output_str(pin_tdi, register_length_bsdl) << reset << std::endl;
                std::cout << "Поле TDO:     " << green << output_str(pin_tdo, register_length_bsdl) << reset << std::endl;
                std::cout << "Поле MASK:    " << green << output_str(pin_mask, register_length_bsdl) << reset  << std::endl;
                std::cout << "\n";

            } else {
                std::cout << "Поле TDI:     " << green << pin_tdi.get_str(out_format) << reset << std::endl;
                std::cout << "Поле TDO:     " << green << pin_tdo.get_str(out_format) << reset << std::endl;
                std::cout << "Поле MASK:    " << green << pin_mask.get_str(out_format)<< reset  << std::endl;
                std::cout << "\n";
            }  
        }

        // Запись строки битовой маски в файл
        svfFile << "SDR " << register_length_bsdl << " TDI (" << pin_tdi.get_str(16) <<  ") TDO (" 
                << pin_tdo.get_str(16) <<  ") MASK ("  << pin_mask.get_str(16) << ");\n";
        
        // Запись строки в файл
        svfFile << "RUNTEST " << runtest_state << " TCK ENDSTATE IDLE;\n\n";
    }

    // Окончание программы тестирования
    svfFile << "! Программа тестирования окончена\n";

    // Закрытие файла
    svfFile.close();

    if(verbose == 1){
        // Успешное завершение программы
        std::cout << "\nФайл " << magenta << filename_svf << reset << " успешно создан." << std::endl;
    }
}
