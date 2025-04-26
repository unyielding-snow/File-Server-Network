// OH Answer: To lock, create Block Number to Lock Map


// see lab :
//      - in listen, you already have a queue of requests to process.
//      - in accept, it returns you a new socket, and you have to pass it on to another thread. 


// Start Code for the server:
/* 
#include <iostream>

static const size_t MAX_MESSAGE_SIZE = 256;

int main(int argc, const char **argv) {
    // Parse command line arguments
    if (argc != 2) {
        std::cout << "Usage: ./server <port>\n";
        return(-1);
    }
    int port = atoi(argv[1]);

    // Create socket for accepting connections.  Use socket().

    // Set the "reuse port" socket option.  Use setsockopt().

    // Configure a sockaddr_in for the accepting socket.

    // Bind to the port.  Use bind().

    // Begin listening for incoming connections.  Use listen().

    // Serve incoming connections one by one forever.
    while (1) {
        // Accept connection from client.  Use accept().

        // Receive message from client.  Use recv().

        // Print message from client.

        // Close connection.  Use close().
    }

    return(0);
}
*/