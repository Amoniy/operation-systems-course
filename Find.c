#include <stdbool.h>
#include <sys/_types/_off_t.h>
#include <sys/_types/_ino64_t.h>
#include <sys/_types/_nlink_t.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

struct {
    bool wasInum;
    char inum[32];

    bool wasName;
    char name[1024];

    bool wasSize;
    int sizeComparator;
    off_t size;

    bool wasNLinks;
    nlink_t nLinks;

    bool wasPath;
    char path[256];
} arguments;

void initializePattern() {
    arguments.wasInum = false;
    arguments.wasName = false;
    arguments.wasNLinks = false;
    arguments.wasPath = false;
    arguments.wasSize = false;
}

bool matchesPattern(struct dirent *file, char *path) {
    struct stat fileStat;

    if (stat(path, &fileStat) < 0) {
        perror("Could not open stat");
        exit(-1);
    }

//    printf("inum");
//    printf("%d\n",arguments.inum);
//    printf("%d",fileStat.st_ino);
//    printf("inum\n");

    if (arguments.wasInum) {
        char str[64];
        sprintf(str, "%ld", fileStat.st_ino);

        printf("kek");
        printf(arguments.inum);
        printf(" = ");
        printf(str);
        printf("kek\n");


        if (strcmp(arguments.inum, (str)) != 0) {
            return false;
        }
    }
    if (arguments.wasNLinks) {
        if (arguments.nLinks != fileStat.st_nlink) {
            return false;
        }
    }
    if (arguments.wasName) {
        if (strcmp(arguments.name, file->d_name) != 0) {
            return false;
        }
    }
    if (arguments.wasSize) {
        if (arguments.sizeComparator == 0) {
            if (arguments.size != fileStat.st_size) {
                return false;
            }
        } else {
            // actual=1 expected=2 actual>expected=false // we needed act>exp // we got false // its what is needed
            // actual=-1 expected=-2 actual>expected=true // we needed act<exp // we got true // its what is needed
            // => we need to change symbol from > to <
            if (fileStat.st_size * arguments.sizeComparator < arguments.size * arguments.sizeComparator) {
                return false;
            }
        }
    }
    if (arguments.wasPath) {
        char *a[] = {arguments.path, path, NULL};
        execve(arguments.path, a, NULL);
    }
    return true;
}

void recursiveDescent(DIR *currentDir, char *path) {
    if (currentDir == NULL) {
        printf(path);
        perror("Current dir == null");
    }

    struct dirent *fileOrDir;
    while ((fileOrDir = readdir(currentDir)) != NULL) {

        if (strcmp(fileOrDir->d_name, ".") == 0) {
            continue;
        } else if (strcmp(fileOrDir->d_name, "..") == 0) {
            continue;
        }

        printf("inode: %ld \n", fileOrDir->d_ino);
//        printf("name: %s \n", fileOrDir->d_name);

        char curPath[256];
        strcpy(curPath, path);
        strcat(curPath, "/");
        strcat(curPath, fileOrDir->d_name);

        switch (fileOrDir->d_type) {
            case DT_REG :
//                printf("regular file\n");
                if (matchesPattern(fileOrDir, curPath)) {
                    printf(curPath);
                    //
                    printf("kek");
                    printf(fileOrDir->d_name);
                    printf("kek");
                    //
                    printf("\n");
                }
                break;
            case DT_DIR :
//                printf("testing dir=");
//                printf(curPath);
//                printf("\n");
//                printf("directory\n");
                recursiveDescent(opendir(curPath), curPath);
                break;
            default :
                printf("%c Some strange format found\n", fileOrDir->d_type);
        }
    }
    closedir(currentDir);
}

int main(int argc, char *argv[]) {
    initializePattern();
    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-inum") == 0) {
            arguments.wasInum = true;
            printf(argv[i + 1]);
            printf("\n");
//            arguments.inum = atoi(argv[i + 1]);
            strcpy(arguments.inum, argv[i + 1]);
        } else if (strcmp(argv[i], "-name") == 0) {
            arguments.wasName = true;
            strcpy(arguments.name, argv[i + 1]);
        } else if (strcmp(argv[i], "-size") == 0) {
            arguments.wasSize = true;
            if (argv[i + 1][0] == '+') {
                arguments.sizeComparator = 1;
            } else if (argv[i + 1][0] == '=') {
                arguments.sizeComparator = 0;
            } else if (argv[i + 1][0] == '-') {
                arguments.sizeComparator = -1;
            } else {
                // err
            }
            arguments.size = atol(argv[i + 1] + 1);
        } else if (strcmp(argv[i], "-nlinks") == 0) {
            arguments.wasNLinks = true;
            arguments.nLinks = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-exec") == 0) {
            arguments.wasPath = true;
            strcpy(arguments.path, argv[i + 1]);
        } else {
            // err
        }
    }
    recursiveDescent(opendir(argv[1]), argv[1]);

    return 0;
}
