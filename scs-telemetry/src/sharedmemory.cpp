#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "sharedmemory.hpp"

void SharedMemory::LogError(const char *logPtr) {
//    printf("%s", logPtr);
#ifdef SHAREDMEM_LOGGING
    if (this->logFilePtr == nullptr)
    {
        this->logFilePtr = fopen(SHAREDMEM_FILENAME, "w");
    }
    if (this->logFilePtr != nullptr)
    {
        fprintf(this->logFilePtr, "%s\n", logPtr);
//        fflush(this->logFilePtr);
    }
#endif
    this->errReason = logPtr;
}

SharedMemory::SharedMemory(const char *newNamePtr, unsigned int size) {
    this->mapsize = size;
    this->namePtr = newNamePtr;
    this->isSharedMemoryHooked = false;
#ifdef SHAREDMEM_LOGGING
    this->logFilePtr = nullptr;
#endif
    struct stat st = {0};
    if (stat("/dev/shm/SCS", &st) == -1) {
        LogError("Creating directory /dev/shm/SCS");
        mkdir("/dev/shm/SCS", 0777); // rw-rw-rw
    }else {
        int perms = (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
        if((st.st_mode & perms) != perms){
//            system("rm -rf /dev/shm/SCS");
            LogError("Changing permission of /dev/shm/SCS");
            chmod("/dev/shm/SCS", 0777); // rw-rw-rw
        }
    }
    hMapFile = open(newNamePtr, O_CREAT | O_RDWR, 0777);
    if (hMapFile == -1) {
        chmod(newNamePtr, 0777); // rw-rw-rw
        hMapFile = open(newNamePtr, O_CREAT | O_RDWR, 0777);
        if(hMapFile == -1){
            printf("%s\n",strerror(errno));
            LogError("Unable to open mmap file");
            return;
        }
    }
    this->pBufferPtr = mmap(nullptr, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, hMapFile, 0);
    LogError("Created file map");
    if (this->pBufferPtr == MAP_FAILED) {
        LogError("Error calling mmap");
        return;
    }

    ftruncate(hMapFile, mapsize);
    memset(this->pBufferPtr, 0, mapsize);
    this->isSharedMemoryHooked = true;
    LogError("Opened MMF!");
}


void SharedMemory::Close() {
#ifdef SHAREDMEM_LOGGING
    if (logFilePtr != nullptr)
    {
        fflush(logFilePtr);
        fclose(logFilePtr);
    }
#endif
    if (isSharedMemoryHooked) {
        if (pBufferPtr != nullptr) {
            munmap(pBufferPtr, this->mapsize);
            pBufferPtr = nullptr;
        }
        if (hMapFile > 0) {
            close(hMapFile);
            hMapFile = 0;
        }
    }

    isSharedMemoryHooked = false;

}
