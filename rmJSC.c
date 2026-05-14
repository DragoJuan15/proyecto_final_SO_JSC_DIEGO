#include <unistd.h>
#include <stdio.h>
#include "rmJSC.h"

void rmJSC(char texto[]) {
    if (unlink(texto) == 0) {
        printf("Borrado\n");
    } else {
        perror("Error\n");
    }
    return;
}
