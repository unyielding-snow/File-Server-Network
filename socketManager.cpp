#include "FileServer.h"

serverReturn FileServer::initializeServerSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); // initialize the server's socket
    
    // if socket constructor failure, propogate failure
    if(serverSocket < 0) {
        DB(cout << "server socket error: Initialization\n");
        return NOCLOSE;
    }

    // Set the "reuse port" socket option.  Use setsockopt().
    int opt = 1;
    // if setsockopt failure, propogate failure
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        DB(cout << "Error: setsockopt failed\n");
        return CLOSE;
    }

    // Configure a sockaddr_in for the accepting socket.
    sockaddr_in addr{}; // initialize new socket address with zeroes
    addr.sin_family = AF_INET; // says we're using a IPv4 address
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // performs host to network switch (little-endian)
    addr.sin_port = htons(port); // performs host to network switch (big-endian)
    
    // if bind failure, propogate failure
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        DB(cout << "Error: bind failed\n");
        return CLOSE;
    }

    socklen_t addr_len = sizeof(addr);
    // if getsockname failure, propogate failure
    if (getsockname(serverSocket, reinterpret_cast<sockaddr*>(&addr), &addr_len) < 0) {
        DB(cout << "Error: getsockname failed\n");
        return CLOSE;
    }
    
    port = ntohs(addr.sin_port); // perform network to host switch (little-endian)

    // if listen failure, propogate failure
    if (listen(serverSocket, 30) < 0) {
        DB(cout << "Error: listen failed\n");
        return CLOSE;
    }

    print_port(port);
    
    return NOCLOSE;
}

void FileServer::handleRequests() {
    while (true) { // indefinitely handle requests
        sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Accept grabs new client socket
        int requestSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

        // if accept failure, propogate failure
        if (requestSocket < 0) {
            DB(cout << "Error: requestSocket failed\n");
            continue;
        }

        DB(cout << "Successfully starting client socket: " << requestSocket << '\n');

        // Process client request as new thread
        thread newThread(&FileServer::processClient, this, requestSocket);
        newThread.detach(); // clean up thread overhead when complete
    }
}

void FileServer::processClient(int clientSocket) {
    SocketGuard guard(clientSocket); // 5. Close client socket on end of function

    string message;
    char buf; // one char
    int byteReceived;
    uint32_t totalBytesReceived = 0;

    // 1. Recieve until null terminator
    do { // do-while from lab
        byteReceived = recv(clientSocket, &buf, 1, 0);

        if (byteReceived <= 0) return;

        totalBytesReceived += byteReceived;
        message.push_back(buf);

    } while (totalBytesReceived <= MaxRequestBytes && byteReceived > 0 && buf != '\0');
    
    DB(cout << "Message recieved: '" <<  message << "'" << '\n');

    requestInfo request = parseClientRequest(message);
    // 2. Check validity of general request
    if (!isRequestValid(request)) {
        DB(cout << "Invalid Request: IsRequestValid() returned false" << '\n');
        return;
    }

    // 3. Read <data> if writeblock
    if (request.type == FS_WRITEBLOCK) {
        vector<char> data(FS_BLOCKSIZE);
        int received = recv(clientSocket, data.data(), FS_BLOCKSIZE, MSG_WAITALL);
        if (static_cast<uint32_t>(received) < FS_BLOCKSIZE) return; // I think impossible to happen
        memcpy(request.data.data(), data.data(), FS_BLOCKSIZE);
    }

    // 4. In this function, create new thread to process client request
    processClientRequest(clientSocket, request);
}

