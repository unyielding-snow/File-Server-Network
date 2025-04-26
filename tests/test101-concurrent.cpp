#include <iostream>
#include <cassert>
#include <cstdlib>
#include <pthread.h>
#include <thread>
#include "fs_client.h"

void test5() {
    int status = fs_create("user1", "/dir", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir1", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir2", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir/dir", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir/dir1", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir/dir2", 'd');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir/file", 'f');
    std::cout << "Test 5 create " << status << '\n';

    status = fs_create("user1", "/dir/file2", 'f');
    std::cout << "Test 5 create " << status << '\n';

    // Test 1: Pathname is invalid 
    status = fs_delete("user1", "/dir/nofile/asdkfjaksdhfajkshdfaksjdfhaklsjdhfaksdhfkaajlksdfkjasdhfklasjdhfkahskldjfhaksjdhfaksdhfajkshdfklsjdfaskdfhaskjdfhaskljdfhaksdhfasjkdfhaskjdhfaksjdhfaklsjdhfaksjhdfajkshfdkasjdfkhaskdjfhaskjdf");
    std::cout << "Test 5 Delete " << status << '\n';

    // Test 2: Pathname is '/'
    status = fs_delete("user1", "/");
    std::cout << "Test 5 Delete " << status << '\n';

    // Test 3: pathname is not owned by username
    status = fs_delete("justin", "/dir/dir1");
    std::cout << "Test 5 Delete " << status << '\n';

    // Test 4: pathname does not exist
    status = fs_delete("user1", "/dir/nofile");
    std::cout << "Test 5 Delete " << status << '\n';

    status = fs_delete("user1", "/dir");
    std::cout << "Test 5 Delete " << status << '\n';
}

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

    // Concurrent Test Cases!
    for(int i = 0; i < 20; i++) {
        std::thread t1(test5);
        t1.join();
    }
}

