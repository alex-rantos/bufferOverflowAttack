#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_LOG_ENTRY_SIZE 4096

void help(void) {
    fprintf(stderr, "Usage: ./securelog <YYYY-mm-ddTHH:MM:SS>_<message>\n");
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t z;
    for (z=0; z < maxlen && s[z]; z++);
    return z;
}

int check_log_format(char *msg) {
    struct tm logtime;
    char *p = strptime(msg, "%Y-%m-%dT%H:%M:%S", &logtime);

    if (!p) {
        fprintf(stderr, "Could not match ISO8601 date format\n");
        return 1;
    }

    if (*p != '_') {
        fprintf(stderr, "Could not find delimiter '_'\n");
        return 1;
    }

    return 0;
}

int append_to_file(const char *path, char *data) {
    int fd = open(path, O_APPEND | O_WRONLY);
    if (fd < 0) {
        perror(path);
        return 1;
    }
    size_t size = strlen(data);
    if (write(fd, data, size) != size) {
        perror("write");
        return 2;
    }
    if (write(fd, "\n", 1) != 1) {
        perror("write");
        return 3;
    }
    return 0;
}

int log_message(char *msg) {
    char buf[MAX_LOG_ENTRY_SIZE];

    time_t now_epoch = time(NULL);
    struct tm now = *localtime(&now_epoch);
    size_t t = strftime(buf, 32, "%Y-%m-%dT%H:%M:%S_", &now);

    size_t u;
    u = sprintf(buf + t, "%05d_", getuid());

    strcpy(buf + u + t, msg);
    return append_to_file("./secure.log", buf);
}

int main(int argc, char **argv) {

    if (argc <= 1) {
        help();
        exit(1);
    }

    char *msg = argv[1];

    if (check_log_format(msg)) {
        fprintf(stderr, "Wrong log format\n");
        help();
        exit(2);
    }

    if (log_message(msg)) {
        fprintf(stderr, "Could not log message\n");
        exit(3);
    }

    return 0;
}
