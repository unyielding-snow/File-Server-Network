#include "FileServer.h"

void FileServer::initializeFileSystem() {
    // 1. Load existing file system (recursion from block 0 ("/"))
    loadFileSystem(0);
    
    // 2. Goes through all possible blocks and adds them to what blocks are free if they are unused
    for(uint32_t i = 1; i < FS_DISKSIZE; i++) {
        if (!usedDiskBlocks[i]) {
            freeDiskBlocks.push(i);
        }
    }
}

void FileServer::loadFileSystem(uint32_t inodeBlock) {
    DB(cout << "Search inode" << '\n');

    usedDiskBlocks[inodeBlock] = true; // sets called node to being used

    fs_inode inode;
    disk_readblock(inodeBlock, &inode); // fills the called inode

    for (uint32_t i = 0; i < inode.size; i++) { // iterates through child blocks
        uint32_t currBlock = inode.blocks[i];

        usedDiskBlocks[currBlock] = true; // sets child block to used

        if (inode.type == 'd') { // if directory, then there are more blocks inside
            fs_direntry entryArr[FS_BLOCKSIZE];
            disk_readblock(currBlock, entryArr); // read from disk into buffer

            size_t numEntries = FS_BLOCKSIZE / sizeof(fs_direntry); // gets # of entries
            for (size_t j = 0; j < numEntries; j++) {
                if (entryArr[j].inode_block != 0) { // if used
                    // recursively call the loadFileSystem function to continue the search down the "tree"
                    loadFileSystem(entryArr[j].inode_block);
                }
            }
        }
    }
}

// used to allocate disk space for when you need a disk block
uint32_t FileServer::allocateFileDiskBlock() {
    lock_guard<mutex> lock(freeDiskBlocksMutex); // creates safety for disk block critical section
    
    if (freeDiskBlocks.empty()) return 0; // if there are no disk blocks, then we error

    uint32_t freeBlock = freeDiskBlocks.front(); // grab the first free disk block we see with O(1)
    freeDiskBlocks.pop();
    usedDiskBlocks[freeBlock] = true; // sets the newly used block to being used
    
    DB(cout << "allocating block: " << freeBlock << "\n");

    return freeBlock;
}

void FileServer::releaseFileDiskBlock(uint32_t diskBlockNumber) {
    lock_guard<mutex> lock(freeDiskBlocksMutex); // creates safety for disk block critical section

    freeDiskBlocks.push(diskBlockNumber); // the disk block is no longer used, so we add it back to the free blocks
    usedDiskBlocks[diskBlockNumber] = false; // set block to being unused

    DB(cout << "we released: " << diskBlockNumber << "\n");
}

// Function used to split path by '/' (ignores the root)
deque<string> FileServer::getFileNameQueue(const string& path) {
    deque<string> output;
    istringstream ss(path);
    string file;
    
    while (getline(ss, file, '/')) { // splits by '/'
        if (!file.empty()) {
            output.push_back(file);
        }
    }
    
    return output;
}

// Used to split path by parent and child respectively
pair<string, string> FileServer::findParentChildPaths(const string& pathname) {
    if (pathname.empty()){
        DB(cout << "ERROR: Parent and child paths are empty\n");
        return {"", ""}; // error
    }
    if (pathname == "/"){
        DB(cout << "ERROR: Parent and child paths are empty\n");
        return {"/", ""}; // error
    }

    size_t lastSlashPosition = pathname.find_last_of('/');
    if (lastSlashPosition == string::npos){
        DB(cout << "ERROR: Parent and child paths are empty\n");
        return {"", ""}; // error
    }

    string parent = pathname.substr(0, lastSlashPosition);
    string child = pathname.substr(lastSlashPosition + 1);

    if (parent.empty()) // bring back "/" if it is the parent
        parent = "/";

    DB(cout << "VALID: Parent and child paths are non-empty\n");
    return {parent, child};
}
