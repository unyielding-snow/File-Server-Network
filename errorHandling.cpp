#include "FileServer.h"

bool FileServer::isRequestValid(const requestInfo &request) {
    if (request.type == INVALID) return false; // if the request we got was INVALID
    
    if (!isUsernameValid(request.username)) return false; // username must be valid    
    if (!isPathnameValid(request.pathname)) return false; // pathname must be valid
    
    if (request.type == FS_READBLOCK || request.type == FS_WRITEBLOCK) {
        if (!isBlockOffsetValid(request.block)) return false; // block must be within bounds
    }
    else if (request.type == FS_CREATE) {
        if (request.fileType != 'f' && request.fileType != 'd') return false; // must be a file or directory
    }
    else if (request.type == FS_DELETE) {
        if (request.pathname == "/") return false; // reject root directory
    }

    return true;
}

// Function to check if the block is valid/invalid
bool FileServer::isBlockOffsetValid(const int64_t &blockIndex) { // only for FS_CREATE and FS_DELETE
    DB(cout << "isBlockOffsetValid() with offset: " << blockIndex << "\n");
    return (blockIndex >= 0 && blockIndex < FS_MAXFILEBLOCKS);
}

// Function used to check if username is valid/invalid
bool FileServer::isUsernameValid(const string &username) {
    DB(cout << "isUserNameValid()\n");
    
    if (username.empty()) return false;
    if (username.size() > FS_MAXUSERNAME) return false;
    
    // Check for whitespace characters
    for (char c : username) {
        if (isspace(c)) return false;
    }
    
    return true;
}

// Function used to check if pathname is valid/invalid
bool FileServer::isPathnameValid(const string &pathname) {
    DB(cout << "isValidPathnameValid()\n");

    if (pathname.size() > FS_MAXPATHNAME || pathname.empty()) return false;
    if (pathname.front() != '/' || pathname.back() == '/') return false;

    if (pathname.find('\0') != std::string::npos){
        DB(cout << pathname << '\n');
        return false; // if '/0' is in any part of the path, then return
    }

    size_t start = 1; // Skip the leading '/'
    while (start < pathname.size()) {
        size_t end = pathname.find('/', start);

        if (end == string::npos) 
            end = pathname.size();
        
        // Empty part, like in "//"
        if (end == start){
            return false;
        }
        
        // If part is too long
        if (end - start > FS_MAXFILENAME){
            return false;
        }
        
        // Check for whitespace in part
        string parsedPart = pathname.substr(start, end - start);
        
        for (char c : parsedPart) {
            if (isspace(c)){
                return false;
            }
        }
        
        start = end + 1;
    }

    return true;
}
