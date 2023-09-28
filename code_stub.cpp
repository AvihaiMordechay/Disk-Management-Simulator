#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#define DISK_SIZE 512

// Function to convert decimal to binary char
char decToBinary(int n) {
    return static_cast<char>(n);
}

// Function to convert binary char to decimal.
int binaryToDec(char c) {
    return static_cast<int>(c);
}

// ============================================================================

class fsInode {
    int fileSize;
    int blockInUseIncludePointers;
    int blockInUseWithoutPointers;

    int internalDoubleBlock;

    int indexSingleBlock{};
    int indexExternalDoubleBlock{};
    int indexInternalDoubleBlock{};


    int directBlock1;
    int directBlock2;
    int directBlock3;


    int singleInDirect;
    int doubleInDirect;
    int blockSize;
    int remainingPlacesInLastBlock;
    bool fileDeleted;


public:
    fsInode(int blockSize) {
        this->fileDeleted = false;
        this->blockSize = blockSize;
        this->fileSize = 0;

        this->blockInUseIncludePointers = 0;
        this->blockInUseWithoutPointers = 0;
        this->remainingPlacesInLastBlock = 0;

        this->directBlock1 = -1;
        this->directBlock2 = -1;
        this->directBlock3 = -1;
        this->singleInDirect = -1;
        this->doubleInDirect = -1;
        this->internalDoubleBlock = -1;


    }

    // ------------------------------------------------------------------------

    void setIndexSingleBlock(int index) {
        this->indexSingleBlock = index;
    }

    // ------------------------------------------------------------------------

    int getIndexSingleBlock() const {
        return this->indexSingleBlock;
    }

    // ------------------------------------------------------------------------

    void setInternalDoubleBlock(int block) {
        this->internalDoubleBlock = block;
    }

    // ------------------------------------------------------------------------

    int getInternalDoubleBlock() const {
        return this->internalDoubleBlock;
    }

    // ------------------------------------------------------------------------

    void setIndexInternalDoubleBlock(int index) {
        this->indexInternalDoubleBlock = index;
    }

    // ------------------------------------------------------------------------

    int getIndexInternalDoubleBlock() const {
        return this->indexInternalDoubleBlock;
    }

    // ------------------------------------------------------------------------

    int getBlockInUseIncludePointers() const {
        return this->blockInUseIncludePointers;
    }

    // ------------------------------------------------------------------------

    void setBlockInUseIncludePointers(int blockInUse) {
        this->blockInUseIncludePointers = blockInUse;
    }

    // ------------------------------------------------------------------------

    int getBlockInUseWithoutPointers() const {
        return this->blockInUseWithoutPointers;
    }

    // ------------------------------------------------------------------------

    void setBlockInUseWithoutPointers(int blockInUse) {
        this->blockInUseWithoutPointers = blockInUse;
    }

    // ------------------------------------------------------------------------

    void setIndexExternalDoubleBlock(int index) {
        this->indexExternalDoubleBlock = index;
    }

    // ------------------------------------------------------------------------

    int getIndexExternalDoubleBlock() const {
        return this->indexExternalDoubleBlock;
    }

    // ------------------------------------------------------------------------

    int getDirectBlock1() const {
        return this->directBlock1;
    }

    // ------------------------------------------------------------------------

    void setDirectBlock1(int directBlock) {
        this->directBlock1 = directBlock;
    }

    // ------------------------------------------------------------------------

    int getDirectBlock2() const {
        return this->directBlock2;
    }

    // ------------------------------------------------------------------------

    void setDirectBlock2(int directBlock) {
        this->directBlock2 = directBlock;
    }

    // ------------------------------------------------------------------------

    int getDirectBlock3() const {
        return this->directBlock3;
    }

    // ------------------------------------------------------------------------

    void setDirectBlock3(int directBlock) {
        this->directBlock3 = directBlock;
    }

    // ------------------------------------------------------------------------

    int getSingleInDirect() const {
        return this->singleInDirect;
    }

    // ------------------------------------------------------------------------

    void setSingleInDirect(int block) {
        this->singleInDirect = block;
    }

    // ------------------------------------------------------------------------

    int getDoubleInDirect() const {
        return this->doubleInDirect;
    }

    // ------------------------------------------------------------------------

    void setDoubleInDirect(int block) {
        this->doubleInDirect = block;
    }

    // ------------------------------------------------------------------------

    int getFileSize() const {
        return this->fileSize;
    }

    // ------------------------------------------------------------------------

    void setFileSize(int size) {
        this->fileSize = size;
    }

    // ------------------------------------------------------------------------

    int getRemainingPlacesInLastBlock() const {
        return this->remainingPlacesInLastBlock;
    }

    // ------------------------------------------------------------------------

    void setRemainingPlacesInLastBlock(int num) {
        this->remainingPlacesInLastBlock = num;
    }

