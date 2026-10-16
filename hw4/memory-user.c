#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    size_t n = (size_t)atoi(argv[1]) * 1024 * 1024;
    int seconds = (argc > 2) ? atoi(argv[2]) : 0;

    char *arr = malloc(n);

    time_t start = time(NULL);
    while (seconds == 0 || time(NULL) - start < seconds) {
        for (size_t i = 0; i < n; i++)
            arr[i]++;
    }

    free(arr);
    return 0;
}
