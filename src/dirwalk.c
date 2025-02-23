#include "header/libs.h"
#define MAX_PATH 4096

// Структура для хранения путей и сортировки
typedef struct
{
    char path[MAX_PATH];
} Entry;

int compare_entries(const void *a, const void *b)
{
    return strcoll(((Entry *)a)->path, ((Entry *)b)->path);
}

// Рекурсивная функция обхода директории
void dirwalk(const char *dir, int show_links, int show_dirs, int show_files, int sort)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    Entry *entries = NULL;
    size_t count = 0, capacity = 0;

    if (!(dp = opendir(dir)))
    {
        perror("opendir");
        return;
    }

    // Считываем все записи в директории
    while ((entry = readdir(dp)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char fullpath[MAX_PATH];
        snprintf(fullpath, MAX_PATH, "%s/%s", dir, entry->d_name);

        if (lstat(fullpath, &statbuf) == -1)
        {
            perror("lstat");
            continue;
        }

        int is_dir = S_ISDIR(statbuf.st_mode);
        int is_file = S_ISREG(statbuf.st_mode);
        int is_link = S_ISLNK(statbuf.st_mode);

        // Фильтрация по типам
        if ((!show_links && !show_dirs && !show_files) ||
            (show_links && is_link) || (show_dirs && is_dir) || (show_files && is_file))
        {
            if (sort)
            {
                if (count >= capacity)
                {
                    capacity = capacity ? capacity * 2 : 10;
                    entries = realloc(entries, capacity * sizeof(Entry));
                    if (!entries)
                    {
                        perror("realloc");
                        exit(EXIT_FAILURE);
                    }
                }
                strncpy(entries[count].path, fullpath, MAX_PATH);
                count++;
            }
            else
            {
                printf("%s\n", fullpath);
            }
        }

        // Рекурсивный обход для каталогов
        if (is_dir)
        {
            dirwalk(fullpath, show_links, show_dirs, show_files, sort);
        }
    }

    // Сортировка и вывод, если требуется
    if (sort && entries)
    {
        qsort(entries, count, sizeof(Entry), compare_entries);
        for (size_t i = 0; i < count; i++)
        {
            printf("%s\n", entries[i].path);
        }
        free(entries);
    }

    closedir(dp);
}
