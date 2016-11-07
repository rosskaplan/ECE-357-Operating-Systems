#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char **argv) {

    int pipefd[2];
    int pip;
    if ((pip = pipe(pipefd)) == -1) {
        fprintf(stderr, "Error setting pipes: %s\n", strerror(errno));
        exit(-1);
    }
    int ctrl = fcntl(pipefd[1], O_NONBLOCK);
    int sum = 0;
    while (1) {
        int bytes = write(pipefd[1], "abcdefgh", 8);
        if (errno == EAGAIN) {
            fprintf(stderr, "Reached error \"EAGAIN\": %s\n", strerror(errno));
            break;
        }
        sum+= bytes;
    }
    printf("%d\n", sum);

return 0;
}
