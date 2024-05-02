#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
	char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }

    // Get the PATH environment variable
    char *path_env = getenv("PATH");
    if (!path_env) {
        path_env = "";
    }

	// Concatinating cwd with env PATH
    char *new_path = malloc(strlen(cwd) + strlen(path_env) + 2);
    sprintf(new_path, "%s:%s", path_env, cwd);

    // Check each directory in the concateneted path
    char *resolved_path = NULL;
    char *dir = strtok(new_path, ":");
    while (dir != NULL) {
        struct dirent **namelist;
        int n;
        n = scandir(dir, &namelist, NULL, alphasort);
        if (n < 0)
            perror("scandir");
        else {
            while (n--) {
                printf("%s\n", namelist[n]->d_name);
                free(namelist[n]);
            }
            free(namelist);
        }
        // Looking to new dir
        dir = strtok(NULL, ":");
    }

    free(new_path);

    return 0;
}