    // ------------------------------------------------------------------------

    int getLastBlockWritten() const {
        int theBlock = -1;
        if (directBlock1 != -1)
            theBlock = directBlock1;
        if (directBlock2 != -1)
            theBlock = directBlock2;
        if (directBlock3 != -1)
            theBlock = directBlock3;
        if (singleInDirect != -1)
            theBlock = singleInDirect;
        if (doubleInDirect != -1)
            theBlock = doubleInDirect;
        return theBlock;
    }

    bool isFileDeleted() const {
        return this->fileDeleted;
    }

    // ------------------------------------------------------------------------

    void setFileDeleted(bool fileDeleted) {
        this->fileDeleted = fileDeleted;
    }
};

// ============================================================================

class FileDescriptor {
    pair<string, fsInode *> file;
    bool inUse;

public:
    FileDescriptor(string FileName, fsInode *fsi) {
        file.first = std::move(FileName);
        file.second = fsi;
        inUse = true;
    }

    // ------------------------------------------------------------------------

    string getFileName() const {
        return file.first;
    }

    // ------------------------------------------------------------------------

    void setFileName(string str) {
        file.first = std::move(str);
    }

    // ------------------------------------------------------------------------

    fsInode *getInode() const {
        return file.second;
    }

    // ------------------------------------------------------------------------

    void setInode(fsInode *fsInode) {
        file.second = fsInode;
    }

    // ------------------------------------------------------------------------

    int getFileSize() const {
        return file.second->getFileSize();
    }

    // ------------------------------------------------------------------------

    bool isInUse() const {
        return inUse;
    }

    // ------------------------------------------------------------------------

    void setInUse(bool _inUse) {
        inUse = _inUse;
    }
};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

// ============================================================================
class fsDisk {
    FILE *sim_disk_fd;

    bool is_formated;

    // bitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if bitVector[0] == 1 , means that the
    //             first block is occupied.
    int bitVectorSize;

    int *bitVector;

    int sizeOfBlock;

    // Unix directories are lists of association structures,
    // each of which contains one filename and one inode number.
    map<string, fsInode *> mainDir;

    // openFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    vector<FileDescriptor> openFileDescriptors;

    // deletedFiles -- when we delete file we will add to this vector and when
    // we delete the disk we will pass on this vector and release the elements.
    vector<fsInode *> deletedFiles;


public:

