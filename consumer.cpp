#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define TABLE_SIZE 2

struct Table {
    int buffer[TABLE_SIZE];
    int count;
};

int main() {
    int smh_fd = shm_open("/table", O_RDWR, 0666);
    if (smh_fd == -1) {
        std::cerr << "Error: could not open shared memory." << std::endl;
        return 1;
    }

    Table* table = (Table*) mmap(0, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, smh_fd, 0);
    if (table == MAP_FAILED) {
        std::cerr << "Error: Map failed." << std::endl;
        return 1;
    }

    sem_t* empty = sem_open("/empty", 0);
    sem_t* full = sem_open("full", 0);
    sem_t* mutex = sem_open("/mutex", 0);

    while (true) {
        sem_wait(full);
        sem_wait(mutex);

        int item = table->buffer[table->count - 1];
        table->count--;

        std::cout << "Consumer consumes: " << item << "| Items on the table: " << table->count << std::endl;

        sem_post(mutex);
        sem_post(empty);
        usleep((rand() % 100 + 5) * 1000);
    }

    return 0;
}