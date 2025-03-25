#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <cstdlib>
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

void consumer() {
    while (true) {
        sem_wait(&buffer->full);
        sem_wait(&buffer->mutex);

        buffer->count--;
        int item = buffer->table[buffer->count];
        std::cout << "Consumed: " << item << " from position " << buffer->count << std::endl;

        sem_post(&buffer->mutex);
        sem_post(&buffer->empty);

        sleep(rand() % 3);
    }
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    buffer = (SharedBuffer*)mmap(nullptr, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    std::thread consThread(consumer);
    consThread.join();

    return 0;
}

