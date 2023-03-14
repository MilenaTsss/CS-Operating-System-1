#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../count.h"

#define readpipe "/tmp/readpipe"
#define writepipe "/tmp/writepipe"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return 1;
    }

    char text_buf[10000] = {};
    int count_num[2] = {};// num of correct words and num of all words

    mknod(readpipe, S_IFIFO | 0666, 0);
    mknod(writepipe, S_IFIFO | 0666, 0);

    pid_t child_pid = fork();

    if (child_pid) {
        printf("Reader process started\n");

        int read_fd = open(argv[1], O_RDONLY);
        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

        read(read_fd, text_buf, sizeof(text_buf));

        int read_pipe = open(readpipe, O_WRONLY);
        int write_pipe = open(writepipe, O_RDONLY);

        write(read_pipe, text_buf, sizeof(text_buf));

        printf("Writer process started\n");
        read(write_pipe, count_num, sizeof(count_num));

        char output_buf[100] = {};
        sprintf(output_buf, "num of good words - %d\n", count_num[0]);
        write(write_fd, output_buf, strlen(output_buf));

        output_buf[0] = '\0';
        sprintf(output_buf, "num of all words - %d\n", count_num[1]);
        write(write_fd, output_buf, strlen(output_buf));

        close(read_fd);
        close(write_fd);
        close(read_pipe);
        close(write_pipe);

        // This process is last to end, so unlink here
        unlink(readpipe);
        unlink(writepipe);
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
}