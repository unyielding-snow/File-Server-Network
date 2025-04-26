#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int port;
    int status;
    
    if (argc != 3) {
        std::cout << "error\n";
        exit(1);
    }
    server = argv[1];
    port = atoi(argv[2]);

    fs_clientinit(server, port);

    status = fs_create("user1", "/shrink", 'd');
    assert(!status);
    
    std::string filename;
    for (int i = 0; i < 100; i++) {
        filename = "/shrink/file" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'f');
        assert(!status);
    }
    
    for (int i = 0; i < 100; i++) {
        filename = "/shrink/file" + std::to_string(i);
        status = fs_delete("user1", filename.c_str());
        std::cout << "Delete " << status << '\n';
        
        if (i % 10 == 9) {
            filename = "/shrink/new" + std::to_string(i/10);
            status = fs_create("user1", filename.c_str(), 'f');
            std::cout << "Create " << status << '\n';
        }
    }
    
    for (int i = 0; i < 10; i++) {
        filename = "/shrink/final" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'f');
        std::cout << "Create " << status << '\n';
    }
}