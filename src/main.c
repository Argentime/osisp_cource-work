#define _POSIX_C_SOURCE 200809L // стандарт posix
#include "header/libs.h"
#include "header/dirwalk.h"

void parse_option(char opt, int *show_links, int *show_dirs, int *show_files, int *sort)
{
    switch (opt)
    {
    case 'l':
        *show_links = 1;
        break;
    case 'd':
        *show_dirs = 1;
        break;
    case 'f':
        *show_files = 1;
        break;
    case 's':
        *sort = 1;
        break;
    default:
        fprintf(stderr, "Unknown option: -%c\n", opt);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int show_links = 0, show_dirs = 0, show_files = 0, sort = 0;
    char *start_dir = ".";
    int opt;

    setlocale(LC_COLLATE, ""); // Для корректной сортировки

    // Обработка опций с помощью getopt
    while ((opt = getopt(argc, argv, "ldfs")) != -1)
    {
        parse_option(opt, &show_links, &show_dirs, &show_files, &sort);
    }

    // Если есть каталог после опций
    if (optind < argc && argv[optind][0] != '-')
    {
        start_dir = argv[optind];
        optind++; // Перемещаем указатель после каталога
    }

    // Второй проход: обработка опций после каталога
    while (optind < argc)
    {
        if (argv[optind][0] == '-')
        {
            // Ручной разбор опций после каталога
            for (int i = 1; argv[optind][i] != '\0'; i++)
            {
                parse_option(argv[optind][i], &show_links, &show_dirs, &show_files, &sort);
            }
        }
        else
        {
            fprintf(stderr, "Error: Multiple directories specified\n");
            exit(EXIT_FAILURE);
        }
        optind++;
    }

    printf("%s\n", start_dir);
    dirwalk(start_dir, show_links, show_dirs, show_files, sort);
    return 0;
}