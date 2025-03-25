#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <semaphore.h>

#define SHM_NAME "/shared_table_cpp"
#define TABLE_SIZE 2

struct SharedBuffer {
    int table[TABLE_SIZE];
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
};

SharedBuffer* buffer;

void producer() {
    while (true) {
        int item = rand() % 100;
        sem_wait(&buffer->empty);
        sem_wait(&buffer->mutex);

        buffer->table[buffer->count] = item;
        std::cout << "Produced: " << item << " at position " << buffer->count << std::endl;
        buffer->count++;

        sem_post(&buffer->mutex);
        sem_post(&buffer->full);

        sleep(rand() % 2);
    }
}

int main() {
    srand(time(nullptr));
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedBuffer));
    buffer = (SharedBuffer*)mmap(nullptr, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Initialize shared buffer and semaphores
    buffer->count = 0;
    sem_init(&buffer->empty, 1, TABLE_SIZE);
    sem_init(&buffer->full, 1, 0);
    sem_init(&buffer->mutex, 1, 1);

    std::thread prodThread(producer);
    prodThread.join();

    return 0;
}

