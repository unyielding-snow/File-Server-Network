#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstring>
#include "fs_client.h"

using std::to_string;
using std::string;

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

    // Test 1
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

    // Test 2: 
    status = fs_create("user1", "/dir", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << status << '\n';

    // In theory, will never complete.
    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    std::cout << status << '\n';

    // status = fs_readblock("user1", "/dir/file", 0, readdata);
    // std::cout << status << readdata << '\n';

    status = fs_create("user1", "/dir/file/dir2", 'd');
    std::cout << "status: " << status;

    status = fs_create("user1", "/dir/dir2", 'd');
    std::cout << status << '\n';

    std::cout << "Hi!" << '\n';

    // Test 3: 
    status = fs_create("user1", "/dir", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << status << '\n';

    status = fs_create("user2", "/dir/file2", 'f');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'd');
    std::cout << status << '\n';

    // Writing to a directory (you can't do this)


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

    // TEST 4: ----------------------------------------------------------------------------------------------------------------
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


    // TEST 5: ----------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/dir", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir1", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir2", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir1", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/file2", 'f');
    std::cout << status << '\n';

    // Test 1: Pathname is invalid 
    status = fs_delete("user1", "/dir/nofile/asdkfjaksdhfajkshdfaksjdfhaklsjdhfaksdhfkaajlksdfkjasdhfklasjdhfkahskldjfhaksjdhfaksdhfajkshdfklsjdfaskdfhaskjdfhaskljdfhaksdhfasjkdfhaskjdhfaksjdhfaklsjdhfaksjhdfajkshfdkasjdfkhaskdjfhaskjdf");
    std::cout << "Test 1: " << status << '\n';

    // Test 2: Pathname is '/'
    status = fs_delete("user1", "/");
    std::cout << "Test 2: " << status << '\n';

    // Test 3: pathname is not owned by username
    status = fs_delete("justin", "/dir/dir1");
    std::cout << "Test 3: " << status << '\n';

    // Test 4: pathname does not exist
    status = fs_delete("user1", "/dir/nofile");
    std::cout << "Test 1: " << status << '\n';


    // Test 5: username is invalid
    // TODO: Test both sides of bounds
        

    // Justin
    status = fs_delete("user1", "/dir");
    std::cout << "Can you delete something that is not empty? : " << status << '\n';
    
    // TEST 6: ----------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/shrink", 'd');
    std::cout << status << "\n";
    
    std::string filename;
    for (int i = 0; i < 100; i++) {
        filename = "/shrink/file" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'f');
        std::cout << status << "\n";
    }
    
    for (int i = 0; i < 100; i++) {
        filename = "/shrink/file" + std::to_string(i);
        status = fs_delete("user1", filename.c_str());
        std::cout << "Delete " << status << '\n';
        
        if (i % 10 == 9) {
            filename = "/shrink/new" + std::to_string(i/10);
            status = fs_create("user1", filename.c_str(), 'f');
            std::cout << "Create " << status << '\n';
        }
    }
    
    for (int i = 0; i < 10; i++) {
        filename = "/shrink/final" + std::to_string(i);
        status = fs_create("user1", filename.c_str(), 'f');
        std::cout << "Create " << status << '\n';
    }

    // TEST 7: ---------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/growth", 'd');
    std::cout << status << "\n";
    
    status = fs_create("user1", "/growth/file", 'f');
    std::cout << status << "\n";
    
    for (int i = 0; i < 130; i++) {
        //memset(writedata, 'A' + (i % 26), FS_BLOCKSIZE);
        status = fs_writeblock("user1", "/growth/file", i, writedata);
        std::cout << i << ": " << status << '\n';
    }
    
    for (int i = 0; i < 130; i++) {
        status = fs_readblock("user1", "/growth/file", i, readdata);
        std::cout << "read " << i << ": " << status << '\n';
    }
    
    status = fs_writeblock("user1", "/growth/file", 50, writedata);
    std::cout << status << '\n';
    status = fs_readblock("user1", "/growth/file", 50, readdata);
    std::cout << status << '\n';

    // TEST 8: ------------------------------------------------------------------------------------------------------------
    status = fs_create("justin", "/dir", 'd');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2", 'd');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2/dir3", 'd');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2/dir3/", 'd');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2.1", 'd');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2/dir3/file", 'f');
   std::cout << "User 1: " << status << '\n';

   status = fs_create("justin", "/dir/dir2/dir3/file1", 'f');    
   std::cout << "User 1: " << status << "\n";

   status = fs_create("justin", "/dir/dir2/dir3/file2", 'f');    
   std::cout << "User 1: " << status << "\n";

   status = fs_create("justin", "/dir/dir2/dir3/file3", 'd');    
   std::cout << "User 1: " << status << "\n";

   status = fs_create("justin", "/dir/dir2/dir3/file4", 'd');    
   std::cout << "User 1: " << status << "\n";

   status = fs_create("justin", "/dir/dir2/dir3/file5", 'f');    
   std::cout << "User 1: " << status << "\n";


   // User 2
   status = fs_create("james", "/dir", 'd');
   std::cout << "User 2: " << status << '\n';

   status = fs_create("james", "/dir/dir2", 'd');
   std::cout << "User 2: " << status << '\n';

   status = fs_create("james", "/dir/dir2/dir3", 'd');
   std::cout << "User 2: " << status << '\n';

   status = fs_create("james", "/dir/dir2/dir3/", 'd');
   std::cout << "User 2: " << status << '\n';

   status = fs_create("james", "/dir/dir2.1", 'd');
   std::cout << "User 2: " << status << '\n';

   status = fs_create("james", "/dir/dir2/dir3/file", 'f');
   std::cout << "User 2: " << status << '\n';

   // User 3
   status = fs_create("ashton", "/dir", 'd');
   std::cout << "User 3: " << status << '\n';

   status = fs_create("ashton", "/dir/dir2", 'd');
   std::cout << "User 3: " << status << '\n';

   status = fs_create("ashton", "/dir/dir2/dir3", 'd');
   std::cout << "User 3: " << status << '\n';

   status = fs_create("ashton", "/dir/dir2/dir3/", 'd');
   std::cout << "User 3: " << status << '\n';

   status = fs_create("ashton", "/dir/dir2.1", 'd');
   std::cout << "User 3: " << status << '\n';

   status = fs_create("ashton", "/dir/dir2/dir3/file", 'f');
   std::cout << "User 3: " << status << '\n';

   // Traversing folders of the same name, but different build folders
   // i.e. /dir/dir2/dir3/file1 {user1}  /dir/dir2/dir3/file1 {user2} and making sure they have different blocks when reading

   status = fs_delete("james", "/dir/dir2/dir3/file");    
   std::cout << "Same folder Test: " << status << "\n";

   status = fs_readblock("justin", "/dir/dir2/dir3/file", 10, readdata);    
   std::cout << "Same folder Test: " << status << "\n";

   status = fs_readblock("ashton", "/dir/dir2/dir3/file", 10, readdata);    
   std::cout << "Same folder Test: " << status << "\n";

   // Delelting one user's dir/file, and making sure the other user can still access their own in the right block.
   // Deleting all the files in a directory, should cause a write / read
   status = fs_delete("justin", "/dir/dir2/dir3/file");
   std::cout << "Delete All Test: " << status << '\n';

   status = fs_delete("justin", "/dir/dir2/dir3/file1");    
   std::cout << "Delete All Test: " << status << "\n";

   status = fs_delete("justin", "/dir/dir2/dir3/file2");    
   std::cout << "Delete All Test: " << status << "\n";

   status = fs_delete("justin", "/dir/dir2/dir3/file3");    
   std::cout << "Delete All Test: " << status << "\n";

   status = fs_delete("justin", "/dir/dir2/dir3/file4");    
   std::cout << "Delete All Test: " << status << "\n";

   status = fs_delete("justin", "/dir/dir2/dir3/file5");    
   std::cout << "Delete All Test: " << status << "\n";


   // Deleting a non-empty directory
   status = fs_delete("james", "/dir/dir2");    
   std::cout << "Non-empty Test: " << status << "\n";

   status = fs_delete("james", "/dir/dir2/dir3");    
   std::cout << "Non-empty Test: " << status << "\n";

   status = fs_delete("james", "/dir/dir2");    
   std::cout << "Non-empty Test: " << status << "\n";

    // TEST 10: ------------------------------------------------------------------------------------------------------------
    status = fs_create("justin", "/dir", 'd');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2", 'd');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2/dir3", 'd');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2/dir3/", 'd');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2.1", 'd');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2/dir3/file", 'f');
    std::cout << "User 1: " << status << '\n';

    status = fs_create("justin", "/dir/dir2/dir3/file1", 'f');    
    std::cout << "User 1: " << status << "\n";

    status = fs_create("justin", "/dir/dir2/dir3/file2", 'f');    
    std::cout << "User 1: " << status << "\n";

    status = fs_create("justin", "/dir/dir2/dir3/file3", 'd');    
    std::cout << "User 1: " << status << "\n";

    status = fs_create("justin", "/dir/dir2/dir3/file4", 'd');    
    std::cout << "User 1: " << status << "\n";

    status = fs_create("justin", "/dir/dir2/dir3/file5", 'f');    
    std::cout << "User 1: " << status << "\n";


    // User 2
    status = fs_create("james", "/dir", 'd');
    std::cout << "User 2: " << status << '\n';

    status = fs_create("james", "/dir/dir2", 'd');
    std::cout << "User 2: " << status << '\n';

    status = fs_create("james", "/dir/dir2/dir3", 'd');
    std::cout << "User 2: " << status << '\n';

    status = fs_create("james", "/dir/dir2/dir3/", 'd');
    std::cout << "User 2: " << status << '\n';

    status = fs_create("james", "/dir/dir2.1", 'd');
    std::cout << "User 2: " << status << '\n';

    status = fs_create("james", "/dir/dir2/dir3/file", 'f');
    std::cout << "User 2: " << status << '\n';

    // User 3
    status = fs_create("ashton", "/dir", 'd');
    std::cout << "User 3: " << status << '\n';

    status = fs_create("ashton", "/dir/dir2", 'd');
    std::cout << "User 3: " << status << '\n';

    status = fs_create("ashton", "/dir/dir2/dir3", 'd');
    std::cout << "User 3: " << status << '\n';

    status = fs_create("ashton", "/dir/dir2/dir3/", 'd');
    std::cout << "User 3: " << status << '\n';

    status = fs_create("ashton", "/dir/dir2.1", 'd');
    std::cout << "User 3: " << status << '\n';

    status = fs_create("ashton", "/dir/dir2/dir3/file", 'f');
    std::cout << "User 3: " << status << '\n';

    // Traversing folders of the same name, but different build folders
    // i.e. /dir/dir2/dir3/file1 {user1}  /dir/dir2/dir3/file1 {user2} and making sure they have different blocks when reading
    std::string pathnamejames = "/dir/dir2/dir3/file";

    status = fs_delete("james", "/dir/dir2/dir3/file");    
    std::cout << "Same folder Test: " << status << "\n";

    status = fs_readblock("justin", "/dir/dir2/dir3/file", 10, readdata);    
    std::cout << "Same folder Test: " << status << "\n";

    status = fs_readblock("ashton", "/dir/dir2/dir3/file", 10, readdata);    
    std::cout << "Same folder Test: " << status << "\n";

    // Delelting one user's dir/file, and making sure the other user can still access their own in the right block.
    // Deleting all the files in a directory, should cause a write / read

    // This should never be zero:
    // FAILS HERE: says "numEntriesInBlock == 0", which should not be? because we have written file1, file2, file3, 
    status = fs_delete("justin", "/dir/dir2/dir3/file");
    std::cout << "Delete All Test: " << status << '\n';

    status = fs_delete("justin", "/dir/dir2/dir3/file1");    
    std::cout << "Delete All Test: " << status << "\n";

    status = fs_delete("justin", "/dir/dir2/dir3/file2");    
    std::cout << "Delete All Test: " << status << "\n";

    status = fs_delete("justin", "/dir/dir2/dir3/file3");    
    std::cout << "Delete All Test: " << status << "\n";

    status = fs_delete("justin", "/dir/dir2/dir3/file4");    
    std::cout << "Delete All Test: " << status << "\n";

    status = fs_delete("justin", "/dir/dir2/dir3/file5");    
    std::cout << "Delete All Test: " << status << "\n";


    // Deleting a non-empty directory
    status = fs_delete("james", "/dir/dir2");    
    std::cout << "Non-empty Test: " << status << "\n";

    status = fs_delete("james", "/dir/dir2/dir3");    
    std::cout << "Non-empty Test: " << status << "\n";

    status = fs_delete("james", "/dir/dir2");    
    std::cout << "Non-empty Test: " << status << "\n";

    // TEST 11: -------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/outorder", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/outorder/file", 'f');
    std::cout << status << '\n';

    char blockdata[FS_BLOCKSIZE];

    memset(blockdata, 'Z', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 0, blockdata);
    std::cout << status << '\n';

    memset(blockdata, 'D', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 1, blockdata);
    std::cout << status << '\n';

    memset(blockdata, 'B', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 2, blockdata);
    std::cout << status << '\n';

    memset(blockdata, 'F', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 3, blockdata);
    std::cout << status << '\n';

    memset(blockdata, 'A', FS_BLOCKSIZE);
    status = fs_writeblock("user1", "/outorder/file", 4, blockdata);
    std::cout << status << '\n';

    status = fs_readblock("user1", "/outorder/file", 0, readdata);
    std::cout << status << '\n';
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

    // TEST 12: -----------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("use r1", "/outorder", 'd');
    std::cout << status << '\n';
    
    status = fs_create("use r1", "/outorder/file", 'f');
    std::cout << status << '\n';

    status = fs_writeblock("use r1", "/outorder/file", 0, writedata);
    std::cout << status << '\n';
    
    status = fs_readblock("use r1", "/outorder/file", 0, readdata);
    std::cout << status << '\n';

    status = fs_delete("use r1", "/outorder/file");
    std::cout << status << '\n';

    status = fs_delete("use r1", "/outorder");
    std::cout << status << '\n';

    // TEST 13: -------------------------------------------------------------------------------------------------------------------------------------
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

    // TEST 14: ----------------------------------------------------------------------------------------------------------------------------------------
    status = fs_writeblock("user1", "/justindir/file", 0, writedata);
    std::cout << "status = 0: " << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'f');
    std::cout << "status = 0: " << status << '\n';

    status = fs_create("user1", "/justindir/dir/dir2", 'd');
    std::cout << "status = 0: " << status << '\n';

    status = fs_readblock("user1", "/justindir/file", 0, readdata);
    std::cout << "status = 0: " << status << '\n';

    // TEST 15: ----------------------------------------------------------------------------------------------------------------------------------------
    string temp = "/justin" + to_string(100);
    status = fs_create("user1", temp.c_str(), 'f');
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_writeblock("user1", temp.c_str(), i, writedata);
        std::cout << "status at " << i << " : " << status << '\n';
    }

    // TEST 16: ----------------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", temp.c_str(), 'f');
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_writeblock("user1", temp.c_str(), i, writedata);
        std::cout << "status at " << i << " : " << status << '\n';
    }

    temp = "/justin" + to_string(10);
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_delete("user1", temp.c_str());
        std::cout << "status at " << i << " : " << status << '\n';
    }

    temp = "/justin" + to_string(11);
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_delete("user1", temp.c_str());
        std::cout << "status at " << i << " : " << status << '\n';
    }

    temp = "/justin" + to_string(200);
    for(uint32_t i = 0; i < FS_MAXFILEBLOCKS + 2; i++){
        status = fs_create("user1", temp.c_str(), 'd');
        std::cout << "status at " << i << " : " << status << '\n';
        temp = "/justin" + to_string(200);
    }

    // TEST 17: --------------------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/dir", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'd');
    std::cout << status << '\n';

    status = fs_create("user1", "/dir/dir2/dir3", 'd');
    std::cout << status;

    status = fs_create("user1", "/dir/dir2/dir3/dir4", 'd');
    std::cout << status;

    status = fs_create("user1", "/dir/dir2/dir3/dir4/dir5", 'd');
    std::cout << status;

    status = fs_delete("user1", "/dir/dir2/dir3/dir4/dir5");
    std::cout << "Hi!" << '\n';

    // TEST 18: ----------------------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/testdir", 'd');
    std::cout << status << '\n';

    // file inside dir
    status = fs_create("user1", "/testdir/testfile", 'f');
    std::cout << status << '\n';

    // write to block
    status = fs_writeblock("user1", "/testdir/testfile", 0, writedata);
    std::cout << status << '\n';
    
    // non-canonical part ?? idk
    status = fs_create("user1", "/testdir/file0000001", 'f');
    std::cout << status << '\n';


    // TEST 19: ------------------------------------------------------------------------------------------------------------------------------------------------
    status = fs_create("user1", "/dir", 'd');
    std::cout << "1. create\n";

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << "2. create\n";

    status = fs_create("user1", "/dir/file2", 'f');
    std::cout << "2. create\n";
    
    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    std::cout << "3. write, status = " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 0, readdata);
    std::cout << "5. readblock, data = " << readdata << '\n';
    std::cout << "5. readblock, status = " << status << '\n';

    status = fs_readblock("user1", "/dir/file", 1, readdata);
    std::cout << "6. readblock, data = " << readdata << '\n';
    std::cout << "6. readblock, status = " << status << '\n';

    std::cout << "Read Data: \n" << readdata << '\n';

    status = fs_create("user1", "/dir/file/dirfail", 'd');
    std::cout << "7. create\n";

    // TEST 20: --------------------------------------------------------------------------------------------------------------------------------------------------

}
