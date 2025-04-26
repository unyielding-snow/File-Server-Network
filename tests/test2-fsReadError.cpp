#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* write_data = "we are only going to put this many characters";

    // char readdata[sizeof(write_data)];
    int status;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/dir", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(!status);

    // In theory, will never complete.
    status = fs_writeblock("user1", "/dir/file", 0, write_data);
    std::cout << status << '\n';

    // status = fs_readblock("user1", "/dir/file", 0, readdata);
    // std::cout << status << readdata << '\n';

    status = fs_create("user1", "/dir/file/dir2", 'd');
    std::cout << "status: " << status;

    status = fs_create("user1", "/dir/dir2", 'd');
    assert(!status);

    std::cout << "Hi!" << '\n';
}
