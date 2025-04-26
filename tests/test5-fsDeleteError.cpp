/*
* Delete the existing file or directory "pathname".
*
* fs_delete returns 0 on success, -1 on failure.  Possible failures include:
*     pathname is invalid
*     pathname is "/"
*     pathname is not owned by username
*     pathname does not exist
*     username is invalid
*
* fs_delete is thread safe.
*/

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    //const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    //char readdata[FS_BLOCKSIZE];
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

    status = fs_create("user1", "/dir1", 'd');
    assert(!status);

    status = fs_create("user1", "/dir2", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/dir", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/dir1", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/dir2", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    assert(!status);

    status = fs_create("user1", "/dir/file2", 'f');
    assert(!status);

    // Test 1: Pathname is invalid 
    // TODO: Test both sides of bounds
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
}

/*
* Delete the existing file or directory "pathname".
*
* fs_delete returns 0 on success, -1 on failure.  Possible failures include:
*     pathname is invalid
*     pathname is "/"
*     pathname is not owned by username
*     pathname does not exist
*     username is invalid
*
* fs_delete is thread safe.
*/
