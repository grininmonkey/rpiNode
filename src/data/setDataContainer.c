#include "../structs/rpiNode.h"

int set_data_container(char *id) {

    pthread_mutex_lock(&rpiNode.lock);

    pthread_mutex_unlock(&rpiNode.lock);

    return -1;
}