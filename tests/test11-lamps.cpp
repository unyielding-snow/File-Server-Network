#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    char* server = argv[1];
    int port = atoi(argv[2]);

    fs_clientinit(server, port);

    int status = fs_create("user1", "/outorder", 'd');
    assert(status == 0);
    status = fs_create("user1", "/outorder/file", 'f');
    assert(status == 0);

    char blockdata[FS_BLOCKSIZE];
    char readdata[FS_BLOCKSIZE];

    memset(blockdata, 'Z', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 0, blockdata);
    assert(status == 0);

    memset(blockdata, 'D', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 1, blockdata);
    assert(status == 0);

    memset(blockdata, 'B', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 2, blockdata);
    assert(status == 0);

    memset(blockdata, 'F', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 3, blockdata);
    assert(status == 0);

    memset(blockdata, 'A', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 4, blockdata);
    assert(status == 0);

    status = fs_readblock("user1", "/outorder/file", 0, readdata);
    assert(status == 0);
    for (uint32_t i = 0; i < FS_BLOCKSIZE; i++) {
        assert(readdata[i] == 'Z');
    }
    std::cout << "Block 0 success\n";

    status = fs_readblock("user1", "/outorder/file", 1, readdata);
    assert(status == 0);
    for (uint32_t i = 0; i < FS_BLOCKSIZE; i++) {
        assert(readdata[i] == 'D');
    }
    std::cout << "Block 1 success\n";

    status = fs_readblock("user1", "/outorder/file", 3, readdata);
    assert(status == 0);
    for (uint32_t i = 0; i < FS_BLOCKSIZE; i++) {
        assert(readdata[i] == 'F');
    }
    std::cout << "Block 3 success\n";

    status = fs_readblock("user1", "/outorder/file", 4, readdata);
    assert(status == 0);
    for (uint32_t i = 0; i < FS_BLOCKSIZE; i++) {
        assert(readdata[i] == 'A');
    }
    std::cout << "Block 5 success\n";

    return 0;
}
