#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

void get_info(struct dirent *ent, char *path) {
    char buffer_path[PATH_MAX];
    buffer_path[0] = '\0';
    strcat(buffer_path, path);
    strcat(buffer_path, "/");
    strcat(buffer_path, ent->d_name);

    struct stat stats;
    memset(&stats, 0, sizeof(struct stat));
    stat(buffer_path, &stats);

    printf((stats.st_mode & S_IFDIR) ? "d" : "-");  // Is directory
    printf((stats.st_mode & S_IRUSR) ? "r" : "-");  // Read permission owner
    printf((stats.st_mode & S_IWUSR) ? "w" : "-");  // Write permission owner
    printf((stats.st_mode & S_IXUSR) ? "x" : "-");  // Execute permission owner
    printf((stats.st_mode & S_IRGRP) ? "r" : "-");  // Read permission group
    printf((stats.st_mode & S_IWGRP) ? "w" : "-");  // Write permission group
    printf((stats.st_mode & S_IXGRP) ? "x" : "-");  // Execute permission group
    printf((stats.st_mode & S_IROTH) ? "r" : "-");  // Read permission other
    printf((stats.st_mode & S_IWOTH) ? "w" : "-");  // Write permission other
    printf((stats.st_mode & S_IXOTH) ? "x" : "-");  // Execute permission other

    printf(" %2lu", stats.st_nlink);

    struct passwd *user = getpwuid(stats.st_uid);
    printf(" %s", (user != 0) ? user->pw_name : " ");

    struct group *group = getgrgid(stats.st_gid);
    printf(" %s", (group != 0) ? group->gr_name : " ");

    printf(" %5ld", stats.st_size);

    struct tm *time_ptr;
    time_t time = stats.st_ctime;
    time_ptr = localtime(&time);

    char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    printf(" %s", months[time_ptr->tm_mon]);
    printf(" %02d", time_ptr->tm_mday);
    printf(" %02d:%02d ", time_ptr->tm_hour, time_ptr->tm_min);

    printf("%s\n", ent->d_name);
}

char *detect_path(int argc, char **argv) {
    switch (argc) {
        case 1:
            return  "./";
        case 2:
            return ((strstr(argv[1], "-l") == NULL) ? argv[1] : "./");
        case 3:
            return ((strstr(argv[1], "-l") == NULL) ? argv[1] : argv[2]);
        default:
            return "ERROR";
    }
}

char *get_path(int argc, char **argv) {
    char *current_path = detect_path(argc, argv);
    if (strstr(current_path, "ERROR")) {
        printf("%s\n", "Невозможно получить доступ. Нет такого файла или каталога");
        exit(1);
    }
    return current_path;
}

void print_list(DIR *dir, struct dirent *ent, char *current_path) {
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;
        printf("%s\n", ent->d_name);
    }
}

void print_list_long(DIR *dir, struct dirent *ent, char *current_path) {
    struct stat s;
    memset(&s, 0, sizeof(struct stat));
    stat(current_path, &s);
    printf("total %ld \n", s.st_blocks);

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;
        get_info(ent, current_path);
    }
}

int main(int argc, char **argv) {
    char *current_path = get_path(argc, argv);
    DIR *dir = opendir(current_path);
    struct dirent *ent = NULL;

    int c;
    int counter = 1;
    while (counter < argc) {
        if ((c = getopt(argc, argv, "l")) != -1) {
            switch (c) {
                case 'l':
                    print_list_long(dir, ent, current_path);
                    closedir(dir);
                    exit(0);
                case '?':
                    closedir(dir);
                    exit(1);
                default:
                    break;
            }
        }
        counter++;
    }
    print_list(dir, ent, current_path);
    closedir(dir);
    return 0;
}