    fsDisk() {
        is_formated = false;
        sim_disk_fd = fopen(DISK_SIM_FILE, "w+");
        assert(sim_disk_fd);
        for (int i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = (int) fwrite("\0", 1, 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);

    }

    // ------------------------------------------------------------------------

    // The function prints the contents of the disk.
    void listAll() {
        int i = 0;
        for (auto it = begin(openFileDescriptors); it != end(openFileDescriptors); ++it) {
            cout << "index: " << i << ": FileName: " << it->getFileName() << " , isInUse: "
                 << it->isInUse() << " file Size: " << it->getFileSize() << endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = (int) fread(&bufy, 1, 1, sim_disk_fd);
            cout << bufy;
        }
        cout << "'" << endl;


    }

    // ------------------------------------------------------------------------

    // The function formats the disk according to block size.
    void fsFormat(int blockSize = 4) {
        if (blockSize <= 0 || log2(blockSize) != floor(log2(blockSize))) {
            cout << "ERR" << endl;
            return;
        } else {
            if (is_formated) {
                for (int i = 0; i < openFileDescriptors.size(); i++)
                    if (openFileDescriptors[i].isInUse()) {
                        closeFile(i);
                    }
                auto it = mainDir.begin();
                int i = 0;
                while (it != mainDir.end()) {
                    delFile(it->first);
                    it = mainDir.begin();
                    i++;
                }
                this->mainDir.clear();
                this->openFileDescriptors.clear();
                delete[] bitVector;
            }
            this->sizeOfBlock = blockSize;
            // Initialize the bitVector
            this->bitVectorSize = DISK_SIZE / blockSize;
            this->bitVector = new int[this->bitVectorSize];
            for (int i = 0; i < this->bitVectorSize; i++) {
                this->bitVector[i] = 0;
            }
        }

        this->is_formated = true;

    }

    // ------------------------------------------------------------------------

    // The function creates a file on disk.
    int CreateFile(const string &fileName) {
        if (!is_formated) {
            cout << "ERR" << endl;
            return -1;
        }
        if (findFsInodeInMainDir(fileName) != nullptr && !findFsInodeInMainDir(fileName)->isFileDeleted()) {
            cout << "ERR" << endl;
            return -1;
        }
        // Create inode and add it to mainDir.
        auto *inode = new fsInode(this->sizeOfBlock);
        this->mainDir[fileName] = inode;
        // Search file deleted or not in use.
        for (int i = 0; i < this->openFileDescriptors.size(); i++) {
            if (this->openFileDescriptors[i].getInode()->isFileDeleted()) {
                this->openFileDescriptors[i].getInode()->setFileDeleted(false);
                this->openFileDescriptors[i].setInUse(true);
                this->openFileDescriptors[i].setInode(inode);
                this->openFileDescriptors[i].setFileName(fileName);
                return i;
            } else if (this->openFileDescriptors[i].isInUse() == 0) {
                this->openFileDescriptors[i].setInUse(true);
                this->openFileDescriptors[i].setInode(inode);
                this->openFileDescriptors[i].setFileName(fileName);
                return i;
            }
        }
        // If we did not find a file that was deleted or closed we will add the nwq
        this->openFileDescriptors.emplace_back(fileName, inode);
        return (int) this->openFileDescriptors.size() - 1;

    }

    // ------------------------------------------------------------------------

    // The function opens a file on disk.
    int openFile(const string &FileName) {
        if (is_formated) {
            fsInode *fsInode = findFsInodeInMainDir(FileName);
            if (fsInode == nullptr) {
                cout << "ERR" << endl;
                return -1;
            } else {
                // Find the file in openFileDescriptors.
                for (auto &file: this->openFileDescriptors) {
                    if (file.getFileName() == FileName) {
                        if (file.isInUse()) {
                            cout << "ERR" << endl;
                            return -1;
                        }
                    }
                }
                // Find file that not in use.
                for (int i = 0; i < this->openFileDescriptors.size(); i++) {
                    if (!this->openFileDescriptors[i].isInUse()) {
                        this->openFileDescriptors[i].setInUse(true);
                        this->openFileDescriptors[i].setInode(fsInode);
                        this->openFileDescriptors[i].setFileName(FileName);
                        return i;
                    }

                }
                // If we did not find the file in openFileDescriptors or find file that not in use
                // we will add it to the vector.
                this->openFileDescriptors.emplace_back(FileName, fsInode);
                return (int) this->openFileDescriptors.size() - 1;
            }
        } else {
            cout << "ERR" << endl;
            return -1;
        }
    }

    // ------------------------------------------------------------------------

    // The function closes a file on disk.
    string closeFile(int fd) {
        if (fd < 0 || fd >= this->openFileDescriptors.size() ||
            this->openFileDescriptors[fd].getInode()->isFileDeleted() ||
            !this->openFileDescriptors[fd].isInUse() ||
            !is_formated) {
            cout << "ERR" << endl;
            return "-1";
        } else {
            this->openFileDescriptors[fd].setInUse(false);
            return this->openFileDescriptors[fd].getFileName();
        }
    }

    // ------------------------------------------------------------------------

    // The function writes to a file.
    int writeToFile(int fd, char *buf, int len) {
        if (len <= 0) {
            cout << "ERR: The number of characters you want to write is less than zero." << endl;
            return -1;
        } else if (!is_formated) {
            cout << "ERR: The disk is not formatted." << endl;
            return -1;
        } else if (fd < 0 || fd >= this->openFileDescriptors.size()) {
            cout << "ERR: The fd num you entered is incorrect." << endl;
            return -1;
        } else if (!this->openFileDescriptors[fd].isInUse()) {
            cout << "ERR: Unable to write to file that is closed." << endl;
            return -1;
        } else {
            fsInode *fsInode = this->openFileDescriptors[fd].getInode();
            if (fsInode->isFileDeleted()) {
                cout << "ERR" << endl;
                return -1;
            }
            if (!isThereFileFreeSpace(fd, len)) {
                len = (3 + sizeOfBlock + (sizeOfBlock * sizeOfBlock)) * sizeOfBlock - fsInode->getFileSize();
                if (len == 0) {
                    cout << "ERR: The file is full." << endl;
                    return -1;
                }
                buf[len] = '\0';
            }
            vector<int> freeBlocks = findFreeBlocksInDisk(fsInode, len);
            if (freeBlocks.empty()) {
                cout << "ERR: There is no space on the disk." << endl;
                return -1;
            } else { // There is space on the disk.
                char block[sizeOfBlock + 1];
                // Option One: If no written before OR written before and
                //             there is no space to write to the last block we wrote in the past.
                if (freeBlocks[0] == -1)
                    writeToFileOptionOne(fsInode, freeBlocks, block, buf);
                else  // Option Two: If written before AND There is space to write to the
                    // last block we wrote in the past.
                    writeToFileOptionTwo(fsInode, freeBlocks, block, buf, len);
                fsInode->setFileSize(fsInode->getFileSize() + len);
                int remainder = len - ((len / sizeOfBlock) * sizeOfBlock);
                if (fsInode->getRemainingPlacesInLastBlock() == 0 && remainder > 0)
                    fsInode->setRemainingPlacesInLastBlock(sizeOfBlock - remainder);
                else {
                    fsInode->setRemainingPlacesInLastBlock(fsInode->getRemainingPlacesInLastBlock() - remainder);
                    if (fsInode->getRemainingPlacesInLastBlock() < 0)
                        fsInode->setRemainingPlacesInLastBlock(fsInode->getRemainingPlacesInLastBlock() + sizeOfBlock);
                }
                return 1;
            }
        }
    }

    // ------------------------------------------------------------------------

    // The function delete a file from the disk.
    int delFile(const string &FileName) {
        if (!is_formated) {
            cout << "ERR" << endl;
            return -1;
        }
        fsInode *fsInode = findFsInodeInMainDir(FileName);
        if (fsInode == nullptr) {
            cout << "ERR" << endl;
            return -1; // The file not exist.
        } else {
            for (auto &file: this->openFileDescriptors) {
                if (file.getFileName() == FileName) {
                    if (file.isInUse()) {
                        cout << "ERR" << endl;
                        return -1;
                    } else {
                        file.setInUse(false);
                        file.setFileName("");
                        this->deletedFiles.emplace_back(file.getInode());
                    }
                }
            }
            deletesDateFromDisk(fsInode);
            fsInode->setFileSize(0);
            fsInode->setFileDeleted(true);
            this->mainDir.erase(FileName);
            return 1;
        }
    }

    // ------------------------------------------------------------------------

    // The function deletes the all chars from the disk include the chars of the pointers.
    void deletesDateFromDisk(fsInode *fsInode) {
        int size = static_cast<int>(ceil(static_cast<double>(fsInode->getFileSize()) / this->sizeOfBlock));
        int pointer;
        int indexSingle = 0;
        int indexDoubleExternal = 0;
        int indexDoubleInternal = 0;
        int pointerInternalBlock;
        char c;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                pointer = fsInode->getDirectBlock1();
            else if (i == 1)
                pointer = fsInode->getDirectBlock2();
            else if (i == 2)
                pointer = fsInode->getDirectBlock3();
            else if (i > 2 && i <= 2 + sizeOfBlock) {
                fseek(sim_disk_fd, fsInode->getSingleInDirect() * sizeOfBlock + indexSingle, SEEK_SET);
                c = (char) fgetc(sim_disk_fd);
                pointer = binaryToDec(c);
                indexSingle++;
            } else {
                if (indexDoubleInternal == sizeOfBlock) {
                    indexDoubleInternal = 0;
                    indexDoubleExternal++;
                }
                fseek(sim_disk_fd, fsInode->getDoubleInDirect() * sizeOfBlock + indexDoubleExternal, SEEK_SET);
                c = (char) fgetc(sim_disk_fd);
                pointerInternalBlock = binaryToDec(c);
                fseek(sim_disk_fd, pointerInternalBlock * sizeOfBlock + indexDoubleInternal, SEEK_SET);
                c = (char) fgetc(sim_disk_fd);
                pointer = binaryToDec(c);
                indexDoubleInternal++;
            }
            for (int j = 0; j < sizeOfBlock; j++) {
                fseek(sim_disk_fd, pointer * sizeOfBlock + j, SEEK_SET);
                fwrite("\0", 1, 1, sim_disk_fd);
            }
            this->bitVector[pointer] = 0;
        }
        if (fsInode->getSingleInDirect() != -1) {
            this->bitVector[fsInode->getSingleInDirect()] = 0;
            for (int i = 0; i < sizeOfBlock; i++) {
                fseek(sim_disk_fd, fsInode->getSingleInDirect() * sizeOfBlock + i, SEEK_SET);
                fwrite("\0", 1, 1, sim_disk_fd);
            }
        }
        if (fsInode->getDoubleInDirect() != -1) {
            for (int i = 0; i < fsInode->getIndexExternalDoubleBlock(); i++) {
                fseek(sim_disk_fd, fsInode->getDoubleInDirect() * sizeOfBlock + i, SEEK_SET);
                c = (char) fgetc(sim_disk_fd);
                pointer = binaryToDec(c);
                this->bitVector[pointer] = 0;
                fseek(sim_disk_fd, fsInode->getDoubleInDirect() * sizeOfBlock + i, SEEK_SET);
                fwrite("\0", 1, 1, sim_disk_fd);
                for (int j = 0; j < sizeOfBlock; j++) {
                    fseek(sim_disk_fd, pointer * sizeOfBlock + j, SEEK_SET);
                    fwrite("\0", 1, 1, sim_disk_fd);
                }
            }
            this->bitVector[fsInode->getDoubleInDirect()] = 0;
        }
    }

