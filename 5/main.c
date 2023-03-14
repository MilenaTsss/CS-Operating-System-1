#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../count.h"

#define readpipe "/tmp/readpipe"
#define writepipe "/tmp/writepipe"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return -1;
    }

    char text_buf[10000] = {};
    int count_num[2] = {};// num of correct words and num of all words

    mknod(readpipe, S_IFIFO | 0666, 0);
    mknod(writepipe, S_IFIFO | 0666, 0);

    pid_t is_analyzer = fork();

    if (is_analyzer == -1) {
        printf("Creating analyzer process went wrong\n");
        exit(-1);
    } else if (is_analyzer == 0) {
        pid_t is_reader = fork();

        if (is_reader == -1) {
            printf("Creating reader process went wrong\n");
            exit(-1);
        } else if (is_reader == 0) {
            printf("Reader process started\n");

            int read_fd = open(argv[1], O_RDONLY);
            read(read_fd, text_buf, sizeof(text_buf));

            int read_pipe = open(readpipe, O_WRONLY);

            write(read_pipe, text_buf, sizeof(text_buf));

            close(read_fd);
            close(read_pipe);
        } else {
            printf("Analyzer process started\n");

            int read_pipe = open(readpipe, O_RDONLY);
            int write_pipe = open(writepipe, O_WRONLY);

            read(read_pipe, text_buf, sizeof(text_buf));

            count(text_buf, count_num);
            write(write_pipe, count_num, sizeof(count_num));

            close(read_pipe);
            close(write_pipe);
        }
    } else {
        printf("Writer process started\n");

        int write_pipe = open(writepipe, O_RDONLY);

        read(write_pipe, count_num, sizeof(count_num));

        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

        char output_buf[100] = {};
        sprintf(output_buf, "num of good words - %d\n", count_num[0]);
        write(write_fd, output_buf, strlen(output_buf));

        output_buf[0] = '\0';

        sprintf(output_buf, "num of all words - %d\n", count_num[1]);
        write(write_fd, output_buf, strlen(output_buf));

        close(write_fd);
        close(write_pipe);

        // Do unlink in the end
        unlink(readpipe);
        unlink(writepipe);
    }
}