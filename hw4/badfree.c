#include <stdlib.h>

int main() {
    int *data = malloc(100 * sizeof(int));
    free(data + 50);
    return 0;
}
