#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char* argv[]) {
   char* server;
   int server_port;

   //const char* writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

   char readdata[FS_BLOCKSIZE];
   int status;

   if (argc != 3) {
       std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
       exit(1);
   }
   server = argv[1];
   server_port = atoi(argv[2]);

   fs_clientinit(server, server_port);

   // User 1
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
}