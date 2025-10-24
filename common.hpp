#ifndef COMMON_HPP
#define COMMON_HPP

#define _XOPEN_SOURCE 700
#include <semaphore.h>

//shared mem
#define SEM_NAME "/prodcons_shm"
#define SEM_MUTEX_NAME "/sem_mutex"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"

#define TABLE_SIZE 2

struct SharedTable {
    int Table_items[2];
    int count;
};


#endif