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

    status = fs_create("user1", "/dir", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(!status);

    status = fs_create("", "/dir/file2", 'f');
    std::cout << status;

    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    assert(!status);

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    assert(!status);

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    std::cout << status;

    status = fs_delete("user1", "/dir/file");
    assert(!status);

    status = fs_delete("user1", "/dir");
    assert(!status);

    std::cout << "Hi!" << '\n';

    // Error handling some edge cases: 
    status = fs_writeblock("user1", "/", 0, writedata);
    std::cout << status << '\n';

    status = fs_writeblock("user1", "/", 0, readdata);
    std::cout << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 513, writedata);
    std::cout << status << '\n';
    status = fs_readblock("user1", "/dir/file", 513, readdata);
    std::cout << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 512, writedata);
    std::cout << status << '\n';
    status = fs_readblock("user1", "/dir/file", 512, readdata);
    std::cout << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 511, writedata);
    std::cout << status << '\n';
    status = fs_readblock("user1", "/dir/file", 511, readdata);
    std::cout << status << '\n';

    status = fs_create("1", "/ ", 'd');
    status = fs_create("1", "/1", 'd');
    status = fs_create("1234567890", "/1234567890", 'd');
    status = fs_create("12345678901", "/12345678901", 'd');
    status = fs_create("123456789", "/12345", 'd');

    
}