    // ------------------------------------------------------------------------

    // The function reads from a file.
    int readFromFile(int fd, char *buf, int len) {
        strcpy(buf, "\0");
        char str[DISK_SIZE];
        int n;
        if (len <= 0) {
            cout << "ERR" << endl;
            return -1;
        } else if (!is_formated) {
            cout << "ERR" << endl;
            return -1;
        } else if (fd < 0 || fd >= this->openFileDescriptors.size()) {
            cout << "ERR" << endl;
            return -1;
        } else if (!this->openFileDescriptors[fd].isInUse()) {
            cout << "ERR" << endl;
            return -1;
        } else {
            fsInode *fsInode = this->openFileDescriptors[fd].getInode();
            if (fsInode->isFileDeleted()) {
                cout << "ERR" << endl;
                return -1;
            }
            if (len > fsInode->getFileSize())
                len = fsInode->getFileSize();
            int numOfBlocks = static_cast<int>(ceil(static_cast<double>(len) / this->sizeOfBlock));
            int pointerToRead;
            int indexSingle = 0;
            int indexDoubleExternal = 0;
            int indexDoubleInternal = 0;
            int pointerInternalBlock;
            char c;
            for (int i = 0; i < numOfBlocks; i++) {
                if (i == 0)
                    pointerToRead = fsInode->getDirectBlock1();
                else if (i == 1)
                    pointerToRead = fsInode->getDirectBlock2();
                else if (i == 2)
                    pointerToRead = fsInode->getDirectBlock3();
                else if (i > 2 && i < 3 + sizeOfBlock) {
                    fseek(sim_disk_fd, fsInode->getSingleInDirect() * sizeOfBlock + indexSingle, SEEK_SET);
                    c = (char) fgetc(sim_disk_fd);
                    pointerToRead = binaryToDec(c);
                    indexSingle++;
                } else {
                    if (indexDoubleInternal == sizeOfBlock) {
                        indexDoubleInternal = 0;
                        indexDoubleExternal++;
                    }
                    if (indexDoubleExternal > sizeOfBlock)
                        return -1;
                    fseek(sim_disk_fd, fsInode->getDoubleInDirect() * sizeOfBlock + indexDoubleExternal, SEEK_SET);
                    c = (char) fgetc(sim_disk_fd);
                    pointerInternalBlock = binaryToDec(c);
                    fseek(sim_disk_fd, pointerInternalBlock * sizeOfBlock + indexDoubleInternal, SEEK_SET);
                    c = (char) fgetc(sim_disk_fd);
                    pointerToRead = binaryToDec(c);
                    indexDoubleInternal++;
                }
                if (i == numOfBlocks - 1 && len < sizeOfBlock)
                    n = len;
                else {
                    n = sizeOfBlock;
                    len -= sizeOfBlock;
                }
                fseek(sim_disk_fd, pointerToRead * sizeOfBlock, SEEK_SET);
                fread(str, sizeof(char), n, sim_disk_fd);
                str[n] = '\0';
                strcat(buf, str);
                strcat(buf, "\0");
            }
            return 1;
        }
    }

// ------------------------------------------------------------------------

