#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int is_executable(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0; // Failed to get info, assume not executable
    return (statbuf.st_mode & S_IXUSR) || (statbuf.st_mode & S_IXGRP) || (statbuf.st_mode & S_IXOTH);
}

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }
    
    char *path_env = getenv("PATH");
    if (!path_env) {
        path_env = "";
    }

    // Include cwd in the path to be searched
    char *new_path = malloc(strlen(cwd) + strlen(path_env) + 2);
    sprintf(new_path, "%s:%s", path_env, cwd);
    
    char *dir = strtok(new_path, ":");
    while (dir != NULL) {
        struct dirent *entry;
        DIR *dp = opendir(dir);
        if (dp != NULL) {
            while ((entry = readdir(dp)) != NULL) {
                char full_path[1024];
                sprintf(full_path, "%s/%s", dir, entry->d_name);

                // Check if the file is executable
                if (entry->d_type == DT_REG && is_executable(full_path)) {
                    printf("%s\n", entry->d_name);
                }
            }
            closedir(dp);
        } else {
            perror("opendir");
        }
        dir = strtok(NULL, ":");
    }

    free(new_path);
    return 0;
}
