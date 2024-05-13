#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAX_EXT_LEN 128
#define MAX_IGNORE_ITEMS 100
#define BUFFER_SIZE 1024

int is_ignored(char *filename, char ignore_items[][MAX_EXT_LEN], int num_ignores) {
    for (int i = 0; i < num_ignores; i++) {
        if (strstr(filename, ignore_items[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

int count_non_blank_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return 0;
    }

    int lines = 0;
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file)) {
        char *ptr = buffer;
        while (*ptr != '\0' && isspace((unsigned char)*ptr)) ptr++;
        if (*ptr != '\0' && *ptr != '\n') lines++;
    }

    fclose(file);
    return lines;
}

void search_dir(const char *dir_name, char extensions[][MAX_EXT_LEN], int num_exts, int *total_lines, char ignore_items[][MAX_EXT_LEN], int num_ignores) {
    DIR *dir = opendir(dir_name);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", dir_name);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char path[BUFFER_SIZE];
                snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                if (!is_ignored(path, ignore_items, num_ignores)) {
                    search_dir(path, extensions, num_exts, total_lines, ignore_items, num_ignores);
                }
            }
        } else if (entry->d_type == DT_REG) {
            char path[BUFFER_SIZE];
            snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
            if (!is_ignored(entry->d_name, ignore_items, num_ignores)) {
                for (int i = 0; i < num_exts; i++) {
                    if (strstr(entry->d_name, extensions[i]) != NULL) {
                        int lines = count_non_blank_lines(path);
                        printf("File: %s - %d lines\n", path, lines);
                        *total_lines += lines;
                        break;
                    }
                }
            }
        }
    }

    closedir(dir);
}

int main() {
    char extensions[MAX_IGNORE_ITEMS][MAX_EXT_LEN];
    char ignore_items[MAX_IGNORE_ITEMS][MAX_EXT_LEN];
    int num_exts = 0, num_ignores = 0;
    int total_lines = 0;

    printf("请输入要统计的文件后缀(中间用,隔开)：");
    char input[BUFFER_SIZE];
    fgets(input, BUFFER_SIZE, stdin);
    char *token = strtok(input, ", \n");
    while (token != NULL) {
        strcpy(extensions[num_exts++], token);
        token = strtok(NULL, ", \n");
    }

    FILE *ignore_file = fopen("ignore_count.txt", "r");
    if (ignore_file) {
        while (fgets(input, BUFFER_SIZE, ignore_file)) {
            input[strcspn(input, "\n")] = 0; // Remove newline character
            if (strlen(input) > 0) {
                strcpy(ignore_items[num_ignores++], input);
            }
        }
        fclose(ignore_file);
    }

    printf("请输入要忽略的文件夹名(使用,隔开，如果没有则留空,默认读取ignore_count.txt文件): ");
    fgets(input, BUFFER_SIZE, stdin);
    token = strtok(input, ", \n");
    while (token != NULL) {
        strcpy(ignore_items[num_ignores++], token);
        token = strtok(NULL, ", \n");
    }

    search_dir(".", extensions, num_exts, &total_lines, ignore_items, num_ignores);
    printf("您查询的文件共有 %d 行代码。\n", total_lines);
    return 0;
}
