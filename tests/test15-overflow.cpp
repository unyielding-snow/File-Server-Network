#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include "fs_client.h"

using std::string;
using std::to_string;

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    // char readdata[FS_BLOCKSIZE];

    int status;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    // Test showfs disk according to spec
    // showfs should have dir alread
    // Disk Size

    // 31 might be too much
    // for(uint32_t num = 0; num < 32; num++) {
    //     string temp = "/justin" + to_string(num);
    //     status = fs_create("user1", temp.c_str(), 'f');
    //     assert(!status);
    //     for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
    //         status = fs_writeblock("user1", temp.c_str(), i, writedata);
    //         std::cout << "status at " << i << " : " << status << '\n';
    //     }
    // }

    string temp = "/justin" + to_string(100);
    status = fs_create("user1", temp.c_str(), 'f');
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_writeblock("user1", temp.c_str(), i, writedata);
        std::cout << "status at " << i << " : " << status << '\n';
    }
}