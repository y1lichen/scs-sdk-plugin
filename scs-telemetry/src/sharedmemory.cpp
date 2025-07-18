#include "sharedmemory.hpp"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <string>

void SharedMemory::LogError(const char *logPtr) {
#ifdef SHAREDMEM_LOGGING
    if (this->logFilePtr == nullptr) {
        this->logFilePtr = fopen(SHAREDMEM_FILENAME, "w");
    }
    if (this->logFilePtr != nullptr) {
        fprintf(this->logFilePtr, "%s\n", logPtr);
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

    // Platform-specific shm dir
#ifdef __APPLE__
    const char *shmDir = "/tmp/SCS";
#else
    const char *shmDir = "/dev/shm/SCS";
#endif

    struct stat st = {0};
    if (stat(shmDir, &st) == -1) {
        LogError("Creating shared memory directory");
        mkdir(shmDir, 0777); // rwxrwxrwx
    } else {
        int perms = (S_IRWXU | S_IRWXG | S_IRWXO);  // rwxrwxrwx
        if ((st.st_mode & perms) != perms) {
            LogError("Changing permission of shared memory directory");
            chmod(shmDir, 0777);
        }
    }

    // Full file path for mmap file
    std::string fullPath = std::string(shmDir) + "/" + newNamePtr;

    hMapFile = open(fullPath.c_str(), O_CREAT | O_RDWR, 0777);
    if (hMapFile == -1) {
        LogError("Unable to open mmap file");
        return;
    }

    // Set file size before mmap
    if (ftruncate(hMapFile, mapsize) == -1) {
        LogError("ftruncate failed");
        close(hMapFile);
        return;
    }

    this->pBufferPtr = mmap(nullptr, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, hMapFile, 0);
    if (this->pBufferPtr == MAP_FAILED) {
        LogError("Error calling mmap");
        close(hMapFile);
        return;
    }

    memset(this->pBufferPtr, 0, mapsize);
    this->isSharedMemoryHooked = true;
    LogError("Opened MMF!");
}

void SharedMemory::Close() {
#ifdef SHAREDMEM_LOGGING
    if (logFilePtr != nullptr) {
        fflush(logFilePtr);
        fclose(logFilePtr);
        logFilePtr = nullptr;
    }
#endif
    if (isSharedMemoryHooked) {
        if (pBufferPtr != nullptr) {
            munmap(pBufferPtr, mapsize);
            pBufferPtr = nullptr;
        }
        if (hMapFile >= 0) {
            close(hMapFile);
            hMapFile = -1;
        }
    }

    isSharedMemoryHooked = false;
}
