#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

#include <cstring>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <server> <port>\n";
        return 1;
    }

    char* server = argv[1];
    int port = atoi(argv[2]);

    fs_clientinit(server, port);

    //const char* user = "justin";

    int st = fs_create("justin", "/huge", 'd');

    char ss[FS_BLOCKSIZE] = {0};
    strcpy(ss, "X");

    for (int i = 0; i < 100; ++i) {
        std::string path = "/huge/f" + std::to_string(i);

        st = fs_create("justin", path.c_str(), 'f');

        std::cout << "create\n";

        if (st != 0) {
            std::cout << "ful\n";
            break;
        }
        fs_writeblock("justin", path.c_str(), 0, ss);
    }
}
