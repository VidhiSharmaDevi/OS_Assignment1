# OS_Assignment1

## Understanding the Problem
The Producer-Consumer problem is a synchronization problem where the producer generates data and places them on a buffer, that is a shared resource.
The consumer, takes that data from the buffer and processes them.
The synchronization is needed because the producer must wait if the table (buffer) is full and the consumer must wait if the table is empty.
The shared memory is used to allow both producer and consumer to communicate.
Semaphores are a generalized mechanism for synchronization between processes and are used to prevent race conditions and ensure synchronization. A semaphore has two atomic operations:  P / wait and V / signal.

## Key Technologies Used
1. Shared Memory (shm_open) → Allows inter-process communication.
2. Semaphores (sem_wait, sem_post) → Synchronize access to the shared table.
3. Threads (std::thread) → Enable concurrency.
4. Mutex (sem_t mutex) → Prevents data corruption in critical sections.

## Designing the Shared Table
struct SharedBuffer {
    int table[TABLE_SIZE];
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
};

As we can see, the table is a max of 2 items and it tracks the number of data that is placed on the buffer. Then there's a variable for a sempahore for empty slots and another of for a sempaphore for filled slots and then a mutex to protect critical section.

## Setting Up Shared Memory
int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
ftruncate(shm_fd, sizeof(SharedBuffer));
buffer = (SharedBuffer*)mmap(nullptr, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

This is how both producer and consumer use shared memory to communicate
shm_open() → that creates or opens shared memory.
ftruncate() → to resizes the shared memory.
mmap() → maps shared memory into the process.

## Implementing the Producer
The producer generates a random data, and uses this variables:
sem_wait(empty) → waits for an empty slot.
sem_wait(mutex) → locks access.
Places the data on the table and updates count.
sem_post(mutex) → unlocks.
sem_post(full) → signals that a slot is full.
And then sleeps to simulate work.
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

## Implementing the Consumer
The consumer: 
sem_wait(full) → waits for a full slot.
sem_wait(mutex) → locks access.
Takes an item and decrements count.
sem_post(mutex) → unlocks.
sem_post(empty) → signals an empty slot.
And then sleeps to simulate processing time.
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

## Running the Producer and Consumer in Separate Threads
std::thread prodThread(producer);
prodThread.join();
std::thread consThread(consumer);
consThread.join();

## Compilation and Execution
To compile both programs, is neccesary:
g++ producer.cpp -pthread -lrt -o producer
g++ consumer.cpp -pthread -lrt -o consumer
To run them together in the background:
./producer & ./consumer &

