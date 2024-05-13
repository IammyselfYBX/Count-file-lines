#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_EXT_LEN 10
#define MAX_EXTS 20

int count_lines(const char *filename);
void search_dir(const char *dir_name, char extensions[][MAX_EXT_LEN], int num_exts, int *total_lines);

int main() {
    char input[256];
    char extensions[MAX_EXTS][MAX_EXT_LEN];
    int num_exts = 0;
    int total_lines = 0;

    printf("请输入要统计的文件后缀(中间用,隔开)：");
    fgets(input, sizeof(input), stdin);

    // Parse extensions
    char *token = strtok(input, ", \n");
    while (token != NULL && num_exts < MAX_EXTS) {
        strncpy(extensions[num_exts++], token, MAX_EXT_LEN);
        token = strtok(NULL, ", \n");
    }

    // Start searching in current directory
    search_dir(".", extensions, num_exts, &total_lines);
    
    printf("您查询的文件共有 %d 行代码。\n", total_lines);

    return 0;
}

void search_dir(const char *dir_name, char extensions[][MAX_EXT_LEN], int num_exts, int *total_lines) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                search_dir(path, extensions, num_exts, total_lines);
            }
        } else {
            for (int i = 0; i < num_exts; i++) {
                char *ext = extensions[i];
                if (strcmp(ext, "Makefile") == 0 && strcmp(entry->d_name, "Makefile") == 0) {
                    char path[1024];
                    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                    *total_lines += count_lines(path);
                    break;
                } else if (strstr(entry->d_name, ext) != NULL) {
                    char path[1024];
                    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
                    *total_lines += count_lines(path);
                    break;
                }
            }
        }
    }

    closedir(dir);
}

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    int lines = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Check if the line is not just whitespace
        char *tmp = line;
        while (*tmp != '\0') {
            if (!isspace((unsigned char)*tmp)) {
                lines++;
                break;
            }
            tmp++;
        }
    }

    fclose(file);
    return lines;
}
