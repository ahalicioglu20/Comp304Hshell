#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

bool has_txt_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL) {
        printf("Provide files with .txt extension.\n");
        printf("%s does not contain .txt\n", filename);
        return false;
    }
    else if (dot == filename) {
        printf(". is not accepted as a file.\n");
        printf("Provide files with .txt extension.\n");
        return false;
    } else {
        return strcmp(dot, ".txt") == 0;
    }
}


void compare_text_files(const char *file1, const char *file2) {

    FILE *file1_pointer = fopen(file1, "r");
    FILE *file2_pointer = fopen(file2, "r");

    if (!file1_pointer) {
        printf("Error opening file 1: %s.\n", file1);
        return;
    } else if (!file2_pointer) {
        printf("Error opening file  2: %s.\n", file2);
        return;
    }

    int diff_lines = 0;

    char *file1_line = NULL;
    char *file2_line = NULL;
    size_t file1_bufsize = 0; 
    size_t file2_bufsize = 0;
    ssize_t file1_line_len; 
    ssize_t file2_line_len;

    int file_finished = 0;
    int line_count = 1;

    while (true) {
        file1_line_len = getline(&file1_line, &file1_bufsize, file1_pointer);
        file2_line_len = getline(&file2_line, &file2_bufsize, file2_pointer);
        if (file1_line_len == -1) {
            // For debug purposes
            //printf("File 1 is finished");
            printf("Line %d\n", line_count);
            printf("%s: (no corresponding line)\n%s: %s",file1, file2, file1_line);
            diff_lines ++;
            file_finished = 1;
            line_count ++;
            break;
        } else if (file2_line_len == -1) {
            //printf("File 2 is finished");
            printf("Line %d\n", line_count);
            printf("%s: %s%s: (no corresponding line)\n",file1, file2, file1_line);
            diff_lines ++;
            file_finished = 2;
            line_count ++;
            break;
        }

        // Removing the new line charachter from the readed line
        // so that while printing the different lines, there will be no consistency
        if (file1_line[file1_line_len-1] == '\n') {
            file1_line[file1_line_len-1] = '\0';
        }
        if (file2_line[file2_line_len-1] == '\n') {
            file2_line[file2_line_len-1] = '\0';
        }

        if (strcmp(file1_line, file2_line) != 0) {
            printf("Line %d\n", line_count);
            printf("%s: %s\n%s: %s\n",file1, file1_line, file2, file2_line);
            diff_lines++;
            line_count ++;
        }

    }
    if (file_finished == 2) {
        // Check remaining lines in file1
        while (true) {
            file1_line_len = getline(&file1_line, &file1_bufsize, file1_pointer);
            if (file1_line_len == -1) {
                // For debug purposes
                //printf("File 1 is finished");
                break;
            } 
            // Removing the new line charachter from the readed line
            // so that while printing the different lines, there will be no consistency
            if (file1_line[file1_line_len-1] == '\n') {
                file1_line[file1_line_len-1] = '\0';
            }
            printf("Line %d\n", line_count);
            printf("%s: %s\n%s: (no corresponding line)\n",file1, file1_line, file2);
            diff_lines++;
            line_count ++;
        }
    } else if (file_finished == 1) {
        // Check remaining lines in file2
        while (true) {
            file2_line_len = getline(&file2_line, &file2_bufsize, file2_pointer);
            if (file2_line_len == -1) {
                // For debug purposes
                //printf("File 2 is finished");
                break;
            } 
            // Removing the new line charachter from the readed line
            // so that while printing the different lines, there will be no consistency
            if (file2_line[file2_line_len-1] == '\n') {
                file2_line[file2_line_len-1] = '\0';
            }
            printf("Line %d\n", line_count);
            printf("%s: (no corresponding line)\n%s: %s\n",file1, file2, file2_line);
            diff_lines++;
            line_count ++;
        }
    }
    
    if (diff_lines == 0)
        printf("The two files are identical\n");
    else
        printf("The files differ in %d line(s)\n", diff_lines);

    fclose(file1_pointer);
    fclose(file2_pointer);
    free(file1_line);
    free(file2_line);
}

void compare_binary_files(const char *file1, const char *file2) {
    FILE *file1_pointer = fopen(file1, "r");
    FILE *file2_pointer = fopen(file2, "r");

    if (!file1_pointer) {
        printf("Error opening file 1: %s.\n", file1);
        return;
    } else if (!file2_pointer) {
        printf("Error opening file  2: %s.\n", file2);
        return;
    }

    int file1_char;
    int file2_char;
    int diff_bytes = 0;

    int finished_file = 0;

    // Compare bytes until one file ends
    while (true) {
        file1_char = fgetc(file1_pointer);
        if (file1_char == EOF ) {
            // For Debug purposes
            printf("File 1 ended.\n");
            finished_file = 1;
            break;
        } 
        file2_char = fgetc(file2_pointer);
        if (file2_char == EOF) {
            printf("File 2 ended.\n");
            finished_file = 2;
            break;
        }
        if (file1_char != file2_char) {
            diff_bytes++;
        }
    }

    // Check remaining bytes in file1
    while (true) {
        file1_char = fgetc(file1_pointer);
        if (file1_char ==  EOF) {
            break;
        }
    } 

    // Check remaining bytes in file2
    while (true) {
        file2_char = fgetc(file2_pointer);
        if (file2_char ==  EOF) {
            break;
        }
        diff_bytes++;
    }

    if (diff_bytes == 0) {
        printf("The two files are identical\n");
    } else {
        printf("The two files are different in %d bytes\n", diff_bytes);
    }
    
    fclose(file1_pointer);
    fclose(file2_pointer);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("In order to use this function you need to either provide\n-a flag or -b flag. If you dont provide any flag, it will\nbe assumed that you use -a flag. After the flag, provide\ntwo files. If you choose -a flag, you have to choose .txt\nextension files, for -b flag, there is no constraint.\n");
        return 1;
    }

    bool text_mode = true;
    int file_index = 1;

    if (strcmp(argv[1], "-b") == 0) {
        text_mode = false;
        file_index = 2;
    } else if (strcmp(argv[1], "-a") == 0) {
        file_index = 2;
    }

    const char *file1 = argv[file_index];
    const char *file2 = argv[file_index + 1];

    if (text_mode) {
        if (!has_txt_extension(file1) || !has_txt_extension(file2)) {
            printf("Both files must have a .txt extension in text mode.\n");
            return 1;
        }
        compare_text_files(file1, file2);
    } else {
        compare_binary_files(file1, file2);
    }

    return 0;
}
