#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "count.h"

#define readpipe "/tmp/readpipe"
#define writepipe "/tmp/writepipe"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return 1;
    }

    char text_buf[20000] = {};
    int count_num[2] = {}; // num of correct words and num of all words

    mknod(readpipe, S_IFIFO | 0666, 0);
    mknod(writepipe, S_IFIFO | 0666, 0);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader process online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, text_buf, sizeof(text_buf));

        int read_pipe = open(readpipe, O_WRONLY);

        write(read_pipe, text_buf, sizeof(text_buf));

        close(read_fd);
        close(read_pipe);
    } else {
        pid_t is_writer = fork();

        if (is_writer) {
            printf("Writer process online\n");

            int write_pipe = open(writepipe, O_RDONLY);

            read(write_pipe, count_num, sizeof(count_num));

            int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

            dprintf(write_fd, "num of good words - %d\n", count_num[0]);
            dprintf(write_fd, "num of all words - %d\n", count_num[1]);

            close(write_fd);
            close(write_pipe);

            // This process is last to end, so unlink here
            unlink(readpipe);
            unlink(writepipe);
        } else {
            printf("Processor process online\n");

            int read_pipe = open(readpipe, O_RDONLY);
            int write_pipe = open(writepipe, O_WRONLY);

            read(read_pipe, text_buf, sizeof(text_buf));

            printf("Processor read string: %s\n", text_buf);

            count(text_buf, count_num);
            write(write_pipe, count_num, sizeof(count_num));

            close(read_pipe);
            close(write_pipe);
        }
    }
}