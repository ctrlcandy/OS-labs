#include <stdio.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void print_help() {
    printf("%s\n", "Первым идет флаг группы атрибутов: user/group/other");
    printf("%s\n", "Второй флаг - добавление и удаление: add/remove");
    printf("%s\n", "Далее идет от одного до трех флагов атрибутов: read/write/execute");
    printf("%s\n", "Примеры:");
    printf("%s\n", "./chmod -uaw filepath");
    printf("%s\n", "./chmod -grr filepath");
    printf("%s\n", "./chmod -oarwx filepath");
}

int main(int argc, char* argv[]) {
    char* path;
    if (argc == 3) {
        path = argv[2];
    }
    else if (argc == 2 && argv[1][1] == 'h') {
        print_help();
    }
    else {
        printf("%s\n", "ERROR!");
        exit(1);
    }

    bool user = false;
    bool group = false;
    bool other = false;

    bool add = false;
    bool remove = false;

    bool read = false;
    bool write = false;
    bool execute = false;

    int c;
    while ((c = getopt(argc, argv, "hugoadrwe")) != -1) {
        switch(c) {
            case 'u': user = true; break;
            case 'g': group = true; break;
            case 'o': other = true; break;
            case 'a': add = true; break;
            case 'r':
                if (!remove && !add) remove = true;
                else read = true;
                break;
            case 'w': write = true; break;
            case 'e': execute = true; break;
            default:
                printf("%s\n", "ERROR!");
                exit(1);
        }
    }

    struct stat stats;
    if (stat(path, &stats) < 0) {
        printf("%s\n", strerror(errno));
        exit(1);
    }

    mode_t mode = stats.st_mode;

    if(add) {
        if (user) {
            if (read) mode |= S_IRUSR;
            if (write) mode |= S_IWUSR;
            if (execute) mode |= S_IXUSR;
        }
        else if (group) {
            if (read) mode |= S_IRGRP;
            if (write) mode |= S_IWGRP;
            if (execute) mode |= S_IXGRP;
        }
        else if (other) {
            if (read) mode |= S_IROTH;
            if (write) mode |= S_IWOTH;
            if (execute) mode |= S_IXOTH;
        }
    }
    else {
        if (user) {
            if (read) mode ^= S_IRUSR;
            if (write) mode ^= S_IWUSR;
            if (execute) mode ^= S_IXUSR;
        }
        else if (group) {
            if (read) mode ^= S_IRGRP;
            if (write) mode ^= S_IWGRP;
            if (execute) mode ^= S_IXGRP;
        }
        else if (other) {
            if (read) mode ^= S_IROTH;
            if (write) mode ^= S_IWOTH;
            if (execute) mode ^= S_IXOTH;
        }
    }

    if (chmod(path, mode) < 0) {
        printf("%s\n", strerror(errno));
        exit(1);
    }
}
