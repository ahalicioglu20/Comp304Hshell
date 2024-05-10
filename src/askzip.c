#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

double original_total = 0;
double compressed_total = 0;

double zip_directory(char *dir_path) {
    // First I prepare the zip command, redirect the output, and run the command
    char zip_command[1000];
    snprintf(zip_command, sizeof(zip_command), "zip -r %s.zip %s > /dev/null", dir_path, dir_path);
    system(zip_command);

    // I get the path and check the size
    char zip_file_path[2000];
    snprintf(zip_file_path, sizeof(zip_file_path), "%s.zip", dir_path);
    struct stat zip_stat;
    if (stat(zip_file_path, &zip_stat) != 0) {
        perror("Error getting zip file stat");
        return 0;
    }

    // Then I remove the ziped file
    char rm_command[2000];
    snprintf(rm_command, sizeof(rm_command), "rm %s", zip_file_path);
    system(rm_command);

    return zip_stat.st_size;
}

double get_directory_size(char *dir_path) {

    double size = 0;

    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Failed to open directory %s\n", dir_path);
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[2000];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
            struct stat statbuf;
            if (stat(path, &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    size += get_directory_size(path);
                } else {
                    size += statbuf.st_size;
                }
            }
        }
    }
    closedir(dir);
    return size;
}

void process_directory(char *dir_path, int depth) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Failed to open directory %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[2000];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

            struct stat statbuf;
            if (stat(path, &statbuf) != 0) continue;

            if (S_ISDIR(statbuf.st_mode)) {
                if (strcmp(entry->d_name, ".git") == 0) {
                    double dir_size = get_directory_size(path);
                    double zipped_size = zip_directory(path);
                    original_total += dir_size;
                    compressed_total += zipped_size;
                    printf("| %-70s | %12.2f MB | %12.2f MB |\n", path, dir_size / (double) 1024*1024, zipped_size / (double) 1024*1024);
                } else if (depth == 0) { 
                    process_directory(path, depth + 1);
                }
            } else {
                double file_size = statbuf.st_size;
                double zipped_size = zip_directory(path);
                original_total += file_size;
                compressed_total += zipped_size;
                printf("| %-70s | %12.2f MB | %12.2f MB |\n", path, file_size / (double) 1024*1024, zipped_size / (double) 1024*1024);
            }
        }
    }
    closedir(dir);
}

int main() {
    char cwd[2000];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }

    printf("Processing directories and files...\n");
    printf("| %-70s | %12s | %12s |\n", "Directory/File Path", "Original Size (MB)", "Compressed Size (MB)");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    process_directory(cwd, 0);

    double compression_percentage = 100.0 * (1.0 - ((double)compressed_total / original_total));
    printf("Total original size: %.2f MB\n", original_total / (double) 1024*1024);
    printf("Total compressed size: %.2f MB\n", compressed_total / (double) 1024*1024);
    printf("Total compression percentage: %.2f%%\n", compression_percentage);

    char answer[10];
    printf("Do you want to zip the entire directory? (y/n): ");
    scanf("%9s", answer);
    if (strcmp(answer, "y") == 0) {
        char zip_command[2048];
        char zip_file_path[2048];
        snprintf(zip_command, sizeof(zip_command), "zip -r %s.zip %s > /dev/null", cwd, cwd);
        snprintf(zip_file_path, sizeof(zip_file_path), "%s.zip", cwd);  
        system(zip_command);
        printf("Directory zipped successfully.\n");
        printf("Zip file created at: %s\n", zip_file_path); 
    }

    return 0;
}
