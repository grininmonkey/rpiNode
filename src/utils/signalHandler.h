// signal_handler.h
#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>
#include <stdio.h>

extern volatile sig_atomic_t running;
extern pthread_mutex_t lock;

// Declaration of the signal handler function
void signal_handler(int sig);

#endif // SIGNAL_HANDLER_H

