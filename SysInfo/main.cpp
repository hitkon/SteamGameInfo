// SPDX-License-Identifier: CC0-1.0
// infoware - C++ System information Library

//#define INFOWARE_USE_D3D="ON"

#include "infoware/gpu.hpp"
#include "infoware/system.hpp"
#include "infoware/cpu.hpp"
#include <string>

#include <iostream>
#include <fstream>
#include <tchar.h>
#include <windows.h>
#include <fileapi.h>
#include <dxdiag.h>
#include <w32api.h>
//#include <vcl.h>

#include <algorithm>

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

void printInfo(){
    unsigned __int64 Storage;
    GetDiskFreeSpaceEx ("C:",
                      NULL,
                      NULL,
                      (PULARGE_INTEGER)&Storage);


    const auto OS_info = iware::system::OS_info();
    const auto memory = iware::system::memory();
    const auto device_properties = iware::gpu::device_properties();

    const auto& properties_of_device = device_properties[0];

    std::string gpu_name = properties_of_device.name;
    gpu_name = gpu_name.substr(gpu_name.find('[') + 1, gpu_name.find('M', gpu_name.find('[')) - gpu_name.find('[') - 1);




    std::cout << "\n"
              << "OS: " << OS_info.full_name << '\n'
              << "Processor: " << iware::cpu::model_name() << '\n'
              << "Memory: " << (memory.physical_total >> 30) << " GB RAM\n"
              << "Graphics: " << gpu_name << " \n"
              << "Storage: " << (Storage >> 30) << " GB\n";

}
void compareInfo(char * filename){

    unsigned __int64 Storage;
    GetDiskFreeSpaceEx ("C:",
                        NULL,
                        NULL,
                        (PULARGE_INTEGER)&Storage);


    const auto OS_info = iware::system::OS_info();
    const auto memory = iware::system::memory();
    const auto device_properties = iware::gpu::device_properties();

    const auto& properties_of_device = device_properties[0];
    std::string gpu_name = properties_of_device.name;
    gpu_name = gpu_name.substr(gpu_name.find('[') + 1, gpu_name.find('M', gpu_name.find('[')) - gpu_name.find('[') - 1);

    std::ifstream in(filename);
    std::string str;

    while(in >> str){
        //std::cout << str << "\n";

        if(str == "OS:"){
            std::getline(in, str);
            str = str.substr(str.find("Windows") + 8);
            int game_version = std::stoi(str);


            std::string OS_name = OS_info.full_name;
            OS_name = (OS_name.substr(OS_name.find(' ') + 1));
            OS_name = (OS_name.substr(OS_name.find(' ') + 1));
            int self_version = std::stoi(OS_name.substr(0, OS_name.find(' ')));
            if(self_version < game_version){
                std::cout << "Problem with Windows OS game version:\n";
                std::cout << "Game OS version: " << game_version << "\n";
                std::cout << "Your OS version: " << self_version << "\n";
                in.close();
                return;
            }

        }
        else if(str == "Processor:"){
            std::ifstream cpu("../cpu.info");
            std::string cpu_model;

            std::string game_cpu1, game_cpu2 ="";
            getline(in, game_cpu1);
            if(game_cpu1.find(" or ") != -1){
                game_cpu2 = game_cpu1.substr(game_cpu1.find(" or ") + 4);
                game_cpu1 = game_cpu1.substr(0, game_cpu1.find(" or "));
            }
            else if(game_cpu1.find(" / ") != -1){
                game_cpu2 = game_cpu1.substr(game_cpu1.find(" / ") + 3);
                game_cpu1 = game_cpu1.substr(0, game_cpu1.find(" / "));
            }
            game_cpu1 = trim(game_cpu1);
            game_cpu2 = trim(game_cpu2);

            for(int i = 0; i < game_cpu1.length(); i++)
                game_cpu1[i] = tolower(game_cpu1[i]);

            for(int i = 0; i < game_cpu2.length(); i++)
                game_cpu2[i] = tolower(game_cpu2[i]);

            std::string self_cpu = iware::cpu::model_name();
            for(int i = 0; i < self_cpu.length(); i++)
                self_cpu[i] = tolower(self_cpu[i]);

            while(std::getline(cpu, cpu_model)){

                if(cpu_model.find(self_cpu) != -1 || self_cpu.find(cpu_model) != -1) {
                    cpu.close();
                    break;
                }
                if(cpu_model.find(game_cpu1) != -1 || game_cpu1.find(cpu_model) != -1){
                    std::cout << "Problem with CPU:\n";
                    std::cout << "Game reuires CPU: " << game_cpu1 << "\n";
                    std::cout << "Your CPU: " << self_cpu << "\n";
                    cpu.close();
                    in.close();
                    return;
                }
                if(game_cpu2!="" && cpu_model.find(game_cpu2) != -1 || game_cpu2.find(cpu_model) != -1){
                    std::cout << "Problem with CPU:\n";
                    std::cout << "Game reuires CPU: " << game_cpu2 << "\n";
                    std::cout << "Your CPU: " << self_cpu << "\n";
                    cpu.close();
                    in.close();
                    return;
                }
            }

        }
        else if(str == "Memory:"){
            int game_memory;
            in >> game_memory;

            int self_memory = (memory.physical_total >> 30);
            if(self_memory < game_memory){
                std::cout << "Problem with RAM amount:\n";
                std::cout << "Game RAM requires: " << game_memory << "\n";
                std::cout << "Your RAM amount: " << self_memory << "\n";
                return;
            }
            std::getline(in, str);
        }
        else if(str == "Graphics:"){
            std::ifstream gpu("../gpu.info");
            std::string gpu_model;

            std::string game_gpu1, game_gpu2 ="";
            getline(in, game_gpu1);
            if(game_gpu1.find(" or ") != -1){
                game_gpu2 = game_gpu1.substr(game_gpu1.find(" or ") + 4);
                game_gpu1 = game_gpu1.substr(1, game_gpu1.find(" or "));
            }
            else if(game_gpu1.find(" / ") != -1){
                game_gpu2 = game_gpu1.substr(game_gpu1.find(" / ") + 3);
                game_gpu1 = game_gpu1.substr(1, game_gpu1.find(" / "));
            }
            game_gpu1 = trim(game_gpu1);
            game_gpu2 = trim(game_gpu2);

            for(int i = 0; i < game_gpu1.length(); i++)
                game_gpu1[i] = tolower(game_gpu1[i]);

            for(int i = 0; i < game_gpu2.length(); i++)
                game_gpu2[i] = tolower(game_gpu2[i]);


            std::string self_gpu = trim(gpu_name);
            for(int i = 0; i < self_gpu.length(); i++)
                self_gpu[i] = tolower(self_gpu[i]);

            while(std::getline(gpu, gpu_model)){
                //std::cout << gpu_model << "\n";
                if(self_gpu == gpu_model || self_gpu.find(gpu_model) != -1 || gpu_model.find(self_gpu) != -1) {
                    gpu.close();
                    break;
                }
                if(gpu_model.find(game_gpu1) != -1 || game_gpu1.find(gpu_model) != -1){
                    std::cout << "Problem with GPU:\n";
                    std::cout << "Game requires GPU: " << game_gpu1 << "\n";
                    std::cout << "Your GPU: " << self_gpu << "\n";
                    gpu.close();
                    in.close();
                    return;
                }
                if(game_gpu2 != "" && (gpu_model.find(game_gpu2) != -1 || game_gpu2.find(gpu_model) != -1)){
                    std::cout << "Problem with GPU:\n";
                    std::cout << "Game reuires GPU: " << game_gpu2 << "\n";
                    std::cout << "Your GPU: " << self_gpu << "\n";
                    gpu.close();
                    in.close();
                    return;
                }
            }
        }
        else if(str == "Storage:"){
            int game_space;
            in >> game_space;
            int storage_gb = (Storage>>30);
            if(game_space > storage_gb){
                std::cout << "Problem with Empty space on C disk:\n";
                std::cout << "Game requires: " << game_space << "\n";
                std::cout << "Your empty space: " << storage_gb << "\n";
                in.close();
                return;
            }
            std::getline(in, str);
        }
        else{
            std::getline(in, str);
        }
    }
    std::cout << "Game is OK\n";
}

int main(int argc, char** argv) {
    if(argc == 2)
        compareInfo(argv[1]);
    else
        printInfo();
}