    // The function copies an existing file to a new file that will be created and will be closed.
    int copyFile(const string &srcFileName, const string &destFileName) {
        if (!is_formated) {
            cout << "ERR" << endl;
            return -1;
        }

        fsInode *srcFsInode = findFsInodeInMainDir(srcFileName);
        if (srcFsInode == nullptr) {
            cout << "ERR" << endl;
            return -1;
        }
        char srcFileData[srcFsInode->getFileSize() + 1];
        for (auto &file: this->openFileDescriptors) {
            if (file.getFileName() == srcFileName && file.isInUse()) {
                cout << "ERR" << endl;
                return -1;
            }
        }
        int fdSrcFile = openFile(srcFileName);
        if (fdSrcFile == -1) {
            return -1;
        }
        if (readFromFile(fdSrcFile, srcFileData, srcFsInode->getFileSize()) == -1) {
            cout << "ERR" << endl;
            return -1;
        }
        if (closeFile(fdSrcFile) == "-1") {
            cout << "ERR" << endl;
            return -1;
        }
        int numOfFreeBlockInDisk = 0;
        for (int i = 0; i < bitVectorSize; i++) {
            if (bitVector[i] == 0) {
                numOfFreeBlockInDisk++;
            }
        }

        fsInode *destFsInode = findFsInodeInMainDir(destFileName);
        int fdDestFile;
        bool destIsCreated = false;
        if (destFsInode != nullptr) {
            fdDestFile = openFile(destFileName);
            if (fdDestFile == -1)
                return -1;
            auto *temp = new fsInode(sizeOfBlock);
            int numOfBlocks = calculateNumOfBlocks(temp, (int) strlen(srcFileData));
            delete temp;
            if (numOfBlocks > destFsInode->getBlockInUseIncludePointers() + numOfFreeBlockInDisk) {
                closeFile(fdDestFile);
                cout << "ERR" << endl;
                return -1;
            }
            deletesDateFromDisk(destFsInode);
            destFsInode->setFileSize(0);
            destFsInode->setBlockInUseIncludePointers(0);
            destFsInode->setBlockInUseWithoutPointers(0);
            destFsInode->setInternalDoubleBlock(-1);
            destFsInode->setIndexSingleBlock(0);
            destFsInode->setIndexExternalDoubleBlock(0);
            destFsInode->setIndexInternalDoubleBlock(0);
            destFsInode->setDirectBlock1(-1);
            destFsInode->setDirectBlock2(-1);
            destFsInode->setDirectBlock3(-1);
            destFsInode->setSingleInDirect(-1);
            destFsInode->setDoubleInDirect(-1);
            destFsInode->setRemainingPlacesInLastBlock(0);
        } else {
            fdDestFile = CreateFile(destFileName);
            destIsCreated = true;
            if (fdDestFile == -1) {
                return -1;
            }
            int numOfBlocks = calculateNumOfBlocks(this->openFileDescriptors[fdDestFile].getInode(),
                                                   (int) strlen(srcFileData));
            if (numOfBlocks > numOfFreeBlockInDisk) {
                closeFile(fdDestFile);
                cout << "ERR" << endl;
                return -1;
            }
        }
        if (writeToFile(fdDestFile, srcFileData, (int) strlen(srcFileData)) == -1) {
            closeFile(fdDestFile);
            if (destIsCreated)
                delFile(destFileName);
            return -1;
        }
        if (closeFile(fdDestFile) == "-1") {
            return -1;
        }
        return 1;
    }

// ------------------------------------------------------------------------

