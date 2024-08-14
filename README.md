# practice-in-the-Module
Библиотека для работы с файлами формата .bsd (JTAG) предназначенными для описания цепей boundary scan устройств. Он содержит информацию о конфигурации физических выводов устройства, регистров boundary scan и дополнительную служебную информацию. Основная цель данных файлов — помочь инструментам автоматизированного тестирования выполнять диагностику и отладку печатных плат. 
Библиотека решает задачи по созданию .svf из файла .bsd (описание цепей тестирования) и .json (описание тестов, которые необходимо провести).

## Формат ввода данных в программный модуль svf-calc
```
./name --filename "filename_BSD" --pins  "pin_name_1, ... ,pin_name_n"
                                 --write "pin_status_1, ...,pin_status_n"
                                 --read  "pin_status_1, ...,pin_status_n"
```

## Пример ввода данных в программный модуль svf-calc
```
./main --filename "STM32F1_Low_density_QFPN36.bsd" --pins "PA8,PA3,PB6" --write "1,1,0" --read "x,x,1"
                                                   --pins "PA8,PA3,PB6,PB4" --write "1,z,z,0" --read "x,x,0,1"
                                                   --pins "PA2,PA1,PB2,PB4" --write "z,1,1,0" --read "1,0,x,x"
```

## Формат ввода данных в программный модуль svf-generator
```
./name -b (--bsdl) name BSDL-file -j (--json) name JSON-file
```

## Пример ввода данных в программный модуль svf-generator
```
./svf_generator_v2 -b STM32F1_Low_density_QFPN36.bsd -j STM32F1_Low_density_QFPN36_test.json 
```