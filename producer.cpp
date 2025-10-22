#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>

#define TABLE_SIZE 2

struct Table {
    int buffer[TABLE_SIZE];
    int count;
};

int main() {
    int smh_fd = shm_open("/table", O_CREAT | O_RDWR, 066);
    ftruncate(smh_fd, sizeof(Table));
    Table* table = (Table*) mmap(0, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, smh_fd, 0);

    table->count = 0;
    for (int i = 0; i < TABLE_SIZE; ++i) table->buffer[i] = 0;

    sem_t* empty = sem_open("/empty", O_CREAT, 0644, TABLE_SIZE);   // 2 slots
    sem_t* full = sem_open("/full", O_CREAT, 0644, 0);              // 0 slots
    sem_t* mutex = sem_open("/mutex", O_CREAT, 0644, 1);   
    
    while (true) {
        int item = rand()%100;

        sem_wait(empty);

        sem_wait(mutex);

        table->buffer[table->count] = item;
        table->count++;

        std::cout << "Producer produces: " << item << " | Items on the table: " << table->count << std::endl; 

        sem_post(mutex);

        sem_post(full);

        usleep((rand() % 100 + 50) * 1000);
    }

    return 0;
}