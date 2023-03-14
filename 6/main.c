#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../count.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return -1;
    }

    int read_pipe[2], write_pipe[2];
    char text_buf[10000] = {};
    int count_num[2] = {};// num of correct words and num of all words

    pipe(read_pipe);
    pipe(write_pipe);

    pid_t child_pid = fork();

    if (child_pid == -1) {
        printf("Creating the first child went wrong\n");
        exit(-1);
    } else if (child_pid == 0) {
        printf("Analyzer process started\n");
        read(read_pipe[0], text_buf, sizeof(text_buf));

        count(text_buf, count_num);
        write(write_pipe[1], count_num, sizeof(count_num));
    } else {
        printf("Reader process started\n");

        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, text_buf, sizeof(text_buf));
        write(read_pipe[1], text_buf, sizeof(text_buf));
        close(read_fd);

        printf("Writer process started\n");

        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);
        read(write_pipe[0], count_num, sizeof(count_num));

        char output_buf[100] = {};
        sprintf(output_buf, "num of good words - %d\n", count_num[0]);
        write(write_fd, output_buf, strlen(output_buf));

        output_buf[0] = '\0';
        sprintf(output_buf, "num of all words - %d\n", count_num[1]);
        write(write_fd, output_buf, strlen(output_buf));

        close(write_fd);
    }

    close(read_pipe[0]);
    close(read_pipe[1]);
    close(write_pipe[0]);
    close(write_pipe[1]);
}