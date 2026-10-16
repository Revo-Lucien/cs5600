#include <stdio.h>
#include <stdlib.h>

int *data = NULL;   // the array
int size = 0;       // number of elements

void add(int x) {
    data = realloc(data, (size + 1) * sizeof(int));
    data[size] = x;
    size++;
}

int main() {
    for (int i = 0; i < 10; i++)
        add(i);

    for (int i = 0; i < size; i++)
        printf("%d ", data[i]);
    printf("\n");

    free(data);
    return 0;
}