    // The function renames the file.
    int renameFile(const string &oldFileName, const string &newFileName) {
        if (!is_formated) {
            cout << "ERR" << endl;
            return -1;
        }
        if (findFsInodeInMainDir(newFileName) != nullptr) {
            cout << "ERR" << endl;
            return -1;
        }
        fsInode *fsInode = findFsInodeInMainDir(oldFileName);
        if (fsInode == nullptr) {
            cout << "ERR" << endl;
            return -1;
        } else {
            for (auto &file: this->openFileDescriptors) {
                if (file.getFileName() == oldFileName) {
                    if (file.isInUse()) {
                        cout << "ERR" << endl;
                        return -1;
                    } else {
                        file.setFileName(newFileName);
                        this->mainDir.erase(oldFileName);
                        this->mainDir[newFileName] = fsInode;
                        return 1;
                    }
                }
            }
            // If we not found the file we want to rename in openFileDescriptors.
            this->mainDir.erase(oldFileName);
            this->mainDir[newFileName] = fsInode;
            return 1;
        }
    }


// ------------------------------------------------------------------------

// The function searches for a file with the name it received as an argument.
    fsInode *findFsInodeInMainDir(const string &fileName) {
        for (const auto &file: this->mainDir) {
            if (file.first == fileName)
                return file.second;
        }
        return nullptr;
    }

// ------------------------------------------------------------------------

//The function looks for free blocks for the file you want to write to,
// first it checks if the file has been written before and if it is possible to write to the last block,
// then the function calculates how many additional blocks need to be allocated
// and returns the block numbers as an array.
    vector<int> findFreeBlocksInDisk(fsInode *fsInode, int len) {
        if (fsInode->getRemainingPlacesInLastBlock() > 0)
            len -= fsInode->getRemainingPlacesInLastBlock();
        if (len < 0)
            len = 0;
        int numOfBlocks = calculateNumOfBlocks(fsInode, len);
        // Check if there is space in the disk for whole the file, if not we will allocate as much as we can.
        int numOfFreeBlockInDisk = 0;
        for (int i = 0; i < bitVectorSize; i++) {
            if (bitVector[i] == 0) {
                numOfFreeBlockInDisk++;
            }
        }
        if (numOfBlocks > numOfFreeBlockInDisk)
            numOfBlocks = numOfFreeBlockInDisk;
        // +1 for a special location in the vector that will indicate if it is possible to write to the last block we
        // wrote in the past, if possible the block number will be written there if not -1 will be written.
        vector<int> blocksFree(numOfBlocks + 1);
        // if written before AND There is space to write to the last block we wrote in the past.
        if (fsInode->getFileSize() != 0 && fsInode->getRemainingPlacesInLastBlock() > 0)
            blocksFree[0] = fsInode->getLastBlockWritten();
        else
            blocksFree[0] = -1;


        int j = 0;
        for (int i = 1; i < blocksFree.size(); i++) {
            for (; j < bitVectorSize; j++) {
                if (bitVector[j] == 0) {
                    blocksFree[i] = j;
                    j++;
                    break;
                }
            }
        }
        return blocksFree;
    }

// ------------------------------------------------------------------------

// The function calculates how many blocks the file needs.
    int calculateNumOfBlocks(fsInode *fsInode, int len) const {
        int numOfBlocks = static_cast<int>(ceil(static_cast<double>(len) / this->sizeOfBlock));
        int num = numOfBlocks;
        int pointersBlocks = 0;
        if (fsInode->getFileSize() != 0)
            numOfBlocks += fsInode->getBlockInUseWithoutPointers();
        if (numOfBlocks > 3)
            pointersBlocks = static_cast<int>(ceil((static_cast<double>(numOfBlocks - 3) / this->sizeOfBlock)));
        if (pointersBlocks > 1)
            pointersBlocks++;
        if (fsInode->getBlockInUseWithoutPointers() != 0) {
            numOfBlocks = static_cast<int>(ceil(static_cast<double>(len) / this->sizeOfBlock));
            pointersBlocks -= fsInode->getBlockInUseIncludePointers() - fsInode->getBlockInUseWithoutPointers();
        }
        return pointersBlocks + numOfBlocks;
    }

// ------------------------------------------------------------------------

// The function checks if there is space in the file
    bool isThereFileFreeSpace(int fd, int len) {
        if (len + this->openFileDescriptors[fd].getFileSize() <=
            (3 + sizeOfBlock + (sizeOfBlock * sizeOfBlock)) * sizeOfBlock)
            return true;
        else
            return false;
    }

// ------------------------------------------------------------------------

// The function updates the bit vector.
    void updateBitVector(vector<int> blocksArr) {
        for (int i = 1; i < blocksArr.size(); i++)
            this->bitVector[blocksArr[i]] = 1;
    }

// ------------------------------------------------------------------------

// The function writes to file if no written before OR written before and
// ere is no space to write to the last block we wrote in the past.
    void writeToFileOptionOne(fsInode *fsInode, vector<int> freeBlocks, char *block, char *buf) {
        char pointerBlockValue;
        for (int i = 1, pointerBuffer = 0; i < freeBlocks.size(); i++) {
            int j = i + fsInode->getBlockInUseIncludePointers();
            if (j < 4) {
                if (j == 1)
                    fsInode->setDirectBlock1(freeBlocks[i]);
                else if (j == 2)
                    fsInode->setDirectBlock2(freeBlocks[i]);
                else if (j == 3)
                    fsInode->setDirectBlock3(freeBlocks[i]);
                fseek(sim_disk_fd, freeBlocks[i] * sizeOfBlock, SEEK_SET);
                strncpy(block, buf + (pointerBuffer * sizeOfBlock), sizeOfBlock);
                block[sizeOfBlock] = '\0';
                fwrite(block, 1, sizeOfBlock, sim_disk_fd);
                pointerBuffer++;
            } else if (j < 5 + sizeOfBlock) {
                if (fsInode->getSingleInDirect() == -1) {
                    fsInode->setSingleInDirect(freeBlocks[i]);
                    fsInode->setIndexSingleBlock(0);
                    i++;
                }
                fseek(sim_disk_fd, fsInode->getSingleInDirect() * sizeOfBlock + fsInode->getIndexSingleBlock(),
                      SEEK_SET);
                pointerBlockValue = decToBinary(freeBlocks[i]);
                fwrite(&pointerBlockValue, 1, 1, sim_disk_fd);
                fsInode->setIndexSingleBlock(fsInode->getIndexSingleBlock() + 1);
                fseek(sim_disk_fd, freeBlocks[i] * sizeOfBlock, SEEK_SET);
                strncpy(block, buf + (pointerBuffer * sizeOfBlock), sizeOfBlock);
                block[sizeOfBlock] = '\0';
                fwrite(block, 1, sizeOfBlock, sim_disk_fd);
                pointerBuffer++;
            } else {
                if (fsInode->getDoubleInDirect() == -1) {
                    fsInode->setDoubleInDirect(freeBlocks[i]);
                    fsInode->setIndexExternalDoubleBlock(0);
                    i++;
                }
                if (fsInode->getIndexExternalDoubleBlock() < sizeOfBlock && fsInode->getInternalDoubleBlock() == -1) {
                    fseek(sim_disk_fd,
                          fsInode->getDoubleInDirect() * sizeOfBlock + fsInode->getIndexExternalDoubleBlock(),
                          SEEK_SET);
                    pointerBlockValue = decToBinary(freeBlocks[i]);
                    fwrite(&pointerBlockValue, 1, 1, sim_disk_fd);
                    fsInode->setIndexExternalDoubleBlock(fsInode->getIndexExternalDoubleBlock() + 1);
                    fsInode->setInternalDoubleBlock(freeBlocks[i]);
                    fsInode->setIndexInternalDoubleBlock(0);
                    i++;
                }
                if (fsInode->getInternalDoubleBlock() == -1) {
                    fsInode->setInternalDoubleBlock(freeBlocks[i]);
                    fsInode->setIndexInternalDoubleBlock(0);
                }
                fseek(sim_disk_fd,
                      fsInode->getInternalDoubleBlock() * sizeOfBlock + fsInode->getIndexInternalDoubleBlock(),
                      SEEK_SET);
                pointerBlockValue = decToBinary(freeBlocks[i]);
                fwrite(&pointerBlockValue, 1, 1, sim_disk_fd);
                fsInode->setIndexInternalDoubleBlock(fsInode->getIndexInternalDoubleBlock() + 1);
                fseek(sim_disk_fd, freeBlocks[i] * sizeOfBlock, SEEK_SET);
                strncpy(block, buf + (pointerBuffer * sizeOfBlock), sizeOfBlock);
                block[sizeOfBlock] = '\0';
                fwrite(block, 1, sizeOfBlock, sim_disk_fd);
                pointerBuffer++;

                if (fsInode->getIndexInternalDoubleBlock() == sizeOfBlock) {
                    fsInode->setIndexInternalDoubleBlock(0);
                    fsInode->setInternalDoubleBlock(-1);
                }
            }
        }
        fsInode->setBlockInUseIncludePointers((int) freeBlocks.size() - 1);
        updateBitVector(freeBlocks);
    }


// ------------------------------------------------------------------------

// The function writes to file if written before AND There is space to write to the
// last block we wrote in the past.
    void writeToFileOptionTwo(fsInode *fsInode, const vector<int> &freeBlocks, char *block, char *buf, int len) {
        strncpy(block, buf, fsInode->getRemainingPlacesInLastBlock());
        block[fsInode->getRemainingPlacesInLastBlock()] = '\0';
        int indexOfTheLastBlock = sizeOfBlock - fsInode->getRemainingPlacesInLastBlock();
        int theLastBlock = fsInode->getLastBlockWritten();
        if (theLastBlock == fsInode->getDirectBlock1() || theLastBlock == fsInode->getDirectBlock2() ||
            theLastBlock == fsInode->getDirectBlock3()) {
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + indexOfTheLastBlock, SEEK_SET);
            fwrite(block, 1, strlen(block), sim_disk_fd);
        } else if (theLastBlock == fsInode->getSingleInDirect()) {
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + fsInode->getIndexSingleBlock() - 1, SEEK_SET);
            char c = (char) fgetc(sim_disk_fd);
            theLastBlock = binaryToDec(c);
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + indexOfTheLastBlock, SEEK_SET);
            fwrite(block, 1, strlen(block), sim_disk_fd);
        } else {
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + fsInode->getIndexExternalDoubleBlock() - 1,
                  SEEK_SET);
            char c = (char) fgetc(sim_disk_fd);
            theLastBlock = binaryToDec(c);
            int indexInternalDoubleBlock = fsInode->getIndexInternalDoubleBlock();
            if (indexInternalDoubleBlock == 0)
                indexInternalDoubleBlock = sizeOfBlock;
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + indexInternalDoubleBlock - 1,
                  SEEK_SET);
            c = (char) fgetc(sim_disk_fd);
            theLastBlock = binaryToDec(c);
            fseek(sim_disk_fd, theLastBlock * sizeOfBlock + indexOfTheLastBlock, SEEK_SET);
            fwrite(block, 1, strlen(block), sim_disk_fd);
        }
        if (freeBlocks.size() > 1)
            writeToFileOptionOne(fsInode, freeBlocks, block, buf + strlen(block));
    }


