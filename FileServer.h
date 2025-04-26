#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include "fs_server.h"
#include "fs_client.h"
#include "fs_param.h"
#include <string>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <deque>
#include <variant>
#include <utility>
#include <stdexcept>
#include "socketGuard.h"
#include "unlockGuard.h"

using std::string;
using std::istringstream;
using std::stringstream;
using std::unordered_set;
using std::unordered_map;
using std::queue;
using boost::mutex;
using boost::lock_guard;
using boost::thread;
using boost::unique_lock;
using boost::shared_lock;
using boost::shared_mutex;
using std::to_string;
using std::deque;
using std::pair;
using std::variant;
using std::make_pair;
using std::holds_alternative;
using std::get;
using std::cout;
using std::vector;
using boost::regex;
using std::stoul;

// Globals (grab from requestProcessing.cpp)
extern vector<shared_mutex> blockMutex;

/*
Here is where we define our debug macro
*/
#ifdef DEBUG
    #define DB(x) \
        do { \
            unique_lock<mutex> _dbg_lock(cout_lock); \
            x; \
        } while (false)
#else
    #define DB(x)  do { } while (false)
#endif

// Here is where we define the maximum bytes any request can receive
#define MaxRequestBytes FS_MAXUSERNAME + FS_MAXPATHNAME + 20 // 3 is the maximum block number length, 3 whitespace, 1 null terminator: 3+3+1=7

// an enum to help us determine what kind of request we've received
enum queryType {
    FS_READBLOCK,
    FS_WRITEBLOCK,
    FS_CREATE,
    FS_DELETE,
    INVALID
};

// an enum to help the server initialization know if we need to close the socket
enum serverReturn {
    CLOSE,
    NOCLOSE
};

// a struct to hold all the associated request information
struct requestInfo {
    string message;
    queryType type = INVALID;
    string username = "";
    string pathname = "";
    vector<char> data;
    int64_t block = -1; // will overflow
    char fileType;

    requestInfo() : data(FS_BLOCKSIZE) {}

    // Copy Constructor
    requestInfo(const requestInfo& other) : 
        message(other.message),
        type(other.type),
        username(other.username),
        pathname(other.pathname),
        data(other.data),
        block(other.block),
        fileType(other.fileType)
    {}
};

class FileServer {
public:
    // Socket Functions    
    serverReturn initializeServerSocket();
    void handleRequests();
    void processClient(int clientSocket);
    void processClientRequest(int clientSocket, requestInfo& request);
    requestInfo parseClientRequest(const string& message);
    bool sendEvenPartial(int socket, const void* buffer, size_t length);

    // Request Processing Functions
    bool process_FS_READBLOCK(requestInfo& request);
    bool process_FS_WRITEBLOCK(requestInfo& request);
    bool process_FS_CREATE(requestInfo& request);
    bool process_FS_DELETE(requestInfo& request);
    variant<bool, pair<uint32_t, fs_inode>> searchPath(requestInfo &request, string &path);
    bool ownerCheck(const string &requestUsername, const char* currentInodeString);
    bool searchParent(int &foundBlockIndex, int &foundEntryIndex, fs_direntry (&parentEntries)[FS_DIRENTRIES], fs_inode &parentDirInode, uint32_t* deleteInodeBlock, string &child, bool* shouldParentInodeWrite, const queryType &requestType, uint32_t &parentDirBlock);
    void freeDataBlocks(fs_inode &deleteNode);
    uint32_t getNumEntriesInBlock(fs_direntry (&parentEntries)[FS_DIRENTRIES]);
    void shiftBlocksInInode(fs_inode &Inode, int &shiftPoint);
    bool checkReadWriteInProcessing(requestInfo &request, uint32_t blockIndex, fs_inode &inode);
    
    // File System Functions
    void initializeFileSystem();
    uint32_t allocateFileDiskBlock();
    void releaseFileDiskBlock(uint32_t diskBlockNumber);
    void loadFileSystem(uint32_t inodeBlock);
    pair<string, string> findParentChildPaths(const string& pathname);
    deque<string> getFileNameQueue(const string &path);
    
    // Request Validation Functions
    bool isRequestValid(const requestInfo& request);
    bool isPathnameValid(const string& pathname);
    bool isBlockOffsetValid(const int64_t& offset);
    bool isUsernameValid(const string& username);

    // Server Variables
    uint16_t port = 0;
    int serverSocket = -1;

    // Lock Variables

    mutex freeDiskBlocksMutex;
    // vector<mutexWrapper> blockMutex(FS_DISKSIZE);

    // File System Variables
    vector<bool> usedDiskBlocks;
    queue<uint32_t> freeDiskBlocks;

    // Constructor
    FileServer(int portVal) : port(portVal) {
        usedDiskBlocks.resize(FS_DISKSIZE, false); // initializes all blocks to being unused
        
        initializeFileSystem(); // to setup pre-existing file system
        
        if (initializeServerSocket() == CLOSE) close(serverSocket); // in case of server failure
    }
};
