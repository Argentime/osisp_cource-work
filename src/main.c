#define _POSIX_C_SOURCE 200809L // стандарт posix
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <locale.h>

#define MAX_PATH 4096

// Структура для хранения путей и сортировки
typedef structd
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

int main(int argc, char *argv[])
{
    int opt;
    int show_links = 0, show_dirs = 0, show_files = 0, sort = 0;
    char *start_dir = ".";

    setlocale(LC_COLLATE, ""); // Для корректной сортировки

    // Обработка опций с помощью getopt
    while ((opt = getopt(argc, argv, "ldfs")) != -1)
    {
        switch (opt)
        {
        case 'l':
            show_links = 1;
            break;
        case 'd':
            show_dirs = 1;
            break;
        case 'f':
            show_files = 1;
            break;
        case 's':
            sort = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [dir] [-l] [-d] [-f] [-s]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Если указан каталог, используем его
    if (optind < argc)
    {
        start_dir = argv[optind];
    }

    dirwalk(start_dir, show_links, show_dirs, show_files, sort);
    return 0;
}