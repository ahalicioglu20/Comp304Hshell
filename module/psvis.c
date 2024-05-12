#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_space(char *str) {
    int count = 0;
    while (*str == ' ' && *str != '\0') {
        count++;
        str++;
    }
    return count;
}

int main() {
    FILE *f = fopen("/proc/psvis", "r");
    FILE *graph = fopen("process_tree.dot", "w");

    if (!f) {
        perror("Failed to open file");
        return 1;
    }
    if (!graph) {
        perror("Failed to graph file");
        return 1;
    }

    fprintf(graph, "digraph G {\n");

    char line[1024];
    int last_pid[256] = {0};
    int current_level = 0;

    while (fgets(line, sizeof(line), f)) {
        int pid;
        char command[256] = {0};
        double start_time;

        line[strcspn(line, "\n")] = 0; 
        int spaces = get_space(line);
        current_level = spaces / 2; 

        int matched = sscanf(line + spaces, "PID: %d, Command: %[^,], Start Time: %lu", &pid, command, &start_time);
        // printf("Matched %d\n", matched);
        fprintf(graph, "    %d [label=\"%s\\nStart Time: %lu\"];\n", pid, command, start_time);
        if (current_level > 0 && last_pid[current_level - 1] != 0) {
            fprintf(graph, "    %d -> %d;\n", last_pid[current_level - 1], pid);
        }
        last_pid[current_level] = pid;
        for (int i = current_level + 1; i < 256; i++) {
            last_pid[i] = 0;
        }

    }

    fprintf(graph, "}\n");
    fclose(f);
    fclose(graph);

    system("dot -Tpng process_tree.dot -o process_tree.png");

    return 0;
}
