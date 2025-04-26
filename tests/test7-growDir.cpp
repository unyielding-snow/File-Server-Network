#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstring>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int port;
    int status;
    
    if (argc != 3) {
        exit(1);
    }


    server = argv[1];
    port = atoi(argv[2]);

    fs_clientinit(server, port);

    status = fs_create("user1", "/growth", 'd');
    assert(!status);
    
    status = fs_create("user1", "/growth/file", 'f');
    assert(!status);
    
    char writedata[FS_BLOCKSIZE];
    char readdata[FS_BLOCKSIZE];
    
    for (int i = 0; i < 130; i++) {
        memset(writedata, 'A' + (i % 26), FS_BLOCKSIZE);
        status = fs_writeblock("user1", "/growth/file", i, writedata);
        std::cout << i << ": " << status << '\n';
    }
    
    for (int i = 0; i < 130; i++) {
        status = fs_readblock("user1", "/growth/file", i, readdata);
        std::cout << "read " << i << ": " << status << '\n';
    }
    
    status = fs_writeblock("user1", "/growth/file", 50, writedata);
    status = fs_readblock("user1", "/growth/file", 50, readdata);
}