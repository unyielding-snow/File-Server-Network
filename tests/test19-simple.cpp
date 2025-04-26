#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
    const char* overflowWrite = "1234567890 We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.12345678910";

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
    std::cout << "1. create\n";
    assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << "2. create\n";
    assert(!status);

    status = fs_create("user1", "/dir/file2", 'f');
    std::cout << "2. create\n";
    assert(!status);

    
    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    std::cout << "3. write, status = " << status << '\n';
    assert(!status);

    status = fs_writeblock("user1", "/dir/file", 1, overflowWrite);
    std::cout << "4. write, status = " << status << '\n';
    assert(!status);

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    std::cout << "5. readblock, data = " << readdata << '\n';
    std::cout << "5. readblock, status = " << status << '\n';
    //assert(!status);

    status = fs_readblock("user1", "/dir/file", 1, readdata);
    std::cout << "6. readblock, data = " << readdata << '\n';
    std::cout << "6. readblock, status = " << status << '\n';
    //assert(!status);

    std::cout << "Read Data: \n" << readdata << '\n';

    status = fs_create("user1", "/dir/file/dirfail", 'd');
    std::cout << "7. create\n";
    assert(status);

    // status = fs_delete("user1", "/dir/file");
    // assert(!status);
}
