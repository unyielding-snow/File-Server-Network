// Class used to close a socket when going out of scope (RAII)

class SocketGuard {
public:
    SocketGuard(int socketVal) : clientSocketNum(socketVal) {}
    ~SocketGuard() {
        close(clientSocketNum);
    }
private:
    int clientSocketNum;
};
