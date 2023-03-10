#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "count.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return 1;
    }

    int read_pipe[2], write_pipe[2];
    char text_buf[20000] = {};
    int count_num[2] = {};

    pipe(read_pipe);
    pipe(write_pipe);

    pid_t is_analyzer = fork();

    if (is_analyzer == -1) {
        printf("Creating child process went wrong\n");
        exit(-1);
    } else if (is_analyzer == 0) {
        pid_t is_reader = fork();

        if (is_reader == -1) {
            printf("Creating child process went wrong\n");
            exit(-1);
        } else if (is_reader == 0) {
            printf("Reader process online\n");
            int read_fd = open(argv[1], O_RDONLY);
            read(read_fd, text_buf, sizeof(text_buf));

            write(read_pipe[1], text_buf, sizeof(text_buf));

            close(read_fd);
        } else {
            printf("Processor process online\n");
            read(read_pipe[0], text_buf, sizeof(text_buf));

            printf("Processor read string: %s\n", text_buf);

            count(text_buf, count_num);
            write(write_pipe[1], count_num, sizeof(count_num));
        }
    } else {
        printf("Writer process online\n");
        read(write_pipe[0], count_num, sizeof(count_num));

        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

        dprintf(write_fd, "num of good words - %d\n", count_num[0]);
        printf("num of all words - %d\n", count_num[1]);

        close(write_fd);
    }

    close(read_pipe[0]);
    close(read_pipe[1]);
    close(write_pipe[0]);
    close(write_pipe[1]);
}
