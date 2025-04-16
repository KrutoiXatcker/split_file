Split & Merge Tool
Простая утилита для разделения больших файлов на части и их последующего объединения.

Описание
Программа предоставляет две основные функции:

Разделение файла на части указанного размера (-c)

Объединение частей обратно в исходный файл (-d)

Установка
Склонируйте репозиторий:

bash
git clone https://github.com/yourusername/split-merge-tool.git
cd split-merge-tool
Скомпилируйте программу:

bash
g++ -std=c++11 split.cpp -o split_tool
Использование
Разделение файла
bash
./split_tool -c <input_file> <output_dir> <chunk_size_mb>
Пример:

bash
./split_tool -c large_file.dat parts 10
Разделит файл large_file.dat на части по 10 МБ в директории parts

Объединение файла
bash
./split_tool -d <input_dir> <output_file>
Пример:

bash
./split_tool -d parts reconstructed.dat
Соберет все части из директории parts в файл reconstructed.dat

Особенности
Работает с бинарными файлами любого типа

Поддерживает большие файлы

Простая проверка целостности при объединении

Подробный вывод информации о процессе

Требования
Компилятор C++ с поддержкой C++11

Linux/Unix система (используются POSIX-функции)