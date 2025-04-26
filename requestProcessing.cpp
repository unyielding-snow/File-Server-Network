#include "FileServer.h"

vector<shared_mutex> blockMutex(FS_DISKSIZE);

bool FileServer::ownerCheck(const string &requestUsername, const char* currentInodeString) {
    return strcmp(currentInodeString, requestUsername.c_str()) == 0 || strlen(currentInodeString) == 0;
}

// Function used to traverse up to a given point
variant<bool, pair<uint32_t, fs_inode>> FileServer::searchPath(requestInfo &request, string &path) {
    // get a deque of the traversal inodes (minus the root)
    deque<string> pathDeque = getFileNameQueue(path);

    // 1. Setup initial node
    uint32_t currentBlock = 0, nextBlock = 0; // start at root
    fs_inode currentInode;
    
    if(path == "/" && (request.type == FS_DELETE || request.type == FS_CREATE)) {
        // Edge Case: If request path == / 
        blockMutex[currentBlock].lock();
    }
    else {
        // Reader Lock for reading first inode by default
        blockMutex[currentBlock].lock_shared();
    }

    disk_readblock(currentBlock, &currentInode);

    DB(cout << "size of queue is: " << pathDeque.size() << '\n');
    DB(cout << "path is: " << path << '\n');

    // 2. Traverse through each inode in pathDeque
    while (!pathDeque.empty()) {
        string currentSection = pathDeque.front(); // get next inode
        pathDeque.pop_front(); // pop inode
    
        // 3. Error checking for each inode in traversal
        if (currentSection.size() > FS_MAXFILENAME) {
            DB(cout << "Error Check unlock_shared fs_maxfilename, block: " << currentBlock << "\n");
            blockMutex[currentBlock].unlock_shared();
            return false;
        }
        if (currentInode.type != 'd') {
            DB(cout << "Error Check unlock_shared != d, block: " << currentBlock << "\n");
            blockMutex[currentBlock].unlock_shared();
            return false;
        }
        if (!ownerCheck(request.username, currentInode.owner)) {
            DB(cout << "Error Check unlock_shared !owner check, block: " << currentBlock << "\n");
            blockMutex[currentBlock].unlock_shared();
            return false;
        }

        bool found = false;

        // 4. For current inode (directory)
        for (uint32_t i = 0; i < currentInode.size && !found; i++) {
            char buffer[FS_BLOCKSIZE];
            uint32_t directoryBlock = currentInode.blocks[i];

            // reading inode's block to get direntries
            disk_readblock(directoryBlock, buffer);
            fs_direntry* entries = reinterpret_cast<fs_direntry*>(buffer);

            // 5. Go over each direntry
            for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {

                // 6. Check if block used and name is same, then jump into next traversal
                if (entries[j].inode_block != 0 && strcmp(entries[j].name, currentSection.c_str()) == 0) { // if found
                    nextBlock = entries[j].inode_block;
                        
                    DB(if (nextBlock >= FS_DISKSIZE) { cout << "shouldn't of gotten here" << "\n"; });
                
                    // 7. Hand-Over-Hand Locking Below

                    // 7a. grab reader or writer lock according to which operation & if final destination or not
                    if (pathDeque.empty() && (request.type == FS_WRITEBLOCK || request.type == FS_CREATE || request.type == FS_DELETE)) {
                        DB(cout << "Traversal End Node: Unique Lock! " << currentBlock << '\n');
                        blockMutex[nextBlock].lock();
                    }
                    else {
                        DB(cout << "Traversal End NOde: Lock Shared " << currentBlock << '\n');
                        blockMutex[nextBlock].lock_shared();
                    }

                    DB(cout << "Traversal Lock Shared " << nextBlock << '\n');
                    
                    // 7b. unlock previously locked inode
                    blockMutex[currentBlock].unlock_shared(); // switched this to before the read

                    DB(cout << "Traversal Unlock Shared " << currentBlock << '\n');

                    disk_readblock(nextBlock, &currentInode); // reading next inode

                    // 8. Set current node to next node
                    currentBlock = nextBlock;
                    found = true;
                    break;
                }
            }

            DB(cout << "the current section is: " << currentSection << '\n');
        }
        if (!found){ // 9. if you couldn't find your next inode, then return failure
            blockMutex[currentBlock].unlock_shared();
            return false;
        }
    }

    // 10. Return current block and inode if found destination
    return make_pair(currentBlock, currentInode);
}

