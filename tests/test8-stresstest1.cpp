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
        exit(1);
    }
    server = argv[1];
    port = atoi(argv[2]);

    fs_clientinit(server, port);

    status = fs_create("user1", "/dirtest", 'd');
    assert(!status);
    
    int count = 0;
    std::string filename;
    
    while (count < 100) {
        filename = "/dirtest/file" + std::to_string(count);
        status = fs_create("user1", filename.c_str(), 'f');
        std::cout << count << ": " << status << '\n';
        
        if (status != 0) break;
        count++;
    }
    
    // Breaks on First Instance of delete: Wrong block number (should be the one we read, not > 1. This means to me, we are not counting right?)
    for (int i = 0; i < count; i++) {
        filename = "/dirtest/file" + std::to_string(i);
        status = fs_delete("user1", filename.c_str());
        // The first time we are calling delete, we are deleting something different than the target we are trying to delete.
        // Deleting file8 instead of /dirtest inode first
        // File0 =  FS_CREATE user1 /dirtest/file0 f (4 = /dirtest/file, 1 = inode /dirtest )
    }
    
    for (int i = 0; i < 10; i++) {
        filename = "/dirtest/newfile" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'f');
        std::cout << "new " << i << ": " << status << '\n';
    }
}