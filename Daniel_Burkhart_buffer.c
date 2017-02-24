/**
 * NAME: DANIEL BURKHART
 * COURSE: CSC 6320
 * DATE: MARCH 6, 2017
 * ASSIGNMENT: Programming Assignment 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1
typedef int buffer_item;
#define BUFFER_SIZE 8

/**
 * Global variables
 */
buffer_item START_NUMBER;
buffer_item buffer[BUFFER_SIZE];
int MIN_SLEEP_TIME = 1;
int MAX_SLEEP_TIME = 5;
pthread_mutex_t mutex;
sem_t empty, full;
int sleepTime, producerThreads, consumerThreads, insertPointer, removePointer, totalCount;

/**
 * Function decalarations.
 */
void getDataFromCommandLine(int argc, char *const *argv);
void initSyncTools();
void createThreads();
void *producer(void *param);
int insert_item(buffer_item item);
void *consumer(void *param);
int remove_item(buffer_item *item);
int randomIntOverRange(int min, int max);

/**
 * Main method that serves as entry point to program
 *
 * @param argc
 *      The count of arguments
 * @param argv
 *      The array that contains the values of the arguments
 * @return
 *      0 if successful, -1 if error
 */
int main(int argc, char *argv[]) {
    
    getDataFromCommandLine(argc, argv);
    
    initSyncTools();
    
    createThreads();
    
    sleep(sleepTime);
    
    return 0;
}

/**
 * Gets the data from the user via the command line and sets it to class variables.
 *
 * @param argCount
 *      The number of arguments.
 * @param argValues
 *      An array of char* values
 */
void getDataFromCommandLine(int argCount, char *const *argValues) {
    if (argCount != 5) {
        fprintf(stderr, "Usage: <sleep time> <producer threads> <consumer threads> <start number>\n");
        exit(1);
    }
    
    sleepTime = atoi(argValues[1]);
    producerThreads = atoi(argValues[2]);
    consumerThreads = atoi(argValues[3]);
    START_NUMBER = atoi(argValues[4]);
}

/**
 * Initializes synchronization tools to be used in program
 */
void initSyncTools() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    totalCount = 0;
    removePointer = 0;
    insertPointer = 0;
}

/**
 * Creates the producer and consumer threads.
 */
void createThreads() {
    pthread_t producers[producerThreads];
    pthread_t consumers[consumerThreads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    int i, j;
    for (i = 0; i < producerThreads; i++) {
        pthread_create(&producers[i], &attr, producer, NULL);
    }
    for (j = 0; j < consumerThreads; j++) {
        pthread_create(&consumers[j], &attr, consumer, NULL);
    }
}

/**
 * Inserts item into buffer
 *
 * @param item
 *      The item being added to the buffer
 * @return
 *      0 if successful, -1 otherwise
 */
int insert_item(buffer_item item) {
    int result = 0;
    
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    
    if (totalCount != BUFFER_SIZE) {
        buffer[insertPointer] = item;
        insertPointer = (insertPointer + 1) % BUFFER_SIZE;
        totalCount++;
    } else {
        result = -1;
    }
    
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    
    return result;
}

/**
 * Remove item from buffer
 * @param item
 *      The item that is being removed from the buffer
 * @return
 *      0 if successful, -1 otherwise
 */
int remove_item(buffer_item *item) {
    int result = 0;
    
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    
    if (totalCount != 0) {
        *item = buffer[removePointer];
        removePointer = (removePointer + 1) % BUFFER_SIZE;
        totalCount--;
    } else {
        result = -1;
    }
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    
    return result;
}

/**
 * Runner method for producer.
 * Sleeps for random time between 1 and 5 seconds then tries to insert item into buffer.
 *
 * @param param
 *      Param that could be used for each thread. Not used in this program.
 * @return
 *      void
 */
void *producer(void *param) {
    buffer_item item;

    while (TRUE) {

        sleep(randomIntOverRange(MIN_SLEEP_TIME, MAX_SLEEP_TIME));
        item = START_NUMBER;
        START_NUMBER++;
        
        if (insert_item(item)) {
            printf("Error occured: producer\n");
        } else {
            printf("Producer %u produced %d\n", (unsigned int) pthread_self(), item);
        }
    }
}

/**
 * Runner method for consumer.
 * Sleeps for random time between 1 and 5 seconds then tries to remove item from buffer.
 *
 * @param param
 *      Param that could be used for each thread. Not used in this program.
 * @return
 *      void
 */
void *consumer(void *param) {
    buffer_item item;
    
    while (TRUE) {
        
        sleep(randomIntOverRange(MIN_SLEEP_TIME, MAX_SLEEP_TIME));
        if (remove_item(&item)) {
            printf("Error occured: consumer\n");
        } else {
            printf("Consumer %u consumed %d\n", (unsigned int) pthread_self(), item);
        }
    }
}

/**
* Private helper method to give a random int over a range inclusively.
*
* @param min
*       The min, starting point of range.
* @param max
*       The max, ending point of the range.
*/
int randomIntOverRange(int min, int max)
{
   return min + rand() % (max + 1 - min);
}
