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

    const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
    char readdata[FS_BLOCKSIZE];

    fs_clientinit(server, port);

    int status = fs_create("use r1", "/outorder", 'd');
    assert(status != 0);
    
    status = fs_create("use r1", "/outorder/file", 'f');
    assert(status != 0);

    status = fs_writeblock("use r1", "/outorder/file", 0, writedata);
    assert(status != 0);
    
    status = fs_readblock("use r1", "/outorder/file", 0, readdata);
    assert(status != 0);

    status = fs_delete("use r1", "/outorder/file");
    assert(status != 0);

    status = fs_delete("use r1", "/outorder");
    assert(status != 0);

    std::cout << "should have gotten here\n";

    return 0;
}
