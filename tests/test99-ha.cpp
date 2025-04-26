#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "fs_client.h"


const int numThreads = 100;
const int filesPerThread = 50000;
const int opsPerThread = 30000;


void generateData(char* buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = 'A' + (rand() % 26);
    }
}
void threadFunc(int threadId) {
    std::string username = "user" + std::to_string(threadId % 5);
    std::string dirName = "/thread_" + std::to_string(threadId);
    std::vector<std::string> files;
    
    char readBuffer[FS_BLOCKSIZE];
    char writeBuffer[FS_BLOCKSIZE];
    
    int status = fs_create(username.c_str(), dirName.c_str(), 'd');
    
    if (status == 0) {
        for (int i = 0; i < filesPerThread; i++) {
            std::string fileName = dirName + "/file_" + std::to_string(i);
            status = fs_create(username.c_str(), fileName.c_str(), 'f');
            
            if (status == 0) {
                files.push_back(fileName);
                
                generateData(writeBuffer, FS_BLOCKSIZE);
                fs_writeblock(username.c_str(), fileName.c_str(), 0, writeBuffer);
            }
        }
        
        for (int i = 0; i < opsPerThread; i++) {
            if (files.empty()) break;
            
            std::string& file = files[rand() % files.size()];
            int operation = rand() % 3;
            
            if (operation == 0) {
                int block = rand() % 3;
                status = fs_readblock(username.c_str(), file.c_str(), block, readBuffer);
                
                if (status != 0) {
                    std::cerr << "Thread " << threadId << ": Error reading block " << block << " from " << file << std::endl;
                }
            } 
            else if (operation == 1) {
                int block = rand() % 5;
                
                generateData(writeBuffer, FS_BLOCKSIZE);
                status = fs_writeblock(username.c_str(), file.c_str(), block, writeBuffer);
                
                if (status != 0) {
                    std::cerr << "Thread " << threadId << ": Error writing block " << block << " to " << file << std::endl;
                }
            }
            else {
                status = fs_delete(username.c_str(), file.c_str());
                
                if (status != 0) {
                    std::cerr << "Thread " << threadId << ": Error deleting " << file << std::endl;
                }
                else {
                    status = fs_create(username.c_str(), file.c_str(), 'f');
                    
                    if (status != 0) {
                        std::cerr << "Thread " << threadId << ": Error recreating " << file << std::endl;
                    }
                    else {
                        generateData(writeBuffer, FS_BLOCKSIZE);
                        fs_writeblock(username.c_str(), file.c_str(), 0, writeBuffer);
                    }
                }
            }
        }
        
        for (const auto& file : files) {
            fs_delete(username.c_str(), file.c_str());
        }
        
        fs_delete(username.c_str(), dirName.c_str());
    }
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        exit(1);
    }
    
    char* server = argv[1];
    int serverPort = atoi(argv[2]);
    
    int status = fs_clientinit(server, serverPort);
    if (status != 0) {
        return 1;
    }
    
    srand(time(NULL));
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(threadFunc, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}