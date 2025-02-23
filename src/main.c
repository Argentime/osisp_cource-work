#define _POSIX_C_SOURCE 200809L // стандарт posix
#include "header/libs.h"
#include "header/dirwalk.h"

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