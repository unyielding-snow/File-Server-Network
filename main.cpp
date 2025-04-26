#include <boost/thread.hpp>
#include "FileServer.h"

#include <string>
#include <unordered_set>
#include <queue>
#include <sstream>

int main(int argc, char* argv[]) {
    int port = 0; // for e.g. "./fs"

    if (argc == 2) { // e.g. "./fs 8000"
        port = atoi(argv[1]);
    }
    else if (argc != 1) { // e.g. "./fs 8000 {extra garbage here}"
        std::cerr << "Main Error: Invalid argument size " << argc << std::endl;
        return -1;
    }

    FileServer startFileServer(port); // This will construct our FileServer class
    startFileServer.handleRequests(); // FileServer class begins to handle incoming requests

    return 0; // should never get here
}