// ------------------------------------------------------------------------

// The function deletes all dynamically allocated.
    ~fsDisk() {
        for (auto &file: mainDir)
            delete file.second;
        for (auto &file: deletedFiles)
            delete file;
        this->mainDir.clear();
        this->openFileDescriptors.clear();
        delete[] bitVector;
        assert(sim_disk_fd);
        for (int i = 0; i < DISK_SIZE; i++) {
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = (int) fwrite("\0", 1, 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        int value = ::fclose(sim_disk_fd);
        if (value == EOF)
            perror("Err");
    }

};

// ------------------------------------------------------------------------

int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    string fileName2;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    auto *fs = new fsDisk();
    int cmd_;
    while (true) {
        cin >> cmd_;
        switch (cmd_) {
            case 0:   // exit
                delete fs;
                exit(0);
            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                cout << "FORMAT DISK: number of blocks: " << DISK_SIZE / blockSize
                     << endl;
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->openFile(fileName);
                cout << "openFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->closeFile(_fd);
                cout << "closeFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->writeToFile(_fd, str_to_write, (int) strlen(str_to_write));
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read;
                fs->readFromFile(_fd, str_to_read, size_to_read);
                cout << "readFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->delFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 9:   // copy file
                cin >> fileName;
                cin >> fileName2;
                fs->copyFile(fileName, fileName2);
                break;

            case 10:  // rename file
                cin >> fileName;
                cin >> fileName2;
                fs->renameFile(fileName, fileName2);
                break;

            default:
                break;
        }
    }
}
