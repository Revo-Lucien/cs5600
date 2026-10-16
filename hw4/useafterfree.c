#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = malloc(100 * sizeof(int));
    data[0] = 5;
    free(data);
    printf("%d\n", data[0]);
    return 0;
}
