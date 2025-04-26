/*
 * Create a new file or directory "pathname".  Type can be 'f' (file) or 'd'
 * (directory).
 *
 * fs_create returns 0 on success, -1 on failure.  Possible failures include:
 *     pathname is invalid
 *     pathname is in a directory that does not exist
 *     pathname is in a directory not owned by username
 *     pathname already exists
 *     the disk or directory containing pathname is out of space
 *     invalid type
 *     username is invalid
 *
 * fs_create is thread safe.
 */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    // const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
    // char readdata[FS_BLOCKSIZE];
    
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

    // Test 1: pathname is invalid
    status = fs_create("user1", "/dir2", 'd');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "/fileA", 'f');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "/", 'f');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "//", 'f');  // fun
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "/ /a", 'f');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "/dir/", 'f');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "hi", 'f');
    std::cout << "Test 1: " << status << '\n';

    status = fs_create("user1", "/dir/file/newfile", 'f');
    std::cout << "Test 1: " << status << '\n';

    // Test 2: pathname is in a directory that does not exist
    status = fs_create("user1", "/dir/file3", 'f');
    std::cout << "Test 2: " << status << '\n';

    // Test 3: pathname is in a directory not owned by username
    status = fs_create("user2", "/dir/file2", 'f');
    std::cout << "Test 3: " << status << '\n';

    // Test 4: pathname already exists
    status = fs_create("user1", "/dir/file", 'f');
    std::cout << "Test 4: " << status << '\n';

    status = fs_create("user1", "/dir/file", 'd');
    std::cout << "Test 4: " << status << '\n';

    status = fs_create("user1", "/dir", 'd');
    std::cout << "Test 4: " << status << '\n';

    // Test 5: the disk containing pathname is out of space
    // FS_BLOCKSIZE = 512;    

    // Test 6: the directory contianing pathname is out of space 
    // FS_MAXFILEBLOCKS = 124
    status = fs_create("justin", "/dir/test6", 'd');
    std::cout << "Test 6: " << status << '\n';

    char pathname[15] = "/dir/test6/!!!";
    for(unsigned int i = 0; i < FS_MAXFILEBLOCKS + 2; i++) {
        if(i % 2 == 0){
            status = fs_create("justin", pathname, 'd');
            std::cout << "Test 6: " << status << '\n';
        }
        else{
            status = fs_create("justin", pathname, 'f');
            std::cout << "Test 6: " << status << '\n';
        }

        if(i % 80 == 0) {
            pathname[12] = pathname[12] + 1;
        }

        pathname[13] = pathname[13] + 1;
    }

    // Test 7: invalid type
    status = fs_create("user1", "/dir/file7", 'a');
    std::cout << "Test 7: " << status << '\n';


    // Test 8: username is invalid
    status = fs_create("", "/dir/file8.1", 'f');
    std::cout << "Test 8: " << status << '\n';

    char test8filename[FS_MAXFILENAME + 3]; 
    for(unsigned int i = 0; i < FS_MAXFILENAME + 2; i++)
    {
        test8filename[i] = 'A';
        test8filename[i+1] = '\0';
        status = fs_create(test8filename, "/dir/file8.1", 'f');
        std::cout << "Test 8: " << status << '\n';
    }
}