bool FileServer::process_FS_READBLOCK(requestInfo &request) {
    // 1. Traverse path to find the inode for file. 
    auto result = searchPath(request, request.pathname);
    if (holds_alternative<bool>(result)) return false; // propogates error if occurs
    auto [fileBlock, fileInode] = get<pair<uint32_t, fs_inode>>(result);

    // 2. Setup unlock guard for when we want to unlock parent
    UnlockGuard guard(blockMutex[fileBlock], READER);

    // 3. Error Check: Validate inode
    if (!checkReadWriteInProcessing(request, request.block, fileInode)) return false;
    
    // 4. Get the target block number for the file block to read actual file data and error check it
    uint32_t dataBlockID = fileInode.blocks[request.block];
    if (dataBlockID >= FS_DISKSIZE) return false; // ensure valid directory block

    DB(cout << "request.block is: " << request.block << "fileInode.size is: " << fileInode.size << '\n');
    
    // 5. Read block data with lock
    char buffer[FS_BLOCKSIZE];
    disk_readblock(dataBlockID, buffer);

    // 6. Save data for response message
    memcpy(request.data.data(), buffer, FS_BLOCKSIZE);
    
    // 7. Return success
    return true;
}

// IMPORTANT: Writer lock the file
bool FileServer::process_FS_WRITEBLOCK(requestInfo &request) {
    // 1. Traverse to find Inode
    auto result = searchPath(request, request.pathname);
    if (holds_alternative<bool>(result)) return false; // propogates error if occurs
    auto [fileBlock, fileInode] = get<pair<uint32_t, fs_inode>>(result);
    
    // 2. Setup unlock guard for when we want to unlock parent
    UnlockGuard guard(blockMutex[fileBlock], WRITER);
    
    // 3. Error Check: Validate inode
    if (!checkReadWriteInProcessing(request, request.block, fileInode)) return false;

    DB(cout << "request.block is: " << request.block << ", fileInode.size is: " << fileInode.size << '\n');


    // 4. Determine if we need to append or overwrite
    if (static_cast<uint32_t>(request.block) == fileInode.size) { // would be append
        // 5. Allocate new block
        uint32_t newBlock = allocateFileDiskBlock();

        // 6. Error check: No Availible space on disk
        if (newBlock == 0) return false;

        // 7. Write the data first
        disk_writeblock(newBlock, request.data.data()); // to write new data block to disk
        
        // 8. Update and write the inode updates
        fileInode.blocks[fileInode.size] = newBlock;
        fileInode.size++;
        disk_writeblock(fileBlock, &fileInode);

        // 9. Return success in append case
        return true;
    }
    else if (static_cast<uint32_t>(request.block) < fileInode.size) { // overwrite case
        // 10. Grab target block
        uint32_t targetBlock = fileInode.blocks[request.block];
        
        // 11. Error check target block if out of bounds for disk
        if (targetBlock >= FS_DISKSIZE) return false; // ensure valid directory block

        // 12. Overwrite with the new data
        disk_writeblock(targetBlock, request.data.data()); // to write target block to disk

        // 13. Return success in overwrite case
        return true;
    }
    else {
        // 14. Return failure
        return false;
    }
}

