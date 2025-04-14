// signal_handler.c
#include <pthread.h>
#include "signalHandler.h"

// Flag to control the program's state
volatile sig_atomic_t running = 1;
  
// Mutex initialization
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  // Mutex initialization

// Signal handler for SIGINT (Ctrl+C) and SIGTERM
void signal_handler(int sig) {
    pthread_mutex_lock(&lock);
    running = 0;
    pthread_mutex_unlock(&lock);
    printf("\nReceived signal %d, shutting down gracefully...\n", sig);
}