// To parse the request we received
requestInfo FileServer::parseClientRequest(const string &message) {
    requestInfo request;
    istringstream ss(message);
    string command, garbage;
    
    request.message = message;

    if (!(ss >> command >> request.username >> request.pathname)) {
        request.type = INVALID;
        return request;
    }

    if (command == "FS_READBLOCK" || command == "FS_WRITEBLOCK") {
        command == "FS_READBLOCK" ? request.type = FS_READBLOCK : request.type = FS_WRITEBLOCK;
        string blockStr;

        if (!(ss >> blockStr)) { // attempt to read in block
            request.type = INVALID;
            return request;
        }

        if (!blockStr.empty() && blockStr.back() == '\0') { // Cut off null-terminator
            blockStr.pop_back();
        }

        // a) if block is too long
        // b) if block has leading zeros
        if (!blockStr.empty() && (blockStr.size() > 3 || (blockStr.size() > 1 && blockStr[0] == '0'))) {
            request.type = INVALID;
            return request;
        }

        // tries to convert block string
        try {
            request.block = stoul(blockStr);
        }
        catch (...) {
            request.type = INVALID;
            return request;
        }

        if (request.block >= FS_MAXFILEBLOCKS) {
            request.type = INVALID;
            return request;
        }

        DB(cout << "request.block in parse is: " << request.block << '\n');
    }
    else if (command == "FS_CREATE") {
        request.type = FS_CREATE;
        string fileTypeTemp;

        if (!(ss >> fileTypeTemp)) { // try to read in type of request ('f' or 'd')
            request.type = INVALID;
            return request;
        }

        if (!fileTypeTemp.empty() && fileTypeTemp.back() == '\0') { // Cut off null-terminator
            fileTypeTemp.pop_back();
        }

        if (fileTypeTemp.size() > 1) { // if type is longer than  1
            request.type = INVALID;
            return request;
        }

        request.fileType = fileTypeTemp[0]; // grab type char
    }
    else if (command == "FS_DELETE") {
        request.type = FS_DELETE;
        if (!request.pathname.empty() && request.pathname.back() == '\0') { // Cut off null-terminator
            request.pathname.pop_back();
        }
    }

    // according to piazza may cause error to try to find garbage
    // if ((ss >> garbage)) {
    //     request.type = INVALID;
    //     return request;
    // }

    return request;
}

// Function to run request
void FileServer::processClientRequest(int clientSocket, requestInfo &request) {
    string response;
    bool success = false; // assume failure by default

    // call function to process respective request and form response message
    switch (request.type) {
        case FS_READBLOCK: {
            success = process_FS_READBLOCK(request);
            response = "FS_READBLOCK " + request.username + " " + request.pathname + " " + to_string(request.block);
            break;
        }
        case FS_WRITEBLOCK: {
            success = process_FS_WRITEBLOCK(request);
            response = "FS_WRITEBLOCK " + request.username + " " + request.pathname + " " + to_string(request.block);
            break;
        }
        case FS_CREATE: {
            success = process_FS_CREATE(request);
            response = "FS_CREATE " + request.username + " " + request.pathname + " " + request.fileType;
            break;
        }
        case FS_DELETE: {
            success = process_FS_DELETE(request);
            response = "FS_DELETE " + request.username + " " + request.pathname;
            break;
        }
        default: {
            DB(cout << "Error: processClientRequest() called on invalid command\n");
            return;
        }
    }
    
    DB(cout << "status of success is: " << success << "\n");

    if (success) {
        size_t responseSize = response.size() + 1;  // +1 for null terminator :)

        if (request.type == FS_READBLOCK) { // FS_READBLOCK
            string finalResponse(responseSize + FS_BLOCKSIZE, '\0');
                
            memcpy(&finalResponse[0], response.c_str(), response.size()); // copy over message
            memcpy(&finalResponse[responseSize], request.data.data(), FS_BLOCKSIZE); // copy over data
                
            // send message buffer function call
            if (!sendEvenPartial(clientSocket, finalResponse.c_str(), finalResponse.size())) return;
        }
        else { // FS_WRITEBLOCK, FS_CREATE, FS_DELETE
            // send message buffer function call
            if (!sendEvenPartial(clientSocket, response.c_str(), responseSize)) return;
        }      
    }
}


// function to send in chunks (in case send doesn't send the whole message)
bool FileServer::sendEvenPartial(int socket, const void* buffer, size_t length) {
    size_t bytesSent = 0;
    const char* buffPtr = static_cast<const char*>(buffer);
    
    // wanna loop until all data is sent
    while (bytesSent < length) {
        int sent = send(socket, buffPtr + bytesSent, length - bytesSent, MSG_NOSIGNAL);
        if (sent < 0) { // I believe should be only < and not <=
            DB(cout << "Error sending data\n");
            return false;
        }
        bytesSent += sent;
    }
    return true;
}
