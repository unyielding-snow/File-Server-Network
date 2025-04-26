#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    char readdata[FS_BLOCKSIZE];

    int status;

    if (argc != 3) {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    // Test showfs disk according to spec
    // showfs should have dir already

    // status = fs_create("user1", "/dir", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/justindir", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/justindir/file", 'f');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/justindir/dir", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/1", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/2", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/3", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/4", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/5", 'd');
    // std::cout << "status = 0: " << status << '\n';

    // status = fs_create("user1", "/6", 'd');
    // std::cout << "status = 0: " << status << '\n';
    
    // status = fs_create("user1", "/7", 'd');
    // std::cout << "status = 0: " << status << '\n';


    status = fs_writeblock("user1", "/justindir/file", 0, writedata);
    std::cout << "status = 0: " << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'f');
    std::cout << "status = 0: " << status << '\n';

    status = fs_create("user1", "/justindir/dir/dir2", 'd');
    std::cout << "status = 0: " << status << '\n';

    status = fs_readblock("user1", "/justindir/file", 0, readdata);
    std::cout << "status = 0: " << status << '\n';

    // // Disk Size
    // status = fs_create("user1", "/justin1", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "/justin1", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }

    // status = fs_create("user1", "/justin2", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "justin2", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }

    // status = fs_create("user1", "/justin3", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "/justin3", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }

    // status = fs_create("user1", "/justin4", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "/justin4", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }

    // status = fs_create("user1", "/justin5", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "/justin5", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }

    // status = fs_create("user1", "/justin6", 'f');
    // assert(!status);
    // for(uint32_t i = 0; i < FS_BLOCKSIZE + 2; i++){
    //     status = fs_writeblock("user1", "/justin", i, writedata);
    //     std::cout << "status = 0: " << status << '\n';    
    // }
}