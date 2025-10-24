// producer.cpp

#include "common.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

int main() {

    //shared mem
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        std::exit(EXIT_FAILURE);
    }

    ftruncate(shm_fd, sizeof(SharedTable));
    void* addr = mmap(nullptr, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        std::exit(EXIT_FAILURE);
    }

    SharedTable* table = static_cast <SharedTable*> (addr);
    std::memset(table, 0, sizeof(SharedTable));

    //semaphores
    sem_t* mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
    sem_t* empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, TABLE_SIZE);
    sem_t* full = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        std::exit(EXIT_FAILURE);
    }

    int item = 0;
    while (true) {
        sem_wait(empty);    // wait if table is full
        sem_wait(mutex);    //enter CS

        // produce item
        table-> Table_items[table->count++] = item++;
        std::cout << "[Producer] Produced item: " << item << " | Count: " << table->count <<std::endl;

        sem_post(mutex);
        sem_post(full);

        sleep(1);
    }

    return 0;
}