// IMPORTANT: Writer lock the directory
bool FileServer::process_FS_CREATE(requestInfo &request) {
    // 1. Split the path into parent and child
    auto [parent, child] = findParentChildPaths(request.pathname);

    // 2. Error check if child doesn't exist
    if (child.empty()) return false;
    
    // 3. Traverse to get parent inode
    auto result = searchPath(request, parent);
    if (holds_alternative<bool>(result)) return false; // propogates error if occurs
    auto [parentDirBlock, parentDirInode] = get<pair<uint32_t, fs_inode>>(result);

    // 4. Setup unlock guard for when we want to unlock parent
    UnlockGuard guard(blockMutex[parentDirBlock], WRITER);

    DB(cout << "ParentDirBlock: " << parentDirBlock << '\n' << "Parent Inode: " << parentDirInode.type << ", owner: " << parentDirInode.owner << ", size: " << parentDirInode.size << '\n');
    
    
    // 5. Error Check: for final parent leafnode
    if (parentDirInode.size > FS_MAXFILEBLOCKS) return false;
    if (child.size() > FS_MAXFILENAME) return false;
    if (parentDirInode.type != 'd') return false;
    if (!ownerCheck(request.username, parentDirInode.owner)) return false;

    fs_direntry parentEntries[FS_DIRENTRIES];

    int foundBlockIndex = -1, foundEntryIndex = -1;
    bool shouldParentInodeWrite = true;
    
    // 6. Search parent directory for duplicate file and find free entry
    if (!searchParent(foundBlockIndex, foundEntryIndex, parentEntries, parentDirInode, nullptr, child, &shouldParentInodeWrite, request.type, parentDirBlock)) return false;
    
    // 7. Check if directory is full
    if (shouldParentInodeWrite && (parentDirInode.size + 1 > FS_MAXFILEBLOCKS)) return false;

    // 8. Allocate new inode block
    uint32_t newInodeBlock = allocateFileDiskBlock();
    
    // 9. If no disk space propogate error
    if (newInodeBlock == 0) return false;
    
    // 10. Allocate and initialize new inode block
    fs_inode newInode;
    newInode.type = request.fileType;
    newInode.size = 0; // for both file and directory cases
    memset(newInode.owner, 0, sizeof(newInode.owner));
    strncpy(newInode.owner, request.username.c_str(), FS_MAXUSERNAME);
    memset(newInode.blocks, 0, sizeof(newInode.blocks));

    // 11. write in new inode block
    disk_writeblock(newInodeBlock, &newInode);

    DB(cout << "FS_CREATE: new entry block: " << newInodeBlock << '\n');

    // 12. Decide between cases on whether we should write to parent
    if (shouldParentInodeWrite) {
        // 13. Allocate new block
        uint32_t newBlock = allocateFileDiskBlock();

        // 14. Error Check: if no space release previously allocated block and propogate error
        if (newBlock == 0) {
            releaseFileDiskBlock(newInodeBlock);
            DB(cout << "FS_CREATE: no new disk found for block in parent, releasing inode block: " << newInodeBlock << '\n');
            return false;
        }
        
        DB(cout << "FS_CREATE: new disk block in parent, the new disk block is: " << newBlock << '\n');

        // 15. Initialize new dir block entries with zeros
        fs_direntry entries[FS_DIRENTRIES];
        memset(entries, 0, sizeof(entries));

        // 16. Add new block to parent directory
        if (parentDirInode.size > FS_MAXFILEBLOCKS) return false; // added 

        parentDirInode.blocks[parentDirInode.size] = newBlock;
        foundBlockIndex = parentDirInode.size;
        foundEntryIndex = 0;
        parentDirInode.size++;
            
        // 17. Insert child into parent directory
        strncpy(entries[foundEntryIndex].name, child.c_str(), FS_MAXFILENAME);
        entries[foundEntryIndex].inode_block = newInodeBlock;

        // 18. Write change to disk
        disk_writeblock(newBlock, entries);
        
        // 19. Update parent on disk
        disk_writeblock(parentDirBlock, &parentDirInode);
    }
    else { // we already have a block to fill
        // 20. Insert child into parent directory
        strncpy(parentEntries[foundEntryIndex].name, child.c_str(), FS_MAXFILENAME);
        parentEntries[foundEntryIndex].inode_block = newInodeBlock;
        
        // 21. Write change to disk
        disk_writeblock(parentDirInode.blocks[foundBlockIndex], parentEntries);
    }

    // 22. Return success
    return true;
}

