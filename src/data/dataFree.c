#include "../structs/rpiNode.h"
#include "../utils/nameValue.h"

void data_free() {

    for (int m = 0; m < RPI_MAX_DATA; m++) {
        for (int c = 0; c < RPI_MAX_COMPONENTS; c++) {
            namevalue_free_all(rpiNode.data[m].components[c].settings);
        }
    }

}