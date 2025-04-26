/*
 * Write a block of data to the file specified by pathname.  offset specifies
 * the block to be written.  offset may refer to an existing block in the file,
 * or it may refer to the block immediately after the current end of the file
 * (this is how files grow in size).  buf specifies where to get the data
 * that will be written to the file.
 *
 * fs_writeblock returns 0 on success, -1 on failure.  Possible failures include:
 *     pathname is invalid
 *     pathname does not exist, is not a file, or is not owned by username
 *     offset is out of range
 *     the disk or file is out of space
 *     username is invalid
 *
 * fs_writeblock is thread safe.
 */


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

    status = fs_create("user2", "/dir/file2", 'f');
    assert(status);

    status = fs_create("user1", "/dir/dir2", 'd');
    assert(!status);

    // JUSTIN - Test 1 to 6 is exactly the same between read and write. 
    // Test 1: Invalid pathname
    status = fs_writeblock("user1", "invalidPathName", 0, writedata);
    std::cout << "Test 1: " << status << '\n';

    char size128[129]; 
    size128[0] = '/';
    for (int i = 1; i < 128; i++) {
        size128[i] = 'o';
    }
    size128[128] = '\0'; 
    
    status = fs_writeblock("user1", size128, 0, writedata);  // MAXPATHNAME = 128, 
    std::cout << "Test 1: " << status << '\n';

    // Test 2: pathname does not exist
    status = fs_writeblock("user1", "/dir/dir2/hi", 0, writedata);  // none 
    std::cout << "Test 2 non-existant file: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file/hi", 0, writedata);
    std::cout << "Test 2 file as directory: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file/hi/three", 0, writedata);
    std::cout << "Test 2 non-existant directory: " << status << '\n';
    
    // Test 3: pathname is not a file
    status = fs_writeblock("user1", "/dir/dir2", 0, writedata);
    std::cout << "Test 3: " << status << '\n';
    
    // Test 4: pathname is not owned by username
    status = fs_writeblock("user1", "/dir/file2", 0, writedata);
    std::cout << "Test 4: " << status << '\n';

    // Test 5: offset is out of range [ lower + upper bounds]
    // TODO: Test both sides of bounds
    status = fs_writeblock("user1", "/dir/file", -1, writedata);
    std::cout << "Test 5 lower bound: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    std::cout << "Test 5 lower bound: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 123, writedata);
    std::cout << "Test 5 within bound: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 124, writedata);
    std::cout << "Test 5 within bound: " << status << '\n';

    status = fs_writeblock("user1", "/dir/file", 125, writedata);
    std::cout << "Test 5 upper bound: " << status << '\n';

    // Test 6:  Username is invalid
    // FS_MAXUSERNAME = 10;
    status = fs_writeblock("myusernameisbiggerthanyours", "/dir/file10", 0, writedata);
    std::cout << "Test 6 : " << status << '\n';

    status = fs_writeblock("", "/dir/file", 0, writedata);
    std::cout << "Test 6 : " << status << '\n';
    
    status = fs_writeblock("user1", "", 0, writedata);
    std::cout << "Test 6 : " << status << '\n';

    // Test 7: the disk or file is out of space (only different test)
    // To do so, we probably need to check
    // FS_MAXFILEBLOCKS = 124
    // FS_BLOCKSIZE = 512;
    for(int i = 0; i < 125; i++) {
        status = fs_writeblock("user1", "/dir/file", i, writedata);  
        std::cout << "Test 7 : " << status << '\n';
    }

    const char* bigdata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";
    status = fs_writeblock("user1", "/dir/file", 0, bigdata);  
    std::cout << "Test 7 : " << status << '\n';

    for(int i = 125; i < 560; i++) {
        status = fs_writeblock("user1", "/dir/file", i, writedata);  
        std::cout << "Test 7 : " << status << '\n';
    }
    status = fs_writeblock("user1", "/dir/file", 0, writedata);  
    std::cout << "Test 7 : " << status << '\n';


    // Big Brain: Double Check Read File
    std::cout << "Read Start Again ---" << '\n';
    // Test 1: Invalid pathname
    status = fs_readblock("user1", "invalidPathName", 0, readdata);
    std::cout << "Test 1: " << status << '\n';

    // status = fs_readblock("user1", size128, 0, readdata);  // MAXPATHNAME = 128, 
    // std::cout << "Test 1: " << status << '\n';

    // Test 2: pathname does not exist
    status = fs_readblock("user1", "/dir/dir2/hi", 0, readdata);  // none 
    std::cout << "Test 2 non-existant file: " << status << '\n';

    status = fs_readblock("user1", "/dir/file/hi", 0, readdata);
    std::cout << "Test 2 file as directory: " << status << '\n';

    status = fs_readblock("user1", "/dir/file/hi/three", 0, readdata);
    std::cout << "Test 2 non-existant directory: " << status << '\n';
    
    // Test 3: pathname is not a file
    status = fs_readblock("user1", "/dir/dir2", 0, readdata);
    std::cout << "Test 3: " << status << '\n';
    
    // Test 4: pathname is not owned by username
    status = fs_readblock("user1", "/dir/file2", 0, readdata);
    std::cout << "Test 4: " << status << '\n';

    // Test 5: offset is out of range [ lower + upper bounds]
    status = fs_readblock("user1", "/dir/file", -1, readdata);
    std::cout << "Test 5 lower bound: " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    std::cout << "Test 5 lower bound: " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 123, readdata);
    std::cout << "Test 5 within bound: " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 124, readdata);
    std::cout << "Test 5 within bound: " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 125, readdata);
    std::cout << "Test 5 upper bound: " << status << '\n';

    // Test 6:  Username is invalid
    // FS_MAXUSERNAME = 10;
    status = fs_readblock("myusernameisbiggerthanyours", "/dir/file", 0, readdata);
    std::cout << "Test 6 : " << status << '\n';


 }
 