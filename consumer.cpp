// concumer.cpp

#include "common.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

int main() {
    // open shared mem
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        std::exit(EXIT_FAILURE);
    }

    void* addr = mmap(nullptr, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        std::exit(EXIT_FAILURE);
    }

    SharedTable* table = static_cast<SharedTable*>(addr);
    
    //open semaphores
    sem_t* mutex = sem_open(SEM_MUTEX_NAME, 0);
    sem_t* empty = sem_open(SEM_EMPTY_NAME, 0);
    sem_t* full = sem_open(SEM_FULL_NAME, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        std::exit(EXIT_FAILURE);
    }

    while(true) {
        sem_wait(full);
        sem_wait(mutex);

        int item = table->Table_items[--table->count];
        std::cout << "[Consumer] consumed item: " << item << " | Count: " << table->count << std::endl;

        sem_post(mutex);
        sem_post(full);

        sleep(2);
    }

    return 0;
}
