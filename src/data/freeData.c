#include "../structs/rpiNode.h"
#include "../utils/nameValue.h"

void free_data() {

    for (int m = 0; m < RPI_MAX_DATA; m++) {
        for (int c = 0; c < RPI_MAX_INTERNAL_OBJECTS; c++) {
            namevalue_free_all(rpiNode.data[m].components[c].settings);
        }
    }

}