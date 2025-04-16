#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <sstream>
#include <cstring>



const size_t MEGABYTE = 1024 * 1024;

void split(const char* file_path, const char* output_dir, size_t chunk_size_mb)
{

    // Открываем исходный файл
    std::ifstream input(file_path, std::ios::binary);
    if (!input) {
        std::cerr << "Error: Could not open input file " << file_path << std::endl;
        return;
    }


    // Создаем директорию для частей
    if (mkdir(output_dir, 0755) != 0 && errno != EEXIST) {
        std::cerr << "Error: Could not create directory " << output_dir << std::endl;
        return;
    }


    // Получаем размер файла
    input.seekg(0, std::ios::end);
    size_t file_size = input.tellg();
    input.seekg(0, std::ios::beg);


    size_t chunk_size = chunk_size_mb * MEGABYTE;
    size_t chunks_count = (file_size + chunk_size - 1) / chunk_size;
    size_t current_part = 1;
    size_t bytes_written_in_current_part = 0;


    std::cout << "Splitting " << file_path << " (" << file_size << " bytes) into " 
         << chunks_count << " parts of " << chunk_size_mb << " MB each" << std::endl;


    // Создаем первый файл части
    std::stringstream part_name;
    part_name << output_dir << "/part_" << current_part << ".bin";
    std::ofstream output(part_name.str(), std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not create part file " << part_name.str() << std::endl;
        input.close();
        return;
    }


    std::cout << "Created " << part_name.str() << std::endl;


    // Читаем и записываем побайтно
    char byte;
    while (input.get(byte)) {
        output.put(byte);
        bytes_written_in_current_part++;

        // Если достигли размера части, создаем новую
        if (bytes_written_in_current_part >= chunk_size) {
            output.close();
            current_part++;
            bytes_written_in_current_part = 0;
            
            part_name.str("");
            part_name << output_dir << "/part_" << current_part << ".bin";
            output.open(part_name.str(), std::ios::binary);
            
            if (!output) {
                std::cerr << "Error: Could not create part file " << part_name.str() << std::endl;
                break;
            }
            
            std::cout << "Created " << part_name.str() << std::endl;
        }
    }



    input.close();
    if (output.is_open()) {
        output.close();
    }

    std::cout << "Splitting completed. Created " << current_part << " parts." << std::endl;
}


void de_split(const char* input_dir, const char* output_file)
{

    // Открываем выходной файл
    std::ofstream output(output_file, std::ios::binary);
    if (!output) {
        std::cerr << "Error: Could not create output file " << output_file << std::endl;
        return;
    }

    std::cout << "Combining files from " << input_dir << " into " << output_file << std::endl;


    size_t part_number = 1;
    size_t total_size = 0;


    while (true) {
        // Формируем имя файла части
        std::stringstream part_name;
        part_name << input_dir << "/part_" << part_number << ".bin";

        // Открываем файл части
        std::ifstream input(part_name.str(), std::ios::binary);
        if (!input) {
            // Если файл не найден, значит это последняя часть
            if (part_number == 1) {
                std::cerr << "Error: No part files found in " << input_dir << std::endl;
            }
            break;
        }


        std::cout << "Adding " << part_name.str() << " to output" << std::endl;


        // Читаем и записываем побайтно
        char byte;
        while (input.get(byte)) {
            output.put(byte);
            total_size++;
            
            // Проверяем ошибки записи после каждой операции
            if (!output) {
                std::cerr << "Error: Failed to write to output file" << std::endl;
                input.close();
                output.close();
                return;
            }
        }


        // Проверяем, что чтение завершилось корректно
        if (!input.eof()) {
            std::cerr << "Error: Failed to read part file completely" << std::endl;
            input.close();
            output.close();
            return;
        }

        input.close();
        part_number++;
    }



    std::cout << "Successfully created " << output_file 
         << " (" << total_size << " bytes)" << std::endl;


    output.close();
}



void print_help() {
    std::cout << "Usage:\n"
         << "  To split:   " << "./program -c <input_file> <output_dir> <chunk_size_mb>\n"
         << "  To combine: " << "./program -d <input_dir> <output_file>\n"
         << "Example:\n"
         << "  Split:      " << "./program -c bigfile.dat parts 10\n"
         << "  Combine:    " << "./program -d parts reconstructed.dat" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0 && argc == 5) {
        size_t chunk_size = std::stoul(argv[4]);
        split(argv[2], argv[3], chunk_size);
    } 
    else if (strcmp(argv[1], "-d") == 0 && argc == 4) {
        de_split(argv[2], argv[3]);
    }
    else {
        print_help();
        return 1;
    }

    return 0;
}