bool FileServer::process_FS_DELETE(requestInfo &request) {
    // 1. Split the path into parent and child
    auto [parent, child] = findParentChildPaths(request.pathname);

    // 2. Error check if child doesn't exist
    if (child.empty()) return false;

    // 3. Traverse to get parent inode
    auto searchResult = searchPath(request, parent);
    if (holds_alternative<bool>(searchResult)) return false; // propogates error if occurs
    auto [parentDirBlock, parentDirInode] = get<pair<uint32_t, fs_inode>>(searchResult);
    
    uint32_t deleteInodeBlock = 0;
    fs_inode deleteInode;

    // 4. Error check: verify parent inode.
    if (parentDirInode.size > FS_MAXFILEBLOCKS) return false;

    if (child.size() > FS_MAXFILENAME) {
        blockMutex[parentDirBlock].unlock();
        return false;
    }
    if (parentDirInode.type != 'd') {
        blockMutex[parentDirBlock].unlock();
        return false;
    }
    if (!ownerCheck(request.username, parentDirInode.owner)){
        blockMutex[parentDirBlock].unlock();
        return false;
    }

    fs_direntry parentEntries[FS_DIRENTRIES];
    int foundBlockIndex = -1, foundEntryIndex = -1;
    
    // 5. Search through the parent's directory blocks for the target entry, error if can't find
    if (!searchParent(foundBlockIndex, foundEntryIndex, parentEntries, parentDirInode, &deleteInodeBlock, child, nullptr, request.type, parentDirBlock)) {
        blockMutex[parentDirBlock].unlock();
        return false;
    }

    // 6. lock the child inode that we are trying to delete
    blockMutex[deleteInodeBlock].lock();
    
    disk_readblock(deleteInodeBlock, &deleteInode);
    
    // 7. Error Check: Is owner of entry
    if (!ownerCheck(request.username, deleteInode.owner)) {
        blockMutex[parentDirBlock].unlock();
        blockMutex[deleteInodeBlock].unlock();
        return false;
    }

    // 8. Error Check: If the target is a directory, ensure it's empty.
    if (deleteInode.type == 'd') { // if directory
        if(deleteInode.size > 0) {
            DB(cout << "ERROR: Cannot delete non-empty directory" << "\n");
            blockMutex[parentDirBlock].unlock();
            blockMutex[deleteInodeBlock].unlock();
            return false;
        }
    }

    // 9. Get the number of entries in the block that the child we're trying to delete is in
    uint32_t numEntriesInBlock = getNumEntriesInBlock(parentEntries);
    DB(cout << "NumEntriesinBlock for the block we're deleting is: " << numEntriesInBlock << '\n');
    
    // 10. Differ approach based on the number of entries currently in that block
    if (numEntriesInBlock == 1) {
        // 11. Grab the empty block
        uint32_t emptyBlock = parentDirInode.blocks[foundBlockIndex];
        
        // 12. Shift blocks accordingly and decrement the parent's size
        shiftBlocksInInode(parentDirInode, foundBlockIndex);
        parentDirInode.size--;
        
        // 13. Update the parent
        disk_writeblock(parentDirBlock, &parentDirInode);
        
        // 14. Unlock the parent asap after we write to parent
        blockMutex[parentDirBlock].unlock();

        // 15. Release newly empty block
        releaseFileDiskBlock(emptyBlock);

        DB(cout << emptyBlock << ' ' << parentDirBlock << "\n"); 
    }
    else if (numEntriesInBlock > 1) {
        // 16. Grab the block
        uint32_t entriesBlock = parentDirInode.blocks[foundBlockIndex];

        // 17. Mark the entry in directory as unused
        parentEntries[foundEntryIndex].inode_block = 0;  // Remove file, then remove connection (crash consistent)
        memset(parentEntries[foundEntryIndex].name, 0, FS_MAXFILENAME);
        disk_writeblock(entriesBlock, parentEntries);

        // 18. Unlock the parent asap after we write to parent
        blockMutex[parentDirBlock].unlock();

        DB(cout << entriesBlock << ' ' << foundEntryIndex << "\n"); 
    }
    else {
        // 19. Else failure and we must unlock parent still
        blockMutex[parentDirBlock].unlock();
        DB(cout << "NumEntriesInBlock = 0, should probably never be the case\n");
    }
    
    // 20. After unlocking parent we must immediately unlock child 
    blockMutex[deleteInodeBlock].unlock();

    // 21. If what we released was a file, release all file's child blocks
    if (deleteInode.type == 'f') { // if deleting a file, then free its blocks from disk
        freeDataBlocks(deleteInode);
    }

    // 22. Release target inode block
    releaseFileDiskBlock(deleteInodeBlock);

    // 23. Return success
    return true;
}

