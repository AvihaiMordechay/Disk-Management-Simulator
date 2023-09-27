# Disk-Management-Simulator

==Description==
The disk file system in the operating system is the way in which file names, locations and contents are organized on the hard disk.
Parts of each file are stored in small units called 'blocks' and organized into one logical unit.
The parts of the file are not directly and continuously on the disk, each block can be in a different place on the disk. The mapping of these block parts into a complete file will be completed by the file system.
This program is a simulation of a disk file system, the user will be able to create files, write to them and more.

Program memory:
map<string, fsInode *> mainDir - A data structure that contains all the files.
vector<FileDescriptor> openFileDescriptors - A data structure that contains all the files that are open.
int *bitVector - An array of numbers that represent whether each block is free.


Classes: 
fsInode - A class that holds information about each file.
FileDescriptor - A class that holds a file's fsInode and its name and other information about the file.
fsDisk - A class that represents the disk.

Main functions are divided into cases (for the user input):
Case 0: exit from the program.
Case 1: Print the disk.
Case 2: fsFormat - The function formats the disk according to block size.
Case 3: CreateFile - The function creates a file on disk.
Case 4: openFile - The function opens a file on disk.
Case 5: closeFile - The function closes a file on disk.
Case 6: writeToFile - The function writes to a file.
Case 7: readFromFile - The function reads from a file.
Case 8: delFile - The function delete a file from the disk.
Case 9: copyFile - The function copies an existing file to a new file that will be created and will be closed.
Case 10: renameFile - The function renames the file.

==Program Files==
code_stub.cpp

==How to compile?==
g++ code_stub.cpp -o myDisk
./myDisk

==Input:==
The user has to key in a number from 1 to 10, each number representing a case.
In addition, in any case there is additional input that the user will have to enter.