// Will search parent for FS_CREATE or FS_DELETE
bool FileServer::searchParent(int &foundBlockIndex, int &foundEntryIndex, fs_direntry (&parentEntries)[FS_DIRENTRIES], fs_inode &parentDirInode, uint32_t* deleteInodeBlock, string &child, bool* shouldParentInodeWrite, const queryType &requestType, uint32_t &parentDirBlock) {
    bool found = false;

    // 1. Iterate over children
    for (uint32_t i = 0; i < parentDirInode.size && !found; i++) {
        char buffer[FS_BLOCKSIZE];
        uint32_t directoryBlock = parentDirInode.blocks[i];
        
        DB(if (directoryBlock >= FS_DISKSIZE) { cout << "ERROR: directoryBlock >= FS_DISKSIZE" << "\n"; });

        // 2. Read in buffer
        disk_readblock(directoryBlock, buffer);
        fs_direntry* tempParentEntries = reinterpret_cast<fs_direntry*>(buffer);
        
        // 3. Iterate over each direntry
        for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {
            // 4. If FS_DELETE or FS_CREATE
            if (requestType == FS_DELETE) { // for FS_DELETE
                // 5. If match for FS_DELETE
                if (tempParentEntries[j].inode_block != 0 && strcmp(tempParentEntries[j].name, child.c_str()) == 0) {
                    found = true;
                    *deleteInodeBlock = tempParentEntries[j].inode_block;
                    
                    DB(if (*deleteInodeBlock >= FS_DISKSIZE) { cout <<  "ERROR: deleteInodeBlock >= FS_DISKSIZE" << "\n"; });

                    foundBlockIndex = i;
                    foundEntryIndex = j;
                    memcpy(parentEntries, buffer, FS_BLOCKSIZE);
                    break;
                }
            }
            if (requestType == FS_CREATE) { // for FS_CREATE
                // 6. If match for FS_CREATE return error response (no duplicates)
                if (tempParentEntries[j].inode_block != 0 && strcmp(tempParentEntries[j].name, child.c_str()) == 0) return false;
                
                // 7. Get first free entry index
                if (*shouldParentInodeWrite && tempParentEntries[j].inode_block == 0) { // found entry
                    foundBlockIndex = i;
                    foundEntryIndex = j;
                    *shouldParentInodeWrite = false;
                    memcpy(parentEntries, buffer, FS_BLOCKSIZE);
                }
            }
        }
    }

    // 8. Propogate final return for FS_DELETE and FS_DELETE respectively
    return (requestType == FS_DELETE) ? found : true;
}

// function to free any file's children disk blocks
void FileServer::freeDataBlocks(fs_inode &deleteInode) {
    for (uint32_t i = 0; i < deleteInode.size; i++) {
        releaseFileDiskBlock(deleteInode.blocks[i]);
    }
}

// Grab the number of entries in a given block
uint32_t FileServer::getNumEntriesInBlock(fs_direntry (&parentEntries)[FS_DIRENTRIES]) {
    uint32_t numEntriesInBlock = 0;
    for (uint32_t j = 0; j < FS_DIRENTRIES; j++) {
        if (parentEntries[j].inode_block != 0) {
            numEntriesInBlock++;
        }
    }
    return numEntriesInBlock;
}

// shift by 1 leftwards all nodes that are right of a given block
void FileServer::shiftBlocksInInode(fs_inode &Inode, int &shiftPoint) {
    for (uint32_t i = static_cast<uint32_t>(shiftPoint); i < Inode.size - 1; i++) {
        Inode.blocks[i] = Inode.blocks[i + 1];
    }
}


bool FileServer::checkReadWriteInProcessing(requestInfo &request, uint32_t blockIndex, fs_inode &inode) {
    if (inode.type != 'f') return false;
    if (!ownerCheck(request.username, inode.owner)) return false;
    
    if (request.type == FS_WRITEBLOCK && blockIndex > inode.size) return false;
    else if (request.type == FS_READBLOCK && blockIndex >= inode.size) return false;

    return true;
}
